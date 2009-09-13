#ifndef CAMERA_H
#define CAMERA_H

#include "Matrix4.hpp"
#include "Frustum.hpp"

class Camera {
public:
	Camera(){}
	virtual ~Camera(){}

	virtual bool UpdateTransform() = 0;

	bool Apply(){
		if(!UpdateTransform()) return false;
		mTransform.glLoad();
		return true;
	}

	Frustum GetFrustum() const {
		return Frustum::CreateFromMatrix(mTransform * mProjectionMatrix);
	}

	void UpdateProjectionMatrix(){
		glGetFloatv(GL_PROJECTION_MATRIX, &mProjectionMatrix.m[0][0]);
	}

protected:
	Matrix4 mTransform;
	Matrix4 mProjectionMatrix;
};

#endif