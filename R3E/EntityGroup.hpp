#ifndef ENTITY_GROUP_H
#define ENTITY_GROUP_H

#include "Entity.hpp"
#include "Array.hpp"

class EntityGroup : public Entity {
public:
	EntityGroup(){}
	virtual ~EntityGroup(){
		mChildren.delete_values();
	}

	virtual int UpdateCulling(const Frustum& camf){
		int ret = mBoundingBoxTransformed.CheckIntersectFrustum(camf);
		if(ret == 2){
			for(unsigned int i = 0; i < mChildren.size(); ++i)
				mChildren[i]->mVisible = true;

			mVisible = true;
			return 2;
		}else if(ret == 0){
			for(unsigned int i = 0; i < mChildren.size(); ++i)
				mChildren[i]->mVisible = false;

			mVisible = false;
			return 0;
		}

		mVisible = true;
		for(unsigned int i = 0; i < mChildren.size(); ++i)
			mChildren[i]->UpdateCulling(camf);

		return 1;
	}

	virtual void Render(){
		glPushMatrix();
		mTransform.glMult();
		for(unsigned int i = 0; i < mChildren.size(); ++i)
			mChildren[i]->Render();
		glPopMatrix();
	}

	virtual void AddChild(Entity* child){
		mChildren.push_back(child);
		mType = child->mType;
		UpdateBoundingBox();
	}

	virtual void RemoveChild(Entity* child){
		mChildren.erase(mChildren.find(child));
		UpdateBoundingBox();
	}

	void SetGroupType(int type){
		mType = type;
	}

	virtual void UpdateBoundingBox(){
		mBoundingBox.Reset();

		for(unsigned int i = 0; i < mChildren.size(); ++i){
			mChildren[i]->UpdateBoundingBox();
			mBoundingBox.AddBox(mChildren[i]->mBoundingBoxTransformed);
		}

		mBoundingBoxTransformed.mMin = mTransform.TransformCoord(mBoundingBox.mMin);
		mBoundingBoxTransformed.mMax = mTransform.TransformCoord(mBoundingBox.mMax);
	}

public:
	Array<Entity*> mChildren;
};


#endif