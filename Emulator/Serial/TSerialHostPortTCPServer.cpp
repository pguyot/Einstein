// ==============================
// File:			TSerialHostPortTCPServer.cpp
// Project:			Einstein
//
// Copyright 2026 Eckhart Koeppen
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================

#include "TSerialHostPortTCPServer.h"

// POSIX
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#if TARGET_OS_WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "Emulator/Log/TLog.h"

void
TSerialHostPortTCPServer::CloseSocketDescriptor(int inSocket)
{
	if (inSocket != -1)
	{
#if TARGET_OS_WIN32
		closesocket(inSocket);
#else
		::close(inSocket);
#endif
	}
}

void
TSerialHostPortTCPServer::ShutdownSocketDescriptor(int inSocket)
{
	if (inSocket != -1)
	{
#if TARGET_OS_WIN32
		shutdown(inSocket, SD_BOTH);
#else
		shutdown(inSocket, SHUT_RDWR);
#endif
	}
}

TSerialHostPortTCPServer::TSerialHostPortTCPServer(
	TLog* inLog,
	KUInt32 inLocation,
	const std::string& inConfig,
	TEmulator* inEmulator) :
		TSerialHostPort(inLog, inLocation, inEmulator)
{
	if (!inConfig.empty())
	{
		size_t colonPos = inConfig.rfind(':');
		if (colonPos != std::string::npos)
		{
			if (colonPos > 0)
			{
				mHost = inConfig.substr(0, colonPos);
			}
			int parsedPort = std::atoi(inConfig.substr(colonPos + 1).c_str());
			if (parsedPort > 0 && parsedPort <= 65535)
			{
				mPort = parsedPort;
			} else if (mLog)
			{
				mLog->FLogLine("TSerialHostPortTCPServer: Invalid port in '%s', using %d", inConfig.c_str(), mPort);
			}
		} else
		{
			int parsedPort = std::atoi(inConfig.c_str());
			if (parsedPort > 65535)
			{
				if (mLog)
					mLog->FLogLine("TSerialHostPortTCPServer: Invalid port in '%s', using %d", inConfig.c_str(), mPort);
			} else if (parsedPort > 0)
			{
				mPort = parsedPort;
			} else
			{
				mHost = inConfig;
			}
		}
	}

	AssignInterruptID();
	CreateServer();
}

TSerialHostPortTCPServer::~TSerialHostPortTCPServer()
{
	mQuitRequested = true;

	// Unblock and join the accept thread FIRST so no new clients can be accepted
	if (mListenSocket != -1)
	{
		int sock = mListenSocket;
		mListenSocket = -1;
		ShutdownSocketDescriptor(sock);
		CloseSocketDescriptor(sock);
	}

	if (mAcceptThread.joinable())
	{
		mAcceptThread.join();
	}

	// Now cleanly close any active client and join worker threads
	CloseClient();

	{
		std::lock_guard<std::mutex> rlock(mReadMutex);
		delete mReadBuffer;
		mReadBuffer = nullptr;
	}
	{
		std::lock_guard<std::mutex> wlock(mWriteMutex);
		delete mWriteBuffer;
		mWriteBuffer = nullptr;
	}
}

void
TSerialHostPortTCPServer::CloseClient()
{
	int sockToClose = -1;
	{
		std::lock_guard<std::mutex> wlock(mWriteMutex);
		std::lock_guard<std::mutex> clock(mClientMutex);
		if (mClientSocket != -1)
		{
			sockToClose = mClientSocket;
			mClientSocket = -1;
			mClientConnected = false;
		}
		mWriteCV.notify_all();
	}

	if (sockToClose != -1)
	{
		mReadCV.notify_all();
		ShutdownSocketDescriptor(sockToClose);
	}

	if (mReadThread.joinable())
	{
		mReadThread.join();
	}
	if (mWriteThread.joinable())
	{
		mWriteThread.join();
	}

	if (sockToClose != -1)
	{
		CloseSocketDescriptor(sockToClose);
	}

	// Flush remaining write buffer
	{
		std::lock_guard<std::mutex> lock(mWriteMutex);
		if (mWriteBuffer)
			mWriteBuffer->Discard(mWriteBuffer->AvailableBytes());
	}
}

bool
TSerialHostPortTCPServer::CreateServer()
{
	mReadBuffer = new TCircleBuffer(8 * 1024);
	mWriteBuffer = new TCircleBuffer(8 * 1024);
	mSerialStatus = kSerialTxBufferEmpty;

#if TARGET_OS_WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	mListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (mListenSocket < 0)
	{
		if (mLog)
			mLog->FLogLine("TSerialHostPortTCPServer: Failed to create socket: %d", errno);
		mSerialStatus = kSerialChipGone;
		return false;
	}

	int reuse = 1;
	setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof(reuse));

	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(static_cast<uint16_t>(mPort));
	if (inet_pton(AF_INET, mHost.c_str(), &servAddr.sin_addr) <= 0)
	{
		if (mLog)
			mLog->FLogLine("TSerialHostPortTCPServer: Invalid IPv4 address '%s'", mHost.c_str());
		CloseSocketDescriptor(mListenSocket);
		mListenSocket = -1;
		mSerialStatus = kSerialChipGone;
		return false;
	}

	if (bind(mListenSocket, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
	{
		if (mLog)
			mLog->FLogLine("TSerialHostPortTCPServer: Failed to bind to %s:%d: %d", mHost.c_str(), mPort, errno);
		CloseSocketDescriptor(mListenSocket);
		mListenSocket = -1;
		mSerialStatus = kSerialChipGone;
		return false;
	}

	if (listen(mListenSocket, 1) < 0)
	{
		if (mLog)
			mLog->FLogLine("TSerialHostPortTCPServer: Failed to listen on socket: %d", errno);
		CloseSocketDescriptor(mListenSocket);
		mListenSocket = -1;
		mSerialStatus = kSerialChipGone;
		return false;
	}

	if (mLog)
	{
		mLog->FLogLine("TSerialHostPortTCPServer: Listening on %s:%d", mHost.c_str(), mPort);
	}

	mAcceptThread = std::thread(&TSerialHostPortTCPServer::AcceptThreadEntry, this);
	return true;
}

void
TSerialHostPortTCPServer::AcceptThreadEntry()
{
#if !TARGET_OS_WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

	while (!mQuitRequested && mListenSocket != -1)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int newClient = ::accept(mListenSocket, (struct sockaddr*) &clientAddr, &clientLen);
		if (newClient < 0)
		{
			if (mQuitRequested)
				break;
			continue;
		}
		if (mQuitRequested)
		{
			CloseSocketDescriptor(newClient);
			break;
		}

		int nodelay = 1;
		setsockopt(newClient, IPPROTO_TCP, TCP_NODELAY, (const char*) &nodelay, sizeof(nodelay));

		// If a client is currently active, reject new incoming connection
		{
			std::lock_guard<std::mutex> lock(mClientMutex);
			if (mClientConnected)
			{
				if (mLog)
				{
					mLog->FLogLine("TSerialHostPortTCPServer: Rejecting new connection (fd=%d), client already connected", newClient);
				}
				CloseSocketDescriptor(newClient);
				continue;
			}
		}

		// If previous client workers are still joinable, clean them up and close prior descriptor
		int oldSock = -1;
		{
			std::lock_guard<std::mutex> lock(mClientMutex);
			oldSock = mClientSocket;
			mClientSocket = -1;
		}
		if (mReadThread.joinable())
			mReadThread.join();
		if (mWriteThread.joinable())
			mWriteThread.join();
		if (oldSock != -1)
			CloseSocketDescriptor(oldSock);

		// Reset buffers for new client session
		{
			std::lock_guard<std::mutex> rlock(mReadMutex);
			if (mReadBuffer)
				mReadBuffer->Discard(mReadBuffer->AvailableBytes());
			mSerialStatus &= ~kSerialRxCharAvailable;
		}
		{
			std::lock_guard<std::mutex> wlock(mWriteMutex);
			if (mWriteBuffer)
				mWriteBuffer->Discard(mWriteBuffer->AvailableBytes());
		}

		{
			std::lock_guard<std::mutex> lock(mClientMutex);
			mClientSocket = newClient;
			mClientConnected = true;
		}

		if (mLog)
		{
			mLog->FLogLine("TSerialHostPortTCPServer: Client connected (fd=%d)", newClient);
		}

		mReadThread = std::thread(&TSerialHostPortTCPServer::ReadThreadEntry, this, newClient);
		mWriteThread = std::thread(&TSerialHostPortTCPServer::WriteThreadEntry, this, newClient);
	}
}

void
TSerialHostPortTCPServer::ReadThreadEntry(int inClientSocket)
{
#if !TARGET_OS_WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

	KUInt8 data[512];
	while (!mQuitRequested && mClientConnected)
	{
		// Backpressure: wait if read buffer is full
		{
			std::unique_lock<std::mutex> lock(mReadMutex);
			mReadCV.wait(lock, [this, inClientSocket]() {
				return mQuitRequested
					|| !mClientConnected
					|| inClientSocket != mClientSocket
					|| (mReadBuffer != nullptr && mReadBuffer->AvailableBytes() < kMaxReadBufferSize);
			});
		}
		if (mQuitRequested || !mClientConnected || inClientSocket != mClientSocket)
			break;

		ssize_t n = ::recv(inClientSocket, (char*) data, sizeof(data), 0);
		if (n > 0)
		{
			{
				std::lock_guard<std::mutex> lock(mReadMutex);
				if (mReadBuffer != nullptr)
				{
					mReadBuffer->Produce(data, static_cast<KUInt32>(n));
					mSerialStatus |= kSerialRxCharAvailable;
				}
			}
			mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
		} else
		{
			// EOF or socket error
			if (mLog && !mQuitRequested && inClientSocket == mClientSocket)
			{
				mLog->FLogLine("TSerialHostPortTCPServer: Client disconnected");
			}
			break;
		}
	}

	// Signal writer thread to stop and shutdown socket to abort blocked writes.
	// Do NOT close descriptor yet: keep it valid until both worker threads have stopped
	// so that accept() cannot reallocate this fd number while the old writer is in flight.
	ShutdownSocketDescriptor(inClientSocket);
	{
		std::lock_guard<std::mutex> wlock(mWriteMutex);
		std::lock_guard<std::mutex> clock(mClientMutex);
		if (mClientSocket == inClientSocket)
		{
			mClientConnected = false;
		}
		mWriteCV.notify_all();
	}
}

void
TSerialHostPortTCPServer::WriteThreadEntry(int inClientSocket)
{
#if !TARGET_OS_WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

	while (!mQuitRequested && mClientConnected)
	{
		KUInt8 wbuf[512];
		KUIntPtr toWrite = 0;
		{
			std::unique_lock<std::mutex> lock(mWriteMutex);
			mWriteCV.wait(lock, [this, inClientSocket]() {
				return mQuitRequested
					|| !mClientConnected
					|| inClientSocket != mClientSocket
					|| (mWriteBuffer != nullptr && !mWriteBuffer->IsEmpty());
			});
			if (mQuitRequested || !mClientConnected || inClientSocket != mClientSocket)
				break;

			if (mWriteBuffer != nullptr)
			{
				toWrite = mWriteBuffer->Peek(wbuf, sizeof(wbuf));
			}
		}

		if (toWrite > 0)
		{
#if TARGET_OS_WIN32
			int sent = ::send(inClientSocket, (const char*) wbuf, static_cast<int>(toWrite), 0);
#else
			ssize_t sent = ::send(inClientSocket, wbuf, toWrite, 0);
#endif
			if (sent > 0)
			{
				std::lock_guard<std::mutex> lock(mWriteMutex);
				if (mWriteBuffer != nullptr)
				{
					mWriteBuffer->Discard(sent);
				}
			} else
			{
				// Error sending to client
				break;
			}
		}
	}
}

void
TSerialHostPortTCPServer::PutByte(KUInt8 nextChar)
{
	{
		std::lock_guard<std::mutex> lock(mWriteMutex);
		if (mWriteBuffer != nullptr)
		{
			mWriteBuffer->Produce(&nextChar, 1);
		}
	}
	mWriteCV.notify_one();

	mSerialStatus |= kSerialTxBufferEmpty;
	mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
}

KUInt8
TSerialHostPortTCPServer::GetByte()
{
	KUInt8 data = 0;
	bool needCVNotify = false;
	{
		std::lock_guard<std::mutex> lock(mReadMutex);
		if (mReadBuffer != nullptr)
		{
			bool wasFull = mReadBuffer->AvailableBytes() >= kMaxReadBufferSize;
			mReadBuffer->Consume(&data, sizeof(data));
			needCVNotify = wasFull && mReadBuffer->AvailableBytes() < kMaxReadBufferSize;
			if (mReadBuffer->IsEmpty())
			{
				mSerialStatus &= ~kSerialRxCharAvailable;
			} else
			{
				mEmulator->GetInterruptManager()->RaiseInterrupt(mInterruptID);
			}
		}
	}

	if (needCVNotify)
	{
		mReadCV.notify_one();
	}

	return data;
}

void
TSerialHostPortTCPServer::SetInterruptEnable(bool /* enable */)
{
}

void
TSerialHostPortTCPServer::Reset()
{
}

void
TSerialHostPortTCPServer::SetBreak(bool /* assert */)
{
}

InterfaceSpeed
TSerialHostPortTCPServer::SetSpeed(BitRate /* bitsPerSec */)
{
	return 0;
}

void
TSerialHostPortTCPServer::SetIOParms(TCMOSerialIOParms* /* opt */)
{
}

void
TSerialHostPortTCPServer::Reconfigure()
{
}

KSInt32
TSerialHostPortTCPServer::InitByOption(TOption* /* initOpt */)
{
	return 0;
}

KSInt32
TSerialHostPortTCPServer::ProcessOption(TOption* /* opt */)
{
	return 0;
}

KSInt32
TSerialHostPortTCPServer::SetSerialMode(SerialMode /* mode */)
{
	return 0;
}

RxErrorStatus
TSerialHostPortTCPServer::GetByteAndStatus(KUInt8* /* nextCharPtr */)
{
	return 0;
}

bool
TSerialHostPortTCPServer::AllSent()
{
	std::lock_guard<std::mutex> lock(mWriteMutex);
	return (mWriteBuffer == nullptr) || mWriteBuffer->IsEmpty();
}

KSInt32
TSerialHostPortTCPServer::WaitForAllSent()
{
	return 0;
}

