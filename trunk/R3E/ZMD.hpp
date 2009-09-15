#ifndef ZMD_H
#define ZMD_H

#include "String.hpp"
#include "FileSystem.hpp"
#include "ScopedPointer.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4.hpp"
#include "Array.hpp"

#include "Resource.hpp"

namespace ROSE {
	class ZMD : public Resource {
	public:
		struct Bone {
			bool mDummy;
			unsigned int mParentID;
			String mName;

			Vector3 mTranslate;
			Quaternion mRotate;

			Matrix4 mTransform;
		};

	public:
		ZMD(){}
		virtual ~ZMD(){}

		virtual void Unload(){
			mBoneList.clear();
		}

		virtual bool Load(const char* path){
			ScopedPointer<File> fh(FILE_SYS()->OpenFile(path, "rb"));
			if(!fh) return false;

			char VersionHeader[7];
			fh->ReadData(VersionHeader, 7);
			if(strncmp(VersionHeader, "ZMD0003", 7) == 0){
				mVersion = 3;
			}else if(strncmp(VersionHeader, "ZMD0002", 7) == 0){
				mVersion = 2;
			}else{
				fh->Close();
				return false;
			}

			unsigned int boneCount;
			fh->Read(boneCount);
			mBoneList.setCount(boneCount);
			for(unsigned int i = 0; i < boneCount; ++i){
				Bone* bone = &mBoneList[i];
				bone->mDummy = false;
				fh->Read(bone->mParentID);
				bone->mName = fh->ReadTerminatedString();
				fh->Read(bone->mTranslate.x);
				fh->Read(bone->mTranslate.y);
				fh->Read(bone->mTranslate.z);

				fh->Read(bone->mRotate.s);
				fh->Read(bone->mRotate.x);
				fh->Read(bone->mRotate.y);
				fh->Read(bone->mRotate.z);
				bone->mTranslate /= 100.0f;

				bone->mTransform = Matrix4::CreateRotation(bone->mRotate);
				bone->mTransform *= Matrix4::CreateTranslation(bone->mTranslate);
			}

			fh->Read(boneCount);
			int offset = mBoneList.size();
			mBoneList.setCount(offset + boneCount);
			for(unsigned int i = 0; i < boneCount; ++i){
				Bone* bone = &mBoneList[i + offset];
				bone->mDummy = true;
				bone->mName = fh->ReadTerminatedString();
				fh->Read(bone->mParentID);
				fh->Read(bone->mTranslate);
				bone->mTranslate /= 100.0f;
				bone->mTransform = Matrix4::IDENTITY;
				if(mVersion == 3){
					bone->mTransform = Matrix4::CreateRotation(bone->mRotate);
					fh->Read(bone->mRotate.s);
					fh->Read(bone->mRotate.x);
					fh->Read(bone->mRotate.y);
					fh->Read(bone->mRotate.z);
				}

				bone->mTransform *= Matrix4::CreateTranslation(bone->mTranslate);
			}

			TransformChildren(0);

			fh->Close();
			return true;
		}

		void TransformChildren(unsigned int parent){
			for(unsigned int i = 0; i < mBoneList.size(); ++i){
				if(i == parent) continue;
				Bone* bone = &mBoneList[i];
				if(bone->mParentID != parent) continue;
				bone->mTransform *= mBoneList[bone->mParentID].mTransform;
				if(bone->mDummy) continue;
				TransformChildren(i);
			}
		}

		void Render(){
			glPointSize(5.0f);
			for(unsigned int i = 0; i < mBoneList.size(); ++i){
				Bone* bone = &mBoneList[i];
				Bone* parent = &mBoneList[bone->mParentID];

				Vector3 pos1 = bone->mTransform.TransformCoord(Vector3());
				glColor3f(1.0f, 0.0f, 0.0f);
				glBegin(GL_POINTS);
				glVertex3f(pos1.x, pos1.y, pos1.z);
				glEnd();

				if(bone == parent) continue;

				Vector3 pos2 = parent->mTransform.TransformCoord(Vector3());

				glColor3f(0.0f, 1.0f, 0.0f);
				glBegin(GL_LINES);
				glVertex3f(pos1.x, pos1.y, pos1.z);
				glVertex3f(pos2.x, pos2.y, pos2.z);
				glEnd();
			}
		}

		unsigned int GetBoneCount(){
			return mBoneList.size();
		}

		Bone* GetBone(int id){
			return &mBoneList[id];
		}

		Bone* GetBone(const char* name){
			String tmp = name;
			for(unsigned int i = 0; i < mBoneList.size(); ++i){
				if(mBoneList[i].mName == tmp) return &mBoneList[i];
			}

			return NULL;
		}

		unsigned int GetBoneID(const char* name){
			String tmp = name;
			for(unsigned int i = 0; i < mBoneList.size(); ++i){
				if(mBoneList[i].mName == tmp) return i;
			}

			return 0xFFFFFFFF;
		}

		const Array<Bone>& GetBoneList(){
			return mBoneList;
		}

	public:
		unsigned int mVersion;
		Array<Bone> mBoneList;
	};
};

#endif