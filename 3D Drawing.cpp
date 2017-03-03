// CS3241Lab3.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#include "glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

// global variable

bool m_Smooth = false;
bool m_Highlight = false;
GLfloat angle = 0;   /* in degrees */
GLfloat angle2 = 0;   /* in degrees */
GLfloat zoom = 1.0;
GLfloat field_of_view = 40.0;
GLfloat x_translation = 0.0;
float zNear = 1.0;
float zFar = 80.0;
float fovy = 40.0;
int shininess = 15;

int mouseButton = 0;
int moving, startx, starty;

#define NO_OBJECT 4;
int current_object = 0;

using namespace std;

void setupLighting()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// Lights, material properties
	GLfloat	ambientProperties[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat	diffuseProperties[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat	specularProperties[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { -100.0f, 100.0f, 100.0f, 1.0f };

	glClearDepth(1.0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularProperties);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

	// Default : lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

}

void handleHighlight() {
	if (m_Highlight)
	{
		GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

		glMaterialf(GL_FRONT, GL_SHININESS, shininess);

		GLfloat mat_emission[] = { 0.05f,  0.05f,  0.05f, 1.0f };
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
	}
	else {
		float no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
	}

}

void drawSphere(double r)
{

	int i, j;
	int n = 20;
	for (i = 0; i < n; i++)
		for (j = 0; j < 2 * n; j++)
			if (m_Smooth)
			{
				glBegin(GL_POLYGON);
				//normal of a point on a sphere is the unit vector of the point (vector from origin)
				glNormal3d(sin(i*M_PI / n)*cos(j*M_PI / n), cos(i*M_PI / n)*cos(j*M_PI / n), sin(j*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos(j*M_PI / n), r*cos(i*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glNormal3d(sin((i + 1)*M_PI / n)*cos(j*M_PI / n), cos((i + 1)*M_PI / n)*cos(j*M_PI / n), sin(j*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos(j*M_PI / n), r*cos((i + 1)*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glNormal3d(sin((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), cos((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), sin((j + 1)*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*cos((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glNormal3d(sin(i*M_PI / n)*cos((j + 1)*M_PI / n), cos(i*M_PI / n)*cos((j + 1)*M_PI / n), sin((j + 1)*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos((j + 1)*M_PI / n), r*cos(i*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glEnd();
			}
			else {
				glBegin(GL_POLYGON);
				// Explanation: the normal of the whole polygon is the coordinate of the center of the polygon for a sphere
				glNormal3d(sin((i + 0.5)*M_PI / n)*cos((j + 0.5)*M_PI / n), cos((i + 0.5)*M_PI / n)*cos((j + 0.5)*M_PI / n), sin((j + 0.5)*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos(j*M_PI / n), r*cos(i*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos(j*M_PI / n), r*cos((i + 1)*M_PI / n)*cos(j*M_PI / n), r*sin(j*M_PI / n));
				glVertex3d(r*sin((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*cos((i + 1)*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glVertex3d(r*sin(i*M_PI / n)*cos((j + 1)*M_PI / n), r*cos(i*M_PI / n)*cos((j + 1)*M_PI / n), r*sin((j + 1)*M_PI / n));
				glEnd();
			}

}

void drawCone(GLdouble btmRad, GLdouble topRad, GLdouble height) {
	GLfloat diff = btmRad - topRad;
	float dist = sqrt(pow(height, 2) + pow(diff, 2));

	for (int i = 0; i < 40; i++) {
		GLfloat radBtm = btmRad - diff * (i / 40.0);
		GLfloat radTop = btmRad - diff * ((i + 1) / 40.0);
		GLfloat btm = i * height / 40;
		GLfloat top = (i + 1) * height / 40;

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= 40; j++) {
			GLfloat angle = M_PI * 2 * j / 40;
			glNormal3f(height / dist * sin(angle), height / dist * cos(angle), diff / dist);
			glTexCoord2f(1 - j / 40.0, i / 40.0);
			glVertex3f(radBtm * sin(angle), radBtm * cos(angle), btm);
			glTexCoord2f(1 - j / 40.0, (i + 1) / 40.0);
			glVertex3f(radTop * sin(angle), radTop * cos(angle), top);
		}
		glEnd();
	}
}

void drawCone() {
	if (!m_Smooth) glShadeModel(GL_FLAT);
	drawCone(0.6, 0.0, 1.3);
	glShadeModel(GL_SMOOTH);
}

void drawSphereWithCones() {
	drawSphere(0.5);

	for (int i = 0; i < 6; i++) {
		glPushMatrix();

		int mod = 1 - (i % 2) * 2;
		float translateCoords[] = { 0.0, 0.0, 0.0 };
		translateCoords[i / 2] = mod*0.3;
		glTranslatef(translateCoords[0], translateCoords[1], translateCoords[2]);

		float rotateCoords[] = { 0.0, 0.0, 0.0 };
		if (i < 2)
			rotateCoords[1] = 1.0;
		else
			rotateCoords[0] = 1.0;
		switch (i)
		{
		case 2:
		case 3:
			mod = -mod;
			break;
		case 4:
			mod = 0;
			break;
		case 5:
			mod = 2;
			break;
		}
		glRotatef(mod * 90, rotateCoords[0], rotateCoords[1], rotateCoords[2]);
		drawCone(0.3, 0.0, 0.6);

		glPopMatrix();
	}
}

void drawObj1() {
	glPushMatrix();

	glTranslatef(-1.5, 0, 0);
	glScalef(0.8, 0.8, 0.8);

	for (int i = 0; i < 5; i++) {
		drawSphereWithCones();
		glTranslatef(1.4, 0, 0);
		glScalef(0.7, 0.7, 0.7);
	}
	glPopMatrix();

}

void drawObj2() {
	glScalef(0.09, 0.09, 0.09);
	glTranslatef(-20, 0, 0);

	for (float i = 0.0; i < 2 * M_PI; i += 0.15)
	{
		glTranslatef(sin(i) * 3, cos(i) * 3, 0);
		glPushMatrix();
		drawCone(0.5, 0.5, 1.5);
		drawSphere(0.5);
		glTranslatef(0, 0, 1.5);
		drawSphere(0.5);
		glPopMatrix();
	}

}

void display(void)
{//Add Projection tool and Camera Movement somewhere here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(0, 0, -6);
	glRotatef(angle2, 1.0, 0.0, 0.0);
	glRotatef(angle, 0.0, 1.0, 0.0);

	glScalef(zoom, zoom, zoom);

	float no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
	float mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };
	float no_shininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	GLfloat lightPosition[] = { -100.0f, 100.0f, 100.0f, 1.0f };
	GLfloat spotlightPosition[] = { 0.0f, 0.0f, 10.0f, 1.0f };

	handleHighlight();

	switch (current_object) {
	case 0:
		drawSphere(1);
		break;
	case 1:
		drawCone();
		break;
	case 2:
		drawObj1();
		break;
	case 3:
		drawObj2();
		break;
	default:
		break;
	};
	glPopMatrix();
	glutSwapBuffers();
}


void adjustView(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, zNear, zFar);
}

void resetCamera() {
	//fill in values below.
	angle = 0;
	angle2 = 0;
	zoom = 1.0;
	field_of_view = 40.0;
	x_translation = 0.0;
	// include resetting of gluPerspective & gluLookAt.
	glScalef(zoom, zoom, zoom);
	glTranslatef(x_translation, 0, 0);
	glMatrixMode(GL_MODELVIEW);
	//gluLookAt(1, 1, 1, 1, 1, -1, 0, 1, 0);
	float z = cos(angle) * 5;
	float x = sin(angle) * 5;
	gluLookAt(sin(angle) * 5, cos(angle) * 5, 0, sin(angle2) * 5, cos(angle2) * 5, 0, 0.0, 1.0, 0.0);
	adjustView(field_of_view, 1.0, 1.0, 80.0);
	return;
}


void setCameraBestAngle() {
	//fill in values below
	angle = 70;
	angle2 = 40;
	zoom = 1.0;
	field_of_view = 40.0;
	x_translation = 0.0;
	//TIPS: Adjust gluLookAt function to change camera position
	glScalef(zoom, zoom, zoom);
	glTranslatef(x_translation, 0, 0);
	glMatrixMode(GL_MODELVIEW);
	float z = cos(angle) * 5;
	float x = sin(angle) * 5;
	gluLookAt(sin(angle) * 5, cos(angle) * 5, 0, sin(angle2) * 5, cos(angle2) * 5, 0, 0.0, 1.0, 0.0);
	adjustView(field_of_view, 1.0, 1.0, 80.0);
	return;
}

void keyboard(unsigned char key, int x, int y)
{//add additional commands here to change Field of View and movement
	switch (key) {
	case 'p':
	case 'P':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'w':
	case 'W':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'v':
	case 'V':
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 's':
	case 'S':
		m_Smooth = !m_Smooth;
		break;
	case 'h':
	case 'H':
		m_Highlight = !m_Highlight;
		break;

	case 'n':
		if (zNear > 1) zNear--;
		adjustView(fovy, 1.0, zNear, zFar);
		break;

	case 'N':
		adjustView(fovy, 1.0, zNear++, zFar);
		break;

	case 'f':
		if (zFar > 1) zFar--;
		adjustView(fovy, 1.0, zNear, zFar);
		break;

	case 'F':
		adjustView(fovy, 1.0, zNear, zFar++);
		break;

	case 'o':
		adjustView(fovy--, 1.0, zNear, zFar);
		break;

	case 'O':
		adjustView(fovy++, 1.0, zNear, zFar);
		break;

	case 'r':
		resetCamera();
		break;

	case 'R':
		setCameraBestAngle();
		break;

	case '1':
	case '2':
	case '3':
	case '4':
		current_object = key - '1';
		break;

	case 'z':
		if (shininess > 1) shininess--;
		break;

	case 'Z':
		if (shininess < 50) shininess++;
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}

	glutPostRedisplay();
}



void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		mouseButton = button;
		moving = 1;
		startx = x;
		starty = y;
	}
	if (state == GLUT_UP) {
		mouseButton = button;
		moving = 0;
	}
}

void motion(int x, int y)
{
	if (moving) {
		if (mouseButton == GLUT_LEFT_BUTTON)
		{
			angle = angle + (x - startx);
			angle2 = angle2 + (y - starty);
		}
		else zoom += ((y - starty)*0.01);
		startx = x;
		starty = y;
		glutPostRedisplay();
	}

}


int main(int argc, char **argv)
{
	cout << "CS3241 Lab 3" << endl << endl;

	cout << "1-4: Draw different objects" << endl;
	cout << "S: Toggle Smooth Shading" << endl;
	cout << "H: Toggle Highlight" << endl;
	cout << "W: Draw Wireframe" << endl;
	cout << "P: Draw Polygon" << endl;
	cout << "V: Draw Vertices" << endl;
	cout << "n, N: Reduce or increase the distance of the near plane from the camera" << endl;
	cout << "f, F: Reduce or increase the distance of the far plane from the camera" << endl;
	cout << "o, O: Reduce or increase the distance of the povy plane from the camera" << endl;
	cout << "r: Reset camera to the initial parameters when the program starts" << endl;
	cout << "R: Change camera to another setting that is has the best viewing angle for your object" << endl;
	cout << "ESC: Quit" << endl << endl;


	cout << "Left mouse click and drag: rotate the object" << endl;
	cout << "Right mouse click and drag: zooming" << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("CS3241 Assignment 3");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(display);
	glMatrixMode(GL_PROJECTION);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	setupLighting();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	//REMOVE FROM THIS SECTION

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( /* field of view in degree */ 40.0,
		/* aspect ratio */ 1.0,
		/* Z near */ 1.0, /* Z far */ 80.0);
	glMatrixMode(GL_MODELVIEW);
	int upVector = 1;
	gluLookAt(1, 1, 1, 1, 1, -1, 0, 1, 0);
	//TO THIS SECTION, after implementation of Projection and Camera movement tools
	//Hint: Transfer these functions over into void display(void), then add other variables

	glutMainLoop();

	return 0;
}