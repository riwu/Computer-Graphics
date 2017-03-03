// CS3241Lab5.cpp 
#include <cmath>
#include <iostream>
#include "GL\glut.h"
#include "vector3D.h"
#include <chrono>

using namespace std;

#define WINWIDTH 600
#define WINHEIGHT 400
#define NUM_OBJECTS 6
#define MAX_RT_LEVEL 50
#define NUM_SCENE 3

float* pixelBuffer = new float[WINWIDTH * WINHEIGHT * 3];

class Ray { // a ray that start with "start" and going in the direction "dir"
public:
	Vector3 start, dir;
};

class RtObject {

public:
	virtual double intersectWithRay(Ray, Vector3& pos, Vector3& normal) = 0; // return a -ve if there is no intersection. Otherwise, return the smallest postive value of t

																			 // Materials Properties
	double ambiantReflection[3];
	double diffusetReflection[3];
	double specularReflection[3];
	double speN = 300;
	double _bump = 0;
	bool active = false;
};

class Sphere : public RtObject {

	Vector3 center_;
	double r_;
public:
	Sphere(Vector3 c, double r) { center_ = c; r_ = r; };
	Sphere() {};
	void set(Vector3 c, double r, double bump = 0) { center_ = c; r_ = r; _bump = bump; active = true; };
	double intersectWithRay(Ray, Vector3& pos, Vector3& normal);
};

class Cylinder : public RtObject {

	Vector3 _centre;
	double _radius;
	double _height;
public:
	void set(Vector3 centre, double radius, double height, double bump = 0) { _centre = centre; _radius = radius; _height = height; _bump = bump; active = true; };
	double intersectWithRay(Ray, Vector3& pos, Vector3& normal);
};


RtObject **objList; // The list of all objects in the scene


					// Global Variables
					// Camera Settings
Vector3 cameraPos(0, 0, -500);

// assume the the following two vectors are normalised
Vector3 lookAtDir(0, 0, 1);
Vector3 upVector(0, 1, 0);
Vector3 leftVector(1, 0, 0);
float focalLen = 500;

// Light Settings

Vector3 lightPos(900, 1000, -1500);
double ambiantLight[3] = { 0.4,0.4,0.4 };
double diffusetLight[3] = { 0.7,0.7, 0.7 };
double specularLight[3] = { 0.5,0.5, 0.5 };


double bgColor[3] = { 0.1,0.1,0.4 };

int sceneNo = 0;

static class perlin { //taken from online resources
	int p[512];
	perlin(void) {
		static int permutation[] = { 151,160,137,91,90,15,
			131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208,89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			23,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,43,172,9,
			129,22,39,253,19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127,4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
		};
		for (int i = 0; i < 256; i++) {
			p[256 + i] = p[i] = permutation[i];
		}
	};
	static perlin & getInstance() {
		static perlin instance;
		return instance;
	}

	static double fade(double t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	static double lerp(double t, double a, double b) {
		return a + t * (b - a);
	}
	static double grad(int hash, double x, double y, double z) {
		int h = hash & 15;
		double u = h < 8 || h == 12 || h == 13 ? x : y,
			v = h < 4 || h == 12 || h == 13 ? y : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
public:
	static double noise(double x, double y, double z) {
		perlin & myPerlin = perlin::getInstance();
		int X = (int)floor(x) & 255,
			Y = (int)floor(y) & 255,
			Z = (int)floor(z) & 255;
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);
		double u = fade(x),
			v = fade(y),
			w = fade(z);
		int A = myPerlin.p[X] + Y,
			AA = myPerlin.p[A] + Z,
			AB = myPerlin.p[A + 1] + Z,
			B = myPerlin.p[X + 1] + Y,
			BA = myPerlin.p[B] + Z,
			BB = myPerlin.p[B + 1] + Z;
		return
			lerp(w, lerp(v, lerp(u, grad(myPerlin.p[AA], x, y, z),
				grad(myPerlin.p[BA], x - 1, y, z)),
				lerp(u, grad(myPerlin.p[AB], x, y - 1, z),
					grad(myPerlin.p[BB], x - 1, y - 1, z))),
				lerp(v, lerp(u, grad(myPerlin.p[AA + 1], x, y, z - 1),
					grad(myPerlin.p[BA + 1], x - 1, y, z - 1)),
					lerp(u, grad(myPerlin.p[AB + 1], x, y - 1, z - 1),
						grad(myPerlin.p[BB + 1], x - 1, y - 1, z - 1))));
	}
};



double Sphere::intersectWithRay(Ray r, Vector3& intersection, Vector3& normal)
// return a -ve if there is no intersection. Otherwise, return the smallest postive value of t
{
	// Step 1
	double a = dot_prod(r.dir, r.dir);
	Vector3 dist = r.start - center_;
	double b = dot_prod(r.dir * 2, dist);
	double c = dot_prod(dist, dist) - r_ * r_;
	double discriminant = b*b - 4 * a * c;
	if (discriminant <= 0) return -1;
	double sqrtTerm = sqrt(discriminant);
	double root1 = (-b + sqrtTerm) / (2 * a);
	double root2 = (-b - sqrtTerm) / (2 * a);
	double t = -1;
	if (root1 <= 0) {
		if (root2 <= 0) return -1;
		t = root2;
	}
	else if (root2 <= 0) {
		t = root1;
	}
	else {
		t = fmin(root1, root2);
	}
	intersection = r.start + r.dir * t;
	normal = intersection - center_;
	return t;
}

double Cylinder::intersectWithRay(Ray r, Vector3& intersection, Vector3& normal)
{
	double a = r.dir.x[0] * r.dir.x[0] + r.dir.x[2] * r.dir.x[2];
	double b = r.start.x[0] * r.dir.x[0] * 2 + r.start.x[2] * r.dir.x[2] * 2 - r.dir.x[0] * 2 * this->_centre.x[0] - r.dir.x[2] * 2 * this->_centre.x[2];
	double c = r.start.x[0] * r.start.x[0] + r.start.x[2] * r.start.x[2] - this->_radius*this->_radius - 2 * r.start.x[0] * this->_centre.x[0] - 2 * r.start.x[2] * this->_centre.x[2] + this->_centre.x[0] * this->_centre.x[0] + this->_centre.x[2] * this->_centre.x[2];
	double discriminant = b*b - 4 * a * c;
	if (discriminant <= 0) return -1;
	double sqrtTerm = sqrt(discriminant);
	double root1 = (-b + sqrtTerm) / (2 * a);
	double root2 = (-b - sqrtTerm) / (2 * a);

	if (root1 > root2) {
		double tmp = root1;
		root1 = root2;
		root2 = tmp;
	}
	double y1 = r.start.x[1] + r.dir.x[1] * root1;
	double y2 = r.start.x[1] + r.dir.x[1] * root2;

	double yTop = this->_centre.x[1] + this->_height / 2.0;
	double yBtm = this->_centre.x[1] - this->_height / 2.0;

	double t = -1;
	if (y1 < yBtm) {
		if (y2 < yBtm) return -1;
		t = root1 + (root2 - root1) * (y1 + 1) / (y1 - y2);
		if (t <= 0) return -1;
	}
	else if (y1 < yTop) {
		t = root1;
		if (t <= 0) return -1;
	}
	else {
		if (y2 > yTop) return -1;
		double t = root1 + (root2 - root1) * (y1 - 1) / (y1 - y2);
		if (t <= 0) return -1;
	}
	intersection = r.start + r.dir*t;
	normal = intersection - this->_centre;
	return t;
}

void rayTrace(Ray ray, double& r, double& g, double& b, int fromObj = -1, int level = 0)
{
	Vector3 intersection, normal;
	double mint = DBL_MAX;
	int index = -1;

	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		if (i == fromObj || !objList[i]->active) continue;
		Vector3 currentIntersection, curentNormal;
		double t;
		if ((t = objList[i]->intersectWithRay(ray, currentIntersection, curentNormal)) > 0)
		{
			if (t < mint) {
				mint = t;
				index = i;
				intersection = currentIntersection;
				normal = curentNormal;
			}
		}
	}

	if (index >= 0) { //Step 3
		if (objList[index]->_bump) {
			normal.x[0] = normal.x[0] * (1.0f - objList[index]->_bump) +
				objList[index]->_bump *
				perlin::noise(0.1 * intersection.x[0], 0.1 * intersection.x[1], 0.1 * intersection.x[2]);
			normal.x[1] = normal.x[1] * (1.0f - objList[index]->_bump) +
				objList[index]->_bump *
				perlin::noise(0.1 * intersection.x[1], 0.1 * intersection.x[2], 0.1 * intersection.x[0]);
			normal.x[2] = normal.x[2] * (1.0f - objList[index]->_bump) +
				objList[index]->_bump *
				perlin::noise(0.1 * intersection.x[2], 0.1 * intersection.x[0], 0.1 * intersection.x[1]);
		}
		normal.normalize();
		Vector3 N = normal;
		Vector3 P = intersection;
		Vector3 L = lightPos - P;
		L.normalize();
		double nDotL = fmax(dot_prod(N, L), 0);
		Vector3 R = N * 2 * nDotL - L;
		R.normalize();
		Vector3 V = -ray.dir;
		double rDotV = fmax(dot_prod(R, V), 0);

		double rgb[3];

		if (level < MAX_RT_LEVEL) { //Step 4
			ray.start = intersection;
			double nDotV = fmax(dot_prod(N, V), 0);
			Vector3 reflectionOfV = N * 2 * nDotV - V;
			ray.dir = reflectionOfV;
			rayTrace(ray, rgb[0], rgb[1], rgb[2], index, level++);
		}


		for (int j = 0; j < 3; j++) {
			rgb[j] = ambiantLight[j] * objList[index]->ambiantReflection[j] +
				diffusetLight[j] * objList[index]->diffusetReflection[j] * nDotL +
				specularLight[j] * objList[index]->specularReflection[j] * pow(rDotV, objList[index]->speN)
				+ objList[index]->specularReflection[j] * rgb[j];
		}

		r = rgb[0];
		g = rgb[1];
		b = rgb[2];
	}
	else {
		r = bgColor[0];
		g = bgColor[1];
		b = bgColor[2];
	}
}


void drawInPixelBuffer(int x, int y, double r, double g, double b)
{
	pixelBuffer[(y*WINWIDTH + x) * 3] = (float)r;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 1] = (float)g;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 2] = (float)b;
}

void renderScene()
{
	int x, y;
	Ray ray;
	double r, g, b;
	r = g = b = 0;

	cout << "Rendering Scene " << sceneNo << " with resolution " << WINWIDTH << "x" << WINHEIGHT << "........... ";
	__int64 time1 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the starting time

	ray.start = cameraPos;

	Vector3 vpCenter = cameraPos + lookAtDir * focalLen;  // viewplane center
	Vector3 startingPt = vpCenter + leftVector * (-WINWIDTH / 2.0) + upVector * (-WINHEIGHT / 2.0);
	Vector3 currPt;

	for (x = 0; x < WINWIDTH; x++)
		for (y = 0; y < WINHEIGHT; y++)
		{
			currPt = startingPt + leftVector*x + upVector*y;
			ray.dir = currPt - cameraPos;
			ray.dir.normalize();
			rayTrace(ray, r, g, b);
			drawInPixelBuffer(x, y, r, g, b);
		}

	__int64 time2 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the ending time

	cout << "Done! \nRendering time = " << time2 - time1 << "ms" << endl << endl;
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_DOUBLEBUFFER);
	glDrawPixels(WINWIDTH, WINHEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
	glutSwapBuffers();
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

void setScene(int i = 0)
{
	if (i > NUM_SCENE)
	{
		cout << "Warning: Invalid Scene Number" << endl;
		return;
	}

	switch (i) {
	case 0:
		((Sphere*)objList[0])->set(Vector3(-130, 80, 120), 100);
		((Sphere*)objList[1])->set(Vector3(130, -80, -80), 100);
		((Sphere*)objList[2])->set(Vector3(-130, -80, -80), 100);
		((Sphere*)objList[3])->set(Vector3(130, 80, 120), 100);

		objList[0]->ambiantReflection[0] = 0.1;
		objList[0]->ambiantReflection[1] = 0.4;
		objList[0]->ambiantReflection[2] = 0.4;
		objList[0]->diffusetReflection[0] = 0;
		objList[0]->diffusetReflection[1] = 1;
		objList[0]->diffusetReflection[2] = 1;
		objList[0]->specularReflection[0] = 0.2;
		objList[0]->specularReflection[1] = 0.4;
		objList[0]->specularReflection[2] = 0.4;
		objList[0]->speN = 300;

		objList[1]->ambiantReflection[0] = 0.6;
		objList[1]->ambiantReflection[1] = 0.6;
		objList[1]->ambiantReflection[2] = 0.2;
		objList[1]->diffusetReflection[0] = 1;
		objList[1]->diffusetReflection[1] = 1;
		objList[1]->diffusetReflection[2] = 0;
		objList[1]->specularReflection[0] = 0.0;
		objList[1]->specularReflection[1] = 0.0;
		objList[1]->specularReflection[2] = 0.0;
		objList[1]->speN = 50;

		objList[2]->ambiantReflection[0] = 0.1;
		objList[2]->ambiantReflection[1] = 0.6;
		objList[2]->ambiantReflection[2] = 0.1;
		objList[2]->diffusetReflection[0] = 0.1;
		objList[2]->diffusetReflection[1] = 1;
		objList[2]->diffusetReflection[2] = 0.1;
		objList[2]->specularReflection[0] = 0.3;
		objList[2]->specularReflection[1] = 0.7;
		objList[2]->specularReflection[2] = 0.3;
		objList[2]->speN = 650;

		objList[3]->ambiantReflection[0] = 0.3;
		objList[3]->ambiantReflection[1] = 0.3;
		objList[3]->ambiantReflection[2] = 0.3;
		objList[3]->diffusetReflection[0] = 0.7;
		objList[3]->diffusetReflection[1] = 0.7;
		objList[3]->diffusetReflection[2] = 0.7;
		objList[3]->specularReflection[0] = 0.6;
		objList[3]->specularReflection[1] = 0.6;
		objList[3]->specularReflection[2] = 0.6;
		objList[3]->speN = 650;
		for (int i = 4; i < NUM_OBJECTS; i++) objList[i]->active = false;
		break;
	case 1:
		//change the values above
		// Step 5
		((Sphere*)objList[0])->set(Vector3(-130, -130, -80), 60, 0.9);
		((Sphere*)objList[1])->set(Vector3(150, 80, -39), 30);
		((Sphere*)objList[2])->set(Vector3(-170, 80, 0), 40);
		((Sphere*)objList[3])->set(Vector3(170, -100, -120), 60, 0.97);

		((Cylinder*)objList[4])->set(Vector3(0, 0, 0), 70, 200);
		for (int i = 5; i < NUM_OBJECTS; i++) objList[i]->active = false;

		objList[4]->ambiantReflection[0] = 0.8;
		objList[4]->ambiantReflection[1] = 0.6;
		objList[4]->ambiantReflection[2] = 0.1;
		objList[4]->diffusetReflection[0] = 1;
		objList[4]->diffusetReflection[1] = 0.3;
		objList[4]->diffusetReflection[2] = 0.2;
		objList[4]->specularReflection[0] = 0.3;
		objList[4]->specularReflection[1] = 0.4;
		objList[4]->specularReflection[2] = 0.2;
		objList[4]->speN = 950;

		objList[0]->ambiantReflection[0] = 0.3;
		objList[0]->ambiantReflection[1] = 0.8;
		objList[0]->ambiantReflection[2] = 0.4;
		objList[0]->diffusetReflection[0] = 0.3;
		objList[0]->diffusetReflection[1] = 0.5;
		objList[0]->diffusetReflection[2] = 1;
		objList[0]->specularReflection[0] = 0.2;
		objList[0]->specularReflection[1] = 0.4;
		objList[0]->specularReflection[2] = 0.4;
		objList[0]->speN = 300;

		objList[1]->ambiantReflection[0] = 0.6;
		objList[1]->ambiantReflection[1] = 0.6;
		objList[1]->ambiantReflection[2] = 0.2;
		objList[1]->diffusetReflection[0] = 1;
		objList[1]->diffusetReflection[1] = 1;
		objList[1]->diffusetReflection[2] = 0;
		objList[1]->specularReflection[0] = 0.0;
		objList[1]->specularReflection[1] = 0.5;
		objList[1]->specularReflection[2] = 0.0;
		objList[1]->speN = 750;

		objList[2]->ambiantReflection[0] = 0.1;
		objList[2]->ambiantReflection[1] = 0.3;
		objList[2]->ambiantReflection[2] = 0.8;
		objList[2]->diffusetReflection[0] = 0.6;
		objList[2]->diffusetReflection[1] = 1;
		objList[2]->diffusetReflection[2] = 0.1;
		objList[2]->specularReflection[0] = 0.3;
		objList[2]->specularReflection[1] = 0.7;
		objList[2]->specularReflection[2] = 0.3;
		objList[2]->speN = 50;

		objList[3]->ambiantReflection[0] = 0.3;
		objList[3]->ambiantReflection[1] = 0.3;
		objList[3]->ambiantReflection[2] = 0.3;
		objList[3]->diffusetReflection[0] = 0.7;
		objList[3]->diffusetReflection[1] = 0.7;
		objList[3]->diffusetReflection[2] = 0.7;
		objList[3]->specularReflection[0] = 0.6;
		objList[3]->specularReflection[1] = 0.6;
		objList[3]->specularReflection[2] = 0.6;
		objList[3]->speN = 650;
		break;
	case 2:
		((Sphere*)objList[0])->set(Vector3(0, 0, 0), 90, 0.96);
		((Sphere*)objList[1])->set(Vector3(0, -140, -80), 40);
		((Sphere*)objList[2])->set(Vector3(0, 150, -90), 50);
		objList[3]->active = false;

		((Cylinder*)objList[4])->set(Vector3(-170, 0, 0), 70, 100, 0.95);
		objList[4]->ambiantReflection[0] = 0.2;
		objList[4]->ambiantReflection[1] = 0.7;
		objList[4]->ambiantReflection[2] = 0.4;
		objList[4]->diffusetReflection[0] = 0.3;
		objList[4]->diffusetReflection[1] = 0.3;
		objList[4]->diffusetReflection[2] = 0.2;
		objList[4]->specularReflection[0] = 0.3;
		objList[4]->specularReflection[1] = 0.8;
		objList[4]->specularReflection[2] = 0.6;
		objList[4]->speN = 50;


		((Cylinder*)objList[5])->set(Vector3(180, 0, 0), 40, 170, 0.99);
		objList[5]->ambiantReflection[0] = 0.8;
		objList[5]->ambiantReflection[1] = 0.1;
		objList[5]->ambiantReflection[2] = 0.1;
		objList[5]->diffusetReflection[0] = 1;
		objList[5]->diffusetReflection[1] = 0.3;
		objList[5]->diffusetReflection[2] = 0.2;
		objList[5]->specularReflection[0] = 0.3;
		objList[5]->specularReflection[1] = 0.4;
		objList[5]->specularReflection[2] = 0.2;
		objList[5]->speN = 950;

		objList[0]->ambiantReflection[0] = 0.3;
		objList[0]->ambiantReflection[1] = 0.4;
		objList[0]->ambiantReflection[2] = 1;
		objList[0]->diffusetReflection[0] = 0.3;
		objList[0]->diffusetReflection[1] = 0.5;
		objList[0]->diffusetReflection[2] = 1;
		objList[0]->specularReflection[0] = 0.2;
		objList[0]->specularReflection[1] = 0.4;
		objList[0]->specularReflection[2] = 0.7;
		objList[0]->speN = 900;

		objList[1]->ambiantReflection[0] = 0.1;
		objList[1]->ambiantReflection[1] = 0.1;
		objList[1]->ambiantReflection[2] = 0.4;
		objList[1]->diffusetReflection[0] = 0.7;
		objList[1]->diffusetReflection[1] = 0.4;
		objList[1]->diffusetReflection[2] = 0.2;
		objList[1]->specularReflection[0] = 0.0;
		objList[1]->specularReflection[1] = 0.5;
		objList[1]->specularReflection[2] = 0.0;
		objList[1]->speN = 50;

		objList[2]->ambiantReflection[0] = 0.1;
		objList[2]->ambiantReflection[1] = 0.8;
		objList[2]->ambiantReflection[2] = 0.8;
		objList[2]->diffusetReflection[0] = 0.6;
		objList[2]->diffusetReflection[1] = 1;
		objList[2]->diffusetReflection[2] = 0.1;
		objList[2]->specularReflection[0] = 0.3;
		objList[2]->specularReflection[1] = 0.7;
		objList[2]->specularReflection[2] = 0.3;
		objList[2]->speN = 450;
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {
	case 's':
	case 'S':
		sceneNo = (sceneNo + 1) % NUM_SCENE;
		setScene(sceneNo);
		renderScene();
		glutPostRedisplay();
		break;
	case 'q':
	case 'Q':
		exit(0);

	default:
		break;
	}
}

int main(int argc, char **argv)
{
	cout << "<<CS3241 Lab 5>>\n\n" << endl;
	cout << "S to go to next scene" << endl;
	cout << "Q to quit" << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WINWIDTH, WINHEIGHT);

	glutCreateWindow("CS3241 Lab 5: Ray Tracing");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboard);

	objList = new RtObject*[NUM_OBJECTS];

	// create four spheres
	objList[0] = new Sphere(Vector3(-130, 80, 120), 100);
	objList[1] = new Sphere(Vector3(130, -80, -80), 100);
	objList[2] = new Sphere(Vector3(-130, -80, -80), 100);
	objList[3] = new Sphere(Vector3(130, 80, 120), 100);

	objList[4] = new Cylinder();
	objList[5] = new Cylinder();


	setScene(0);

	setScene(sceneNo);
	renderScene();

	glutMainLoop();

	for (int i = 0; i < NUM_OBJECTS; i++)
		delete objList[i];
	delete[] objList;

	delete[] pixelBuffer;

	return 0;
}
