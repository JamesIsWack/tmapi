#include <stdio.h>
#include <cell/pad/libpad.h>
#include <sys/spu_initialize.h>
#include <vectormath/cpp/vectormath_aos.h>
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include "Client.h"
#include "cube.h"
#include "Mesh.h"
#include "util.h"

//void Client::PrintMatrix(GLfloat m[16]) const
//{
//	printf("[%3.3f, %3.3f, %3.3f, %3.3f]\n", m[0], m[4], m[8], m[12]);
//	printf("[%3.3f, %3.3f, %3.3f, %3.3f]\n", m[1], m[5], m[9], m[13]);
//	printf("[%3.3f, %3.3f, %3.3f, %3.3f]\n", m[2], m[6], m[10], m[14]);
//	printf("[%3.3f, %3.3f, %3.3f, %3.3f]\n\n", m[3], m[7], m[11], m[15]);
//}

Client::Client()
: m_runLevel(9)
{
}

Client::~Client()
{
	TermGfx();
	TermPad();
	Term();
}

int Client::Init()
{
	sys_spu_initialize(6, 1);

	if (InitGfx() != 0)
		return 1;

	if (InitPad() != 0)
		return 1;

	InitObjects();

	return 0;
}

int Client::Term()
{
	{
		ObjectContainer::iterator iter = m_objects.begin();
		while (iter != m_objects.end())
		{
			delete *iter;
			++iter;
		}
	}

	{
		TextureContainer::iterator iter = m_textures.begin();
		while (iter != m_textures.end())
		{
			glDeleteTextures(1, &(*iter));
			++iter;
		}
	}

	return 0;
}

void Client::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
	cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));

	cgGLEnableTextureParameter(m_cgpTexture);

	ObjectContainer::const_iterator object = m_objects.begin();

	// printf("Drawing %d objects.\n", m_objects.size());

	while (object != m_objects.end())
	{
		const Mesh *pMesh = *object;

		glPushMatrix();
		Vectormath::Aos::Matrix4 t = pMesh->GetWorldTransform();
		glMultMatrixf((GLfloat *)&(t));

		//float m[16];
		//glGetFloatv(GL_MODELVIEW_MATRIX, m);
		//PrintMatrix(m);

		cgGLSetStateMatrixParameter(m_cgpMvp, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

		cgGLSetTextureParameter(m_cgpTexture, pMesh->GetTexture());

		pMesh->Draw();
		++object;

		glPopMatrix();
	}

	cgGLDisableTextureParameter(m_cgpTexture);
	cgGLDisableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
	cgGLDisableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));

	psglSwap();
}

void Client::Update()
{
	cellPadGetInfo2(&m_padInfo);
	cellPadGetData(0, &m_padData);

	if (m_padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_START)
	{
		m_runLevel = 0;
		return;
	}

	ObjectContainer::const_iterator object = m_objects.begin();
	while (object != m_objects.end())
	{
		(*object)->Update();
		++object;
	}

}

int Client::InitGfx()
{
	PSGLinitOptions options =
	{
		enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS,
		maxSPUs: 1,
		initializeSPUs: false,
		persistentMemorySize: 0,
		transientMemorySize: 0,
		errorConsole: 0,
		fifoSize: 0,
		hostMemorySize: 128 * 1024 * 1024,
	};

	psglInit(&options);

	m_pDevice = psglCreateDeviceAuto(GL_ARGB_SCE, GL_NONE, GL_MULTISAMPLING_NONE_SCE);
	if (m_pDevice == NULL)
	{
		fprintf(stderr, "psglCreateDeviceAuto failed.");
		return 1;
	}

	m_pContext = psglCreateContext();
	if (m_pContext == NULL)
	{
		fprintf(stderr, "psglCreateContext failed.");
		return 1;
	}

	psglMakeCurrent(m_pContext, m_pDevice);
	psglResetCurrentContext();

	GLuint width;
	GLuint height;
	GLfloat aspect = psglGetDeviceAspectRatio(m_pDevice);
	psglGetRenderBufferDimensions(m_pDevice, &width, &height);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspectivef(45.0f, aspect, 0.1f, 2000.0f);
	glClearColor(0.5f, 0.65f, 1.0f, 0.0f);
	glClearDepthf(1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VSYNC_SCE);

	printf("Loading shaders... ");
	m_cgContext = cgCreateContext();
	m_vsProgram = cgCreateProgramFromFile(m_cgContext, CG_BINARY, VERTEX_SHADER, cgGLGetLatestProfile(CG_GL_VERTEX), NULL, NULL);
	m_fsProgram = cgCreateProgramFromFile(m_cgContext, CG_BINARY, FRAGMENT_SHADER, cgGLGetLatestProfile(CG_GL_FRAGMENT), NULL, NULL);

	assert(m_vsProgram != NULL && m_fsProgram != NULL);

	cgGLBindProgram(m_vsProgram);
	cgGLBindProgram(m_fsProgram);
	cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
	cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));

	m_cgpMvp = cgGetNamedParameter(m_vsProgram, "mvp");
	m_cgpTexture = cgGetNamedParameter(m_fsProgram, "texture");

	printf("OK.\n");
	return 0;
}

int Client::TermGfx()
{
	cgDestroyParameter(m_cgpMvp);
	cgDestroyParameter(m_cgpTexture);

	cgDestroyProgram(m_vsProgram);
	cgDestroyProgram(m_fsProgram);

	cgDestroyContext(m_cgContext);
	psglDestroyContext(m_pContext);
	psglDestroyDevice(m_pDevice);
	psglExit();
	return 0;
}

int Client::InitPad()
{
	uint32_t ret;
	ret = cellPadInit(1);
	if (ret != CELL_OK)
	{
		fprintf(stderr, "cellPadInit failed (0x%08x).\n", ret);
		return 1;
	}
	return 0;
}

int Client::TermPad()
{
	cellPadEnd();
	return 0;
}

void Client::InitObjects()
{
	printf("Loading texture: %s...", CUBE_TEXTURE);
	int width;
	int height;
	uint8_t *png = LoadPNG(CUBE_TEXTURE, &width, &height);
	GLuint texture;
	glGenTextures(1, &texture);
	m_textures.push_back(texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, png);
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] png;

	printf("OK.\n");

	Mesh *pMesh = new Mesh;
	pMesh->SetData(NUM_CUBE_VERTS, cubeVertices, cubeUV, NULL);
	pMesh->SetTexture(texture);
	pMesh->SetPosition(Vectormath::Aos::Vector3(0.0, -2.0, -10.0));
	m_objects.push_back(pMesh);
}
