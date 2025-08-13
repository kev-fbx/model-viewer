#pragma once
#include <glad/glad.h>
#include <vector>

#include "Renderer.h"

struct VertexAttribute
{
	unsigned int count;
	unsigned int type;
	unsigned char normalised;

	static unsigned int GetSizeOfGLType(unsigned int GLType)
	{
		switch (GLType)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:   return 4;
		case GL_UNSIGNED_BYTE:  return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexAttribute> m_Attributes;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {}

	void Push(unsigned int type, unsigned int count)
	{
		m_Attributes.push_back({type, count, GL_FALSE});
		m_Stride += count * VertexAttribute::GetSizeOfGLType(type);
	} 

	inline unsigned int GetStride() const { return m_Stride; }
	inline std::vector<VertexAttribute> GetAttributes() const { return m_Attributes; }
};

