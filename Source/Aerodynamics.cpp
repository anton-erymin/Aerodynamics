// Aerodynamics.cpp : Defines the entry point for the console application.
//

// test.cpp : Defines the entry point for the console application.
//

#include "graphics.h"
#include "terrain.h"

#include <gl/glut.h>

#include <math.h>

#include "Aerodynamics.h"

#include <iostream>
#include <algorithm>
#include <map>

#include <vector>

bool wireframe = false;



int main(int argc, char* argv[])
{

	glutInit(&argc, argv);
	glutInitWindowSize(1280, 1024);
	glutInitWindowPosition(300, 150);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	wnd.mainWindow = glutCreateWindow(APPLICATION_NAME);

	printf("OpenGL window created.\n");

	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);

	//glutFullScreen();

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Can't initialize GLEW\n");
		return 1;
	}

	printf("GLEW initialized.\n");


	if (!GLEW_VERSION_2_1)
	{
		fprintf(stderr, "OpenGL 2.1 not available\n");
		return 1;
	}

	const GLubyte *sv = glGetString(GL_VENDOR);
	const GLubyte *sr = glGetString(GL_RENDERER);
	const GLubyte *sver = glGetString(GL_VERSION);
	sprintf(wnd.glinfo, "%s %s OpenGL %s", sv, sr, sver);

	printf("%s\n", wnd.glinfo);

	initGL();
	init();
	initPhysics();

	glutMainLoop();

	return 0;
}


// Инициализация OpenGL
void initGL()
{
	// Цвет очистки буфера
	glClearColor(0.7f, 0.8f, 1.0f, 0.0f);

	// Включаем тест глубины
	glEnable(GL_DEPTH_TEST);

	// Сглаживание точек
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10.0f);

	glEnable(GL_CULL_FACE);

	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderProgram shader;

	bool res = make_program("Shaders\\phong_per_fragment.v.glsl", "Shaders\\phong_per_fragment.f.glsl", shader);

	if (!res)
	{
		fprintf(stderr, "Failed to make a shader program phong_per_fragment.\n");
	}
	else printf("Phong shader compiled.\n");

	shader.attribs.position = glGetAttribLocation(shader.program, "position");
	shader.attribs.normal	 = glGetAttribLocation(shader.program, "normal");
	shader.attribs.texcoord = glGetAttribLocation(shader.program, "texcoord");

	shader.uniforms.p_mat   = glGetUniformLocation(shader.program, "p_mat");
	shader.uniforms.mv_mat  = glGetUniformLocation(shader.program, "mv_mat");
	shader.uniforms.cam_mat  = glGetUniformLocation(shader.program, "cam_mat");
	shader.uniforms.shadow_mat  = glGetUniformLocation(shader.program, "shadow_mat");
	
	shader.uniforms.diffuse_color  = glGetUniformLocation(shader.program, "diffuse_color");
	shader.uniforms.specular_color  = glGetUniformLocation(shader.program, "specular_color");
	shader.uniforms.ambient_color  = glGetUniformLocation(shader.program, "ambient_color");
	shader.uniforms.shininess  = glGetUniformLocation(shader.program, "shininess");

	shader.uniforms.shadowmap  = glGetUniformLocation(shader.program, "shadowmap");

	gr.shaders[SHADER_MAIN] = shader;



	res = make_program("Shaders\\shadowmap.v.glsl", "", shader);


	if (!res)
	{
		fprintf(stderr, "Failed to make a shader program shadowmap.\n");
	}
	else printf("Shadow shader compiled.\n");


	shader.attribs.position = glGetAttribLocation(shader.program, "position");

	shader.uniforms.mv_mat  = glGetUniformLocation(shader.program, "mv_mat");
	shader.uniforms.p_mat  = glGetUniformLocation(shader.program, "p_mat");

	gr.shaders[SHADER_SHADOW] = shader;




	res = make_program("Shaders\\cubemap.v.glsl", "Shaders\\cubemap.f.glsl", shader);
	if (!res)
	{
		fprintf(stderr, "Failed to make a shader program cubemap.\n");
	}
	else printf("Cubemap shader compiled.\n");

	shader.attribs.position = glGetAttribLocation(shader.program, "position");
	shader.attribs.texcoord = glGetAttribLocation(shader.program, "texcoord");

	shader.uniforms.mv_mat  = glGetUniformLocation(shader.program, "mv_mat");
	shader.uniforms.p_mat  = glGetUniformLocation(shader.program, "p_mat");
	shader.uniforms.texture  = glGetUniformLocation(shader.program, "texture");

	gr.shaders[SHADER_CUBEMAP] = shader;





	shmap.framebuffer = make_framebuffer(shmap.depthTexture);
	if (!shmap.framebuffer)
	{
		fprintf(stderr, "Failed to make a depth FBO.\n");
	}
	else printf("FBO created.\n");
}


void init()
{
	// Загружаем модель самолета
	OBJModel obj;
	bool res = loadOBJ("Data\\aircraft2\\model.obj", obj);

	if (!res) 
	{
		printf("Loading aircraft model failed.\n");
	} 
	else printf("Aircraft model loaded.\n");
	
	// Конвертируем во внутренний формат
	convertOBJIndexed(obj, gr.airplaneMesh, MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);
	// Загружаем в видюху
	make_mesh_buffers(gr.airplaneMesh);

	// Строим ландшафт из карты высот
	res = make_terrain_fromheightmap(gr.terrain, "Data\\ps_height_1k.bmp", 100000.0f, 8000.0f, 0);
	if (!res) 
	{
		printf("Loading terrain from bitmap failed.\n");
		exit(1);
	} 
	else printf("Terrain loaded.\n");

	// Создаем скайбокс
	const char *sky_sides[6] = { "Data\\skybox\\miramar_ft.tga", "Data\\skybox\\miramar_bk.tga", 
	"Data\\skybox\\miramar_up.tga", "Data\\skybox\\miramar_dn.tga", 
	"Data\\skybox\\miramar_rt.tga", "Data\\skybox\\miramar_lf.tga" };
	make_skybox(gr.skybox, (const char**)sky_sides, 10.0f);


	camera.cur = 0;
	camera.z = 30.0f;
	camera.alphaX = 0.0f;
	camera.alphaY = 0.0f;

	wnd.run = false;

	accTime = 0.0f;
	lastTime = GetTickCount();
}



// Обработчик изменения размеров окна
void reshape(int width, int height)
{
	S_WIDTH = glutGet(GLUT_WINDOW_WIDTH);
	S_HEIGHT = glutGet(GLUT_WINDOW_HEIGHT);

	// Вьюпорт на весь экран
	glViewport(0, 0, S_WIDTH, S_HEIGHT);

	gr.p_mat = glm::perspective(45.0f, (GLfloat)S_WIDTH / (GLfloat)S_HEIGHT, 0.1f, 1000000.0f);

	hud.controlCenterX = (int)(S_WIDTH / 4.0f * 3.0f);
	hud.controlCenterY = (int)(S_HEIGHT / 4.0f);
	hud.controlX = hud.controlCenterX;
	hud.controlY = hud.controlCenterY;
}



// Обработка клавиш
void processKeys()
{

	if (wnd.keymap['e'])
	{
		hud.ctrls.thrust += 0.125f * dtime;
		if (hud.ctrls.thrust > 1.0f) hud.ctrls.thrust = 1.0f;
	}
	if (wnd.keymap['q'])
	{
		hud.ctrls.thrust -= 0.125f * dtime;
		if (hud.ctrls.thrust < 0.0f) hud.ctrls.thrust = 0.0f;

	}
	if (wnd.keymap['r'])
	{
		hud.ctrls.thrust = 0.0f;
	}
	if (wnd.keymap['t'])
	{
		hud.ctrls.thrust = 1.0f;
	}



	if (wnd.keymap['s'])
	{
		hud.ctrls.elevator += 0.02f;
		if (hud.ctrls.elevator > 1.0f) hud.ctrls.elevator = 1.0f;
	}
	if (wnd.keymap['w'])
	{
		hud.ctrls.elevator -= 0.02f;
		if (hud.ctrls.elevator < -1.0f) hud.ctrls.elevator = -1.0f;
	}

	/*if (keymap['a'])
	{
		aileronsCoeff += 0.02f;
		if (aileronsCoeff > 1.0f) aileronsCoeff = 1.0f;
	}
	if (keymap['d'])
	{
		aileronsCoeff -= 0.02f;
		if (aileronsCoeff < -1.0f) aileronsCoeff = -1.0f;
	}*/

	if (wnd.keymap['x'])
	{
		hud.ctrls.rudder += 0.5f * dtime;
		if (hud.ctrls.rudder > 1.0f) hud.ctrls.rudder = 1.0f;
	}
	if (wnd.keymap['z'])
	{
		hud.ctrls.rudder -= 0.5f * dtime;
		if (hud.ctrls.rudder < -1.0f) hud.ctrls.rudder = -1.0f;
	}

	if (wnd.keymap['+'])
	{
		camera.z -= 10000.0f * dtime;
	}

	if (wnd.keymap['-'])
	{
		camera.z += 10000.0f * dtime;
	}


}


void idle()
{
	processKeys();
	
	hud.ctrls.elevator = -(float)(hud.controlY - hud.controlCenterY) / (float)CONTROL_RADIUS;
	hud.ctrls.aileron = -(float)(hud.controlX - hud.controlCenterX) / (float)CONTROL_RADIUS;
	
	ap.ctrls = hud.ctrls;

	terrain_update_lods(gr.terrain, ap.rb->m_pos.m_x, ap.rb->m_pos.m_z);


	// Отрисовка сцены
	glutPostRedisplay();
	

	newTime = GetTickCount();
	frameTime = (newTime - lastTime) * 0.001f;
	lastTime = newTime;
	

	float ft = frameTime; 
	dtime = frameTime;

	// Расчет FPS

	static int lastUpdate = 0;
	static int frames = 0;

	int currentTime = GetTickCount();
	frames++;

	if (currentTime - lastUpdate >= 1000)
	{
		sprintf(wnd.caption, "%s [FPS: %f, FTime: %f s.] %s", APPLICATION_NAME, (float)frames * 1000 / (currentTime - lastUpdate), ft, wnd.glinfo);
		glutSetWindowTitle(wnd.caption);
		frames = 0;
		lastUpdate = currentTime;
	}
	

	if (wnd.run)
	{
		if (frameTime > 0.1f) frameTime = 0.1f;
		accTime += frameTime;
		
		calcPhysics();
		step(dt, accTime);
	}

	
}


void keyboard(unsigned char key, int x, int y)
{
	wnd.keymap[key] = true;

	if (key >= '1' && key <= '9')
	{
		camera.cur = key - '1';

		if (camera.cur == 3)
		{
			float d = 0.1f * ap.rb->m_pos.m_y + 100.0f;
			if (d > 300.0f) d = 300.0f;
			float x = rand() % (int)(2 * d) - d;
			float z = rand() % (int)(2 * d) - d;
			camera.cam5_tar = ap.rb->m_pos;
			camera.cam5_eye = ap.rb->m_pos;
			camera.cam5_eye.m_y += rand() % (int)(2 * 300) - 300;
			camera.cam5_eye.m_x += x;
			camera.cam5_eye.m_z += z;


			camera.cam5_eye = ap.rb->m_pos + ap.rb->m_linearVel * float((rand() % 20));
			camera.cam5_eye.m_y += rand() % (int)(2 * 100) - 50;
		}
	}

	if (key == ' ')
	{
		if (wnd.run)
		{
			wnd.run = false;
		}
		else
		{
			wnd.run = true;
			lastTime = GetTickCount();
		}
	}

	if (key == 'm')
	{
		if (wireframe) wireframe = false;
		else wireframe = true;
	}	
}


void keyboardUp(unsigned char key, int x, int y)
{
	wnd.keymap[key] = false;

	if (key == 27) 
	{
		glutDestroyWindow(wnd.mainWindow);
	}

	if (key == 'z' || key == 'x') hud.ctrls.rudder = 0.0f;
}


void mouse(int btn, int state, int x, int y)
{
	if (btn == 0 || btn == 2)
	{
		if (state == 0)
		{
			if (btn == 0) wnd.mouse.leftBtnPressed = true;
			else wnd.mouse.rightBtnPressed = true;


			wnd.mouse.oldx = x;
			wnd.mouse.oldy = y;
		}
		else
		{
			wnd.mouse.leftBtnPressed = false;
			wnd.mouse.rightBtnPressed = false;
		}
	}
}


void motion(int x, int y)
{
	int dx = x - wnd.mouse.oldx;
	int dy = y - wnd.mouse.oldy;

	if (wnd.mouse.leftBtnPressed)
	{
		wnd.mouse.oldx = x;
		wnd.mouse.oldy = y;

		//alphaX += (float)dx * 0.5f;
		//alphaY += (float)dy * 0.5f;

		int newx = hud.controlX + dx;
		int newy = hud.controlY + dy;


		//float r2 = (newx - controlCenterX) * (newx - controlCenterX) + (newy - controlCenterY) * (newy - controlCenterY);

		/*if (r2 > CONTROL_RADIUS2)
		{

		}
		else
		{
			controlX = newx;
			controlY = newy;
		}*/

		if (fabs((float)(newx - hud.controlCenterX)) < CONTROL_RADIUS) hud.controlX = newx;
		if (fabs((float)(newy - hud.controlCenterY)) < CONTROL_RADIUS) hud.controlY = newy;

	}
	else if (wnd.mouse.rightBtnPressed)
	{
		wnd.mouse.oldx = x;
		wnd.mouse.oldy = y;

		camera.alphaX += (float)dx * 0.01f;
		camera.alphaY += (float)dy * 0.01f;
	}
}


void passiveMotion(int x, int y)
{
	
}






// Отрисовка строки текста на экран
void draw_string(const char* s) 
{
	while (*s) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s++);
}


// Возвращение в режим 3D
void leave2D()
{	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	glUseProgram(gr.shaders[gr.curshader].program);
}


// Переход в режим 2D для написания информации
void enter2D()
{
	glUseProgram(0);

	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// Ортогональная проекция
	glOrtho(0, S_WIDTH, S_HEIGHT, 0.0f, -1000.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
}


// Отрисовка информации
void drawInfo()
{
	glColor3f(0.0f, 0.0f, 0.0f);

	char buf[255];

	glRasterPos2f(20.0f, 20.0f);
	draw_string(wnd.caption);

	glRasterPos2f(20.0f, 60.0f);
	draw_string("Controls: Q, E, R, T - Engine; Z, X - RUDDER, 1-4, +/- - Cameras; Mouse; SPACE - PAUSE; M - Wireframe toggle");

	float margin = 20.0f;
	float basey = 140.0f;

	sprintf(buf, "ALT     %d m", (int)ap.rb->m_pos.m_y);
	glRasterPos2f(20.0f, basey);
	draw_string(buf);

	sprintf(buf, "LSP     %d km/h", (int)(ap.rb->m_linearVel.norm() / 1000.0f * 3600.0f));
	glRasterPos2f(20.0f, basey + 1 * margin);
	draw_string(buf);

	lpVec3 horVel = ap.rb->m_linearVel;
	horVel.m_y = 0;
	sprintf(buf, "HSP     %d km/h", (int)(horVel.norm() / 1000.0f * 3600.0f));
	glRasterPos2f(20.0f, basey + 2 * margin);
	draw_string(buf);

	sprintf(buf, "VSP     %d m/s", (int)(ap.rb->m_linearVel.m_y));
	glRasterPos2f(20.0f, basey + 3 * margin);
	draw_string(buf);

	sprintf(buf, "THR     %d N", (int)ap.T_mag);
	glRasterPos2f(20.0f, basey + 4 * margin);
	draw_string(buf);

	sprintf(buf, "ALH     %f deg", (ap.alpha));
	glRasterPos2f(20.0f, basey + 5 * margin);
	draw_string(buf);

	sprintf(buf, "ELV     %d %%", (int)(hud.ctrls.elevator * 100.0f));
	glRasterPos2f(20.0f, basey + 6 * margin);
	draw_string(buf);

	sprintf(buf, "ALR     %d %%", (int)(hud.ctrls.aileron * 100.0f));
	glRasterPos2f(20.0f, basey + 7 * margin);
	draw_string(buf);

	sprintf(buf, "RDR     %d %%", (int)(hud.ctrls.rudder * 100.0f));
	glRasterPos2f(20.0f, basey + 8 * margin);
	draw_string(buf);

	sprintf(buf, "B       %f deg", ap.beta);
	glRasterPos2f(20.0f, basey + 9 * margin);
	draw_string(buf);

	sprintf(buf, "ALL     %f deg", (ap.wingAttackAngle_l));
	glRasterPos2f(20.0f, basey + 10 * margin);
	draw_string(buf);

	sprintf(buf, "ALR     %f deg", (ap.wingAttackAngle_r));
	glRasterPos2f(20.0f, basey + 11 * margin);
	draw_string(buf);
	
}


// Отрисовка векторов и центров
//void drawVectors()
//{
//	glDisable(GL_LIGHTING);
//	glDisable(GL_DEPTH_TEST);
//
//	calcAeroCenters();
//
//	lpVec3 v;
//	
//	// Рисуем центры
//	glPointSize(10.0f);
//	glBegin(GL_POINTS);
//		glColor3f(0.0f, 0.8f, 0.0f);
//
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(-FOCUS_WING_X, FOCUS_WING_Y, FOCUS_WING_Z);
//		glVertex3f(-FOCUS_WING_X, FOCUS_WING_Y, -FOCUS_WING_Z);
//		glVertex3f(-FOCUS_STAB_X, FOCUS_STAB_Y, FOCUS_STAB_Z);
//	glEnd();
//
//	
//
//
//	// Вектора
//
//	// Рисуем локальную систему координат
//	/*glLineWidth(0.5f);
//	glEnable(GL_DEPTH_TEST);
//	glBegin(GL_LINES);
//		glColor3f(0.9f, 0.0f, 0.0f);
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(-AIRPLANE_LENGTH, 0.0f, 0.0f);
//		glColor3f(0.0f, 0.9f, 0.0f);
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(0.0f, AIRPLANE_HEIGHT, 0.0f);
//		glColor3f(0.0f, 0.0f, 0.9f);
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(0.0f, 0.0f, AIRPLANE_WIDTH);
//	glEnd();*/
//
//	glDisable(GL_DEPTH_TEST);
//	glLineWidth(2.0f);
//
//
//	glPopMatrix();
//	glPushMatrix();
//	glTranslatef(ap.rb->m_pos.m_x, ap.rb->m_pos.m_y, ap.rb->m_pos.m_z);
//	glBegin(GL_LINES);
//		glColor3f(0.8f, 0.0f, 0.0f);
//		//// Рисуем вектор силы тяжести
//		//glVertex3f(0.0f, 0.0f, 0.0f);
//		//glVertex3f(0.0f, -ap.weightMag * VECTOR_SCALE, 0.0f);
//
//		// Рисуем вектор сопротивления фюзеляжа
//		v = ap.Df * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//
//		// Рисуем вектор тяги
//
//		/*v = ap.T * VECTOR_SCALE;
//		glColor3f(0.0f, 0.0f, 0.8f);
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);*/
//	glEnd();
//	
//	
//	// Рисуем вектора крыльев
//	glPopMatrix();
//	glPushMatrix();
//	glTranslatef(ap.ac1.m_x, ap.ac1.m_y, ap.ac1.m_z);
//	glBegin(GL_LINES);
//		glColor3f(0.8f, 0.8f, 0.0f);
//		v = ap.Lw_l * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//
//		glColor3f(0.8f, 0.0f, 0.0f);
//		v = ap.Dw_l * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//	glEnd();
//
//
//	glPopMatrix();
//	glPushMatrix();
//	glTranslatef(ap.ac2.m_x, ap.ac2.m_y, ap.ac2.m_z);
//	glBegin(GL_LINES);
//		glColor3f(0.8f, 0.8f, 0.0f);
//		v = ap.Lw_r * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//
//		glColor3f(0.8f, 0.0f, 0.0f);
//		v = ap.Dw_r * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//	glEnd();
//
//
//	glPopMatrix();
//	glPushMatrix();
//	glTranslatef(ap.tailPoint.m_x, ap.tailPoint.m_y, ap.tailPoint.m_z);
//	glBegin(GL_LINES);
//		glColor3f(0.8f, 0.8f, 0.0f);
//		v = ap.Lhor * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//
//		v = ap.Lvert * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//
//		glColor3f(0.8f, 0.0f, 0.0f);
//		v = (ap.Dhor + ap.Dvert) * VECTOR_SCALE;
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(v.m_x, v.m_y, v.m_z);
//	glEnd();
//
//
//	glPopMatrix();
//	glPushMatrix();
//
//	if (ap.wing_forces)
//	{
//		glBegin(GL_LINES);
//		for (int i = 0; i < WING_N; i++)
//		{
//			glColor3f(0.8f, 0.8f, 0.0f);
//			v = ap.wing_forces[3 * i];
//			lpVec3 v2 = ap.wing_forces[3 * i + 1];
//			glVertex3f(v.m_x, v.m_y, v.m_z);
//			glVertex3f(v2.m_x, v2.m_y, v2.m_z);
//
//			glColor3f(0.8f, 0.0f, 0.0f);
//			v2 = ap.wing_forces[3 * i + 2];
//			glVertex3f(v.m_x, v.m_y, v.m_z);
//			glVertex3f(v2.m_x, v2.m_y, v2.m_z);
//		}
//		glEnd();
//	}
//
//	glPopMatrix();
//
//	glEnable(GL_LIGHTING);
//	glEnable(GL_DEPTH_TEST);
//}



// Отрисовка HUD
void drawHUD()
{
	glBegin(GL_LINES);
		glVertex2i(hud.controlCenterX - CONTROL_RADIUS, hud.controlCenterY);
		glVertex2i(hud.controlCenterX + CONTROL_RADIUS, hud.controlCenterY);
		glVertex2i(hud.controlCenterX, hud.controlCenterY - CONTROL_RADIUS);
		glVertex2i(hud.controlCenterX, hud.controlCenterY + CONTROL_RADIUS);
	glEnd();
	glBegin(GL_POINTS);
		glVertex2i(hud.controlX, hud.controlY);
	glEnd();
}


// Отрисовка земли
void drawFloor()
{
	glUniform4fv(gr.shaders[gr.curshader].uniforms.diffuse_color, 1, &glm::vec4(0.15f, 0.30f, 0.0f, 1.0f)[0]);
	glUniform4fv(gr.shaders[gr.curshader].uniforms.ambient_color, 1, &glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)[0]);
	glUniform4fv(gr.shaders[gr.curshader].uniforms.specular_color, 1, &glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)[0]);
	glUniform1f(gr.shaders[gr.curshader].uniforms.shininess, 2);
	
	render_terrain(gr.terrain, gr.shaders[gr.curshader]);


}



void getBodyModelMatrix(lpRigidBody *b, glm::mat4 &mv)
{
	mv[0][0] = b->m_orientation.m_data[0][0];
	mv[0][1] = b->m_orientation.m_data[1][0];
	mv[0][2] = b->m_orientation.m_data[2][0];
	mv[0][3] = 0.0f;

	mv[1][0] = b->m_orientation.m_data[0][1];
	mv[1][1] = b->m_orientation.m_data[1][1];
	mv[1][2] = b->m_orientation.m_data[2][1];
	mv[1][3] = 0.0f;

	mv[2][0] = b->m_orientation.m_data[0][2];
	mv[2][1] = b->m_orientation.m_data[1][2];
	mv[2][2] = b->m_orientation.m_data[2][2];
	mv[2][3] = 0.0f;

	mv[3][0] = b->m_pos.m_x;
	mv[3][1] = b->m_pos.m_y;
	mv[3][2] = b->m_pos.m_z;
	mv[3][3] = 1.0f;

	mv = glm::translate(mv, glm::vec3(1.0f, -1.8f, 0.0f));
	mv = glm::rotate(mv, -(float)M_PI_2, glm::vec3(0.0f, 1.0f, 0.0f));
}

// Отрисовка самолета
void drawAirplane()
{
	glCullFace(GL_FRONT);
	render_submeshes(gr.airplaneMesh, gr.shaders[gr.curshader]);

	glCullFace(GL_BACK);
	render_submeshes(gr.airplaneMesh, gr.shaders[gr.curshader]);

	//if (curCamera != 3 && curCamera != 4) drawVectors();
}



void drawSkybox()
{
	glm::mat4 skybox_mat;
	glm::mat4 inv = glm::inverse(gr.camera_mat);
	skybox_mat[3][0] = inv[3][0];
	skybox_mat[3][1] = inv[3][1];
	skybox_mat[3][2] = inv[3][2];

	//skybox_mat[3][0] = ap.rb->m_pos.m_x;
	//skybox_mat[3][1] = ap.rb->m_pos.m_y;
	//skybox_mat[3][2] = ap.rb->m_pos.m_z;


	skybox_mat = gr.p_mat * gr.camera_mat * skybox_mat;


	glUseProgram(gr.shaders[SHADER_CUBEMAP].program);
	gr.curshader = SHADER_CUBEMAP;

	glUniformMatrix4fv(gr.shaders[SHADER_CUBEMAP].uniforms.mv_mat, 1, GL_FALSE, &skybox_mat[0][0]);

    glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, gr.skybox.cubemap);

	glDepthMask(GL_FALSE);
	render_mesh_pos_only(gr.skybox.box, gr.shaders[SHADER_CUBEMAP]);
	glDepthMask(GL_TRUE);
}


void renderShadowmap()
{
	// Рендер карты теней

	// Матрица проекции источника света
	shmap.light_p_mat = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -5.0f, 6000.0f);
	
	glm::vec3 apos(ap.rb->m_pos.m_x, ap.rb->m_pos.m_y, ap.rb->m_pos.m_z);
	glm::vec3 light_dir(-1.0f, -2.3f, -1.4f);

	// Видовая матрица источника света (камера)
	shmap.light_v_mat = glm::lookAt(apos - light_dir, apos, glm::vec3(0,1,0));

	// Произведение видовой матрицы, матрицы проекции и матрицы переводящий из [-1;1] в [0;1]
	shmap.shadow_mat = normalization_mat * shmap.light_p_mat * shmap.light_v_mat;

	

	// Используем специальный шейдер, чтобы не расчитывать свет и прочее
	glUseProgram(gr.shaders[SHADER_SHADOW].program);
	gr.curshader = SHADER_SHADOW;
	

	// Матрица источника света
	gr.mv_mat = shmap.light_p_mat * shmap.light_v_mat;
	glUniformMatrix4fv(gr.shaders[SHADER_SHADOW].uniforms.mv_mat, 1, GL_FALSE, &gr.mv_mat[0][0]);
	

	// Переключаем фреймбуфер
	glBindFramebuffer(GL_FRAMEBUFFER, shmap.framebuffer);

	// Настраиваем вьюпорт
	glViewport(0, 0, 1024, 1024);

	// Очищаем буфер глубины
	glClear(GL_DEPTH_BUFFER_BIT);
	
	
	getBodyModelMatrix(ap.rb, gr.mv_mat);
	gr.mv_mat = shmap.light_p_mat * shmap.light_v_mat * gr.mv_mat;
	glUniformMatrix4fv(gr.shaders[gr.curshader].uniforms.mv_mat, 1, GL_FALSE, &gr.mv_mat[0][0]);

	// Рисуем окклюдеры

	render_mesh_pos_only(gr.airplaneMesh, gr.shaders[gr.curshader]);

	// Переключаем на фреймбуфер окна
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Восстанавливаем вьюпорт
	glViewport(0, 0, S_WIDTH, S_HEIGHT);
}




// Отрисовка сцены
void renderShadowed()
{
	// 1.225 0
	// 0.5   1

	//float k = (1.225 - rho) / (1.225 - 0.5);
	//glClearColor(0.7f - 0.2f * k, 0.8f - 0.1f * k, 1.0f, 1.0f);

	

	glClear(GL_DEPTH_BUFFER_BIT);



	//gr.p_mat = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -5.0f, 100.0f);
	//
	//glm::vec3 apos(ap.rb->m_pos.m_x, ap.rb->m_pos.m_y, ap.rb->m_pos.m_z);
	//glm::vec3 light_dir(-1.0f, -2.3f, -1.4f);

	//// Видовая матрица источника света (камера)
	//gr.camera_mat = glm::lookAt(apos + light_dir, apos, glm::vec3(0,1,0));


	// Рисуем скайбокс
	drawSkybox();

	if (wireframe) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// Включаем обычный шейдер
	gr.curshader = SHADER_MAIN;
	int cursh = gr.curshader;
	glUseProgram(gr.shaders[cursh].program);

	// Матрица проекции в шейдер
	glUniformMatrix4fv(gr.shaders[cursh].uniforms.p_mat, 1, GL_FALSE, &gr.p_mat[0][0]);
	
	// Матрица камеры в шейдер
	glUniformMatrix4fv(gr.shaders[cursh].uniforms.cam_mat, 1, GL_FALSE, &gr.camera_mat[0][0]);



	// Модельвью матрица земли в шейдер
	glUniformMatrix4fv(gr.shaders[cursh].uniforms.mv_mat, 1, GL_FALSE, &gr.camera_mat[0][0]);
	// Теневая матрица земли в шейдер
	glUniformMatrix4fv(gr.shaders[cursh].uniforms.shadow_mat, 1, GL_FALSE, &shmap.shadow_mat[0][0]);


	glBindTexture(GL_TEXTURE_2D, shmap.depthTexture);
	glUniform1i(gr.shaders[cursh].uniforms.shadowmap, 0);

	drawFloor();


	// Получаем модельную матрицу тела
	getBodyModelMatrix(ap.rb, gr.mv_mat);
	// Теневую матрицу
	glm::mat4 sh_mat = shmap.shadow_mat * gr.mv_mat;
	// Умножаем на матрицу камеры
	gr.mv_mat = gr.camera_mat * gr.mv_mat;
	
	// Моделвью матрицу в шейдер
	glUniformMatrix4fv(gr.shaders[cursh].uniforms.mv_mat, 1, GL_FALSE, &gr.mv_mat[0][0]);
	// Теневую матрицу в шейдер
	glUniformMatrix4fv(gr.shaders[cursh].uniforms.shadow_mat, 1, GL_FALSE, &sh_mat[0][0]);

	// Рисуем самолет
	drawAirplane();

	

	if (wireframe) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	

	
}



void draw()
{
	lpVec3 v;

	switch (camera.cur)
	{	
	case 0:
		glTranslatef(0.0f, 0.0f, -camera.z); 
		glRotatef(camera.alphaY, 1.0f, 0.0f, 0.0f); 
		glRotatef(camera.alphaX, 0.0f, 1.0f, 0.0f); 
		glTranslatef(-ap.rb->m_pos.m_x, -ap.rb->m_pos.m_y, -ap.rb->m_pos.m_z);

		gr.camera_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -camera.z));
		gr.camera_mat = glm::rotate(gr.camera_mat, camera.alphaY, glm::vec3(1.0f, 0.0f, 0.0f));
		gr.camera_mat = glm::rotate(gr.camera_mat, camera.alphaX, glm::vec3(0.0f, 1.0f, 0.0f));
		gr.camera_mat = glm::translate(gr.camera_mat, glm::vec3(-ap.rb->m_pos.m_x, -ap.rb->m_pos.m_y, -ap.rb->m_pos.m_z));

		break;
	case 1:
		v = ap.localZ;
		v.m_y = 0.0f;
		v.normalize();
		v = ap.rb->m_pos - v * 60.0f;
		v.m_y = 10.0f;
		/*glTranslatef(-v.m_x, 0.0f, -v.m_z);
		glRotatef(alphaY, 1.0f, 0.0f, 0.0f); 
		glRotatef(alphaX, 0.0f, 1.0f, 0.0f); 
		glTranslatef(-ap.rb->m_pos.m_x, 0, -ap.rb->m_pos.m_z);*/

		//gluLookAt(v.m_x, v.m_y, v.m_z, ap.rb->m_pos.m_x, 10.0f, ap.rb->m_pos.m_z, 0.0f, 1.0f, 0.0f);
		gr.camera_mat = glm::lookAt(glm::vec3(v.m_x, v.m_y, v.m_z), glm::vec3(ap.rb->m_pos.m_x, 10.0f, ap.rb->m_pos.m_z), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 2:
		v = ap.rb->m_pos + ap.localX * -camera.z + ap.localY * (camera.z / 3.0f);
		
		//gluLookAt(v.m_x, v.m_y, v.m_z, ap.rb->m_pos.m_x, ap.rb->m_pos.m_y, ap.rb->m_pos.m_z, ap.localY.m_x, ap.localY.m_y, ap.localY.m_z);
		//gluLookAt(v.m_x, v.m_y, v.m_z, ap.rb->m_pos.m_x, ap.rb->m_pos.m_y, ap.rb->m_pos.m_z, 0.0f, 1.0f, 0.0f);
		gr.camera_mat = glm::lookAt(glm::vec3(v.m_x, v.m_y, v.m_z), 
			glm::vec3(ap.rb->m_pos.m_x, ap.rb->m_pos.m_y, ap.rb->m_pos.m_z), 
			glm::vec3(ap.localY.m_x, ap.localY.m_y, ap.localY.m_z));
		break;
	case 3:
		//gluLookAt(cam5_eye.m_x, cam5_eye.m_y, cam5_eye.m_z, cam5_tar.m_x, cam5_tar.m_y, cam5_tar.m_z, 0.0f, 1.0f, 0.0f);
		/*gr.camera_mat = glm::lookAt(glm::vec3(camera.cam5_eye.m_x, camera.cam5_eye.m_y, camera.cam5_eye.m_z), 
			glm::vec3(camera.cam5_tar.m_x, camera.cam5_tar.m_y, camera.cam5_tar.m_z), 
			glm::vec3(0.0f, 1.0f, 0.0f));*/

		gr.camera_mat = glm::lookAt(glm::vec3(camera.cam5_eye.m_x, camera.cam5_eye.m_y, camera.cam5_eye.m_z), 
			glm::vec3(ap.rb->m_pos.m_x, ap.rb->m_pos.m_y, ap.rb->m_pos.m_z), 
			glm::vec3(0.0f, 1.0f, 0.0f));

		break;

	case 4:
		//gluLookAt(cam5_eye.m_x, cam5_eye.m_y, cam5_eye.m_z, cam5_tar.m_x, cam5_tar.m_y, cam5_tar.m_z, 0.0f, 1.0f, 0.0f);
		/*gr.camera_mat = glm::lookAt(glm::vec3(camera.cam5_eye.m_x, camera.cam5_eye.m_y, camera.cam5_eye.m_z), 
			glm::vec3(camera.cam5_tar.m_x, camera.cam5_tar.m_y, camera.cam5_tar.m_z), 
			glm::vec3(0.0f, 1.0f, 0.0f));*/

		gr.camera_mat = glm::lookAt(glm::vec3(0, camera.z * 0.1, 0), glm::vec3(0, 0, 0), glm::vec3(0.0f, 0.0f, 1.0f));

		break;
	}


	renderShadowmap();

	renderShadowed();

	enter2D();
	drawInfo();
	drawHUD();
	leave2D();

	glutSwapBuffers();  
	
}