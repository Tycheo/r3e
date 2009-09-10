#ifndef SCENE_MAN_H
#define SCENE_MAN_H

#include "SafeDelete.hpp"
#include "Array.hpp"
#include "Camera.hpp"
#include "Entity.hpp"

class SceneManager {
public:
	SceneManager() : mCamera(0), mCulling(true) {}
	~SceneManager(){
		SAFE_DELETE(mCamera);
		
		mAllEntities.delete_values();
	}

	void AddEntity(Entity* entity){
		mEntityList[entity->mType].push_back(entity);
		mAllEntities.push_back(entity);
	}

	void SetCamera(Camera* camera){
		mCamera = camera;
	}

	void SetCulling(bool enabled){
		mCulling = enabled;
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

			for(unsigned int i = 0; i < lst.size(); ++i)
				lst[i]->Render();
		}

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		if(mEntityList[ENTITY_1TEX_MESH].size()){
			glEnable(GL_TEXTURE_2D);

			Array<Entity*, 10>& lst = mEntityList[ENTITY_1TEX_MESH];
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			for(unsigned int i = 0; i < lst.size(); ++i)
				lst[i]->Render();

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}

	void EndScene(){
	}

private:
	bool mCulling;
	Camera* mCamera;
	Array<Entity*, 10> mEntityList[MAX_ENTITY];
	Array<Entity*, 10> mAllEntities;

	int mWidth, mHeight;
};

#endif