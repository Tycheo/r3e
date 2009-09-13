#ifndef ROSE_DATA_HPP
#define ROSE_DATA_HPP

#include "ZSC.hpp"
#include "NpcManager.hpp"

namespace ROSE {
	enum ItemType {
		IT_MASK = 1,
		IT_CAP = 2,
		IT_BODY = 3,
		IT_ARM = 4,
		IT_FOOT = 5,
		IT_BACK = 6,
		IT_JEWEL = 7,
		IT_WEAPON = 8,
		IT_SUBWPN = 9,
		IT_USE = 10,
		IT_ETC = 11,
		IT_NATURAL = 12,
		IT_QUEST = 13,
		IT_PAT = 14,
		IT_FACE = 15,
		IT_HAIR = 16,
		IT_MAX
	};

	enum Gender {
		G_MALE = 0,
		G_FEMALE = 1,
		MAX_GENDER
	};

	class Data {
	public:
		static void Load(){
			static bool loaded = false;
			if(loaded) return;
			loaded = true;
			mModelLists[G_MALE].setCount(IT_MAX);
			mModelLists[G_FEMALE].setCount(IT_MAX);

			mModelLists[G_FEMALE][IT_MASK] = mModelLists[G_MALE][IT_MASK] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_FACEIEM.ZSC");
			mModelLists[G_FEMALE][IT_BACK] = mModelLists[G_MALE][IT_BACK] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_BACK.ZSC");

			mModelLists[G_FEMALE][IT_WEAPON] = mModelLists[G_MALE][IT_WEAPON] = new ZSC<Mesh1TexEntity>("3DDATA\\WEAPON\\LIST_WEAPON.ZSC");
			mModelLists[G_FEMALE][IT_SUBWPN] = mModelLists[G_MALE][IT_SUBWPN] = new ZSC<Mesh1TexEntity>("3DDATA\\WEAPON\\LIST_SUBWPN.ZSC");

			mModelLists[G_FEMALE][IT_PAT] = mModelLists[G_MALE][IT_PAT] = new ZSC<Mesh1TexEntity>("3DDATA\\PAT\\LIST_PAT.ZSC");

			mModelLists[G_MALE][IT_CAP] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_MCAP.ZSC");
			mModelLists[G_MALE][IT_BODY] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_MBODY.ZSC");
			mModelLists[G_MALE][IT_ARM] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_MARMS.ZSC");
			mModelLists[G_MALE][IT_FOOT] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_MFOOT.ZSC");
			mModelLists[G_MALE][IT_FACE] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_MFACE.ZSC");
			mModelLists[G_MALE][IT_HAIR] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_MHAIR.ZSC");

			mModelLists[G_FEMALE][IT_CAP] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_WCAP.ZSC");
			mModelLists[G_FEMALE][IT_BODY] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_WBODY.ZSC");
			mModelLists[G_FEMALE][IT_ARM] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_WARMS.ZSC");
			mModelLists[G_FEMALE][IT_FOOT] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_WFOOT.ZSC");
			mModelLists[G_FEMALE][IT_FACE] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_WFACE.ZSC");
			mModelLists[G_FEMALE][IT_HAIR] = new ZSC<Mesh1TexEntity>("3DDATA\\AVATAR\\LIST_WHAIR.ZSC");

			mModelLists[G_MALE][IT_BACK]->SetBindDummy(3);

			mModelLists[G_MALE][IT_MASK]->SetBindDummy(4);

			mModelLists[G_MALE][IT_CAP]->SetBindDummy(6);
			mModelLists[G_FEMALE][IT_CAP]->SetBindDummy(6);

			mModelLists[G_MALE][IT_FACE]->SetBindBone(4);
			mModelLists[G_MALE][IT_HAIR]->SetBindBone(4);

			mModelLists[G_FEMALE][IT_FACE]->SetBindBone(4);
			mModelLists[G_FEMALE][IT_HAIR]->SetBindBone(4);

			NpcManager::Instance().Init();
		}

	public:
		static Array<ROSE::ZSC<Mesh1TexEntity>*> mModelLists[MAX_GENDER];
	};
};

#endif