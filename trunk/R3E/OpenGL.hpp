#ifndef OPENGL_H
#define OPENGL_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/glext.h>
#include <IL/il.h>
#include <IL/ilut.h>

#include "String.hpp"
#include "FileSystem.hpp"

#define OGLEXT(x, y) if(!y) y = (x)wglGetProcAddress(#y);

class OpenGL {
private:
	static OpenGL mInstance;

public:
	OpenGL(){}
	~OpenGL(){}

	static void InitGL(){
		OGLEXT(PFNGLCLIENTACTIVETEXTUREPROC, glClientActiveTexture);
		OGLEXT(PFNGLACTIVETEXTUREPROC, glActiveTexture);

		OGLEXT(PFNGLDELETEBUFFERSARBPROC, glDeleteBuffers);
		OGLEXT(PFNGLGENBUFFERSPROC, glGenBuffers);
		OGLEXT(PFNGLBINDBUFFERPROC, glBindBuffer);
		OGLEXT(PFNGLBUFFERDATAPROC, glBufferData);

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glShadeModel(GL_SMOOTH);

		OpenGL::ClientActiveTexture(GL_TEXTURE0_ARB);
	}

	static void BindBuffer(GLenum target, GLuint buffer){
		glBindBuffer(target, buffer);
	}

	static void BufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage){
		glBufferData(target, size, data, usage);
	}

	static void GenBuffers(GLsizei n, GLuint *buffers){
		glGenBuffers(n, buffers);
	}

	static void ClientActiveTexture(GLenum target){
		glClientActiveTexture(target);
	}

	static void ActiveTexture(GLenum target){
		glActiveTexture(target);
	}

	static void DeleteBuffers(GLsizei n, const GLuint *buffers){
		glDeleteBuffers(n, buffers);
	}

	static GLuint LoadTexture(const char* pathV, int* width = NULL, int* height = NULL){
		String path = pathV;
		FILE_SYS()->GetFullPath(path);

		static bool hasInitIL = false;
		if(!hasInitIL){
			ilInit();
			ilutRenderer(ILUT_OPENGL);
			hasInitIL = true;
		}

		ILuint ilTex;
		ilGenImages(1, &ilTex);
		ilBindImage(ilTex);
		if(!ilLoadImage(path)) return GL_INVALID_INDEX;
		if(width) *width = ilGetInteger(IL_IMAGE_WIDTH);
		if(height) *height = ilGetInteger(IL_IMAGE_HEIGHT);
		iluFlipImage();
		GLuint tmpTex = ilutGLBindTexImage();
		ilDeleteImages(1, &ilTex);
		return tmpTex;
	}

	static void UnloadTexture(GLuint& tex){
		if(tex == GL_INVALID_INDEX) return;
		glDeleteTextures(1, &tex);
		tex = GL_INVALID_INDEX;
	}

	static void UnloadBuffer(GLuint& buf){
		if(buf == GL_INVALID_INDEX) return;
		glDeleteBuffers(1, &buf);
		buf = GL_INVALID_INDEX;
	}

private:
	static PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
	static PFNGLACTIVETEXTUREPROC glActiveTexture;

	static PFNGLGENBUFFERSPROC glGenBuffers;
	static PFNGLBINDBUFFERPROC glBindBuffer;
	static PFNGLBUFFERDATAPROC glBufferData;
	static PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
};

#endif
