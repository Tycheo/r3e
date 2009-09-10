#ifndef ENTITY_H
#define ENTITY_H

#include "BoundingBox.hpp"
#include "Matrix4.hpp"
#include "Frustum.hpp"

enum EntityType {
	ENTITY_COLOUR,
	ENTITY_COLOUR_MESH,
	ENTITY_1TEX_SPRITE,
	ENTITY_1TEX_MESH,
	ENTITY_2TEX_MESH,
	ENTITY_3TEX_TERRAIN,
	MAX_ENTITY,
};

class Entity {
public:
	Entity() : mVisible(true), mType(MAX_ENTITY) {}
	virtual ~Entity(){}

	virtual int UpdateCulling(const Frustum& camf){
		int ret = mBoundingBox.CheckIntersectFrustum(camf);
		mVisible = (ret == 0);
		return ret;
	}

	virtual void Render(){}

public:
	int mType;
	bool mVisible;
	Matrix4 mTransform;
	BoundingBox mBoundingBox;
};

#endif