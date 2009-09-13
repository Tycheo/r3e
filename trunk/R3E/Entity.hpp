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
	ENTITY_1TEX_MESH_SKINNED,
	ENTITY_2TEX_MESH,
	ENTITY_3TEX_TERRAIN,
	MAX_ENTITY,
};

class Entity {
public:
	Entity() : mVisible(true), mType(MAX_ENTITY) {}
	virtual ~Entity(){}

	virtual int UpdateCulling(const Frustum& camf){
		int ret = mBoundingBoxTransformed.CheckIntersectFrustum(camf);
		mVisible = (ret != 0);
		return ret;
	}

	virtual void Render(){}

	virtual void UpdateBoundingBox(){
		mBoundingBoxTransformed.Reset();
		mBoundingBoxTransformed.AddTransformedBox(mBoundingBox, mTransform);
	}

	void Transform(const Matrix4& mult){
		mTransform *= mult;
		UpdateBoundingBox();
	}

	void SetTransform(const Matrix4& mat){
		mTransform = mat;
		UpdateBoundingBox();
	}

	void SetBoundingBox(const BoundingBox& box){
		mBoundingBox = box;
		UpdateBoundingBox();
	}

public:
	bool mVisible;
	int mType;

protected:
	Matrix4 mTransform;
	BoundingBox mBoundingBox;
	BoundingBox mBoundingBoxTransformed;

	friend class SceneManager;
	friend class EntityGroup;
};

#endif