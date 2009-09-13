#ifndef MESH_1TEX_ENTITY
#define MESH_1TEX_ENTITY

#include "Entity.hpp"

#include "MeshManager.hpp"
#include "Material.hpp"

class Mesh1TexEntity : public Entity {
public:
	Mesh1TexEntity(){
		mType = ENTITY_1TEX_MESH;
	}

	virtual ~Mesh1TexEntity(){}

	virtual void Render(){
		glPushMatrix();
		mTransform.glMult();

		mMaterial.Apply();
		mMesh->Render();
		glPopMatrix();
	}

	virtual bool IsSkinned(){
		return mMesh.Get()->mFormat.HasSkin();
	}

	Mesh1TexEntity* SetMesh(const char* path){
		mMesh = MeshManager::Instance().LoadMesh(path);
		SetBoundingBox(mMesh->mBoundingBox);
		return this;
	}

	Mesh1TexEntity* SetMaterial(Material& mat){
		mMaterial = mat;
		return this;
	}

private:
	SmartMesh mMesh;
	Material mMaterial;
};

#endif