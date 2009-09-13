#ifndef FILE_H
#define FILE_H

#include "String.hpp"

class File {
public:
	File(){}
	virtual ~File(){}

	virtual bool Open(const char* path, const char* method) = 0;
	virtual void Close() = 0;

	virtual int ReadData(void* data, int size) = 0;
	virtual int WriteData(void* data, int size) = 0;

	virtual bool IsOpen() = 0;

	virtual long Position() = 0;
	virtual long Size() = 0;

	virtual void Skip(int bytes) = 0;
	virtual void Seek(int position) = 0;

	template <class T> T Read(){
		T val;
		ReadData(&val, sizeof(T));
		return val;
	}

	template <class T> void Read(T& val){
		ReadData(&val, sizeof(T));
	}

	template <class T> void Write(const T& val){
		WriteData(&val, sizeof(T));
	}

	String ReadString(unsigned int length){
		char* str = new char[length + 1];
		ReadData(str, length);
		str[length] = 0;
		return String(str, true);
	}

	unsigned char* ReadBytes(int length){
		unsigned char* buffer = new unsigned char[length];
		if(ReadData(buffer, length))
			return buffer;

		delete [] buffer;
		return NULL;
	}

	template <class T> String ReadStringLength(){
		T length;
		ReadData(&length, sizeof(T));
		char* str = new char[length + 1];
		ReadData(str, length);
		str[length] = 0;
		return String(str, true);
	}

	String ReadTerminatedString(char terminator = 0){
		char buffer[255];
		for(int i = 0; i <= 255; ++i){
			if(i == 255) return String();

			if(ReadData(&buffer[i], 1) != 1){
				if(i == 0){
					return String();
				}else{
					buffer[i] = 0;
					break;
				}
			}

			if(buffer[i] == terminator) break;
		}

		return String(buffer);
	}

	String ReadLine(){
		char buffer[255];
		for(int i = 0; i <= 255; ++i){
			if(i == 255) return String();

			if(ReadData(&buffer[i], 1) != 1){
				if(i == 0){
					return String();
				}else{
					buffer[i] = 0;
					break;
				}
			}

			if(buffer[i] == 0xA || buffer[i] == 0xD){
				buffer[i] = 0;
				break;
			}
		}

		char tmp;
		int pos = Position();
		ReadData(&tmp, 1);
		if(tmp != 0xA && tmp != 0xD) Seek(pos);

		return String(buffer);
	}

	String ReadPascalString(){
		unsigned int length;
		unsigned char curBLen;
		ReadData(&curBLen, 1);
		length = curBLen & 0x7F;
		if(curBLen & 0x80){
			ReadData(&curBLen, 1);
			length |= (curBLen & 0x7F) << 7;
		}

		char* str = new char[length + 1];
		ReadData(str, length);
		str[length] = 0;

		return String(str, true);
	}

	void WriteTerminatedString(const String& str, char terminator = 0){
		WriteData((void*)(const char*)str, str.Length());
		WriteData(&terminator, 1);
	}

	template <class T> void WriteStringLength(const char* str){
		T length;
		length = strlen(str);
		WriteData(&length, sizeof(T));
		WriteData((void*)(const char*)str, length);
	}

	void WriteLine(const char* str){
		static const char* endl = "\r\n";

		WriteData((void*)str, strlen(str));
		WriteData((void*)endl, 2);
	}
};

#endif