#ifndef PLAYER_H
#define PLAYER_H

#include "EntityGroup.hpp"
#include "SkeletalEntity.hpp"
#include "SafeDelete.hpp"
#include "ROSEData.hpp"

class Player : public SkeletalEntity {
public:
	Player(){
		mType = ENTITY_1TEX_MESH_SKINNED;
		mGender = 0;
		mItemEntities.setCount(ROSE::IT_MAX);
	}

	virtual ~Player(){}

	bool SetItem(int type, int id){
		if(type >= ROSE::IT_MAX) return false;
		if(type <= 0) return false;

		Entity* entity = ROSE::Data::mModelLists[mGender][type]->LoadModel(id);
		if(!entity) return false;

		if(mItemEntities[type]){
			RemoveChild(mItemEntities[type]);
			delete mItemEntities[type];
		}

		AddChild(entity);
		mItemEntities[type] = entity;

		if(type == ROSE::IT_FACE || type == ROSE::IT_HAIR)
			BindEntityToBone(entity, "b1_head");
		else if(type == ROSE::IT_WEAPON)
			BindEntityToBone(entity, "p_00");
		else if(type == ROSE::IT_SUBWPN)
			BindEntityToBone(entity, "p_02");
		else if(type == ROSE::IT_BACK)
			BindEntityToBone(entity, "p_03");

		return true;
	}

private:
	Array<Entity*> mItemEntities;
	int mGender;
};

#endif