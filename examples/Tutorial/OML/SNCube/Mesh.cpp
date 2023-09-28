#include "Mesh.h"
#include <assert.h>
#include <PSGL/psgl.h>

Mesh::Mesh()
: m_numVertices(0), m_vbo(0), m_nbo(0), m_texture(0), m_tbo(0)
{
}

Mesh::~Mesh()
{
	if (m_vbo)
		glDeleteBuffers(1, &m_vbo);
	if (m_tbo)
		glDeleteBuffers(1, &m_tbo);
}

void Mesh::Draw() const
{
	if (m_texture)
		glBindTexture(GL_TEXTURE_2D, m_texture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (m_tbo)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	glDrawArrays(GL_TRIANGLES, 0, m_numVertices);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh::Update()
{
	Vectormath::Aos::Matrix4 translate = Vectormath::Aos::Matrix4::translation(m_position);
	m_lwMtx = translate;
}

void Mesh::SetData(const int numVertices, const float *pVerts, const float *pUV, const float *pNorm)
{
	m_numVertices = numVertices;

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_numVertices * 3 * sizeof(float), pVerts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (pUV)
	{
		glGenBuffers(1, &m_tbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
		glBufferData(GL_ARRAY_BUFFER, m_numVertices * 2 * sizeof(float), pUV, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (pNorm)
	{
		glGenBuffers(1, &m_nbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
		glBufferData(GL_ARRAY_BUFFER, m_numVertices * 2 * sizeof(float), pNorm, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

Vectormath::Aos::Matrix4 Mesh::GetWorldTransform() const
{
	return Vectormath::Aos::Matrix4::identity();
}

