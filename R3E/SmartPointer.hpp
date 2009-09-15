#ifndef SMART_POINTER_H
#define SMART_POINTER_H

#include "SmartObject.hpp"

template<class T> class SmartPointer {
public:
	SmartPointer(){
		mObject = 0;
	}

	SmartPointer(SmartObject& obj){
		mObject = &obj;
		mObject->AddReference();
	}

	SmartPointer(SmartObject* obj){
		mObject = obj;
		mObject->AddReference();
	}

	~SmartPointer(){
		if(mObject)
			mObject->DeleteReference();
	}

	T* operator->(){
		return (T*)mObject->Get();
	}

	T* Get(){
		return (T*)mObject->Get();
	}

	SmartPointer<T>& operator=(const SmartPointer<T>& rhs){
		if(mObject) mObject->DeleteReference();
		mObject = rhs.mObject;
		mObject->AddReference();
		return (*this);
	}

private:
	SmartObject* mObject;
};

#endif