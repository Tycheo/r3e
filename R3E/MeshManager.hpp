#ifndef MESHMAN_H
#define MESHMAN_H

#include "ZMS.hpp"
#include "String.hpp"
#include "SafeDelete.hpp"
#include "FastStringMapL2.hpp"

namespace OMGHAXLOL {
	struct Mesh {
		Mesh(const char* path) : mPath(path), mMesh(0), mRefCount(0) {}
		~Mesh(){
			SAFE_DELETE(mMesh);
		}

		void LoadMesh(){
			mMesh = new ROSE::ZMS();
			mMesh->Open(mPath);
		}

		void AddReference(){
			++mRefCount;
		}

		void RemoveReference(){
			if(mRefCount == 0) return;
			--mRefCount;
			if(!mRefCount){
				delete mMesh;
				mMesh = 0;
			}
		}

		ROSE::ZMS* GetMesh(){
			return mMesh;
		}

	protected:
		String mPath;
		ROSE::ZMS* mMesh;

		int mRefCount;

		friend class MeshManager;
	};
};

class SmartMesh {
public:
	SmartMesh()
		: mMesh(0)
	{
	}

	SmartMesh(OMGHAXLOL::Mesh* mesh)
		: mMesh(mesh)
	{
		mMesh->AddReference();
	}

	~SmartMesh(){
		if(!mMesh) return;
		mMesh->RemoveReference();
	}

	ROSE::ZMS* operator->() const {
		return mMesh->GetMesh();
	}

	operator ROSE::ZMS*() const {
		return mMesh->GetMesh();
	}

	SmartMesh& operator=(const SmartMesh& rhs){
		if(mMesh)
			mMesh->RemoveReference();
		mMesh = rhs.mMesh;
		mMesh->AddReference();
		return (*this);
	}

private:
	OMGHAXLOL::Mesh* mMesh;
};

class MeshManager {
protected:
	friend class SmartMesh;
	static MeshManager mInstance;

public:
	MeshManager(){}
	~MeshManager(){
		mLoadedMesh.delete_values();
	}

	void CleanUp(){
		mLoadedMesh.delete_values();
	}

	static MeshManager& Instance(){
		return mInstance;
	}

	SmartMesh LoadMesh(const char* path){
		FastStringMapL2<OMGHAXLOL::Mesh*>::Entry* entry = mLoadedMesh.find(path);
		OMGHAXLOL::Mesh* mesh;
		if(!entry){
			mesh = new OMGHAXLOL::Mesh(path);
			mLoadedMesh.add(path, mesh);
		}else{
			mesh = (OMGHAXLOL::Mesh*)entry;
		}

		mesh->LoadMesh();

		return SmartMesh(mesh);
	}

private:
	FastStringMapL2<OMGHAXLOL::Mesh*> mLoadedMesh;
};

#endif
