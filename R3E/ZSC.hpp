#ifndef ZSC_H
#define ZSC_H

#include "String.hpp"
#include "FileSystem.hpp"
#include "Material.hpp"
#include "EntityGroup.hpp"
#include "SkeletalEntity.hpp"

namespace ROSE {
	template<class T> class ZSC {
	private:
		enum PartData {
			PART_POSITION = 0x1,
			PART_ROTATION = 0x2,
			PART_SCALE = 0x3,
			PART_AXISROTATION = 0x4,
			PART_BONEINDEX = 0x5,
			PART_DUMMYINDEX = 0x6,
			PART_PARENT = 0x7,
			PART_COLLISION = 0x1D,
			PART_ZMOPATH = 0x1E,
			PART_RANGEMODE = 0x1F,
			PART_LIGHTMAPMODE = 0x20,
		};

		enum EffectData {
			EFFECT_POSITION = 0x1,
			EFFECT_ROTATION = 0x2,
			EFFECT_SCALE = 0x3,
			EFFECT_PARENT = 0x7,
		};

		enum TransformFlags {
			TRANS_NONE,
			TRANS_ROTATE = 1 << 0,
			TRANS_SCALE = 1 << 1,
			TRANS_TRANSLATE = 1 << 2,
		};

		struct Mesh {
			String mPath;
		};

		struct Effect {
			String mPath;
		};

		struct ModelPart {
			short mMesh;
			short mMaterial;

			short mBoneID;
			short mDummyID;

			Matrix4 mTransform;
		};

		struct ModelEffect {
			short mEffect;
			short mType;
			Matrix4 mTransform;
		};

		struct Model {
			Array<ModelPart> mPartList;
			Array<ModelEffect> mEffectList;

			BoundingBox mBoundingBox;
		};

	public:
		ZSC()
			: mBoneID(-1), mDummyID(-1)
		{
		}

		ZSC(const char* path)
			: mBoneID(-1), mDummyID(-1) 
		{
			Open(path);
		}

		~ZSC(){
			mMeshList.clear();
			mMaterialList.clear();
			mEffectList.clear();
			mModelList.clear();
		}
		
		bool Open(const char* path){
			ScopedPointer<File> fh(FILE_SYS()->OpenFile(path, "rb"));
			if(!fh) return false;
			short count, subCount;

			fh->Read(count);
			mMeshList.setCount(count);
			for(short i = 0; i < count; ++i)
				mMeshList[i].mPath = fh->ReadTerminatedString();

			fh->Read(count);
			mMaterialList.setCount(count);
			for(short i = 0; i < count; ++i){
				Material* mat = &mMaterialList[i];
				mat->SetTexture(fh->ReadTerminatedString());
				fh->Skip(2);
				fh->Read(mat->mIsAlpha);
				fh->Read(mat->mIs2Side);
				fh->Read(mat->mAlphaTest);
				fh->Read(subCount);
				mat->mAlphaRef = float(subCount) / 256.0f;
				fh->Read(mat->mZTest);
				fh->Read(mat->mZWrite);
				fh->Read(mat->mBlendType);
				fh->Read(mat->mSpecular);
				fh->Skip(18);
			}

			fh->Read(count);
			mEffectList.setCount(count);
			for(short i = 0; i < count; ++i)
				mEffectList[i].mPath = fh->ReadTerminatedString();

			
			fh->Read(count);
			mModelList.setCount(count);
			for(short i = 0; i < count; ++i){
				Model* model = &mModelList[i];
				fh->Skip(12);
				fh->Read(subCount);
				if(!subCount) continue;

				model->mPartList.setCount(subCount);
				for(short j = 0; j < subCount; ++j){
					ModelPart* part = &model->mPartList[j];
					part->mBoneID = -1;
					part->mDummyID = -1;

					fh->Read(part->mMesh);
					fh->Read(part->mMaterial);
					
					int transFlags = TRANS_NONE;
					char flag, size;
					Vector3 translate, scale;
					Quaternion rotate;

					fh->Read(flag);
					while(flag){
						fh->Read(size);
						switch(flag){
							case PART_POSITION:
								fh->Read(translate);
								transFlags |= TRANS_TRANSLATE;
								break;
							case PART_ROTATION:
								fh->Read(rotate);
								transFlags |= TRANS_ROTATE;
								break;
							case PART_SCALE:
								fh->Read(scale);
								transFlags |= TRANS_SCALE;
								break;
							case PART_BONEINDEX:
								fh->Read(part->mBoneID);
								break;
							case PART_DUMMYINDEX:
								fh->Read(part->mDummyID);
								break;
							default:
								fh->Skip(size);
						};

						fh->Read(flag);
					}

					part->mTransform = Matrix4::IDENTITY;
					if(transFlags & TRANS_ROTATE) part->mTransform = Matrix4::CreateRotation(rotate);
					if(transFlags & TRANS_SCALE) part->mTransform = part->mTransform * Matrix4::CreateScaling(scale);
					if(transFlags & TRANS_TRANSLATE) part->mTransform = part->mTransform * Matrix4::CreateTranslation(translate);
				}

				fh->Read(subCount);
				model->mEffectList.setCount(subCount);
				for(short j = 0; j < subCount; ++j){
					ModelEffect* effect = &model->mEffectList[j];
					fh->Read(effect->mEffect);
					fh->Read(effect->mType);

					int transFlags = TRANS_NONE;
					char flag, size;
					Vector3 translate, scale;
					Quaternion rotate;

					fh->Read(flag);
					while(flag){
						fh->Read(size);
						switch(flag){
							case EFFECT_POSITION:
								fh->Read(translate);
								transFlags |= TRANS_TRANSLATE;
								break;
							case EFFECT_ROTATION:
								fh->Read(rotate);
								transFlags |= TRANS_ROTATE;
								break;
							case EFFECT_SCALE:
								fh->Read(scale);
								transFlags |= TRANS_SCALE;
								break;
							default:
								fh->Skip(size);
						};

						fh->Read(flag);
					}

					effect->mTransform = Matrix4::IDENTITY;
					if(transFlags & TRANS_ROTATE) effect->mTransform = Matrix4::CreateRotation(rotate);
					if(transFlags & TRANS_SCALE) effect->mTransform = effect->mTransform * Matrix4::CreateScaling(scale);
					if(transFlags & TRANS_TRANSLATE) effect->mTransform = effect->mTransform * Matrix4::CreateTranslation(translate);
				}

				fh->Read(model->mBoundingBox);

				model->mBoundingBox.mMin /= 100.0f;
				model->mBoundingBox.mMax /= 100.0f;
			}

			fh->Close();
			return true;
		}

		Entity* LoadModel(unsigned int index, Entity* parent = 0){
			if(index >= mModelList.size()) return NULL;
			Model* model = &mModelList[index];
			if(model->mPartList.size() == 0) return NULL;

			EntityGroup* group = new EntityGroup();
			group->SetBoundingBox(model->mBoundingBox);
			for(unsigned int i = 0; i < model->mPartList.size(); ++i){
				T* entity = new T();

				ModelPart* part = &model->mPartList[i];
				Mesh* mesh = &mMeshList[part->mMesh];
				Material* material = &mMaterialList[part->mMaterial];

				entity->SetMesh(mesh->mPath);
				entity->SetMaterial(*material);
				entity->SetTransform(part->mTransform);

				group->AddChild(entity);

				if(part->mBoneID == -1 && mBoneID != -1) part->mBoneID = mBoneID;
				if(part->mDummyID == -1 && mDummyID != -1) part->mDummyID = mDummyID;

				if(parent){
					if(part->mBoneID != -1){
						if(parent->mType == ENTITY_1TEX_MESH_SKINNED){
							SkeletalEntity* skel = (SkeletalEntity*)parent;
							skel->BindEntityToBone(entity, part->mBoneID);
						}
					}else if(part->mDummyID != -1){
						if(parent->mType == ENTITY_1TEX_MESH_SKINNED){
							SkeletalEntity* skel = (SkeletalEntity*)parent;
							skel->BindEntityToDummy(entity, part->mDummyID);
						}
					}
				}
			}

			return group;
		}

		void SetBindBone(int id){
			mBoneID = id;
		}

		void SetBindDummy(int id){
			mDummyID = id;
		}

	private:
		Array<Mesh> mMeshList;
		Array<Material> mMaterialList;
		Array<Effect> mEffectList;
		Array<Model> mModelList;

		int mBoneID;
		int mDummyID;
	};
};

#endif