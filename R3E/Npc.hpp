#ifndef NPC_H
#define NPC_H

#include "SkeletalEntity.hpp"

class Npc : public SkeletalEntity {
public:
	Npc(){
		mType = ENTITY_1TEX_MESH_SKINNED;
	}

	virtual ~Npc(){}

private:
};

#endif