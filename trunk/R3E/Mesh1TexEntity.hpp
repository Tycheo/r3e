#ifndef MESH_1TEX_ENTITY
#define MESH_1TEX_ENTITY

#include "ZMS.hpp"
#include "Entity.hpp"
#include "Material.hpp"
#include "ResourceManager.hpp"

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
		return mMesh->mFormat.HasSkin();
	}

	Mesh1TexEntity* SetMesh(const char* path){
		mMesh = ResourceManager<ROSE::ZMS>::Instance().Load(path);
		SetBoundingBox(mMesh->mBoundingBox);
		return this;
	}

	Mesh1TexEntity* SetMaterial(Material& mat){
		mMaterial = mat;
		return this;
	}

private:
	SmartPointer<ROSE::ZMS> mMesh;
	Material mMaterial;
};

#endif