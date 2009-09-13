#ifndef SKIN_SHADER_DATA
#define SKIN_SHADER_DATA

#include "OpenGL.hpp"

class SkinShaderData {
public:
	static void LoadFromShader(GLint programObj){
		mBoneLoc = OpenGL::GetAttribLocation(programObj, "bones");
		mWeightLoc = OpenGL::GetAttribLocation(programObj, "weights");
		mBMatLoc = OpenGL::GetUniformLocation(programObj, "bmats");
		mBindBoneLoc = OpenGL::GetUniformLocation(programObj, "bindBone");
	}

public:
	static GLint mBoneLoc;
	static GLint mWeightLoc;
	static GLint mBMatLoc;
	static GLint mBindBoneLoc;
};

#endif