// CS3241Lab2.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <time.h>
#include <Windows.h>

/* Include header files depending on platform */
#ifdef _WIN32
#include "glut.h"
#define M_PI 3.14159
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

using namespace std;

#define numStars 100
#define numPlanets 8

class planet
{
public:
	float distFromRef;
	int angularSpeed;
	GLfloat color[3];
	float size;
	float angle;
	boolean clockwise;
	GLfloat new_color[3];

	planet() {}

	planet(float _distFromRef, int _angularSpeed, GLfloat r, GLfloat b, GLfloat g, float _size, float _angle, boolean _clockwise)
	{
		distFromRef = _distFromRef;
		angularSpeed = _angularSpeed;
		color[0] = r;
		color[1] = b;
		color[2] = g;

		new_color[0] = r;
		new_color[1] = b;
		new_color[2] = g;
		size = _size;
		angle = _angle;
		clockwise = _clockwise;
	}
};

float alpha = 0.0, k = 1;
float tx = 0.0, ty = 0.0;
planet planetList[numPlanets];
boolean clock_mode = false;

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.3, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void createPlanets() {
	planetList[0] = planet(0, 0, 1, 0, 1, 0.6, 0, false);
	planetList[1] = planet(1.3, 100, 0, 0, 1, 0.4, 0, true);
	planetList[2] = planet(3, 50, 0, 1, 1, 0.3, 0, false);
	planetList[3] = planet(6, 70, 1, 0.2, 0.4, 0.2, 0, true);
	planetList[4] = planet(9, 20, 0.1, 0.4, 0.1, 0.25, 0, false);

	planetList[5] = planet(1, 10, 1, 1, 1, 0.15, 0, false);
	planetList[6] = planet(1, 10, 0.3, 0.5, 0.9, 0.1, 0, true);
	planetList[7] = planet(0.5, 10, 0.2, 0.9, 0.3, 0.12, 0, true);
}



void drawPlanet(planet p, double angle, int totalMS) {
	glRotatef(angle, 0, 0, 1);
	glTranslatef(0, p.distFromRef, 0);

	glBegin(GL_POLYGON);

	int randR = rand(), randB = rand(), randG = rand();
	if (!clock_mode) {
		if (totalMS % (p.angularSpeed + 1) > p.angularSpeed) {
			p.new_color[0] = (GLfloat)(randR % 11) / 10.0;
			p.new_color[1] = (GLfloat)(randB % 11) / 10.0;
			p.new_color[2] = (GLfloat)(randG % 11) / 10.0;
		}
		glColor3f(p.new_color[0], p.new_color[1], p.new_color[2]);
	} else
		glColor3f(p.color[0], p.color[1], p.color[2]);

	for (int i = 0; i < 360; i++)
	{
		float deg = i*(M_PI / 180);
		glVertex2f(cos(deg)*p.size, sin(deg)*p.size);
	}

	glEnd();
}

void drawPlanets()
{

	time_t current_time = time(NULL);
	struct tm * timeinfo = localtime(&current_time);
	float timeInSec = (float)timeinfo->tm_sec / 60 * 360;

	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);
						//why assignment wrong: was using timeinfo->tm_hour / 24, which is only correct when tm_hour > 12
	float angles[] = { (float)(timeinfo->tm_hour % 12) / 12 * 360, (float)timeinfo->tm_min / 60 * 360, (float)timeinfo->tm_sec / 60 * 360, (float)(systemTime.wSecond * 1000 + systemTime.wMilliseconds) / 60000 * 360 };
	for (int i = 0; i <= 4; i++) {
		planet p = planetList[i];
		glPushMatrix();

		double angle = 0;
		int totalMS = systemTime.wSecond * 1000 + systemTime.wMilliseconds;
		if (i > 0) {
			if (clock_mode)
				angle = 360 - (float)angles[i - 1];
			else {
				angle = (float)(totalMS / p.angularSpeed) / 60 * 360.0;
				if (p.clockwise)
					angle = 360 - angle;
			}
		}

		drawPlanet(p, angle, totalMS);

		if (i == 3) {
			double angle = 360 - (float)systemTime.wMilliseconds / 1000 * 360;
			drawPlanet(planetList[5], angle, totalMS);

			angle = 360 - (float)systemTime.wMilliseconds / 100 * 360;
			drawPlanet(planetList[7], angle, totalMS);
		}
		else if (i == 4) {
			double angle = (float)systemTime.wMilliseconds / 500 * 360;
			drawPlanet(planetList[6], angle, totalMS);
		}

		glPopMatrix();

	}
}

void drawSomething() {
	glPushMatrix();

	glRotated(0, 0, 0, 1);

	drawPlanets();

	glPopMatrix();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);
	glTranslatef(tx, ty, 0);
	glRotatef(alpha, 0, 0, 1);

	//draw stuff here!
	glPushMatrix();
	drawSomething();
	glPopMatrix();

	glPopMatrix();
	glFlush();
}

void idle()
{
	//update animation here

	glutPostRedisplay();	//after updating, draw the screen again
}

void keyboard(unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (tolower(key)) {

	case 'a':
		alpha += 10;
		glutPostRedisplay();
		break;

	case 'd':
		alpha -= 10;
		glutPostRedisplay();
		break;

	case 'q':
		k += 0.1;
		glutPostRedisplay();
		break;

	case 'e':
		if (k>0.1)
			k -= 0.1;
		glutPostRedisplay();
		break;

	case 'z':
		tx -= 0.1;
		glutPostRedisplay();
		break;

	case 'c':
		tx += 0.1;
		glutPostRedisplay();
		break;

	case 's':
		ty -= 0.1;
		glutPostRedisplay();
		break;

	case 'w':
		ty += 0.1;
		glutPostRedisplay();
		break;

	case 't':
		clock_mode = !clock_mode;
		glutPostRedisplay();
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}
}

int main(int argc, char **argv)
{
	cout << "CS3241 Lab 2\n\n";
	cout << "+++++CONTROL BUTTONS+++++++\n\n";
	cout << "Scale Up/Down: Q/E\n";
	cout << "Rotate Clockwise/Counter-clockwise: A/D\n";
	cout << "Move Up/Down: W/S\n";
	cout << "Move Left/Right: Z/C\n";
	cout << "ESC: Quit\n";

	createPlanets();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
