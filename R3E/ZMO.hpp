#ifndef ZMO_H
#define ZMO_H

#include "String.hpp"
#include "FileSystem.hpp"
#include "ScopedPointer.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4.hpp"
#include "Array.hpp"
#include "SafeDelete.hpp"
#include "ZMD.hpp"

namespace ROSE {
	class ZMO {
	private:
		enum ChannelTypes {
			CTYPE_NONE = (1 << 0),
			CTYPE_POSITION = (1 << 1),
			CTYPE_ROTATION = (1 << 2),
			CTYPE_NORMAL = (1 << 3),
			CTYPE_ALPHA = (1 << 4),
			CTYPE_UV0 = (1 << 5),
			CTYPE_UV1 = (1 << 6),
			CTYPE_UV2 = (1 << 7),
			CTYPE_UV3 = (1 << 8),
			CTYPE_TEXTUREANIM = (1 << 9),
			CTYPE_SCALE = (1 << 10),
		};

	public:
		class Channel {
		public:
			Channel() : mData(0) {}
			~Channel(){
				SAFE_DELETE_ARRAY(mData);
			}

			int mType;
			int mBoneID;
			char* mData;
		};

	public:
		ZMO(){}
		ZMO(const char* path){
			Open(path);
		}

		~ZMO(){
			mChannels.clear();
		}

		bool Open(const char* path){
			ScopedPointer<File> fh(FILE_SYS()->OpenFile(path, "rb"));
			if(!fh) return false;

			char VersionHeader[8];
			fh->ReadData(VersionHeader, 8);
			if(strncmp(VersionHeader, "ZMO0002", 7)){
				fh->Close();
				return false;
			}

			unsigned int channels;
			fh->Read(mFPS);
			fh->Read(mFrameCount);
			fh->Read(channels);

			mChannels.setCount(channels);
			for(unsigned int i = 0; i < channels; ++i){
				Channel* chan = &mChannels[i];
				fh->Read(chan->mType);
				fh->Read(chan->mBoneID);
				if(chan->mType == CTYPE_POSITION) chan->mData = (char*)new Vector3[mFrameCount];
				else if(chan->mType == CTYPE_ROTATION) chan->mData = (char*)new Quaternion[mFrameCount];
			}

			for(unsigned int i = 0; i < mFrameCount; ++i){
				for(unsigned int j = 0; j < channels; ++j){
					Channel* chan = &mChannels[j];
					if(chan->mType == CTYPE_POSITION){
						Vector3* pos = (Vector3*)chan->mData;
						fh->Read(pos[i].x);
						fh->Read(pos[i].y);
						fh->Read(pos[i].z);
						pos[i] /= 100.0f;
					}else if(chan->mType == CTYPE_ROTATION){
						Quaternion* rot = (Quaternion*)chan->mData;
						fh->Read(rot[i].s);
						fh->Read(rot[i].x);
						fh->Read(rot[i].y);
						fh->Read(rot[i].z);
					}
				}
			}

			fh->Close();
			return true;
		}

		Channel* GetBoneChannel(int bone, int type){
			for(unsigned int i = 0; i < mChannels.size(); ++i){
				Channel* chan = &mChannels[i];
				if(chan->mBoneID == bone && chan->mType == type) return chan;
			}

			return NULL;
		}

		unsigned int GetFrameCount(){
			return mFrameCount;
		}

		unsigned int GetFPS(){
			return mFPS;
		}

		void CreateBoneMatrices(int frame, const Array<ZMD::Bone>& bindBoneList, Array<Matrix4>& matrices){
			matrices.setCount(bindBoneList.size());
			for(unsigned int i = 0; i < bindBoneList.size(); ++i){
				ZMD::Bone* bone = &bindBoneList[i];
				Channel* posC = GetBoneChannel(i, CTYPE_POSITION);
				Channel* rotC = GetBoneChannel(i, CTYPE_ROTATION);

				matrices[i] = Matrix4::IDENTITY;

				if(rotC) matrices[i] = Matrix4::CreateRotation(((Quaternion*)rotC->mData)[frame]);
				else matrices[i] = Matrix4::CreateRotation(bone->mRotate);
				
				if(posC) matrices[i] *= Matrix4::CreateTranslation(((Vector3*)posC->mData)[frame]);
				else matrices[i] *= Matrix4::CreateTranslation(bone->mTranslate);
			}

			TransformChildren(bindBoneList, matrices, 0);
		}

	private:
		void TransformChildren(const Array<ZMD::Bone>& bindBoneList, Array<Matrix4>& matrices, unsigned int parent){
			for(unsigned int i = 0; i < bindBoneList.size(); ++i){
				if(i == parent) continue;
				ZMD::Bone* bone = &bindBoneList[i];
				if(bone->mParentID != parent) continue;
				matrices[i] *= matrices[bone->mParentID];
				if(bone->mDummy) continue;
				TransformChildren(bindBoneList, matrices, i);
			}
		}

	public:
		unsigned int mFPS;
		unsigned int mFrameCount;
		Array<Channel> mChannels;
	};
};

#endif