#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include "Array.hpp"

#include "OpenGL.hpp"

class IIndexBuffer {
public:
	IIndexBuffer(){}
	virtual ~IIndexBuffer(){}

	virtual void AddIndex(void* index) = 0;

	virtual char* Buffer() = 0;

	virtual unsigned int Size() = 0;
	virtual unsigned int BufferSize() = 0;

	virtual void SetSize(int indices) = 0;
	virtual void SetCount(int indices) = 0;

	virtual void Draw() = 0;
};

template <class T> class IndexBuffer : public IIndexBuffer {
public:
	IndexBuffer() : mBufferID(GL_INVALID_INDEX) {}
	~IndexBuffer(){
		mIndices.clear();

		if(mBufferID != GL_INVALID_INDEX)
			OpenGL::DeleteBuffers(1, &mBufferID);
	}

	void SetSize(int indices){
		mIndices.resize(indices);
	}

	void SetCount(int indices){
		mIndices.setCount(indices);
	}

	void AddIndex(void* index){
		mIndices.push_back(*((T*)index));
	}

	unsigned int Size(){
		return mIndices.size();
	}

	unsigned int BufferSize(){
		return Size() * sizeof(T);
	}

	char* Buffer(){
		return (char*)mIndices.GetData();
	}

	void Draw(){
		if(mBufferID == GL_INVALID_INDEX) LoadBuffer();
		OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
		glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, 0);
	}

private:
	bool LoadBuffer(){
		OpenGL::GenBuffers(1, &mBufferID);
		if(!mBufferID){
			mBufferID = GL_INVALID_INDEX;
			return false;
		}

		OpenGL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferID);
		OpenGL::BufferData(GL_ELEMENT_ARRAY_BUFFER, BufferSize(), Buffer(), GL_STATIC_DRAW);
		return true;
	}

private:
	Array<T> mIndices;
	GLuint mBufferID;
};

#endif