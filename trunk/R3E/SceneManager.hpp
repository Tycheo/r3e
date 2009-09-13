#ifndef SCENE_MAN_H
#define SCENE_MAN_H

#include "SafeDelete.hpp"
#include "Array.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "ShaderPair.hpp"
#include "SkinShaderData.hpp"
#include "ROSEData.hpp"

class SceneManager {
public:
	SceneManager() : mCamera(0), mCulling(true) {
		mShaders.setCount(MAX_ENTITY);
	}

	~SceneManager(){
		SAFE_DELETE(mCamera);
		
		mAllEntities.delete_values();
		mShaders.delete_values();
	}

	Entity* AddEntity(Entity* entity){
		if(!entity) return NULL;
		mEntityList[entity->mType].push_back(entity);
		mAllEntities.push_back(entity);
		return entity;
	}

	void SetCamera(Camera* camera){
		mCamera = camera;
	}

	void SetCulling(bool enabled){
		mCulling = enabled;
	}

	void Init(){
		ShaderPair* skinShader1 = new ShaderPair("1tex_skin_mesh.vsh", "1tex_skin_mesh.psh");
		SkinShaderData::LoadFromShader(skinShader1->GetProgramObject());

		mShaders[ENTITY_1TEX_MESH_SKINNED] = skinShader1;

		ROSE::Data::LoadZSC();
	}

	void BeginScene(){
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		if(!mCamera) return;
		if(!mCamera->Apply()) return;
		if(!mCulling) return;

		Frustum camf = mCamera->GetFrustum();
		for(unsigned int i = 0; i < mAllEntities.size(); ++i)
			mAllEntities[i]->UpdateCulling(camf);
	}

	void ResizeScene(int width, int height){
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, float(width) / float(height), 0.1f, 1000.0f);

		glMatrixMode(GL_MODELVIEW);

		mWidth = width;
		mHeight = height;

		if(mCamera)
			mCamera->UpdateProjectionMatrix();
	}

	void RenderScene(){
		if(mEntityList[ENTITY_COLOUR].size()){
			glDisable(GL_TEXTURE_2D);

			Array<Entity*, 10>& lst = mEntityList[ENTITY_COLOUR];

			for(unsigned int i = 0; i < lst.size(); ++i){
				if(!lst[i]->mVisible) continue;
				lst[i]->Render();
			}
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		if(mEntityList[ENTITY_1TEX_MESH].size()){
			Array<Entity*, 10>& lst = mEntityList[ENTITY_1TEX_MESH];

			for(unsigned int i = 0; i < lst.size(); ++i){
				if(!lst[i]->mVisible) continue;
				lst[i]->Render();
			}
		}

		if(mEntityList[ENTITY_1TEX_MESH_SKINNED].size()){
			Array<Entity*, 10>& lst = mEntityList[ENTITY_1TEX_MESH_SKINNED];

			mShaders[ENTITY_1TEX_MESH_SKINNED]->Apply();

			OpenGL::EnableVertexAttribArray(SkinShaderData::mBoneLoc);
			OpenGL::EnableVertexAttribArray(SkinShaderData::mWeightLoc);

			for(unsigned int i = 0; i < lst.size(); ++i){
				if(!lst[i]->mVisible) continue;
				lst[i]->Render();
			}

			OpenGL::DisableVertexAttribArray(SkinShaderData::mBoneLoc);
			OpenGL::DisableVertexAttribArray(SkinShaderData::mWeightLoc);

			ShaderPair::Remove();
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	void EndScene(){
	}

private:
	bool mCulling;
	Camera* mCamera;
	Array<Entity*, 10> mEntityList[MAX_ENTITY];
	Array<Entity*, 10> mAllEntities;

	int mWidth, mHeight;

	Array<ShaderPair*> mShaders;
};

#endif