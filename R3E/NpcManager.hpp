#ifndef NPCMAN_H
#define NPCMAN_H

#include "Mesh1TexEntity.hpp"
#include "CHR.hpp"
#include "ZSC.hpp"
#include "Npc.hpp"

class NpcManager {
private:
	static NpcManager mInstance;

public:
	NpcManager() : mZSC(0), mCHR(0) {}
	~NpcManager(){
		SAFE_DELETE(mZSC);
		SAFE_DELETE(mCHR);
	}

	static NpcManager& Instance(){
		return mInstance;
	}

	void Init(){
		mCHR = new ROSE::CHR("3DDATA\\NPC\\LIST_NPC.CHR");
		mZSC = new ROSE::ZSC<Mesh1TexEntity>("3DDATA\\NPC\\PART_NPC.ZSC");
	}

	Npc* Load(int id){
		ROSE::CHR::Character* chr = mCHR->GetCharacter(id);
		if(!chr) return NULL;
		if(!chr->mActive) return NULL;
		if(chr->mAnimationLinks.size() < 1) return NULL;

		ROSE::CHR::Skeleton* skel = mCHR->GetSkeleton(chr->mSkeleton);
		ROSE::CHR::Animation* anim = mCHR->GetAnimation(chr->mAnimationLinks[1].mAnimation);
		if(!skel || !anim) return NULL;

		Npc* npc = new Npc();
		npc->SetSkeleton(skel->mPath);
		npc->SetAnimation(anim->mPath);
		
		for(unsigned int i = 0; i < chr->mModelLinks.size(); ++i)
			npc->AddChild(mZSC->LoadModel(chr->mModelLinks[i].mModel));

		return npc;
	}

private:
	ROSE::ZSC<Mesh1TexEntity>* mZSC;
	ROSE::CHR* mCHR;
};

#endif