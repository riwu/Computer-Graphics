// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>

/* Include header files depending on platform */
#ifdef _WIN32
#include "glut.h"
#define M_PI 3.14159
#elif __APPLE__
#include <OpenGL/gl.>
#include <GLUT/GLUT.h>
#endif

using namespace std;

float alpha = 0.0, k = 0.7;
float tx = 0.0, ty = 0.0;


void specifyColor(int i, int randR, int randB, int randG) {
	if (i<2)
		glColor3f((randR % 11) / 10.0, (randB % 11) / 10.0, (randG % 11) / 10.0);
	else 
		glColor3f(0.4 + i*0.1, 0.1 + i*0.1, 0);
}
void drawBits(int i, int randR, int randB, int randG)
{
	glBegin(GL_POLYGON);
	specifyColor(i, randR, randB, randG);
	glVertex2f(-0.2, -0.2);
	specifyColor(i, randR, randB, randG);
	glVertex2f(0, 0.2);
	specifyColor(i, randR, randB, randG);
	glVertex2f(0.2, -0.2);
	glEnd();
}

void draw() {
	for (int l = 0; l < 360/45; l++) {
		glPushMatrix();

		for (int k = 0; k < 10; k++) {
			glPushMatrix();

			int randR = rand(), randB = rand(), randG = rand();
			for (int j = 0; j < 36; j++) {
				glPushMatrix();
				for (int i = 0; i < 100; i++) {
					drawBits(i, randR, randB, randG);
					glRotatef(10, 0, 0, 1);
					glTranslatef(0, 0.3, 0);
					glScalef(0.8, 0.8, 0);
				}
				glPopMatrix();
				glRotatef(15, 0, 0, 1);
			}

			glPopMatrix();
			glRotatef(20, 0, 0, 1);
			glTranslatef(2.3, 0, 0);
		}

		glPopMatrix();
		glRotatef(45, 0, 0, 1);
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);
	glTranslatef(tx, ty, 0);
	glRotatef(alpha, 0, 0, 1);

	//draw your stuff here
	draw();

	glPopMatrix();
	glFlush();

}

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
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glShadeModel(GL_SMOOTH);
}



void keyboard(unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

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

	case 27:
		exit(0);
		break;

	default:
		break;
	}
}

int main(int argc, char **argv)
{
	cout << "CS3241 Lab 1\n\n";
	cout << "+++++CONTROL BUTTONS+++++++\n\n";
	cout << "Scale Up/Down: Q/E\n";
	cout << "Rotate Clockwise/Counter-clockwise: A/D\n";
	cout << "Move Up/Down: W/S\n";
	cout << "Move Left/Right: Z/C\n";
	cout << "ESC: Quit\n";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
