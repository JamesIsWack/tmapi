#ifndef MESH_H
#define MESH_H

#include <vectormath/cpp/vectormath_aos.h>
#include <PSGL/psgl.h>

class Mesh
{
public:

	Mesh();
	~Mesh();

	void Draw() const;
	void Update();
	void SetData(const int numVertices, const float *pVerts, const float *pUV, const float *pNorm);
	GLuint GetTexture() const;
	void SetTexture(GLuint t);
	void SetPosition(const Vectormath::Aos::Vector3 &p);
	Vectormath::Aos::Matrix4 GetWorldTransform() const;

private:
	unsigned int m_numVertices;
	GLuint m_vbo;

	GLuint m_nbo;

	GLuint m_texture;
	GLuint m_tbo;

	Vectormath::Aos::Matrix4 m_lwMtx;
	Vectormath::Aos::Vector3 m_position;
};

inline GLuint Mesh::GetTexture() const
{
	return m_texture;
}

inline void Mesh::SetTexture(GLuint t)
{
	m_texture = t;
}

inline void Mesh::SetPosition(const Vectormath::Aos::Vector3 &p)
{
	m_position = p;
}

#endif
