#ifndef PLAYER_H
#define PLAYER_H

#include "SkeletalEntity.hpp"
#include "ROSEData.hpp"

class Player : public SkeletalEntity {
public:
	Player(){
		mType = ENTITY_1TEX_MESH_SKINNED;
		mGender = 0;
		mItemEntities.setCount(ROSE::IT_MAX);
	}

	virtual ~Player(){}

	void SetGender(int gender){
		if(gender == ROSE::G_FEMALE){
			SetSkeleton("3DDATA\\AVATAR\\FEMALE.ZMD");
			mGender = ROSE::G_FEMALE;
		}else if(gender == ROSE::G_MALE){
			SetSkeleton("3DDATA\\AVATAR\\MALE.ZMD");
			mGender = ROSE::G_MALE;
		}
	}

	bool SetItem(int type, int id){
		if(type >= ROSE::IT_MAX) return false;
		if(type <= 0) return false;

		Entity* entity = ROSE::Data::mModelLists[mGender][type]->LoadModel(id, this);
		if(!entity) return false;

		if(mItemEntities[type]){
			RemoveChild(mItemEntities[type]);
			delete mItemEntities[type];
		}

		AddChild(entity);
		mItemEntities[type] = entity;

		return true;
	}

private:
	Array<Entity*> mItemEntities;
	int mGender;
};

#endif