#ifndef ARRAY_H
#define ARRAY_H

#include <string.h>
#include "SafeDelete.hpp"

template <class T, int INC_ARRAY_SIZE = 1> class Array {
public:
	static const unsigned int NOT_FOUND = 0xFFFFFFFF;

	Array() : mData(0), mCount(0), mMaxSize(0) {}
	~Array(){
		clear();
	}

	void clear(){
		SAFE_DELETE_ARRAY(mData);
		mCount = 0;
		mMaxSize = 0;
	}

	void erase(int idx){
		if(!mData) return;
		if(mCount == 1) return clear();
		if(idx == NOT_FOUND) return;
		if(idx >= mCount) return;

		int sizeofRemainingMemory = sizeof(T) * (mMaxSize - idx);
		int sizeofCopyMemory = sizeofRemainingMemory - 4;

		memcpy_s(&mData[idx], sizeofRemainingMemory, &mData[idx + 1], sizeofCopyMemory);
		--mCount;
		mData[mCount] = 0;
	}

	void push_front(T value){
		if(!mData){
			mData = new T[INC_ARRAY_SIZE];
		}else{
			if(mCount < mMaxSize){
				memcpy_s(mData + sizeof(T), sizeof(T) * mMaxSize, mData, sizeof(T) * mCount);
			}else{
				mMaxSize = mCount + INC_ARRAY_SIZE;
				T* newData = new T[mMaxSize];
				memcpy_s(newData + sizeof(T), sizeof(T) * mMaxSize, mData, sizeof(T) * mCount);
				mData = newData;
			}
		}

		++mCount;
		mData[0] = value;
	}

	void push_back(T value){
		if(!mData)
			mData = new T[INC_ARRAY_SIZE];

		if(mCount == mMaxSize){
			resize(mCount + INC_ARRAY_SIZE);
		}

		mData[mCount] = value;
		++mCount;
	}

	void setCount(int count){
		resize(count);
		mCount = count;
	}

	void resize(int size){
		if(!mData){
			mData = new T[size];
			memset(mData, 0, sizeof(T) * size);
			mMaxSize = size;
			return;
		}

		if(!size){
			clear();
			return;
		}

		if(size == mMaxSize) return;

		if(size > mMaxSize){
			T* newData = new T[size];
			memcpy_s(newData, sizeof(T) * size, mData, sizeof(T) * mCount);
			memset(newData + (sizeof(T) * mCount), 0, (mMaxSize - mCount) * sizeof(T));

			mData = newData;
			mMaxSize = size;
		}else{
			memset(mData + (sizeof(T) * mCount), 0, mCount - size);
			mCount = size;
			mMaxSize = size;
		}
	}

	T& operator[] (unsigned int idx) const {
		return mData[idx];
	}

	unsigned int find(T value) const {
		for(unsigned int i = 0; i < size(); ++i){
			if(mData[i] == value) return i;
		}

		return NOT_FOUND;
	}

	unsigned int size() const {
		return mCount;
	}

	unsigned int maxSize() const {
		return mMaxSize;
	}

	void delete_values(bool isArray = false){
		if(!mData) return;

		for(int i = 0; i < mCount; ++i){
			if(isArray){
				SAFE_DELETE_ARRAY(mData[i]);
			}else{
				SAFE_DELETE(mData[i]);
			}
		}

		clear();
	}

	T* GetData() const {
		return mData;
	}

private:
	T* mData;
	int mCount;
	int mMaxSize;
};

#endif