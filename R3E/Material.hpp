#ifndef MATERIAL_H
#define MATERIAL_H

#include "OpenGL.hpp"
#include "Texture.hpp"
#include "SmartPointer.hpp"
#include "ResourceManager.hpp"

class Material {
public:
	Material(){
		Default();
	}

	Material(const char* path){
		SetTexture(path);
		Default();
	}

	~Material(){}

	void Default(){
		mIs2Side = 0;
		mAlphaTest = 1;
		mAlphaRef = 0.5f;
		mZTest = 1;
		mZWrite = 1;
		mIsAlpha = 0;
		mBlendType = 0;
		mSpecular = 0;
	}

	void Apply(){
		if(mIs2Side) glDisable(GL_CULL_FACE);
		else glEnable(GL_CULL_FACE);

		if(mAlphaTest && !mSpecular){
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, mAlphaRef);
		}else{
			glDisable(GL_ALPHA_TEST);
		}

		if(mZTest) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);

		if(mZWrite) glDepthMask(GL_TRUE);
		else glDepthMask(GL_FALSE);

		if(mIsAlpha && !mSpecular){
			glEnable(GL_BLEND);

			if(mBlendType == 0){
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}else if(mBlendType == 1){
				glBlendFunc(GL_ONE, GL_ONE);
			}
		}else{
			glDisable(GL_BLEND);
		}

		glBindTexture(GL_TEXTURE_2D, mTexture->GetTextureID());
	}

	void SetTexture(const char* path){
		mTexture = ResourceManager<Texture>::Instance().Load(path);
	}

public:
	short mIs2Side;
	short mIsAlpha;
	short mAlphaTest;
	float mAlphaRef;
	short mZTest;
	short mZWrite;
	short mBlendType;
	short mSpecular;

private:
	SmartPointer<Texture> mTexture;
};

#endif