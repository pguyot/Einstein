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

// POSIX
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

using namespace llvm;

// -------------------------------------------------------------------------- //
//  * TJITLLVMObjectCache(const TROMImage&)
// -------------------------------------------------------------------------- //
TJITLLVMObjectCache::TJITLLVMObjectCache(const TROMImage& inROMImage)
    :
        mCacheDir(GetCacheDir(inROMImage))
{
	InvalidateCacheIfRequired(inROMImage);
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
//  * notifyObjectCompiled(const Module*, const MemoryBuffer*)
// -------------------------------------------------------------------------- //
void
TJITLLVMObjectCache::notifyObjectCompiled(const Module* inModule, const MemoryBuffer* objectCode) {
	const std::string& moduleName = inModule->getModuleIdentifier();
	if (!moduleName.empty()) {
		std::string::size_type n = moduleName.find("_");
		std::string pagePrefix = moduleName.substr(0, n);
		SmallString<128> path(mCacheDir);
		sys::path::append(path, pagePrefix);
		(void) ::mkdir(path.c_str(), 0755);
		sys::path::append(path, moduleName + ".o");
		std::string err;
		raw_fd_ostream stream(path.c_str(), err, sys::fs::F_Excl);
		if (err != "" || stream.has_error()) {
			fprintf(stderr, "Could not open object file for writing: %s\n", err.c_str());
		} else {
			stream << objectCode->getBuffer();
		}
		stream.close();
	}
}

// -------------------------------------------------------------------------- //
//  * getObject(const Module*)
// -------------------------------------------------------------------------- //
MemoryBuffer*
TJITLLVMObjectCache::getObject(const llvm::Module* inModule) {
    return nullptr;
}

// -------------------------------------------------------------------------- //
//  * LoadPageFunctions(llvm::ExecutionEngine&, const TJITLLVMPage&)
// -------------------------------------------------------------------------- //
std::map<KUInt32, JITFuncPtr>
TJITLLVMObjectCache::LoadPageFunctions(llvm::ExecutionEngine& engine, const TJITLLVMPage& page) {
	std::map<KUInt32, JITFuncPtr> result;
	SmallString<112> path(mCacheDir);
	sys::path::append(path, page.PagePrefix());
	DIR* dir = ::opendir(path.c_str());
	while (dir != NULL) {
		struct dirent theEntry;
		struct dirent* theEntryPtr = nullptr;
		int r = readdir_r(dir, &theEntry, &theEntryPtr);
		if (r || theEntryPtr == nullptr) break;
		if (theEntry.d_name[theEntry.d_namlen - 1] == 'o'
			&& theEntry.d_name[theEntry.d_namlen - 2] == '.'
			&& theEntry.d_type == DT_REG) {
			SmallString<128> objPath(path);
			sys::path::append(objPath, std::string(theEntry.d_name, theEntry.d_namlen));
			ErrorOr<object::ObjectFile*> objFile = object::ObjectFile::createObjectFile(objPath.c_str());
			if (!objFile) {
				fprintf(stderr, "Could not create object file %s (%s)\n", objPath.c_str(), objFile.getError().message().c_str());
				abort();
			}
			engine.addObjectFile(std::move(std::unique_ptr<object::ObjectFile>(objFile.get())));
			// Object file hasn't been deleted yet...
			for (auto it = objFile.get()->symbol_begin_impl(); it != objFile.get()->symbol_end_impl(); ++it) {
				uint32_t flags = it->getFlags();
				if ((flags & (object::SymbolRef::SF_Undefined | object::SymbolRef::SF_Global)) == object::SymbolRef::SF_Global) {
					std::string buffer;
					raw_string_ostream stream(buffer);
					it->printName(stream);
					std::string name = stream.str();
					if (name[0] == '_') {
						name = name.substr(1);
					}
					JITFuncPtr ptr = (JITFuncPtr) engine.getFunctionAddress(name);
					KUInt32 functionOffset = page.OffsetFromFunctionName(name);
					result[functionOffset] = ptr;
				}
			}
		}
	}
	if (dir) {
		::closedir(dir);
	}
	return result;
}

#endif