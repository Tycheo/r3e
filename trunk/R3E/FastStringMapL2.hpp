#ifndef FAST_STRING_MAP_L1_H
#define FAST_STRING_MAP_L1_H

#include "SafeDelete.hpp"
#include "String.hpp"
#include "Hash.hpp"

#ifndef ENTRY_LIST_STEP_SIZE
# define ENTRY_LIST_STEP_SIZE 5
#endif

template<class T> class FastStringMapL2 {
public:
	class Entry {
	public:
		Entry(){}
		Entry(Hash hash, T value) : mHash(hash), mValue(value) {}

		Hash mHash;
		T mValue;

		operator T() const {
			return mValue;
		}
	};

private:
	class List {
	public:
		List() : mEntries(0), mCount(0), mArraySize(0) {}
		~List(){
			if(!mEntries) return;
			for(unsigned int i = 0; i < mCount; ++i)
				delete mEntries[i];

			delete [] mEntries;
		}

		void AddEntry(Entry* entry){
			CheckArraySize();
			mEntries[mCount] = entry;
			++mCount;
		}

		void CheckArraySize(){
			if(mCount >= mArraySize)
				mArraySize += ENTRY_LIST_STEP_SIZE;
			else return;

			Entry** oldArray = mEntries;
			mEntries = new Entry*[mArraySize];
			memcpy_s(mEntries, sizeof(Entry*) * mArraySize, oldArray, sizeof(Entry*) * mCount);
			memset(mEntries + mCount, 0, sizeof(Entry*) * ENTRY_LIST_STEP_SIZE);
		}

		Entry* GetEntry(Hash hash) const {
			for(unsigned int i = 0; i < mCount; ++i){
				if(mEntries[i]->mHash.mHash == hash.mHash) return mEntries[i];
			}
			return NULL;
		}

		bool RemoveEntry(Hash hash){
			for(unsigned int i = 0; i < mCount; ++i){
				if(mEntries[i]->mHash.mHash != hash.mHash) continue;
				delete mEntries[i];
				mEntries[i] = 0;
				return true;
			}

			return false;
		}

		void delete_values(){
			for(unsigned int i = 0; i < mCount; ++i){
				SAFE_DELETE(mEntries[i]->mValue);
			}
		}

		unsigned int mArraySize;
		unsigned int mCount;
		Entry** mEntries;
	};

	class Node {
	public:
		List mChildren[256];
	};
	
public:
	FastStringMapL2(){}
	~FastStringMapL2(){}

	Entry* add(const char* keyV, T value){
		String key;
		CleanString(keyV, key);
		Hash hash = Hash::Generate(key);
		if(GetEntry(hash)) return NULL;
		Entry* entry = new Entry(hash, value);
		AddEntry(entry);
		return entry;
	}

	T& operator[](const char* keyV){
		Entry* entry = find(key);
		if(!entry)
			entry = add(key, T());

		return entry->mValue;
	}

	Entry* find(const char* keyV) const {
		String key;
		CleanString(keyV, key);
		Hash hash = Hash::Generate(key);
		return GetEntry(hash);
	}

	void delete_values(){
		for(int x = 0; x < 256; ++x){
			for(int y = 0; y < 256; ++y){
				mKeyNodes[x].mChildren[y].delete_values();
			}
		}
	}

private:
	Entry* GetEntry(Hash hash) const {
		return mKeyNodes[hash.mBranch1].mChildren[hash.mBranch2].GetEntry(hash);
	}

	void AddEntry(Entry* entry){
		mKeyNodes[entry->mHash.mBranch1].mChildren[entry->mHash.mBranch2].AddEntry(entry);
	}

	bool RemoveEntry(Hash hash){
		return mKeyNodes[hash.mBranch1].mChildren[hash.mBranch2].RemoveEntry(hash);
	}

	void CleanString(const char* in, String& out) const {
		out = in;
		char* pch = const_cast<char*>((const char*)out);
		int pos = 0;
		while(*pch){
			char cChar = *pch;
			if(cChar == '/') *pch = '\\';
			if(cChar == '\\' && *(pch + 1) == '\\') memcpy_s(pch, 255 - pos, pch + 1, 254 - pos);
			if(cChar >= 'a' && cChar <= 'z') *pch = cChar - 32;
			++pch;
			++pos;
		}
	}

	Node mKeyNodes[256];
};

#endif