#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H

#include "Vector2.hpp"
#include "Vector3.hpp"

#include "OpenGL.hpp"
#include "SkinShaderData.hpp"

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
		glTexCoordPointer(2, GL_FLOAT, sizeof(V3F_UV0), (void*)sizeof(V3F));
	}
};

struct V3F_UV0_BD {
	Vector3 pos;
	Vector2 uv0;
	Vector4 bone;
	Vector4 weight;

	static void Bind(){
		glVertexPointer(3, GL_FLOAT, sizeof(V3F_UV0_BD), 0);
		glTexCoordPointer(2, GL_FLOAT, sizeof(V3F_UV0_BD), (void*)sizeof(V3F));
		OpenGL::VertexAttribPointer(SkinShaderData::mBoneLoc, 4, GL_FLOAT, GL_FALSE, sizeof(V3F_UV0_BD), (void*)sizeof(V3F_UV0));
		OpenGL::VertexAttribPointer(SkinShaderData::mWeightLoc, 4, GL_FLOAT, GL_FALSE, sizeof(V3F_UV0_BD), (void*)(sizeof(V3F_UV0) + sizeof(Vector4)));
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