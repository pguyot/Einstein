// ==============================
// File:			TJITLLVMObjectCache.cp
// Project:			Einstein
//
// Copyright 2014 by Paul Guyot (pguyot@kallisys.net).
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

#include <K/Defines/KDefinitions.h>

#include "Emulator/JIT/JIT.h"

#ifdef JITTARGET_LLVM

#include "Emulator/JIT/LLVM/TJITLLVMObjectCache.h"
#include "Emulator/JIT/LLVM/TJITLLVMPage.h"
#include "Emulator/ROM/TROMImage.h"

// llvm
#include <llvm/Support/Path.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/Object/SymbolicFile.h>
#include <llvm/ExecutionEngine/RuntimeDyld.h>
#include <llvm/ExecutionEngine/ObjectImage.h>

// POSIX
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

using namespace llvm;

// -------------------------------------------------------------------------- //
//  * TJITLLVMObjectCache(const TROMImage&, RTDyldMemoryManager*)
// -------------------------------------------------------------------------- //
TJITLLVMObjectCache::TJITLLVMObjectCache(const TROMImage& inROMImage,
										 RuntimeDyld* dynamicLinker,
										 TJITLLVMRecordingMemoryManager* memoryManager)
    :
        mCacheDir(GetCacheDir(inROMImage)),
		mDynamicLinker(dynamicLinker),
		mMemoryManager(memoryManager)
{
	InvalidateCacheIfRequired(inROMImage);
}

// -------------------------------------------------------------------------- //
//  * ~TJITLLVMObjectCache()
// -------------------------------------------------------------------------- //
TJITLLVMObjectCache::~TJITLLVMObjectCache() {
	for (auto loadedPage : mLoadedPages) {
		::free(loadedPage.second.second);
	}
	for (auto image : mLoadedImages) {
		delete image;
	}
}

// -------------------------------------------------------------------------- //
//  * GetCacheDir()
// -------------------------------------------------------------------------- //
SmallString<128>
TJITLLVMObjectCache::GetCacheDir(const TROMImage& inROMImage) {
	SmallString<128> path = sys::path::parent_path(inROMImage.GetPath());
	sys::path::append(path, inROMImage.GetMachineString() + ".llvm-cache");
	// If directory does not exist, create it.
	struct stat theInfos;
	int err = ::stat( path.c_str(), &theInfos );
	if (err < 0) {
		::mkdir(path.c_str(), 0755);
	}
	return path;
}

// -------------------------------------------------------------------------- //
//  * InvalidateCacheIfRequired()
// -------------------------------------------------------------------------- //
void
TJITLLVMObjectCache::InvalidateCacheIfRequired(const TROMImage& inROMImage) {
	struct stat theROMInfos;
	::stat(inROMImage.GetPath(), &theROMInfos);
	DIR* dir = ::opendir(mCacheDir.str().str().c_str());
	while (dir != NULL) {
		struct dirent theEntry;
		struct dirent* theEntryPtr = nullptr;
		int r = readdir_r(dir, &theEntry, &theEntryPtr);
		if (r || theEntryPtr == nullptr) break;
		if (theEntry.d_namlen > 2) {
			// implicitly skips . and ..
			if (theEntry.d_type == DT_DIR) {
				SmallString<128> path(mCacheDir);
				sys::path::append(path, std::string(theEntry.d_name, theEntry.d_namlen));
				DIR* subdir = ::opendir(path.c_str());
				while (subdir != NULL) {
					struct stat theFileInfos;
					struct dirent theSubEntry;
					struct dirent* theSubEntryPtr = nullptr;
					r = readdir_r(subdir, &theSubEntry, &theSubEntryPtr);
					if (r || theSubEntryPtr == nullptr) break;
					if (theSubEntry.d_name[theSubEntry.d_namlen - 1] == 'o'
						&& theSubEntry.d_name[theSubEntry.d_namlen - 2] == '.'
						&& theSubEntry.d_type == DT_REG) {
						SmallString<128> subpath(path);
						sys::path::append(subpath, std::string(theEntry.d_name, theEntry.d_namlen));
						bool shouldUnlink = true;
						int r = ::stat( subpath.c_str(), &theFileInfos );
						if (r == 0 &&
							((theFileInfos.st_ctimespec.tv_sec > theROMInfos.st_mtimespec.tv_sec)
							 || ((theFileInfos.st_ctimespec.tv_sec == theROMInfos.st_mtimespec.tv_sec)
								 && (theFileInfos.st_ctimespec.tv_nsec > theROMInfos.st_mtimespec.tv_nsec)))) {
								 shouldUnlink = false;
							 }
						if (shouldUnlink) {
							::unlink(subpath.c_str());
						}
					}
				}
				if (subdir) ::closedir(subdir);
			}
		}
	}
	if (dir) ::closedir(dir);
}

// -------------------------------------------------------------------------- //
//  * SaveCompiledObject(const Module*, const StringRef objectCode)
// -------------------------------------------------------------------------- //
void
TJITLLVMObjectCache::SaveCompiledObject(const Module* inModule, const StringRef objectCode) {
	const std::string& moduleName = inModule->getModuleIdentifier();
	if (!moduleName.empty()) {
		std::string::size_type n = moduleName.find("_");
		std::string pagePrefix = moduleName.substr(0, n);
		const auto pagesIt = mLoadedPages.find(pagePrefix);
		if (pagesIt != mLoadedPages.end()) {
			::free(pagesIt->second.second);
			mLoadedPages.erase(pagePrefix);
		}
		SmallString<128> path(mCacheDir);
		sys::path::append(path, pagePrefix);
		(void) ::mkdir(path.c_str(), 0755);
		sys::path::append(path, moduleName + ".o");
		std::string err;
		raw_fd_ostream stream(path.c_str(), err, sys::fs::F_Excl);
		if (err != "" || stream.has_error()) {
			fprintf(stderr, "Could not open object file for writing: %s\n", err.c_str());
		} else {
			stream << objectCode;
		}
		stream.close();
	}
}

// -------------------------------------------------------------------------- //
//  * GetPageFunctions(const TJITLLVMPage&, JITFuncPtr*)
// -------------------------------------------------------------------------- //
void
TJITLLVMObjectCache::GetPageFunctions(const TJITLLVMPage& page, JITFuncPtr* outEntryPoints) {
	ssize_t outputSize = page.GetInstructionCount() * sizeof(JITFuncPtr);
	const auto pagesIt = mLoadedPages.find(page.PagePrefix());
	if (pagesIt != mLoadedPages.end()) {
		memcpy(outEntryPoints, pagesIt->second.second, std::min(pagesIt->second.first, outputSize));
	} else {
		ssize_t bufferSize = outputSize;
		JITFuncPtr* cachedResult = (JITFuncPtr*) ::malloc(bufferSize);
		::memset(cachedResult, 0, bufferSize);
		SmallString<112> path(mCacheDir);
		sys::path::append(path, page.PagePrefix());
		DIR* dir = ::opendir(path.c_str());
		bool loadedObjectFiles = false;
		while (dir != NULL) {
			struct dirent theEntry;
			struct dirent* theEntryPtr = nullptr;
			int r = readdir_r(dir, &theEntry, &theEntryPtr);
			if (r || theEntryPtr == nullptr) break;
			if (theEntry.d_name[theEntry.d_namlen - 1] == 'o'
				&& theEntry.d_name[theEntry.d_namlen - 2] == '.'
				&& theEntry.d_type == DT_REG) {
				auto it = mLoadedObjectFiles.find(theEntry.d_ino);
				if (it != mLoadedObjectFiles.end()) {
					for (const std::pair<KUInt32, JITFuncPtr>& previouslyLoadedEntryPoint : it->second) {
						if (previouslyLoadedEntryPoint.first >= bufferSize / sizeof(JITFuncPtr)) {
							ssize_t newBufferSize = (previouslyLoadedEntryPoint.first + 1) * sizeof(JITFuncPtr);
							cachedResult = (JITFuncPtr*) ::realloc(cachedResult, newBufferSize);
							bufferSize = newBufferSize;
						}
						cachedResult[previouslyLoadedEntryPoint.first] = previouslyLoadedEntryPoint.second;
					}
				} else {
					loadedObjectFiles = true;
					std::vector<std::pair<KUInt32, JITFuncPtr>> loadedFunctions;
					SmallString<128> objPath(path);
					sys::path::append(objPath, std::string(theEntry.d_name, theEntry.d_namlen));
					ErrorOr<object::ObjectFile*> objFile = object::ObjectFile::createObjectFile(objPath.c_str());
					if (!objFile) {
						fprintf(stderr, "Could not create object file %s (%s)\n", objPath.c_str(), objFile.getError().message().c_str());
						abort();
					}
					mMemoryManager->FlushRecordedAllocations();
					ObjectImage* image = mDynamicLinker->loadObject(std::move(std::unique_ptr<object::ObjectFile>(objFile.get())));
					mLoadedImages.push_back(image);
					uint8_t* sectionLoadAddr = mMemoryManager->GetLatestSectionLoadAddress();
					auto section = image->begin_sections();
					for (auto it = image->begin_symbols(); it != image->end_symbols(); ++it) {
						uint32_t flags = it->getFlags();
						if ((flags & (object::SymbolRef::SF_Undefined | object::SymbolRef::SF_Global)) == object::SymbolRef::SF_Global) {
							StringRef name;
							std::error_code err = it->getName(name);
							if (err) {
								fprintf(stderr, "Could not get name of symbol (%s)\n", err.message().c_str());
							}
							uint64_t addr;
							err = it->getAddress(addr);
							if (err) {
								fprintf(stderr, "Could not get address of symbol (%s)\n", err.message().c_str());
							}
							err = it->getSection(section);
							if (err) {
								fprintf(stderr, "Could not get section of symbol (%s)\n", err.message().c_str());
							}
							uint64_t sectionAddr;
							err = section->getAddress(sectionAddr);
							if (err) {
								fprintf(stderr, "Could not get address of section (%s)\n", err.message().c_str());
							}
							JITFuncPtr ptr = (JITFuncPtr) (sectionLoadAddr + addr - sectionAddr);
							KUInt32 functionOffset;
							if (name[0] == '_') {
								functionOffset = page.OffsetFromFunctionName(name.substr(1));
							} else {
								functionOffset = page.OffsetFromFunctionName(name);
							}
							if (functionOffset >= bufferSize * sizeof(JITFuncPtr)) {
								ssize_t newBufferSize = (functionOffset + 1) * sizeof(JITFuncPtr);
								cachedResult = (JITFuncPtr*) ::realloc(cachedResult, newBufferSize);
								bufferSize = newBufferSize;
							}
							cachedResult[functionOffset] = ptr;
							loadedFunctions.push_back(std::make_pair(functionOffset, ptr));
						}
					}
					mLoadedObjectFiles[theEntry.d_ino] = loadedFunctions;
				}
			}
		}
		if (dir) {
			::closedir(dir);
		}
		if (loadedObjectFiles) {
			// Resolve location & set memory permissions only once per page.
			mDynamicLinker->resolveRelocations();
			mDynamicLinker->registerEHFrames();
			mMemoryManager->finalizeMemory();
		}
		mLoadedPages[page.PagePrefix()] = std::make_pair(bufferSize, cachedResult);
		memcpy(outEntryPoints, cachedResult, outputSize);
	}
}

#endif