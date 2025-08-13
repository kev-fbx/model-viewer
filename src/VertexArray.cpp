#include "VertexArray.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RenderedID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RenderedID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	const auto& attributes = layout.GetAttributes();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < attributes.size(); i++)
	{
		const auto& attribute = attributes[i];

		// position attribute
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, attribute.count, attribute.type, attribute.normalised, layout.GetStride(), (const void*)offset));
		offset += attribute.count * VertexAttribute::GetSizeOfGLType(attribute.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RenderedID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}