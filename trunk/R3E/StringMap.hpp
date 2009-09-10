#ifndef STRING_MAP_H
#define STRING_MAP_H

#include <assert.h>
#include "String.hpp"
#include "Hash.hpp"

template <class T> class StringMap {
public:
	class Node {
	public:
		Node() : mNext(0) {}
		Node(Hash hash, const char* key, T value, Node* next) : mHash(hash), mKey(key), mValue(value), mNext(next) {}

		Hash mHash;
		String mKey;
		T mValue;
		Node* mNext;
	};

	class Iterator {
	public:
		Iterator(Node* node) : mNode(node) {}

		bool IsValid() const {
			return mNode != 0;
		}

		Iterator& operator++(){
			if(mNode)
				mNode = mNode->mNext;
			return (*this);
		}

		bool operator==(const Iterator& other){
			return other.mNode == mNode;
		}

		bool operator!=(const Iterator& other){
			return other.mNode != mNode;
		}

		T& operator*() const {
			return mNode->mValue;
		}

		const String& GetKey() const {
			return mNode->mKey;
		}

		unsigned int GetHash() const {
			return mNode->mHash;
		}

	protected:
		Node* GetNode(){
			return mNode;
		}

		friend class StringMap;

	private:
		Node* mNode;
	};

public:
	StringMap() : mFirst(0), mLast(0), mCount(0) {}

	~StringMap(){
		clear();
	}

	Iterator begin() const {
		if(!mFirst) return end();
		return Iterator(mFirst);
	}

	Iterator end() const {
		return Iterator(NULL);
	}

	Iterator add(const String& key, T value){
		if(find(key).IsValid()) return Iterator(NULL);
		Node* node = new Node(Hash::Generate(key), key, value, NULL);
		if(!mFirst) mFirst = node;
		else mLast->mNext = node;

		mLast = node;
		++mCount;

		return Iterator(node);
	}

	T& operator[](const String& key){
		Iterator itr = find(key);
		Node* node = itr.GetNode();
		if(!node)
			itr = add(key, T());
		return (*itr);
	}

	void erase(Iterator pos){
		Node* remNode = pos.GetNode();
		for(Iterator itr = begin(); itr != end(); ++itr){
			if(itr.GetNode()->mNext != remNode) continue;
			itr.GetNode()->mNext = remNode->mNext;
			--mCount;
			if(remNode == mLast)
				mLast = itr.GetNode();

			delete remNode;
			return;
		}
	}

	Iterator find(const String& key) const {
		Hash hash = Hash::Generate(key);
		Node* node = 0;
		for(Iterator itr = begin(); itr != end(); ++itr){
			if(itr.GetHash() != hash) continue;
			node = itr.GetNode();
			break;
		}
		return Iterator(node);
	}

	unsigned int size() const {
		return mCount;
	}

	void clear(){
		if(!mFirst) return;
		Node* cur = mFirst;
		Node* next = mFirst;
		while(next && cur){
			next = cur->mNext;
			delete cur;
			cur = next;
		}

		mCount = 0;
		mFirst = 0;
		mLast = 0;
	}

	void delete_values(bool isArray = false){
		if(!mFirst) return;
		Node* cur = mFirst;
		Node* next = mFirst;
		while(next && cur){
			next = cur->mNext;
			if(isArray) delete [] cur->mValue;
			else delete cur->mValue;
			delete cur;
			cur = next;
		}

		mCount = 0;
		mFirst = 0;
		mLast = 0;
	}

private:
	unsigned int mCount;
	Node* mFirst;
	Node* mLast;
};

#endif
