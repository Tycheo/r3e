#ifndef FLAT_FILE_SYSTEM_H
#define FLAT_FILE_SYSTEM_H

#include "String.hpp"
#include "FileSystem.hpp"

#include "FlatFile.hpp"

class FlatFileSystem {
public:
	FlatFileSystem(){}
	virtual ~FlatFileSystem(){}

	virtual File* OpenFile(const char* path, const char* mode){
		String realpath = path;
		GetFullPath(realpath);

		FlatFile* file = new FlatFile();
		if(!file->Open(path, mode)){
			delete file;
			return NULL;
		}

		return file;
	}

	virtual bool FileExists(const char* path){
		File* fh = OpenFile(path, "rb");
		if(!fh) return false;
		delete fh;
		return true;
	}

	virtual long FileSize(const char* path){
		File* fh = OpenFile(path, "rb");
		if(!fh) return 0;
		long size = fh->Size();
		delete fh;
		return size;
	}

	virtual void GetFullPath(String& path){
		path = mBaseDirectory + path;
	}

	void SetBaseDirectory(const char* dir){
		mBaseDirectory = dir;
		mBaseDirectory.ReplaceAll("/", "\\");
		char end = mBaseDirectory.At(mBaseDirectory.Length() - 1);
		if(end == '\\') return;
		mBaseDirectory += "\\";
	}

	const char* GetBaseDirectory(){
		return mBaseDirectory;
	}

private:
	String mBaseDirectory;
};

#endif