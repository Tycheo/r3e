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

		OGLEXT(PFNGLATTACHSHADERPROC, glAttachShader);
		OGLEXT(PFNGLCOMPILESHADERPROC, glCompileShader);
		OGLEXT(PFNGLCREATEPROGRAMPROC, glCreateProgram);
		OGLEXT(PFNGLCREATESHADERPROC, glCreateShader);
		OGLEXT(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
		OGLEXT(PFNGLDELETESHADERPROC, glDeleteShader);
		OGLEXT(PFNGLLINKPROGRAMPROC, glLinkProgram);
		OGLEXT(PFNGLSHADERSOURCEPROC, glShaderSource);
		OGLEXT(PFNGLUSEPROGRAMPROC, glUseProgram);
		OGLEXT(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
		OGLEXT(PFNGLVERTEXATTRIB4FVARBPROC, glVertexAttrib4fvARB);
		OGLEXT(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
		OGLEXT(PFNGLUNIFORMMATRIX4FVARBPROC, glUniformMatrix4fvARB);
		OGLEXT(PFNGLUNIFORM1IPROC, glUniform1i);

		OGLEXT(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
		OGLEXT(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
		OGLEXT(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

		glShadeModel(GL_SMOOTH);

		OpenGL::ClientActiveTexture(GL_TEXTURE0_ARB);
	}

	static void VertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer){
		glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	}

	static void UniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value){
		glUniformMatrix4fvARB(location, count, transpose, value);
	}

	static GLint GetAttribLocation(GLhandleARB programObj, const GLcharARB *name){
		return glGetAttribLocation(programObj, name);
	}

	static void VertexAttrib4fv(GLuint index, const GLfloat *v){
		glVertexAttrib4fvARB(index, v);
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

	static GLint GetUniformLocation(GLuint program, const GLchar *name){
		return glGetUniformLocation(program, name);
	}

	static void AttachShader(GLuint program, GLuint shader){
		glAttachShader(program, shader);
	}

	static void CompileShader(GLuint shader){
		glCompileShader(shader);
	}

	static GLuint CreateProgram(){
		return glCreateProgram();
	}

	static GLuint CreateShader(GLenum type){
		return glCreateShader(type);
	}

	static void DeleteProgram(GLuint& program){
		if(program == GL_INVALID_INDEX) return;
		glDeleteProgram(program);
		program = GL_INVALID_INDEX;
	}

	static void DeleteShader(GLuint& shader){
		if(shader == GL_INVALID_INDEX) return;
		glDeleteShader(shader);
		shader = GL_INVALID_INDEX;
	}

	static void LinkProgram(GLuint program){
		glLinkProgram(program);
	}

	static void ShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint *length){
		glShaderSource(shader, count, string, length);
	}

	static void UseProgram(GLuint program){
		glUseProgram(program);
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

	static void EnableVertexAttribArray(GLuint index){
		glEnableVertexAttribArray(index);
	}

	static void DisableVertexAttribArray(GLuint index){
		glDisableVertexAttribArray(index);
	}

	static void Uniform1i(GLint location, GLint v0){
		glUniform1i(location, v0);
	}

private:
	static PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
	static PFNGLACTIVETEXTUREPROC glActiveTexture;

	static PFNGLGENBUFFERSPROC glGenBuffers;
	static PFNGLBINDBUFFERPROC glBindBuffer;
	static PFNGLBUFFERDATAPROC glBufferData;
	static PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;

	static PFNGLATTACHSHADERPROC glAttachShader;
	static PFNGLCOMPILESHADERPROC glCompileShader;
	static PFNGLCREATEPROGRAMPROC glCreateProgram;
	static PFNGLCREATESHADERPROC glCreateShader;
	static PFNGLDELETEPROGRAMPROC glDeleteProgram;
	static PFNGLDELETESHADERPROC glDeleteShader;
	static PFNGLLINKPROGRAMPROC glLinkProgram;
	static PFNGLSHADERSOURCEPROC glShaderSource;
	static PFNGLUSEPROGRAMPROC glUseProgram;
	static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	static PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
	static PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
	static PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB;
	static PFNGLUNIFORM1IPROC glUniform1i;

	static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	static PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
};

#endif
