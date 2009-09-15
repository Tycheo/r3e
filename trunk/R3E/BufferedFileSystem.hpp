#ifndef BUFFERED_FILE_SYSTEM_H
#define BUFFERED_FILE_SYSTEM_H

#include "String.hpp"
#include "FileSystem.hpp"

#include "BufferedFile.hpp"

class BufferedFileSystem {
public:
	BufferedFileSystem(){}
	BufferedFileSystem(const char* baseDir) : mBaseDirectory(baseDir) {
		mBaseDirectory.ReplaceAll("\"", "");
	}
	virtual ~BufferedFileSystem(){}

	virtual File* OpenFile(const char* path, const char* mode, bool dataFile = true){
		String modeStr = mode;
		if(modeStr.Find("w") != 0xFFFFFFFF) return NULL;

		String realpath = path;
		if(dataFile) GetFullPath(realpath);

		BufferedFile* file = new BufferedFile();
		if(!file->Open(realpath, mode)){
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