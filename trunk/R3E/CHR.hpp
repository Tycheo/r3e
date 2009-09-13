#ifndef ROSECHR_H
#define ROSECHR_H

#include "String.hpp"
#include "FileSystem.hpp"
#include "ZSC.hpp"

namespace ROSE {
	class CHR {
	public:
		struct Skeleton {
			String mPath;
		};

		struct Animation {
			String mPath;
		};

		struct Effect {
			String mPath;
		};

		struct ModelLink {
			short mModel;
		};

		struct AnimationLink {
			short mType;
			short mAnimation;
		};

		struct EffectLink {
			short mBoneIdx;
			short mEffect;
		};

		struct Character {
			unsigned char mActive;
			String mName;
			unsigned short mSkeleton;
			Array<ModelLink> mModelLinks;
			Array<AnimationLink> mAnimationLinks;
			Array<EffectLink> mEffectLinks;
		};

	public:
		CHR(){}
		CHR(const char* path){
			Open(path);
		}

		~CHR(){}

		bool Open(const char* path){
			ScopedPointer<File> fh(FILE_SYS()->OpenFile(path, "rb"));
			if(!fh) return false;

			unsigned short count, subcount;

			fh->Read(count);
			mSkeletonList.setCount(count);
			for(unsigned short i = 0; i < count; ++i)
				mSkeletonList[i].mPath = fh->ReadTerminatedString();

			fh->Read(count);
			mAnimationList.setCount(count);
			for(unsigned short i = 0; i < count; ++i)
				mAnimationList[i].mPath = fh->ReadTerminatedString();

			fh->Read(count);
			mEffectList.setCount(count);
			for(unsigned short i = 0; i < count; ++i)
				mEffectList[i].mPath = fh->ReadTerminatedString();

			fh->Read(count);
			mCharacterList.setCount(count);
			for(unsigned short i = 0; i < count; ++i){
				Character* character = &mCharacterList[i];
				fh->Read(character->mActive);
				if(!character->mActive) continue;
				
				fh->Read(character->mSkeleton);
				character->mName = fh->ReadTerminatedString();

				fh->Read(subcount);
				character->mModelLinks.setCount(subcount);
				for(unsigned short i = 0; i < subcount; ++i)
					fh->Read(character->mModelLinks[i].mModel);

				fh->Read(subcount);
				character->mAnimationLinks.setCount(subcount);
				for(unsigned short i = 0; i < subcount; ++i){
					fh->Read(character->mAnimationLinks[i].mType);
					fh->Read(character->mAnimationLinks[i].mAnimation);
				}

				fh->Read(subcount);
				character->mEffectLinks.setCount(subcount);
				for(unsigned short i = 0; i < subcount; ++i){
					fh->Read(character->mEffectLinks[i].mBoneIdx);
					fh->Read(character->mEffectLinks[i].mEffect);
				}
			}

			fh->Close();
			return true;
		}

		Skeleton* GetSkeleton(unsigned int id){
			if(id >= mSkeletonList.size()) return NULL;
			return &mSkeletonList[id];
		}

		Animation* GetAnimation(unsigned int id){
			if(id >= mAnimationList.size()) return NULL;
			return &mAnimationList[id];
		}

		Effect* GetEffect(unsigned int id){
			if(id >= mEffectList.size()) return NULL;
			return &mEffectList[id];
		}

		Character* GetCharacter(unsigned int id){
			if(id >= mCharacterList.size()) return NULL;
			return &mCharacterList[id];
		}

	public:
		Array<Skeleton> mSkeletonList;
		Array<Animation> mAnimationList;
		Array<Effect> mEffectList;
		Array<Character> mCharacterList;
	};
};

#endif
