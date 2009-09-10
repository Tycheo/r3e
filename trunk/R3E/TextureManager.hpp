#ifndef TEXMAN_H
#define TEXMAN_H

#include "OpenGL.hpp"
#include "FastStringMapL2.hpp"

namespace OMGHAXLOL {
	struct Texture {
		Texture(const char* path) : mPath(path), mTexID(GL_INVALID_INDEX), mRefCount(0) {}
		~Texture(){
			OpenGL::UnloadTexture(mTexID);
		}

		void AddReference(){
			++mRefCount;
		}

		void RemoveReference(){
			if(mRefCount == 0) return;
			--mRefCount;
			if(!mRefCount)
				OpenGL::UnloadTexture(mTexID);
		}

		int Width(){
			if(mTexID == GL_INVALID_INDEX) LoadTexture();
			return mWidth;
		}

		int Height(){
			if(mTexID == GL_INVALID_INDEX) LoadTexture();
			return mHeight;
		}

		GLuint TexID(){
			if(mTexID == GL_INVALID_INDEX) LoadTexture();
			return mTexID;
		}

	private:
		void LoadTexture(){
			mTexID = OpenGL::LoadTexture(mPath, &mWidth, &mHeight);
		}

	protected:
		String mPath;
		int mWidth;
		int mHeight;

		GLuint mTexID;

		int mRefCount;

		friend class TextureManager;
	};
};

class SmartTexture {
public:
	SmartTexture()
		: mTexture(0)
	{
	}

	SmartTexture(OMGHAXLOL::Texture* texture)
		: mTexture(texture)
	{
		if(!mTexture) return;
		mTexture->AddReference();
	}

	~SmartTexture(){
		if(!mTexture) return;
		mTexture->RemoveReference();
	}

	int Width(){
		if(!mTexture) return 0;
		return mTexture->Width();
	}

	int Height(){
		if(!mTexture) return 0;
		return mTexture->Height();
	}

	operator GLuint() const {
		if(!mTexture) return 0;
		return mTexture->TexID();
	}

	SmartTexture& operator=(const SmartTexture& rhs){
		if(mTexture) mTexture->RemoveReference();
		mTexture = rhs.mTexture;
		mTexture->AddReference();
		return (*this);
	}

private:
	OMGHAXLOL::Texture* mTexture;
};

class TextureManager {
protected:
	friend class SmartTexture;
	static TextureManager mInstance;

public:
	TextureManager(){}
	~TextureManager(){
		mLoadedTextures.delete_values();
	}

	static TextureManager& Instance(){
		return mInstance;
	}

	SmartTexture LoadTexture(const char* path){
		FastStringMapL2<OMGHAXLOL::Texture*>::Entry* entry = mLoadedTextures.find(path);
		OMGHAXLOL::Texture* texture;
		if(!entry){
			texture = new OMGHAXLOL::Texture(path);
			mLoadedTextures.add(path, texture);
		}else{
			texture = (OMGHAXLOL::Texture*)entry;
		}

		return SmartTexture(texture);
	}

private:
	FastStringMapL2<OMGHAXLOL::Texture*> mLoadedTextures;
};

#endif
