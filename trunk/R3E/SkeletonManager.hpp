#ifndef SKELMAN_H
#define SKELMAN_H

#include "ZMD.hpp"
#include "FastStringMapL2.hpp"

namespace OMGHAXLOL {
	struct Skeleton {
		Skeleton(const char* path) : mPath(path), mSkeleton(0), mRefCount(0) {}
		~Skeleton(){
			SAFE_DELETE(mSkeleton);
		}

		void AddReference(){
			++mRefCount;
		}

		void RemoveReference(){
			if(mRefCount == 0) return;
			--mRefCount;
			if(!mRefCount){
				SAFE_DELETE(mSkeleton);
			}
		}

		ROSE::ZMD* Get(){
			if(!mSkeleton) LoadSkeleton();
			return mSkeleton;
		}

	private:
		void LoadSkeleton(){
			mSkeleton = new ROSE::ZMD();
			mSkeleton->Open(mPath);
		}

	protected:
		String mPath;
		ROSE::ZMD* mSkeleton;
		int mRefCount;

		friend class SkeletonManager;
	};
};

class SmartSkeleton {
public:
	SmartSkeleton()
		: mSkeleton(0)
	{
	}

	SmartSkeleton(OMGHAXLOL::Skeleton* Skeleton)
		: mSkeleton(Skeleton)
	{
		if(!mSkeleton) return;
		mSkeleton->AddReference();
	}

	~SmartSkeleton(){
		if(!mSkeleton) return;
		mSkeleton->RemoveReference();
	}

	ROSE::ZMD* Get() const {
		if(!mSkeleton) return 0;
		return mSkeleton->Get();
	}

	SmartSkeleton& operator=(const SmartSkeleton& rhs){
		if(mSkeleton) mSkeleton->RemoveReference();
		mSkeleton = rhs.mSkeleton;
		mSkeleton->AddReference();
		return (*this);
	}

private:
	OMGHAXLOL::Skeleton* mSkeleton;
};

class SkeletonManager {
protected:
	friend class SmartSkeleton;
	static SkeletonManager mInstance;

public:
	SkeletonManager(){}
	~SkeletonManager(){
		mLoadedSkeletons.delete_values();
	}

	static SkeletonManager& Instance(){
		return mInstance;
	}

	SmartSkeleton LoadSkeleton(const char* path){
		FastStringMapL2<OMGHAXLOL::Skeleton*>::Entry* entry = mLoadedSkeletons.find(path);
		OMGHAXLOL::Skeleton* skeleton;
		if(!entry){
			skeleton = new OMGHAXLOL::Skeleton(path);
			mLoadedSkeletons.add(path, skeleton);
		}else{
			skeleton = entry->GetValue();
		}

		return SmartSkeleton(skeleton);
	}

private:
	FastStringMapL2<OMGHAXLOL::Skeleton*> mLoadedSkeletons;
};

#endif
