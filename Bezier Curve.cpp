// CS3241Lab4.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <iostream>
#include <fstream>

/* Include header files depending on platform */
#ifdef _WIN32
#include "glut.h"
#define M_PI 3.14159
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

#define MAXPTNO 1000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 50
using namespace std;

// Global variables that you can use
struct Point {
	int x, y;
};

// Storage of control points
int nPt = 0;
Point ptList[MAXPTNO];

// Display options
bool displayControlPoints = true;
bool displayControlLines = true;
bool displayTangentVectors = false;
bool displayObjects = false; 
bool C1Continuity = false; 

bool randColor = true;
int degree = 3;


void setColor(int randR, int randB, int randG) {
	glColor3f((randR % 11) / 10.0, (randB % 11) / 10.0, (randG % 11) / 10.0);
}

void setRandColor() {
	setColor(rand(), rand(), rand());
}

void drawRightArrow()
{
	glColor3f(0, 1, 0);
	if (randColor) setRandColor();
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(100, 0);
	glVertex2f(95, 5);
	glVertex2f(100, 0);
	glVertex2f(95, -5);
	glEnd();
}

bool isCCPoint(int i) {
	return C1Continuity && (i >= degree && (i % degree == 1));
}

void drawCircle(int offsetX, int offsetY) {
	setRandColor();
	glBegin(GL_POLYGON);
	double radius = 4;
	for (int i = 0; i < 360; i++)
	{
		glVertex2f(offsetX + cos(i)*radius, offsetY + sin(i)*radius);
	}
	glEnd();

}

void drawObjects() {
	int size = 20;
	glBegin(GL_POLYGON);
	setRandColor();
	glVertex2f(-size, 0);
	setRandColor();
	glVertex2f(0, -size);
	setRandColor();
	glVertex2f(size, 0);
	setRandColor();
	glVertex2f(0, size);
	glEnd();

	double offset = -7;
	drawCircle(size + offset, size + offset);
	drawCircle(-size - offset, -size - offset);
	drawCircle(-size - offset, size + offset);
	drawCircle(size + offset, -size - offset);
}

long long C(int n, int r) {
	if (r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
	long long ans = 1;
	int i;

	for (i = 1; i <= r; i++) {
		ans *= n - r + i;
		ans /= i;
	}

	return ans;
}

double getCoeff(int k, double t, int deg) {
	return C(deg, k) * pow(1 - t, deg - k) * pow(t, k);
}

void getCurvePoint(double &x, double &y, double t, int i) {
	x = 0;
	y = 0;
	for (int k = 0; k <= degree; k++) {
		double coeff = getCoeff(k, t, degree);
		x += coeff * ptList[i + k].x;
		y += coeff * ptList[i + k].y;
	}
}

void display(void)
{
	Point* original;
	if (C1Continuity) {
		original = new Point[nPt];
		for (int i = 0; i < nPt; i++) {
			original[i] = ptList[i];
			if (isCCPoint(i)) {
				ptList[i].x = ptList[i - 1].x + (ptList[i - 1].x - ptList[i - 2].x);
				ptList[i].y = ptList[i - 1].y + (ptList[i - 1].y - ptList[i - 2].y);
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	if (displayControlPoints)
	{
		glPointSize(5);
		glBegin(GL_POINTS);
		for (int i = 0; i < nPt; i++)
		{
			if (isCCPoint(i)) {
				glColor3f(0.5, 0.5, 0.5); //for original point
				glVertex2d(original[i].x, original[i].y);

				glColor3f(1, 0, 0); //for C1 point
			}
			else {
				glColor3f(0, 0, 0); //no CC
			}
			glVertex2d(ptList[i].x, ptList[i].y);
		}
		glEnd();
		glPointSize(1);

	}
	if (displayControlLines)
	{
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < nPt; i++)
		{
			glVertex2d(ptList[i].x, ptList[i].y);
		}
		glEnd();
	}

	glColor3f(0, 0, 1);
	int ratio = NLINESEGMENT / (NOBJECTONCURVE); //for displayObjects, can consider changing color (or rainbow scaled from top to end)
	for (int i = 0; i < nPt - degree; i += degree) {
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= NLINESEGMENT; j++) {
			if (randColor) setRandColor();

			double t = (double)j / NLINESEGMENT;
			double x = 0, y = 0;
			getCurvePoint(x, y, t, i);
			glVertex2d(x, y);
		}
		glEnd();
	}

	if (displayTangentVectors || displayObjects) {
		for (int i = 0; i < nPt - degree; i += degree) {
			for (int j = 0; j <= (NOBJECTONCURVE - 1); j++) {
				double t = (double)j / (NOBJECTONCURVE - 1);
				double x, y;
				getCurvePoint(x, y, t, i);

				double x_tangent = 0, y_tangent = 0;
				for (int k = 0; k < degree; k++) {
					double coeff = getCoeff(k, t, degree - 1);
					x_tangent += coeff * (ptList[i + k + 1].x - ptList[i + k].x);
					y_tangent += coeff * (ptList[i + k + 1].y - ptList[i + k].y);
				}
				x_tangent *= degree;
				y_tangent *= degree;

				glPushMatrix();
				glTranslatef(x, y, 0);
				glRotatef(atan2(y_tangent, x_tangent) * 180 / M_PI, 0, 0, 1);

				if (displayObjects)
					drawObjects();
				if (displayTangentVectors) {
					drawRightArrow();
				}
				glPopMatrix();
			}
		}
	}

	glPopMatrix();
	glutSwapBuffers();

	if (C1Continuity) {
		for (int i = 0; i < nPt; i++) {
			if (isCCPoint(i)) ptList[i] = original[i];
		}
		delete[] original;
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void readFile()
{

	std::ifstream file;
	file.open("savefile.txt");
	file >> nPt;

	if (nPt > MAXPTNO)
	{
		cout << "Error: File contains more than the maximum number of points." << endl;
		nPt = MAXPTNO;
	}

	for (int i = 0; i < nPt; i++)
	{
		file >> ptList[i].x;
		file >> ptList[i].y;
	}
	file.close();// is not necessary because the destructor closes the open file by default
}

void writeFile()
{
	std::ofstream file;
	file.open("savefile.txt");
	file << nPt << endl;

	for (int i = 0; i < nPt; i++)
	{
		file << ptList[i].x << " ";
		file << ptList[i].y << endl;
	}
	file.close();// is not necessary because the destructor closes the open file by default
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':
	case 'R':
		readFile();
		break;

	case 'w':
	case 'W':
		writeFile();
		break;

	case 'T':
	case 't':
		displayTangentVectors = !displayTangentVectors;
		break;

	case 'o':
	case 'O':
		displayObjects = !displayObjects;
		break;

	case 'p':
	case 'P':
		displayControlPoints = !displayControlPoints;
		break;

	case 'L':
	case 'l':
		displayControlLines = !displayControlLines;
		break;

	case 'C':
	case 'c':
		C1Continuity = !C1Continuity;
		break;

	case 'e':
	case 'E':
		nPt = 0;
		break;

	case 'd':
		if (degree > 1) degree--;
		break;
	case 'D':
		degree++;
		break;

	case 's':
	case 'S':
		randColor = !randColor;
		break;

	case 'u':
	case 'U':
		if (nPt > 0) nPt--;
		break;

	case 27:
		exit(0);
		break;

	default:
		break;
	}

	glutPostRedisplay();
}



void mouse(int button, int state, int x, int y)
{
	/*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
	/*state: GLUT_UP or GLUT_DOWN */
	enum
	{
		MOUSE_LEFT_BUTTON = 0,
		MOUSE_MIDDLE_BUTTON = 1,
		MOUSE_RIGHT_BUTTON = 2,
		MOUSE_SCROLL_UP = 3,
		MOUSE_SCROLL_DOWN = 4
	};
	if ((button == MOUSE_LEFT_BUTTON) && (state == GLUT_UP))
	{
		if (nPt == MAXPTNO)
		{
			cout << "Error: Exceeded the maximum number of points." << endl;
			return;
		}
		ptList[nPt].x = x;
		ptList[nPt].y = y;
		nPt++;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	cout << "CS3241 Lab 4" << endl << endl;
	cout << "Left mouse click: Add a control point" << endl;
	cout << "ESC: Quit" << endl;
	cout << "P: Toggle displaying control points" << endl;
	cout << "L: Toggle displaying control lines" << endl;
	cout << "E: Erase all points (Clear)" << endl;
	cout << "C: Toggle C1 continuity" << endl;
	cout << "T: Toggle displaying tangent vectors" << endl;
	cout << "O: Toggle displaying objects" << endl;
	cout << "R: Read in control points from \"savefile.txt\"" << endl;
	cout << "W: Write control points to \"savefile.txt\"" << endl;
	cout << "d: Reduce degree by 1" << endl;
	cout << "D: Increase degree by 1" << endl;
	cout << "s: Toggle rainbow curve" << endl;
	cout << "u: Remove last drawn point" << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("CS3241 Assignment 4");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
