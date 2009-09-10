#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

#include "Vector2.hpp"
#include "Vector3.hpp"

#include "OpenGL.hpp"

struct V3F {
	Vector3 pos;

	static void Bind(){
		glVertexPointer(3, GL_FLOAT, sizeof(V3F), 0);
	}
};

struct V3F_UV0 {
	Vector3 pos;
	Vector2 uv0;

	static void Bind(){
		glVertexPointer(3, GL_FLOAT, sizeof(V3F_UV0), 0);
		glTexCoordPointer(2, GL_FLOAT, sizeof(V3F_UV0), (void*)sizeof(Vector3));
	}
};

struct V3F_UV0_UV1 {
	Vector3 pos;
	Vector2 uv0;
	Vector2 uv1;

	static void Bind(){
		glVertexPointer(3, GL_FLOAT, sizeof(V3F_UV0_UV1), 0);

		glTexCoordPointer(2, GL_FLOAT, sizeof(V3F_UV0_UV1), (void*)sizeof(Vector3));

		OpenGL::ClientActiveTexture(GL_TEXTURE1_ARB);
		glTexCoordPointer(2, GL_FLOAT, sizeof(V3F_UV0_UV1), (void*)sizeof(V3F_UV0));

		OpenGL::ClientActiveTexture(GL_TEXTURE0_ARB);
	}
};

#endif