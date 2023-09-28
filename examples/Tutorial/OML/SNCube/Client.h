#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <cell/pad/libpad.h>
#include <sys/paths.h>
#include <PSGL/psgl.h>
#include <cg/cg.h>

class Mesh;

#define VERTEX_SHADER (SYS_APP_HOME"/vertex.cgelf")
#define FRAGMENT_SHADER (SYS_APP_HOME"/fragment.cgelf")
#define CUBE_TEXTURE (SYS_APP_HOME"/sn_logo_dark_256.png")

class Client
{
public:
	Client();
	~Client();

	const char *GetLastError();
	int Init();
	int Term();

	void Render();
	void Update();
	bool Running() const;

private:

	int InitGfx();
	int TermGfx();
	int InitPad();
	int TermPad();
	void InitObjects();

	// void PrintMatrix(GLfloat m[16]) const;

	const char *m_lastError;
	int m_runLevel;

	PSGLdevice *m_pDevice;
	PSGLcontext *m_pContext;
	CGcontext m_cgContext;
	CGprogram m_vsProgram;
	CGprogram m_fsProgram;
	CGparameter m_cgpMvp;
	CGparameter m_cgpTexture;

	typedef std::vector<Mesh *> ObjectContainer;
	ObjectContainer m_objects;

	typedef std::vector<GLuint> TextureContainer;
	TextureContainer m_textures;

	CellPadInfo2 m_padInfo;
	CellPadData m_padData;

};

inline bool Client::Running() const
{
	return m_runLevel != 0;
}

#endif
