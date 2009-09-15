#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Resource.hpp"
#include "SmartPointer.hpp"
#include "FastStringMapL2.hpp"

template<class T> class ResourceManager {
public:
	ResourceManager(){}
	~ResourceManager(){
		mResources.delete_values();
	}

	static ResourceManager<T>& Instance(){
		return mInstance;
	}
	
	SmartPointer<T> Load(const char* path){
		FastStringMapL2<Resource*>::Entry* entry = mResources.find(path);
		Resource* res;
		if(!entry){
			res = new T();
			res->Create(path);
			mResources.add(path, res);
		}else{
			res = entry->GetValue();
		}

		return SmartPointer<T>(res);
	}

private:
	static ResourceManager<T> mInstance;
	FastStringMapL2<Resource*> mResources;
};

#endif