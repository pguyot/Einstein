// ==============================
// File:			TJITLLVMObjectCache.h
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

#ifndef TJITLLVMOBJECTCACHE_H
#define TJITLLVMOBJECTCACHE_H

// llvm
#include <llvm/ExecutionEngine/ObjectCache.h>
#include <llvm/ADT/SmallString.h>

// C++
#include <cstdint>
#include <unordered_map>

class TROMImage;
class TJITLLVMPage;
class TJITLLVMRecordingMemoryManager;

///
/// LLVM object cache.
/// Used through translation to avoid translating functions we already have.
///
class TJITLLVMObjectCache : public llvm::ObjectCache {
public:
    ///
    /// Constructor from a ROM Image.
	/// Cached objects older than ROM image are deleted.
	/// Memory manager is owned by this object (through dynamic linker).
    ///
    TJITLLVMObjectCache(const TROMImage& inROMImage, TJITLLVMRecordingMemoryManager* inMemoryManager);

	///
	/// Load functions for a given page.
	/// Return the function pointers.
	///
	const std::map<KUInt32, JITFuncPtr>& LoadPageFunctions(llvm::SmallVector<llvm::ObjectImage *, 2>& ioLoadedObjects, const TJITLLVMPage& inPage);
	
	///
	/// ObjectCache interface: write machine code to disk.
	/// Determine the page name from the module name and add the object to the corresponding
	/// directory.
	///
	void notifyObjectCompiled(const llvm::Module *M, const llvm::MemoryBuffer *Obj) override;
	
	///
	/// ObjectCache interface: read machine code from disk.
	/// This function always return nullptr (objects are loaded with
	/// LoadPageFunctions).
	///
	llvm::MemoryBuffer* getObject(const llvm::Module* M) override;

private:
	///
	/// Get the cache directory from the ROM image.
	///
	static llvm::SmallString<128> GetCacheDir(const TROMImage& inROMImage);

	///
	/// Invalidate the cache if required. Delete all objects.
	///
	void InvalidateCacheIfRequired(const TROMImage& inROMImage);

    /// We never unload functions and therefore keep a cache of all
	/// loaded object files.
	std::unordered_map<uint64_t, std::vector<std::pair<KUInt32, JITFuncPtr>>> mLoadedObjectFiles;
	/// readdir is expensive, so we also keep a cache of all directories.
	std::unordered_map<std::string, std::map<KUInt32, JITFuncPtr>>	mLoadedPages;
	llvm::RuntimeDyld								mDynamicLinker;
	const llvm::SmallString<128>                    mCacheDir;
	TJITLLVMRecordingMemoryManager*					mMemoryManager;
};

#endif
