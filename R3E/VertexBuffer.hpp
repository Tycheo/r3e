#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "Array.hpp"
#include "OpenGL.hpp"

class IVertexBuffer {
public:
	IVertexBuffer(){}
	virtual ~IVertexBuffer(){}

	virtual void AddVertex(void* vertex) = 0;

	virtual char* Buffer() = 0;

	virtual unsigned int Size() = 0;
	virtual unsigned int BufferSize() = 0;

	virtual void SetSize(int vertices) = 0;
	virtual void SetCount(int vertices) = 0;

	virtual void Bind() = 0;
};

template<class T> class VertexBuffer : public IVertexBuffer {
public:
	VertexBuffer() : mBufferID(GL_INVALID_INDEX) {}
	~VertexBuffer(){
		mVertices.clear();
		OpenGL::UnloadBuffer(mBufferID);
	}

	void SetSize(int vertices){
		mVertices.resize(vertices);
	}

	void SetCount(int vertices){
		mVertices.setCount(vertices);
	}

	void AddVertex(void* vertex){
		mVertices.push_back(*((T*)vertex));
	}

	unsigned int Size(){
		return mVertices.size();
	}

	unsigned int BufferSize(){
		return Size() * sizeof(T);
	}

	char* Buffer(){
		return (char*)mVertices.GetData();
	}

	void Bind(){
		if(mBufferID == GL_INVALID_INDEX) LoadBuffer();
		OpenGL::BindBuffer(GL_ARRAY_BUFFER, mBufferID);
		T::Bind();
		OpenGL::mVertexCount += mVertices.size();
	}

private:
	bool LoadBuffer(){
		OpenGL::GenBuffers(1, &mBufferID);
		if(!mBufferID){
			mBufferID = GL_INVALID_INDEX;
			return false;
		}

		OpenGL::BindBuffer(GL_ARRAY_BUFFER, mBufferID);
		OpenGL::BufferData(GL_ARRAY_BUFFER, BufferSize(), Buffer(), GL_STATIC_DRAW);
		return true;
	}

private:
	Array<T> mVertices;
	GLuint mBufferID;
};

#endif