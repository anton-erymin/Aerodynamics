#pragma once

#include "core.h"

#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"


const char *APPLICATION_NAME = "Aerodynamics Demo 2.0";


// ������ ����
int			S_WIDTH;
// ������ ����
int			S_HEIGHT;


// ����������� ��������������� �������� ��� ���������
#define			VECTOR_SCALE			0.0001f

// �������� ������ �����
#define			FLOOR_SIZE				10000.0f

// ������ �������� ����������� ����� �� ������
#define			CONTROL_RADIUS			200
#define			CONTROL_RADIUS2			40000


// ����������� ��������������� ������ (����������� ��� ������ ������)
#define			MODEL_SCALE				0.00003f


struct
{
	// ������� ����
	int mainWindow;
	// ���������
	char caption[256];
	char glinfo[256];

	// ����� ����������
	bool keymap[256];

	struct
	{
		// �����
		int oldx, oldy;
		bool leftBtnPressed;
		bool rightBtnPressed;
	} mouse;

	// ��� �����
	bool run;

} wnd;


struct
{
	// ��� ����������� �����
	int cur;
	float z;
	float alphaX, alphaY;

	// ��� ������ � �����
	lpVec3 cam5_eye, cam5_tar;

} camera;





// ������ ������� �����
// ���������� ������������� ��� �� �������
const float dt = 1.0f / 60.0f;
unsigned long newTime, lastTime;
float accTime, frameTime, dtime;



struct
{
	int controlCenterX, controlCenterY, controlX, controlY;
	Controls ctrls;
} hud;




extern Airplane ap;
extern float rho;


#define		SHADER_MAIN		0
#define		SHADER_SHADOW	1
#define		SHADER_CUBEMAP	2

struct
{
	Mesh airplaneMesh;
	Mesh land;

	Terrain terrain;

	SkyBox skybox;

	ShaderProgram shaders[3];

	GLubyte curshader;

	glm::mat4 p_mat;
	glm::mat4 camera_mat;
	glm::mat4 mv_mat;
} gr;


ShadowMap shmap;



// �����������
void draw();
void reshape(int, int);
void idle();
void keyboard(unsigned char, int, int);
void keyboardUp(unsigned char, int, int);
void mouse(int, int, int, int);
void motion(int, int);
void passiveMotion(int, int);


void initGL();
void init();

void drawVectors();