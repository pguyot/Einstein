// ==============================
// File:			TCLIApp.cp
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
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
// $Id$
// ==============================

#include <K/Defines/KDefinitions.h>
#include "TCLIApp.h"

// ANSI C & POSIX
#include <Emulator/Serial/TSerialPortDriverTcpClient.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <chrono>
#include <termios.h>
#include <thread>
#include <unistd.h>

// Einstein
#include "Emulator/EinsteinConfig.h"
#include "Emulator/ROM/TAIFROMImageWithREXes.h"
#include "Emulator/ROM/TFlatROMImageWithREX.h"
#include "Emulator/ROM/TROMImage.h"
#if HAVE_COREAUDIO
#include "Emulator/Sound/TCoreAudioSoundManager.h"
#endif
#if HAVE_PULSEAUDIO
#include "Emulator/Sound/TPulseAudioSoundManager.h"
#endif
#include "Emulator/Sound/TNullSoundManager.h"
#include "Emulator/Screen/TNullScreenManager.h"
#include "Emulator/Screen/TScreenManager.h"
#include "Emulator/TEmulator.h"
#include "Emulator/TMemory.h"
#include "Monitor/TMonitor.h"
#include "Monitor/TSymbolList.h"
#include "Emulator/Log/TBufferLog.h"
#include "Emulator/Log/TFileLog.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/Network/TNetworkManager.h"
#include "Emulator/Platform/TPlatformManager.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TCLIApp( void )
// -------------------------------------------------------------------------- //
TCLIApp::TCLIApp(void) :
		mProgramName(nullptr),
		mROMImage(nullptr),
		mEmulator(nullptr),
		mNetworkManager(nullptr),
		mSoundManager(nullptr),
		mScreenManager(nullptr),
		mPlatformManager(nullptr),
		mLog(nullptr),
		mMonitor(nullptr),
		mSymbolList(nullptr),
		mQuit(false)
{
	if (::pipe(mCmdPipe) != 0)
	{
		mCmdPipe[0] = -1;
		mCmdPipe[1] = -1;
	}
}

// -------------------------------------------------------------------------- //
//  * ~TCLIApp( void )
// -------------------------------------------------------------------------- //
TCLIApp::~TCLIApp(void)
{
	if (mCmdPipe[0] >= 0)
	{
		::close(mCmdPipe[0]);
	}
	if (mCmdPipe[1] >= 0)
	{
		::close(mCmdPipe[1]);
	}
	if (mEmulator)
	{
		delete mEmulator;
	}
	if (mScreenManager)
	{
		delete mScreenManager;
	}
	if (mNetworkManager)
	{
		delete mNetworkManager;
	}
	if (mSoundManager)
	{
		delete mSoundManager;
	}
	if (mLog)
	{
		delete mLog;
	}
	if (mROMImage)
	{
		delete mROMImage;
	}
	if (mMonitor)
	{
		delete mMonitor;
	}
	if (mSymbolList)
	{
		delete mSymbolList;
	}
}

// -------------------------------------------------------------------------- //
// Run( int, char** )
// -------------------------------------------------------------------------- //
void
TCLIApp::Run(int argc, char* argv[])
{
	mProgramName = argv[0];

	const char* defaultMachineString = "717006";
	const char* theMachineString = nullptr;
	const char* theRestoreFile = nullptr;
	const char* theSoundManagerClass = nullptr;
	const char* theScreenManagerClass = nullptr;
	const char* theDataPath = ::getenv("EINSTEIN_HOME");
	const char* theSerialPortDriver = "tcp"; // default settings,
	int portraitWidth = TScreenManager::kDefaultPortraitWidth;
	int portraitHeight = TScreenManager::kDefaultPortraitHeight;
	int ramSize = 0x40;
	bool useAIFROMFile = false; // Default is to use flat rom format.
	bool faceless = false; // Default is to run with an interactive console.
	bool useMonitor = false; // Default is to not have a monitor.
	int indexArgs = 1;
	if (argc < 2 && theDataPath == NULL)
	{
		SyntaxError();
	}
	if (theDataPath == NULL)
		theDataPath = argv[argc - 1];

	while (indexArgs < argc)
	{
		// TODO: add code for network manager
		if ((::strcmp(argv[1], "--help") == 0)
			|| (::strcmp(argv[1], "-h") == 0))
		{
			Help();
		} else if ((::strcmp(argv[1], "--version") == 0)
			|| (::strcmp(argv[1], "-v") == 0))
		{
			Version();
		} else if (::strcmp(argv[indexArgs], "-a") == 0)
		{
			indexArgs++;
			if ((indexArgs == argc - 1) || (mSoundManager != nullptr))
			{
				SyntaxError(argv[indexArgs - 1]);
			}

			theSoundManagerClass = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--audio=", 8) == 0)
		{
			if (mSoundManager != nullptr)
			{
				SyntaxError(argv[indexArgs]);
			}

			theSoundManagerClass = &argv[indexArgs][8];
		} else if (::strcmp(argv[indexArgs], "-s") == 0)
		{
			indexArgs++;
			if ((indexArgs == argc - 1) || (mScreenManager != nullptr))
			{
				SyntaxError(argv[indexArgs - 1]);
			}

			theScreenManagerClass = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--screen=", 9) == 0)
		{
			if (mScreenManager != nullptr)
			{
				SyntaxError(argv[indexArgs]);
			}

			theScreenManagerClass = &argv[indexArgs][9];
		} else if (::strcmp(argv[indexArgs], "-l") == 0)
		{
			indexArgs++;
			if ((indexArgs == argc - 1) || (mLog != nullptr))
			{
				SyntaxError(argv[indexArgs - 1]);
			}

			CreateLog(argv[indexArgs]);
		} else if (::strncmp(argv[indexArgs], "--log=", 6) == 0)
		{
			if (mLog != nullptr)
			{
				SyntaxError(argv[indexArgs]);
			}

			CreateLog(&argv[indexArgs][6]);
		} else if (::strcmp(argv[indexArgs], "--monitor") == 0)
		{
			if (mLog)
			{
				(void) ::printf("A log already exists (--monitor & --log are exclusive)\n");
				::exit(1);
			}
			mLog = new TBufferLog();
			useMonitor = true;
		} else if (::strcmp(argv[indexArgs], "-r") == 0)
		{
			indexArgs++;
			if ((indexArgs == argc - 1) || (theRestoreFile != nullptr))
			{
				SyntaxError(argv[indexArgs - 1]);
			}

			theRestoreFile = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--restore=", 10) == 0)
		{
			if (theRestoreFile != nullptr)
			{
				SyntaxError(argv[indexArgs]);
			}

			theRestoreFile = &argv[indexArgs][10];
		} else if (::strcmp(argv[indexArgs], "-m") == 0)
		{
			indexArgs++;
			if ((indexArgs == argc - 1) || (theMachineString != nullptr))
			{
				SyntaxError(argv[indexArgs - 1]);
			}

			theMachineString = argv[indexArgs];
		} else if (::strncmp(argv[indexArgs], "--machine=", 10) == 0)
		{
			if (theMachineString != nullptr)
			{
				SyntaxError(argv[indexArgs]);
			}

			theMachineString = &argv[indexArgs][10];
		} else if (::sscanf(argv[indexArgs], "--width=%i", &portraitWidth) == 1)
		{
			if (portraitWidth < (int) TScreenManager::kDefaultPortraitWidth)
			{
				(void) ::fprintf(
					stderr,
					"Warning, width is smaller than original (%i)\n",
					(int) TScreenManager::kDefaultPortraitWidth);
			}
		} else if (::sscanf(argv[indexArgs], "--height=%i", &portraitHeight) == 1)
		{
			if (portraitHeight < (int) TScreenManager::kDefaultPortraitHeight)
			{
				(void) ::fprintf(
					stderr,
					"Warning, height is smaller than original (%i)\n",
					(int) TScreenManager::kDefaultPortraitHeight);
			}
		} else if (::sscanf(argv[indexArgs], "--ram=%i", &ramSize) == 1)
		{
			if ((ramSize < 1) || (ramSize > 0x100))
			{
				(void) ::fprintf(
					stderr,
					"Ram size must be between 1 and 255 (for now, because only "
					"first bank is handled)\nI'll boot with 4 MB (64).\n");
				ramSize = 0x40;
			}
		} else if (::strncmp(argv[indexArgs], "--serial=tcp:", 13) == 0)
		{
			theSerialPortDriver = argv[indexArgs] + 9;
		} else if (::strcmp(argv[indexArgs], "--serial=tcp") == 0)
		{
			theSerialPortDriver = argv[indexArgs] + 9;
		} else if (::strcmp(argv[indexArgs], "--serial=null") == 0)
		{
			theSerialPortDriver = nullptr; // no string sets null driver
		} else if (::strcmp(argv[indexArgs], "--aif") == 0)
		{
			useAIFROMFile = true;
		} else if (::strcmp(argv[indexArgs], "--faceless") == 0)
		{
			faceless = true;
		} else if ((::strcmp(argv[indexArgs], "--help") == 0)
			|| (::strcmp(argv[indexArgs], "-h") == 0))
		{
			Help();
		} else if ((::strcmp(argv[indexArgs], "--version") == 0)
			|| (::strcmp(argv[indexArgs], "-v") == 0))
		{
			Version();
		}

		indexArgs++;
	}

	if (faceless && useMonitor)
	{
		(void) ::printf("--monitor and --faceless are exclusive.\n");
		::exit(0);
	}

	if (portraitHeight < portraitWidth)
	{
		(void) ::fprintf(
			stderr,
			"Warning, (portrait) height (%i) is smaller than width (%i). Boot screen won't be displayed properly\n",
			portraitHeight,
			portraitWidth);
	}

	(void) ::printf("Welcome to Einstein console.\n");
	(void) ::printf("This is %s.\n", VERSION_STRING);
	if (theSoundManagerClass == nullptr)
	{
#if HAVE_COREAUDIO
		mSoundManager = new TCoreAudioSoundManager(mLog);
#else
		mSoundManager = new TNullSoundManager(mLog);
#endif
	} else
	{
		CreateSoundManager(theSoundManagerClass);
	}
	if (theScreenManagerClass == nullptr)
	{
		CreateScreenManager("null", portraitWidth, portraitHeight);
	} else
	{
		CreateScreenManager(theScreenManagerClass, portraitWidth, portraitHeight);
	}
	if (theMachineString == nullptr)
	{
		theMachineString = defaultMachineString;
	}
	char theROMImagePath[512];
	char theREX1Path[512];
	char theFlashPath[512];
	(void) ::snprintf(theREX1Path, 512, "%s/Einstein.rex", theDataPath);
	(void) ::snprintf(theFlashPath, 512, "%s/flash", theDataPath);

	if (useAIFROMFile)
	{
		char theREX0Path[512];
		(void) ::snprintf(theREX0Path, 512, "%s/%s.rex", theDataPath, theMachineString);
		(void) ::snprintf(theROMImagePath, 512, "%s/%s.aif", theDataPath, theMachineString);
		mROMImage = new TAIFROMImageWithREXes(
			theROMImagePath, theREX0Path, theREX1Path);
	} else
	{
		(void) ::snprintf(theROMImagePath, 512, "%s/%s", theDataPath, theMachineString);
		mROMImage = new TFlatROMImageWithREX(
			theROMImagePath, theREX1Path);
	}
	if (mROMImage->GetErrorCode() != TROMImage::kNoError)
	{
		(void) ::fprintf(
			stderr,
			"Failed to load ROM image %s (error %u)\n",
			theROMImagePath,
			(unsigned int) mROMImage->GetErrorCode());
		::exit(1);
	}
	mNetworkManager = new TNullNetworkManager(mLog);
	mEmulator = new TEmulator(
		mLog, mROMImage, theFlashPath,
		mSoundManager, mScreenManager, mNetworkManager, ramSize << 16);

	mPlatformManager = mEmulator->GetPlatformManager();
	mNextStopCount = mEmulator->GetStopCount();

	if (!useMonitor)
	{
		mEmulator->CallOnQuit(
			[this]() {
				{
					std::lock_guard<std::mutex> lock(mSaveMutex);
					if (mSaveRequested)
					{
						return;
					}
				}
				if (mCmdPipe[1] >= 0)
				{
					(void) ::write(mCmdPipe[1], "Q", 1);
				}
			});
	}

	// the commandline argument is --serial=tcp:server:port . We currently support only TCP connections
	// theSerialDriver is either nil or text in the format 'server' or 'server:port' without single quotes
	auto extrPortDriverType = TSerialPorts::kNullDriver;
	if (theSerialPortDriver && strncmp(theSerialPortDriver, "tcp", 3) == 0)
		extrPortDriverType = TSerialPorts::kTcpClientDriver;
	mEmulator->SerialPorts.Initialize(
		extrPortDriverType,
		TSerialPorts::kNullDriver,
		TSerialPorts::kNullDriver,
		TSerialPorts::kNullDriver);
	if (extrPortDriverType == TSerialPorts::kTcpClientDriver)
	{
		TSerialPortDriver* sDriver = mEmulator->SerialPorts.GetDriverFor(TSerialPorts::kExtr);
		TSerialPortDriverTcpClient* tcp = dynamic_cast<TSerialPortDriverTcpClient*>(sDriver);
		if (tcp)
		{
			int port = 3679;
			theSerialPortDriver += 3; // skip 'tcp'
			if (theSerialPortDriver[0] == ':')
				theSerialPortDriver++; // skip the ':' in 'tcp:'
			char* server = strdup(theSerialPortDriver);
			char* sep = strrchr(server, ':');
			if (sep)
			{
				port = atoi(sep + 1);
				*sep = 0;
			}
			if (server[0] == 0)
			{
				::free(server);
				server = strdup("127.0.0.1");
			}
			tcp->SetServerPort(port);
			tcp->SetServerAddress(server);
			::free(server);
		}
	}
	if (theRestoreFile != nullptr)
	{
		if (!mEmulator->LoadState(theRestoreFile))
		{
			(void) ::fprintf(
				stderr,
				"Failed to restore state from %s\n",
				theRestoreFile);
			::exit(1);
		}
	}

	if (useMonitor)
	{
		char theSymbolListPath[512];
		(void) ::snprintf(theSymbolListPath, 512, "%s/%s.symbols",
			theDataPath, theMachineString);
		mSymbolList = new TSymbolList(theSymbolListPath);
		mMonitor = new TMonitor((TBufferLog*) mLog, mEmulator, mSymbolList, theDataPath);
	} else
	{
		(void) ::printf("Booting...\n");
	}

	pthread_t theThread;
	int theErr = ::pthread_create(&theThread, NULL, SThreadEntry, this);
	if (theErr)
	{
		(void) ::fprintf(stderr, "Error with pthread_create (%i)\n", theErr);
		::exit(2);
	}

	if (!faceless)
	{
		MenuLoop();
	}

	// Wait for the thread to finish.
	(void) ::pthread_join(theThread, NULL);
}

// -------------------------------------------------------------------------- //
// ThreadEntry( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::ThreadEntry(void)
{
	if (mMonitor)
	{
		mMonitor->Run();
		if (mCmdPipe[1] >= 0)
		{
			(void) ::write(mCmdPipe[1], "Q", 1);
		}
	} else
	{
		while (true)
		{
			KUInt32 theStopCount = 0;
			{
				std::unique_lock<std::mutex> lock(mSaveMutex);
				if (mSaveRequested)
				{
					mEmulatorStoppedForSave = true;
					mSaveCondVar.notify_all();
					mSaveCondVar.wait(lock, [this]() { return !mSaveRequested; });
					mEmulatorStoppedForSave = false;
					continue;
				}
				theStopCount = mNextStopCount;
			}
			mEmulator->Run(theStopCount);
			std::unique_lock<std::mutex> lock(mSaveMutex);
			if (!mSaveRequested)
			{
				mThreadExited = true;
				mSaveCondVar.notify_all();
				break;
			}
			mEmulatorStoppedForSave = true;
			mSaveCondVar.notify_all();
			mSaveCondVar.wait(lock, [this]() { return !mSaveRequested; });
			mEmulatorStoppedForSave = false;
		}
	}
	// Quit if the emulator quitted.
	mQuit = true;
}

// -------------------------------------------------------------------------- //
// MenuLoop( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::MenuLoop(void)
{
	PrintLine("Type help for help on available commands.");

	mQuit = false;

	if (mMonitor)
	{
		MonitorMenuLoop();
	} else
	{
		AppMenuLoop();
	}
}

// -------------------------------------------------------------------------- //
// MonitorMenuLoop( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::MonitorMenuLoop(void)
{
	fd_set theSocketSet;

	// Configure the terminal.
	struct termios theOldOptions;
	(void) ::tcgetattr(STDIN_FILENO, &theOldOptions);
	struct termios theNewOptions = theOldOptions;
	// Disable echo, canonical processing and signals.
	theNewOptions.c_lflag &= ~(ECHO | ICANON | ISIG | ECHOE);
	// Only one byte.
	theNewOptions.c_cc[VMIN] = 1;
	(void) ::tcsetattr(STDIN_FILENO, TCSANOW, &theNewOptions);

	int monitor_fd = mMonitor->GetMonitorSocket();

	char theCommand[2048];
	KUInt32 theIndex = 0;
	while (!mQuit)
	{
		theCommand[theIndex] = 0;
		bool hasCommand = false;
		mMonitor->DrawScreen();

		// Prompt.
		(void) ::printf("\033[Keinstein> %s", theCommand);
		(void) ::fflush(stdout);
		(void) ::fflush(stdin);

		// Wait for a command via select.
		FD_ZERO(&theSocketSet);
		FD_SET(STDIN_FILENO, &theSocketSet);
		FD_SET(monitor_fd, &theSocketSet);
		FD_SET(mCmdPipe[0], &theSocketSet);

		int readyFd = ::select(FD_SETSIZE, &theSocketSet, NULL, NULL, NULL);
		if (readyFd > 0)
		{
			if (FD_ISSET(STDIN_FILENO, &theSocketSet))
			{
				// Read one byte.
				ssize_t amount = ::read(STDIN_FILENO, &theCommand[theIndex], 1);

				if (amount < 1)
				{
					// File was closed.
					break;
				}

				// Seek a return.
				if ((theIndex == 2047) || (theCommand[theIndex] == '\n'))
				{
					// Found.
					// Stop here (ignore what's next).
					theCommand[theIndex] = 0;
					theIndex = 0;
					hasCommand = true;
				} else
				{
					// Processing: delete.
					if (theCommand[theIndex] == theNewOptions.c_cc[VERASE])
					{
						if (theIndex > 0)
						{
							theIndex--;
						} else
						{
							(void) ::printf("\007");
						}
					} else if ((theCommand[theIndex] == theNewOptions.c_cc[VINTR])
						|| (theCommand[theIndex] == theNewOptions.c_cc[VEOF]))
					{
						// Control-C & Control-D
						mQuit = true;
						break;
					} else
					{
						theIndex++;
					}
				}
			}

			if (FD_ISSET(monitor_fd, &theSocketSet))
			{
				// Read one (garbage) byte.
				char theByte;
				(void) ::read(monitor_fd, &theByte, 1);
			}

			if (FD_ISSET(mCmdPipe[0], &theSocketSet))
			{
				// Emulator is quitting, so leave the loop, too.
				mQuit = true;
				break;
			}
		} else
		{
			// Interrupted.
			// Let's exit the loop.
			mQuit = true;
			break;
		}

		if (hasCommand)
		{
			bool knownCommand = ExecuteCommand(theCommand);
			if (!knownCommand)
			{
				char buffer[256];
				(void) ::snprintf(buffer, sizeof(buffer), "Unknown command '%s'", theCommand);
				(void) ::printf("\007");
				PrintLine(buffer);
			}
		}
	}

	mMonitor->Stop();

	// Revert termios options.
	(void) ::tcsetattr(STDIN_FILENO, TCSANOW, &theOldOptions);
}

// -------------------------------------------------------------------------- //
// AppMenuLoop( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::AppMenuLoop(void)
{
	char theCommand[2048];
	while (!mQuit)
	{
		// Prompt.
		(void) ::printf("einstein> ");
		(void) ::fflush(stdout);

		// This input loop blocks until the user enters text or we receive anything through the mCmdPipe
		fd_set watchFDs;
		FD_ZERO(&watchFDs);
		FD_SET(STDIN_FILENO, &watchFDs);
		FD_SET(mCmdPipe[0], &watchFDs);
		int ret = select(FD_SETSIZE, &watchFDs, 0L, 0L, nullptr);
		if (ret == -1)
			continue;
		if (FD_ISSET(mCmdPipe[0], &watchFDs))
		{
			mQuit = true;
			return;
		}
		if (FD_ISSET(STDIN_FILENO, &watchFDs))
		{
			if (::fgets(theCommand, 2048, stdin) == NULL)
			{
				mQuit = true;
				break;
			}
		}

		// Strip the end line.
		auto theLength = ::strlen(theCommand);
		if (theLength > 1)
		{
			theCommand[theLength - 1] = '\0';

			bool knownCommand = ExecuteCommand(theCommand);
			if (!knownCommand)
			{
				char buffer[256];
				(void) ::snprintf(buffer, sizeof(buffer), "Unknown command '%s'", theCommand);
				PrintLine(buffer);
			}
		} // else: ignore empty lines.
	}

	mEmulator->Quit();
}

// -------------------------------------------------------------------------- //
// ExecuteCommand( const char* inCommand )
// -------------------------------------------------------------------------- //
bool
TCLIApp::ExecuteCommand(const char* inCommand)
{
	char theArg[2048];

	bool knownCommand = true;
	if (::strcmp(inCommand, "help") == 0)
	{
		PrintHelp();
	} else if (::sscanf(inCommand, "install %s", theArg) == 1)
	{
		mPlatformManager->InstallPackage(theArg);
	} else if (::sscanf(inCommand, "ns %s", theArg) == 1)
	{
		mPlatformManager->EvalNewtonScript(theArg);
	} else if (::strcmp(inCommand, "power") == 0)
	{
		mPlatformManager->SendPowerSwitchEvent();
	} else if (::strcmp(inCommand, "backlight") == 0)
	{
		mPlatformManager->SendBacklightEvent();
	} else if (::sscanf(inCommand, "save %s", theArg) == 1)
	{
		if (mMonitor)
		{
			if (!mMonitor->RequestSaveState(theArg))
			{
				PrintLine("Monitor is busy, could not save state");
			} else
			{
				while (mMonitor->IsStateRequestPending())
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}
			}
		} else
		{
			bool exited = false;
			{
				std::unique_lock<std::mutex> lock(mSaveMutex);
				if (mThreadExited)
				{
					exited = true;
				} else
				{
					mSaveRequested = true;
					mEmulator->Stop();
					mSaveCondVar.wait(lock, [this]() { return mEmulatorStoppedForSave || mThreadExited; });
					exited = mThreadExited;
				}
			}
			bool saved = mEmulator->SaveState(theArg);
			{
				std::lock_guard<std::mutex> lock(mSaveMutex);
				mNextStopCount = mEmulator->GetStopCount();
				mSaveRequested = false;
				mSaveCondVar.notify_all();
			}
			if (!saved)
			{
				PrintLine("Saving emulator state failed");
			} else if (exited)
			{
				PrintLine("Emulator state saved");
			}
		}
	} else if (::strcmp(inCommand, "quit") == 0)
	{
		mQuit = true;
	} else
	{
		knownCommand = false;
		if (mMonitor)
		{
			knownCommand = mMonitor->ExecuteCommand(inCommand);
		}
	}

	return knownCommand;
}

// -------------------------------------------------------------------------- //
// PrintHelp( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::PrintHelp(void)
{
	PrintLine("help               show this help");
	PrintLine("install path       install package");
	PrintLine("power              press the power button");
	PrintLine("backlight          press the power button (long)");
	PrintLine("ns command         compile and execute NewtonScript command");
	PrintLine("save path          save state to file");
	if (mMonitor)
	{
		mMonitor->PrintHelp();
	}
	PrintLine("quit or ctrl-D     quit");
}

// -------------------------------------------------------------------------- //
// PrintLine( const char* )
// -------------------------------------------------------------------------- //
void
TCLIApp::PrintLine(const char* inLine)
{
	if (mMonitor)
	{
		mMonitor->PrintLine(inLine, 0);
	} else
	{
		(void) ::printf("%s\n", inLine);
	}
}

// -------------------------------------------------------------------------- //
// SyntaxError( const char* )
// -------------------------------------------------------------------------- //
void
TCLIApp::SyntaxError(const char* inBadOption)
{
	(void) ::fprintf(
		stderr,
		"%s -- syntax error with option %s\n",
		mProgramName,
		inBadOption);
	(void) ::fprintf(
		stderr,
		"Try %s --help for more help\n",
		mProgramName);
	::exit(1);
}

// -------------------------------------------------------------------------- //
// SyntaxError( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::SyntaxError(void)
{
	(void) ::fprintf(
		stderr,
		"%s -- syntax error\n",
		mProgramName);
	(void) ::fprintf(
		stderr,
		"syntax is %s [options] [path_to_data] (or set the environment variable EINSTEIN_HOME)\n",
		mProgramName);
	(void) ::fprintf(
		stderr,
		"Try %s --help for more help\n",
		mProgramName);
	::exit(1);
}

// -------------------------------------------------------------------------- //
// Help( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::Help(void)
{
	(void) ::printf(
		"%s - Einstein Platform\n",
		mProgramName);
	(void) ::printf(
		"%s [options] [data_path] (or set the environment variable EINSTEIN_HOME)\n",
		mProgramName);
	(void) ::printf(
		"  -a | --audio=audiodriver        (null, coreaudio, pulseaudio)\n");
	(void) ::printf(
		"  -s | --screen=screen driver     (null)\n");
	(void) ::printf(
		"  --serial=serialdriver           (null, tcp:server:port, default is tcp:127.0.0.1:3679)\n");
	(void) ::printf(
		"  --width=portrait width          (default is 320)\n");
	(void) ::printf(
		"  --height=portrait height        (default is 480)\n");
	(void) ::printf(
		"  -l | --log=log file             (default to no log)\n");
	(void) ::printf(
		"  -r | --restore=restore file     (default to start from scratch)\n");
	(void) ::printf(
		"  -m | --machine=machine string   (717006, 737041, 747129)\n");
	(void) ::printf(
		"  --monitor                       monitor mode\n");
	(void) ::printf(
		"  --ram=size                      ram size in 64 KB (1-255) (default: 64, i.e. 4 MB)\n");
	(void) ::printf(
		"  --aif                           read aif files\n");
	(void) ::printf(
		"  --faceless                      run without interactive console menu\n");
	::exit(1);
}

// -------------------------------------------------------------------------- //
// Version( void )
// -------------------------------------------------------------------------- //
void
TCLIApp::Version(void)
{
	(void) ::printf("%s\n", VERSION_STRING);
	(void) ::printf("%s.\n", COPYRIGHT_STRING);
	::exit(0);
}

// -------------------------------------------------------------------------- //
// CreateLog( const char* )
// -------------------------------------------------------------------------- //
void
TCLIApp::CreateLog(const char* inFilePath)
{
	if (mLog)
	{
		(void) ::printf("A log already exists (--monitor & --log are exclusive)\n");
		::exit(1);
	}
	mLog = new TFileLog(inFilePath);
}

// -------------------------------------------------------------------------- //
// CreateSoundManager( const char* )
// -------------------------------------------------------------------------- //
void
TCLIApp::CreateSoundManager(const char* inClass)
{
	if (::strcmp(inClass, "null") == 0)
	{
		mSoundManager = new TNullSoundManager(mLog);
#if HAVE_COREAUDIO
	} else if (::strcmp(inClass, "coreaudio") == 0)
	{
		mSoundManager = new TCoreAudioSoundManager(mLog);
#endif
#if HAVE_PULSEAUDIO
	} else if (::strcmp(inClass, "pulseaudio") == 0)
	{
		mSoundManager = new TPulseAudioSoundManager(mLog);
#endif
	} else
	{
		(void) ::fprintf(stderr, "Unknown sound manager class %s\n", inClass);
		::exit(1);
	}
}

// -------------------------------------------------------------------------- //
// CreateScreenManager( const char*, int, int )
// -------------------------------------------------------------------------- //
void
TCLIApp::CreateScreenManager(
	const char* inClass,
	int inPortraitWidth,
	int inPortraitHeight)
{
	if (::strcmp(inClass, "null") == 0)
	{
		mScreenManager = new TNullScreenManager(
			mLog,
			(KUInt32) inPortraitWidth,
			(KUInt32) inPortraitHeight);
	} else
	{
		(void) ::fprintf(stderr, "Unknown screen manager class %s\n", inClass);
		::exit(1);
	}
}

// ======================================================================= //
//         THE LESSER-KNOWN PROGRAMMING LANGUAGES #10: SIMPLE              //
//                                                                         //
// SIMPLE is an acronym for Sheer Idiot's Monopurpose Programming Language //
// Environment.  This language, developed at the Hanover College for       //
// Technological Misfits, was designed to make it impossible to write code //
// with errors in it.  The statements are, therefore, confined to BEGIN,   //
// END and STOP.  No matter how you arrange the statements, you can't make //
// a syntax error.  Programs written in SIMPLE do nothing useful.  Thus    //
// they achieve the results of programs written in other languages without //
// the tedious, frustrating process of testing and debugging.              //
// ======================================================================= //
