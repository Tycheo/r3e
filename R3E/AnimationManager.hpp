#ifndef ANIMMAN_H
#define ANIMMAN_H

#include "ZMO.hpp"
#include "FastStringMapL2.hpp"

namespace OMGHAXLOL {
	struct Animation {
		Animation(const char* path) : mPath(path), mAnimation(0), mRefCount(0) {}
		~Animation(){
			SAFE_DELETE(mAnimation);
		}

		void AddReference(){
			++mRefCount;
		}

		void RemoveReference(){
			if(mRefCount == 0) return;
			--mRefCount;
			if(!mRefCount){
				SAFE_DELETE(mAnimation);
			}
		}

		ROSE::ZMO* Get(){
			if(!mAnimation) LoadAnimation();
			return mAnimation;
		}

	private:
		void LoadAnimation(){
			mAnimation = new ROSE::ZMO();
			mAnimation->Open(mPath);
		}

	protected:
		String mPath;
		ROSE::ZMO* mAnimation;
		int mRefCount;

		friend class AnimationManager;
	};
};

class SmartAnimation {
public:
	SmartAnimation()
		: mAnimation(0)
	{
	}

	SmartAnimation(OMGHAXLOL::Animation* Animation)
		: mAnimation(Animation)
	{
		if(!mAnimation) return;
		mAnimation->AddReference();
	}

	~SmartAnimation(){
		if(!mAnimation) return;
		mAnimation->RemoveReference();
	}

	ROSE::ZMO* Get() const {
		if(!mAnimation) return 0;
		return mAnimation->Get();
	}

	SmartAnimation& operator=(const SmartAnimation& rhs){
		if(mAnimation) mAnimation->RemoveReference();
		mAnimation = rhs.mAnimation;
		mAnimation->AddReference();
		return (*this);
	}

private:
	OMGHAXLOL::Animation* mAnimation;
};

class AnimationManager {
protected:
	friend class SmartAnimation;
	static AnimationManager mInstance;

public:
	AnimationManager(){}
	~AnimationManager(){
		mLoadedAnimations.delete_values();
	}

	static AnimationManager& Instance(){
		return mInstance;
	}

	SmartAnimation LoadAnimation(const char* path){
		FastStringMapL2<OMGHAXLOL::Animation*>::Entry* entry = mLoadedAnimations.find(path);
		OMGHAXLOL::Animation* animation;
		if(!entry){
			animation = new OMGHAXLOL::Animation(path);
			mLoadedAnimations.add(path, animation);
		}else{
			animation = entry->GetValue();
		}

		return SmartAnimation(animation);
	}

private:
	FastStringMapL2<OMGHAXLOL::Animation*> mLoadedAnimations;
};

#endif
