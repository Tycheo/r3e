#ifndef EX_STR_CLASS
#define EX_STR_CLASS

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "List.hpp"

class String {
public:
	String() : mData(0) {}

	String(char* source){
		mData = _strdup(source);
	}

	String(const char* source){
		mData = _strdup(source);
	}

	String(const String& source){
		mData = _strdup(source.mData);
	}

	String(char* source, bool noCopy){
		if(noCopy) mData = source;
		else mData = _strdup(source);
	}

public:
	~String(){
		if(mData)
			delete [] mData;
		mData = 0;
	}

	const String& Trim(){
		if(!IsValid()) return (*this);
		const char toTrim[] = { 0x09, 0x0a, 0x0b, 0x0d, 0x20 };
		unsigned int len = Length();
		unsigned int start = 0xFFFFFFFF;
		unsigned int end = 0;

		for(unsigned int i = 0; i < len; ++i){
			for(unsigned int j = 0; j < sizeof(toTrim); ++j){
				if(mData[i] != toTrim[j]) continue;
				start = i;
				break;
			}
			if(start != i) break;
		}
		if(start == 0xFFFFFFFF) start = 0;
		else ++start;

		end = len;
		for(unsigned int i = len; i > 0; --i){
			for(unsigned int j = 0; j < sizeof(toTrim); ++j){
				if(mData[i] != toTrim[j]) continue;
				end = i;
				break;
			}
			if(end != i) break;
		}

		if(start != 0 || end != len){
			unsigned int buflen = 1 + end - start;
			char* buffer = new char[buflen];
			buffer[buflen - 1] = 0;

			memcpy_s(buffer, buflen, mData + start, end - start);

			delete [] mData;
			mData = buffer;
		}

		return (*this);
	}

	static String Create(const char* format, ...){
		char buffer[256];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, 256, format, args);
		va_end(args);
		return String(buffer);
	}

	const String& Format(const char* format, ...){
		char buffer[256];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, 256, format, args);
		if(mData)
			delete [] mData;
		mData = _strdup(buffer);
		va_end(args);
		return (*this);
	}

	const String& AddFormat(const char* format, ...){
		char buffer[256];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, 256, format, args);
		(*this) += buffer;
		va_end(args);
		return (*this);
	}

	const String& Erase(unsigned int pos, unsigned int length = 1){
		if(!IsValid()) return (*this);
		unsigned int len = Length() - length + 1;
		char* buffer = new char[len];
		memcpy_s(buffer, len, mData, pos);
		memcpy_s(buffer + pos, len - pos, mData + pos + length, Length() - pos - length);
		buffer[len - 1] = 0;
		delete [] mData;
		mData = buffer;

		return (*this);
	}

	String SubStr(unsigned int pos, unsigned int length) const {
		char* buffer = new char[length + 1];
		buffer[length] = 0;
		memcpy_s(buffer, length, mData + pos, length);
		return String(buffer, true);
	}

	const String& ToUpper(){
		unsigned int len = Length();
		for(unsigned int i = 0; i < len; ++i){
			if(mData[i] >= 'a' && mData[i] <= 'z')
				mData[i] -= 32;
		}
		return (*this);
	}

	const String& ToLower(){
		unsigned int len = Length();
		for(unsigned int i = 0; i < len; ++i){
			if(mData[i] >= 'A' && mData[i] <= 'Z')
				mData[i] += 32;
		}
		return (*this);
	}

	const String& NullAt(unsigned int pos){
		if(!IsValid()) return (*this);

		if(pos < Length())
			mData[pos] = 0;

		return (*this);
	}

	const String& ReplaceAll(const String& find, const String& replace){
		if(!IsValid()) return (*this);
		unsigned int pos = Find(find);
		unsigned int length = find.Length();
		unsigned int rlength = replace.Length();
		while(pos != 0xFFFFFFFF){
			Replace(pos, length, replace);
			pos = Find(find, pos + rlength);
		}
		return (*this);
	}

	const String& Replace(unsigned int position, unsigned int length, const String& text){
		if(!IsValid()) return (*this);
		unsigned int size = Length();
		if(position >= size) return (*this);

		unsigned int sublen = text.Length();
		unsigned int newsize = size + sublen - length + 1;
		unsigned int sizeleft = newsize - 1;
		char* newData = new char[newsize];
		char* pch = newData;

		memcpy_s(pch, newsize, mData, position); sizeleft -= position; pch += position;
		memcpy_s(pch, sizeleft, text.mData, sublen); sizeleft -= sublen; pch += sublen;
		if(sizeleft >= 0){
			memcpy_s(pch, sizeleft, mData + position + length, sizeleft);
			pch += sizeleft;
		}
		*pch = 0;

		delete [] mData;
		mData = newData;
		return (*this);
	}

private:
	char Hex2Char(char *what){
		register char digit;

		digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
		digit *= 16;
		digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
		return digit;
	}

public:
	void UrlUnescape(){
		unsigned int idx, last = 0;
		char buf[2];
		buf[1] = 0;
		idx = Find("%", last);
		while(idx != 0xFFFFFFFF){
			buf[0] = Hex2Char(mData + idx + 1);
			Replace(idx, 3, buf);
			last = idx + 1;
			idx = Find("%", last);
		}
	}

	int Split(const String& terminator, List<String>& strList){
		unsigned int last = 0;
		unsigned int idx = Find(terminator);

		while(idx != 0xFFFFFFFF){
			String cur = SubStr(last, idx - last);
			strList.push_back(cur);
			last = idx+1;
			idx = Find(terminator, last);
		}

		String cur = SubStr(last, Length() - last);
		strList.push_back(cur);

		return strList.size();
	}

	unsigned int Find(const String& substr, unsigned int start = 0) const {
		if(!IsValid()) return 0xFFFFFFFF;
		unsigned int length = Length();
		unsigned int sublen = substr.Length();
		for(unsigned int i = start; i < length; ++i){
			if(mData[i] != substr.mData[0]) continue;
			for(unsigned int j = 1; j <= sublen; ++j){
				if(j == sublen) return i;
				if(mData[i+j] != substr.mData[j]) break;
			}
		}

		return 0xFFFFFFFF;
	}

	unsigned int Length() const {
		return (mData)?strlen(mData):0;
	}

	bool IsValid() const {
		return mData != 0;
	}

	char* CopyData() const {
		if(!mData) return NULL;
		return _strdup(mData);
	}

	char At(unsigned int pos) const {
		return mData[pos];
	}

	String& operator=(const String& rhs){
		if(this == &rhs)
			return *this;

		if(mData)
			delete [] mData;

		if(!rhs.mData)
			mData = 0;
		else
			mData = _strdup(rhs.mData);

		return *this;
	}

	String& operator+=(const String& rhs){
		return ((*this) += rhs.mData);
	}

	String& operator+=(const char* rhsmData){
		if(!rhsmData) return *this;
		if(!mData){
			mData = _strdup(rhsmData);
			return *this;
		}

		int len1 = strlen(mData);
		int len2 = strlen(rhsmData);
		int total = len1 + len2 + 1;
		char* newData = new char[total];
		memcpy_s(newData, total, mData, len1);
		memcpy_s(newData + len1, len2 + 1, rhsmData, len2);
		newData[total - 1] = 0;
		delete [] mData;
		mData = newData;

		return *this;
	}

	String& operator+=(unsigned int val){
		char tmp[16];
		sprintf_s(tmp, 16, "%08X", val);
		String blah(tmp);
		return ((*this) += blah);
	}

	const String operator+(const String& other) const {
		String result = (*this);
		result += other;
		return result;
	}

	bool operator==(const String& other) const {
		return (strcmp(mData, other.mData) == 0);
	}

	bool operator!=(const String& other) const {
		return !(*this == other);
	}

	operator const char*() const {
		return mData;
	}

	unsigned int ToUInt() const {
		return strtoul(mData, 0, 10);
	}

	int ToInt() const {
		return atoi(mData);
	}

	wchar_t* ToUnicode() const {
		int len = strlen(mData) + 1;
		wchar_t* unicode = new wchar_t[len];
		mbstowcs_s(NULL, unicode, len, mData, len - 1);
		return unicode;
	}

private:
	char* mData;
};

#endif
