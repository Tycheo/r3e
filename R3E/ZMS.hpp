#ifndef ROSE_ZMS_H
#define ROSE_ZMS_H

#include "FlatFile.hpp"
#include "FileSystem.hpp"
#include "ScopedPointer.hpp"

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "BoundingBox.hpp"

#include "VertexTypes.hpp"
#include "VertexBuffer.hpp"

#include "IndexBuffer.hpp"

#include "Resource.hpp"

namespace ROSE {
	class ZMS : public Resource {
	private:
		struct BoneWeights {
			float mWeights[4];
			short mBones[4];
		};

		struct VertexFormat {
			enum {
				VF_NONE = (1 << 0),
				VF_POSITION = (1 << 1),
				VF_NORMAL = (1 << 2),
				VF_COLOR = (1 << 3),
				VF_BLEND_WEIGHT = (1 << 4),
				VF_BLEND_INDEX = (1 << 5),
				VF_TANGENT = (1 << 6),
				VF_UV0 = (1 << 7),
				VF_UV1 = (1 << 8),
				VF_UV2 = (1 << 9),
				VF_UV3 = (1 << 10),
			};

			bool HasPosition(){
				return (mFormat & VF_POSITION) != 0;
			}

			bool HasNormal(){
				return (mFormat & VF_NORMAL) != 0;
			}

			bool HasColour(){
				return (mFormat & VF_COLOR) != 0;
			}

			bool HasBoneWeight(){
				return (mFormat & VF_BLEND_WEIGHT) != 0;
			}

			bool HasBoneIndex(){
				return (mFormat & VF_BLEND_INDEX) != 0;
			}

			bool HasSkin(){
				return HasBoneWeight() && HasBoneIndex();
			}

			bool HasTangents(){
				return (mFormat & VF_TANGENT) != 0;
			}

			bool HasUV(int channel){
				return (mFormat & (VF_UV0 << channel)) != 0;
			}

			bool HasUV0(){
				return (mFormat & VF_UV0) != 0;
			}

			bool HasUV1(){
				return (mFormat & VF_UV1) != 0;
			}

			bool HasUV2(){
				return (mFormat & VF_UV2) != 0;
			}

			bool HasUV3(){
				return (mFormat & VF_UV3) != 0;
			}

			int UVCount(){
				if(HasUV3()) return 4;
				if(HasUV2()) return 3;
				if(HasUV1()) return 2;
				if(HasUV0()) return 1;
				return 0;
			}

			int mFormat;
		};

	public:
		ZMS() : mVersion(0), mBoneLookup(0), mVertices(0), mNormals(0), mWeights(0),
			mColours(0), mBones(0), mTangents(0), mIndexBuffer(0), mVertexBuffer(0)
		{
			mUVMaps[0] = 0;
			mUVMaps[1] = 0;
			mUVMaps[2] = 0;
			mUVMaps[3] = 0;
		}

		virtual ~ZMS(){}

		void Render(){
			OpenGL::ActiveTexture(GL_TEXTURE0_ARB);
			OpenGL::ClientActiveTexture(GL_TEXTURE0_ARB);
			mVertexBuffer->Bind();
			mIndexBuffer->Draw();
		}

		virtual void Unload(){
			SAFE_DELETE_ARRAY(mBoneLookup);
			SAFE_DELETE_ARRAY(mVertices);
			SAFE_DELETE_ARRAY(mNormals);
			SAFE_DELETE_ARRAY(mColours);
			SAFE_DELETE_ARRAY(mBones);
			SAFE_DELETE_ARRAY(mWeights);
			SAFE_DELETE_ARRAY(mTangents);
			SAFE_DELETE_ARRAY(mUVMaps[0]);
			SAFE_DELETE_ARRAY(mUVMaps[1]);
			SAFE_DELETE_ARRAY(mUVMaps[2]);
			SAFE_DELETE_ARRAY(mUVMaps[3]);
			SAFE_DELETE(mVertexBuffer);
			SAFE_DELETE(mIndexBuffer);
		}

		virtual bool Load(const char* path){
			ScopedPointer<File> fh(FILE_SYS()->OpenFile(path, "rb"));
			if(!fh) return false;

			char VersionHeader[8];
			fh->ReadData(VersionHeader, 8);
			if(strcmp(VersionHeader, "ZMS0008")){
				mVersion = 8;
			}else if(strcmp(VersionHeader, "ZMS0007")){
				mVersion = 7;
			}else if(strcmp(VersionHeader, "ZMS0006")){
				mVersion = 6;
			}else if(strcmp(VersionHeader, "ZMS0005")){
				mVersion = 5;
			}else{
				fh->Close();
				return false;
			}

			if(mVersion >= 7){
				LoadMesh8(fh);
			}else{
				LoadMesh6(fh);
			}

			mBoundingBox.mMax = Vector3(-99999.0f);
			mBoundingBox.mMin = Vector3( 99999.0f);
			for(int i = 0; i < mVertexCount; ++i)
				mBoundingBox.AddPoint(mVertices[i]);

			if(!mFormat.HasUV0()){
				mVertexBuffer = new VertexBuffer<V3F>();
				mVertexBuffer->SetSize(mVertexCount);
				for(int i = 0; i < mVertexCount; ++i)
					mVertexBuffer->AddVertex(&mVertices[i]);
			}else if(!mFormat.HasUV1()){

				/*Animator anim;
				ROSE::ZMD* skeleton = new ROSE::ZMD();
				ROSE::ZMO* motion = new ROSE::ZMO();
				anim.SetSkeleton(skeleton);
				anim.SetAnimation(motion);

				skeleton->Open("3DDATA\\AVATAR\\MALE.ZMD");
				motion->Open("3DDATA\\MOTION\\AVATAR\\EMPTY_STOP1_M1.ZMO");

				anim.PreCacheFrames();
					/*Vector3 tmppos;
					BoneWeights* w = &mVertBones[i];

					for(int j = 0; j < 4; ++j){
						Matrix4 mat = anim.GetBoneMatrix(w->mBones[j]);
						tmppos += mat.TransformCoord(mVertices[i]) * w->mWeights[j];
					}

					tmp.pos = tmppos;*/

				if(mFormat.HasSkin()){
					V3F_UV0_BD tmp;
					mVertexBuffer = new VertexBuffer<V3F_UV0_BD>();
					mVertexBuffer->SetSize(mVertexCount);
					for(int i = 0; i < mVertexCount; ++i){
						tmp.pos = mVertices[i];
						tmp.uv0 = mUVMaps[0][i];
						tmp.bone = mBones[i];
						tmp.weight = mWeights[i];
						mVertexBuffer->AddVertex(&tmp);
					}
				}else{
					V3F_UV0 tmp;
					mVertexBuffer = new VertexBuffer<V3F_UV0>();
					mVertexBuffer->SetSize(mVertexCount);
					for(int i = 0; i < mVertexCount; ++i){
						tmp.pos = mVertices[i];
						tmp.uv0 = mUVMaps[0][i];
						mVertexBuffer->AddVertex(&tmp);
					}
				}
			}else{
				V3F_UV0_UV1 tmp;

				mVertexBuffer = new VertexBuffer<V3F_UV0_UV1>();
				mVertexBuffer->SetSize(mVertexCount);
				for(int i = 0; i < mVertexCount; ++i){
					tmp.pos = mVertices[i];
					tmp.uv0 = mUVMaps[0][i];
					tmp.uv1 = mUVMaps[1][i];
					mVertexBuffer->AddVertex(&tmp);
				}
			}

			fh->Close();
			return true;
		}

	private:
		void LoadMesh6(File* fh){
			int idx;
			fh->Read(mFormat);
			fh->Read(mBoundingBox.mMin);
			fh->Read(mBoundingBox.mMax);

			mBoneCount = (short)fh->Read<int>();
			mBoneLookup = new short[mBoneCount];
			for(int i = 0; i < mBoneCount; ++i){
				fh->Read(idx);
				mBoneLookup[i] = (short)fh->Read<int>();
			}

			mVertexCount = (short)fh->Read<int>();
			mVertices = new Vector3[mVertexCount];
			for(int i = 0; i < mVertexCount; ++i){
				fh->Read(idx);
				fh->Read(mVertices[i]);
			}

			if(mFormat.HasNormal()){
				mNormals = new Vector3[mVertexCount];
				for(int i = 0; i < mVertexCount; ++i){
					fh->Read(idx);
					fh->Read(mNormals[i]);
				}
			}

			if(mFormat.HasColour()){
				mColours = new Vector4[mVertexCount];
				for(int i = 0; i < mVertexCount; ++i){
					fh->Read(idx);
					fh->Read(mColours[i]);
				}
			}

			if(mFormat.HasSkin()){
				mWeights = new Vector4[mVertexCount];
				mBones = new Vector4[mVertexCount];
				int bones[4];
				for(int i = 0; i < mVertexCount; ++i){
					fh->Read(idx);

					fh->Read(mWeights[i]);
					fh->ReadData(bones, sizeof(int) * 4);
					mBones[i] = Vector4(mBoneLookup[bones[0]], mBoneLookup[bones[1]], mBoneLookup[bones[2]], mBoneLookup[bones[3]]);
				}
			}

			if(mFormat.HasTangents()){
				mTangents = new Vector3[mVertexCount];
				for(int i = 0; i < mVertexCount; ++i){
					fh->Read(idx);
					fh->Read(mTangents[i]);
				}
			}

			for(int c = 0; c < 4; ++c){
				if(!mFormat.HasUV(c)) continue;
				mUVMaps[c] = new Vector2[mVertexCount];
				for(int i = 0; i < mVertexCount; ++i){
					fh->Read(idx);
					fh->Read(mUVMaps[c][i]);
				}
			}

			short faceCount = (short)fh->Read<int>();
			mIndexCount = faceCount * 3;

			mIndexBuffer = new IndexBuffer<short>();
			mIndexBuffer->SetSize(mIndexCount);

			short tmp;
			for(int i = 0; i < faceCount; ++i){
				fh->Read(idx);

				for(int j = 0; j < 3; ++j){
					tmp = (short)fh->Read<int>();
					mIndexBuffer->AddIndex(&tmp);
				}
			}
		}

		void LoadMesh8(File* fh){
			fh->Read(mFormat);
			fh->Read(mBoundingBox.mMin);
			fh->Read(mBoundingBox.mMax);

			fh->Read(mBoneCount);
			mBoneLookup = new short[mBoneCount];
			fh->ReadData(mBoneLookup, sizeof(short) * mBoneCount);

			fh->Read(mVertexCount);
			mVertices = new Vector3[mVertexCount];
			fh->ReadData(mVertices, sizeof(Vector3) * mVertexCount);

			if(mFormat.HasNormal()){
				mNormals = new Vector3[mVertexCount];
				fh->ReadData(mNormals, sizeof(Vector3) * mVertexCount);
			}

			if(mFormat.HasColour()){
				mColours = new Vector4[mVertexCount];
				fh->ReadData(mColours, sizeof(Vector4) * mVertexCount);
			}

			if(mFormat.HasSkin()){
				mWeights = new Vector4[mVertexCount];
				mBones = new Vector4[mVertexCount];
				short bones[4];
				for(int i = 0; i < mVertexCount; ++i){
					fh->Read(mWeights[i]);
					fh->ReadData(bones, sizeof(short) * 4);
					mBones[i] = Vector4(mBoneLookup[bones[0]], mBoneLookup[bones[1]], mBoneLookup[bones[2]], mBoneLookup[bones[3]]);
				}
			}

			if(mFormat.HasTangents()){
				mTangents = new Vector3[mVertexCount];
				fh->ReadData(mTangents, sizeof(Vector3) * mVertexCount);
			}

			for(int c = 0; c < 4; ++c){
				if(!mFormat.HasUV(c)) continue;
				mUVMaps[c] = new Vector2[mVertexCount];
				fh->ReadData(mUVMaps[c], sizeof(Vector2) * mVertexCount);
			}

			fh->Read(mIndexCount);

			mIndexCount *= 3;

			mIndexBuffer = new IndexBuffer<short>();
			mIndexBuffer->SetCount(mIndexCount);
			fh->ReadData(mIndexBuffer->Buffer(), sizeof(short) * mIndexCount);
		}

	public:
		VertexFormat mFormat;
		BoundingBox mBoundingBox;

	private:
		int mVersion;

		short mBoneCount;
		short* mBoneLookup;

		short mVertexCount;
		Vector3* mVertices;
		Vector3* mNormals;
		Vector4* mColours;
		Vector4* mBones;
		Vector4* mWeights;
		Vector3* mTangents;
		Vector2* mUVMaps[4];

		short mIndexCount;

		IVertexBuffer* mVertexBuffer;
		IIndexBuffer* mIndexBuffer;
	};
};

#endif