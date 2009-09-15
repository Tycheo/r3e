#ifndef TEXTURE_H
#define TEXTURE_H

#include "Resource.hpp"
#include "OpenGL.hpp"

#include <IL/il.h>
#include <IL/ilut.h>

class Texture : public Resource {
public:
	Texture() : mTexID(GL_INVALID_INDEX) {}
	virtual ~Texture(){}

	virtual void Unload(){
		if(mTexID == GL_INVALID_INDEX) return;
		glDeleteTextures(1, &mTexID);
		mTexID = GL_INVALID_INDEX;
	}

	virtual bool Load(const char* pathV){
		static bool hasInitIL = false;
		if(!hasInitIL){
			ilInit();
			ilutRenderer(ILUT_OPENGL);
			hasInitIL = true;
		}

		String path = pathV;
		FILE_SYS()->GetFullPath(path);
		ILuint ilTex;
		ilGenImages(1, &ilTex);
		ilBindImage(ilTex);
		if(!ilLoadImage(path)) return false;
		mWidth = ilGetInteger(IL_IMAGE_WIDTH);
		mHeight = ilGetInteger(IL_IMAGE_HEIGHT);
		iluFlipImage();

		mTexID = ilutGLBindTexImage();
		ilDeleteImages(1, &ilTex);

		return true;
	}

	GLuint GetTextureID(){
		return mTexID;
	}

	int GetWidth(){
		return mWidth;
	}

	int GetHeight(){
		return mHeight;
	}

private:
	int mWidth;
	int mHeight;
	GLuint mTexID;
};

#endif