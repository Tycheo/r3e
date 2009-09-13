#ifndef SKELETAL_ENTITY_H
#define SKELETAL_ENTITY_H

#include "EntityGroup.hpp"
#include "SkeletonManager.hpp"
#include "AnimationManager.hpp"
#include "SkinShaderData.hpp"
#include <time.h>

class SkeletalEntity : public EntityGroup {
private:
	struct BoundEntity {
		Entity* mEntity;
		int mBoneID;
	};

public:
	SkeletalEntity()
		: mBoneMatrices(0), mBoneMatricesAbs(0), mCurFrame(0), mUpdateInterval(0)
	{
		mLastUpdate = clock();
		mType = ENTITY_1TEX_MESH_SKINNED;
	}

	virtual ~SkeletalEntity(){
		SAFE_DELETE_ARRAY(mBoneMatrices);
		SAFE_DELETE_ARRAY(mBoneMatricesAbs);
	}

	virtual void AddChild(Entity* child){
		mChildren.push_back(child);
		UpdateBoundingBox();

		mBindList.push_back(-1);
	}

	virtual void RemoveChild(Entity* child){
		int idx = mChildren.find(child);
		mChildren.erase(idx);
		mBindList.erase(idx);
		UpdateBoundingBox();
	}

	virtual void Render(){
		Update();

		GLint bmatLoc = SkinShaderData::mBMatLoc;
		GLint bindBoneLoc = SkinShaderData::mBindBoneLoc;

        for(unsigned int i = 0; i < GetBoneCount(); ++i){
            const float* mat = &mBoneMatrices[mCurFrame][i].m[0][0];
            GLint currentMatrix = bmatLoc + i;
			OpenGL::UniformMatrix4fv(currentMatrix, 1, GL_FALSE, mat);
        }

		glPushMatrix();
		mTransform.glMult();

		for(unsigned int i = 0; i < mChildren.size(); ++i){
			if(mBindList[i] != -1){
				OpenGL::Uniform1i(bindBoneLoc, 1);
				mChildren[i]->Render();
				OpenGL::Uniform1i(bindBoneLoc, 0);
			}else{
				mChildren[i]->Render();
			}
		}

		glPopMatrix();
	}

	void SetSkeleton(const char* path){
		mSkeleton = SkeletonManager::Instance().LoadSkeleton(path);
	}

	void SetAnimation(const char* path){
		mAnimation = AnimationManager::Instance().LoadAnimation(path);
		PreloadFrames();
	}

	void SetFrame(int frame){
		if(frame < 0) frame = 0;
		mCurFrame = frame;
		if(mCurFrame >= mAnimation.Get()->GetFrameCount()) mCurFrame = 0;
	}

	unsigned int GetBoneCount(){
		if(!mBoneMatrices) return 0;
		return mBoneMatrices[0].size();
	}

	void BindEntityToBone(Entity* entity, const char* bone){
		unsigned int boneID = mSkeleton.Get()->GetBoneID(bone);
		if(boneID == 0xFFFFFFFF) return;
		BindEntityToBone(entity, boneID);
	}

	void BindEntityToDummy(Entity* entity, int boneID){
		ROSE::ZMD* skeleton = mSkeleton.Get();
		if(!skeleton) return;
		const Array<ROSE::ZMD::Bone>& bindBoneList = skeleton->GetBoneList();
		for(unsigned int i = 0; i < bindBoneList.size(); ++i){
			ROSE::ZMD::Bone* bone = &bindBoneList[i];
			if(!bone->mDummy) continue;
			boneID += i;
			break;
		}

		BindEntityToBone(entity, boneID);
	}

	void BindEntityToBone(Entity* entity, int bone){
		unsigned int idx = mChildren.find(entity);
		if(idx != Array<Entity*>::NOT_FOUND){
			mBindList[idx] = bone;
			return;
		}

		BoundEntity bound;
		bound.mEntity = entity;
		bound.mBoneID = bone;
		mBoundEntities.push_back(bound);
	}


private:
	void Update(){
		if(clock() - mLastUpdate > mUpdateInterval){
			SetFrame(mCurFrame + 1);
			mLastUpdate = clock();
		}

		for(unsigned int i = 0; i < mBindList.size(); ++i){
			if(mBindList[i] == -1) continue;
			mChildren[i]->SetTransform(mBoneMatricesAbs[mCurFrame][mBindList[i]]);
		}

		for(unsigned int i = 0; i < mBoundEntities.size(); ++i){
			BoundEntity& bound = mBoundEntities[i];
			bound.mEntity->SetTransform(mBoneMatricesAbs[mCurFrame][bound.mBoneID]);
		}
	}
	
private:
	void PreloadFrames(){
		ROSE::ZMO* motion = mAnimation.Get();
		ROSE::ZMD* skeleton = mSkeleton.Get();

		if(!motion || !skeleton) return;

		SAFE_DELETE_ARRAY(mBoneMatrices);
		SAFE_DELETE_ARRAY(mBoneMatricesAbs);

		Array<Matrix4> frameMatrices;
		mUpdateInterval = 1000 / motion->GetFPS();

		mBoneMatrices = new Array<Matrix4>[motion->GetFrameCount()];
		mBoneMatricesAbs = new Array<Matrix4>[motion->GetFrameCount()];

		for(unsigned int f = 0; f < motion->GetFrameCount(); ++f){
			const Array<ROSE::ZMD::Bone>& bindBoneList = skeleton->GetBoneList();

			mBoneMatrices[f].setCount(skeleton->GetBoneCount());
			mBoneMatricesAbs[f].setCount(skeleton->GetBoneCount());
			motion->CreateBoneMatrices(f, bindBoneList, frameMatrices);

			for(unsigned int i = 0; i < bindBoneList.size(); ++i){
				ROSE::ZMD::Bone* bone = &bindBoneList[i];
				mBoneMatrices[f][i] = Matrix4::CreateInverse(bone->mTransform) * frameMatrices[i];
				mBoneMatricesAbs[f][i] = frameMatrices[i];
			}
		}
	}
private:
	unsigned int mCurFrame;

	clock_t mLastUpdate;
	clock_t mUpdateInterval;

	SmartSkeleton mSkeleton;
	SmartAnimation mAnimation;

	Array<Matrix4>* mBoneMatrices;
	Array<Matrix4>* mBoneMatricesAbs;
	
	Array<int> mBindList;

	Array<BoundEntity> mBoundEntities;
};

#endif