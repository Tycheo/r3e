#ifndef TARGET_CAMERA_H
#define TARGET_CAMERA_H

#include "Camera.hpp"

class TargetCamera : public Camera {
public:
	TargetCamera(){
		mEye = Vector3(0.0f);
		mUp = Vector3(0.0f, 0.0f, 1.0f);

		mTarget = Vector3(0.0f, 0.0f, 1.0f);
		mDistance = 5.0f;
		mVRotate = 1.32f;
		mHRotate = 5.28f;

		mHasChanges = true;
	}

	virtual ~TargetCamera(){}

	void SetTarget(Vector3 target){
		mTarget = target;
		mHasChanges = true;
	}

	void SetDistance(float dist){
		mDistance = dist;
		mHasChanges = true;
	}

	void SetHRotate(float rotate){
		mHRotate = rotate;
		mHasChanges = true;
	}

	void SetVRotate(float rotate){
		mHRotate = rotate;
		mHasChanges = true;
	}

	virtual bool UpdateTransform(){
		if(!mHasChanges) return false;
		UpdateEye();
		mTransform = Matrix4::CreateLookAtRH(mEye, mTarget, mUp);
		mHasChanges = false;
		return true;
	}

	void AddDistance(float dist){
		mDistance += dist;
		mHasChanges = true;
	}

	void RotateV(float amount){
		mVRotate += amount;
		mHasChanges = true;
	}

	void RotateH(float amount){
		mHRotate += amount;
		mHasChanges = true;
	}

private:
	void UpdateEye(){
		mEye.x = mTarget.x + (mDistance * sin(mVRotate) * cos(mHRotate));
		mEye.y = mTarget.y + (mDistance * sin(mVRotate) * sin(mHRotate));
		mEye.z = mTarget.z + (mDistance * cos(mVRotate));
	}

private:
	bool mHasChanges;

	Vector3 mEye;
	Vector3 mTarget;
	Vector3 mUp;

	float mDistance;
	float mHRotate;
	float mVRotate;
};

#endif