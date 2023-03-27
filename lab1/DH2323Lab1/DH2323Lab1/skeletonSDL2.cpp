// DH2323 Lab 1
// Introduction lab that covers:
// * SDL2 (https://www.libsdl.org/)
// * C++, std::vector and glm::vec3 (https://glm.g-truc.net)
// * 2D graphics
// * Plotting pixels
// * Video memory
// * Color representation
// * Linear interpolation

#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include "SDL2Auxiliary.h"
#include <math.h>
using namespace std;
using glm::vec3;

// ---------------------------------------------------------
// GLOBAL VARIABLES
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float ASPECT_RATIO = (float)SCREEN_WIDTH / SCREEN_WIDTH;
vec3 topLeft(1, 0, 0); // red
vec3 topRight(0, 0, 1); // blue
vec3 bottomLeft(1, 1, 0); // yellow
vec3 bottomRight(0, 1, 0); // green //I swapped these two to obtain exactly the image shown in the lab instructions
const double pi = 3.1415926535897932385;
vector<vec3> stars( 1000 );
int t;
SDL2Aux *sdlAux;

// ---------------------------------------------------------
// FUNCTION DECLARATIONS
void DrawInterpolation();
void DrawStarfield(vector<vec3>& stars);
void Interpolate(float a, float b, vector<float>& result);
void Interpolate(vec3 a, vec3 b, vector<vec3>& result);

void Update();
// ---------------------------------------------------------

// FUNCTION DEFINITIONS

//MAIN FOR INTERPOLATION
// 
//int main(int argc, char* argv[])
//{
//	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
//	vector<vec3> result(4);
//	vec3 a(1, 4, 9.2);
//	vec3 b(4, 1, 9.8);
//	Interpolate(a, b, result);
//	for (int i = 0; i < result.size(); ++i)
//	{
//		cout << "( "
//			<< result[i].x << ", "
//			<< result[i].y << ", "
//			<< result[i].z << " ) ";
//	}
//
//	while (!sdlAux->quitEvent()) {
//		DrawInterpolation();
//	}
//	sdlAux->saveBMP("screenshot.bmp");
//	return 0;
//}

inline float rad2deg(float rad) {
	return rad * 180.0 / pi;
}

inline float HFOVfromVFOV(float VFOV) {
	return rad2deg(2 * atan(tan(VFOV / 2) * ASPECT_RATIO));
}

inline float randomWithRange(float a, float b) {
	return a + ((float(rand()) / float(RAND_MAX)) * (b - a));
}

//MAIN FOR STARFIELD
int main(int argc, char* argv[])
{
	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	
	cout << "Resulting HFOV: " << HFOVfromVFOV(90);
	for (auto& star : stars) {
		star = vec3(randomWithRange(-1, 1), randomWithRange(-1, 1), randomWithRange(0.001, 1));
	}
	t = SDL_GetTicks(); //This function returns the number of milliseconds since SDL was initialized
	while (!sdlAux->quitEvent()) {
		Update();
		DrawStarfield(stars);
	}
	sdlAux->saveBMP("screenshot.bmp");
	return 0;
}

void Update() {
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;

	for (auto &star : stars)
	{
		star.z = star.z - 0.00085 * dt;

		// This is to wrap it around and make them go back once they reach the z boundary
		if (star.z <= 0) star.z += 1;
		if (star.z > 1) star.z -= 1;
	}

}


void Interpolate(vec3 a, vec3 b, vector<vec3>& result) {
	if (result.size() == 1) {
		result[0] = a;
	}
	else if (result.size() == 2) {
		result[0] = a;
		result[1] = b;
	}
	else {
		//I need to define a step in all three the directions
		result[0] = a;
		result[result.size() - 1] = b;
		float stepX = (b.x - a.x) / (result.size() - 1);
		float stepY = (b.y - a.y) / (result.size() - 1);
		float stepZ = (b.z - a.z) / (result.size() - 1);
		vec3 stepVector(stepX, stepY, stepZ);
		vec3 currentVecPos = a;
		for(int i=1; i<result.size()-1; i++){
			currentVecPos += stepVector;
			result[i] = currentVecPos;
		}
	}
}

void Interpolate(float a, float b, vector<float>& result) {
	if (result.size() == 1) {
		result[0] = a;
	}
	else if (result.size() == 2) {
		result[0] = a;
		result[1] = b;
	}
	else {
		result[0] = a;
		result[result.size() - 1] = b;
		float step = (b - a) / (result.size() - 1);
		float currentVal = a;
		for (int i = 1; i < result.size() - 1; i++) {
			currentVal += step;
			result[i] = currentVal;
		}
	}
}

void DrawStarfield(vector<vec3>& stars)
{
	sdlAux->clearPixels();

	//Adding black background
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			sdlAux->putPixel(x, y, vec3(0,0,0));
		}
	}

	float f = (float)(SCREEN_HEIGHT / 2);
	for (auto& star : stars) {
		float u = f * (star.x / star.z);
		float v = f * (star.y / star.z);
		//we want its luminosity to depend on the distance on the z axis, the farther, the darker
		//to make it physically motivated, color brightness will be inversely proportional to the inverse of the the squared distance
		vec3 color = 0.2f * vec3(1, 1, 1) / (star.z * star.z);
		sdlAux->putPixel(u, v, color);
	}

	sdlAux->render();
}

void DrawInterpolation()
{
	sdlAux->clearPixels();
	vector<vec3> leftSide(SCREEN_HEIGHT);
	vector<vec3> rightSide(SCREEN_HEIGHT);
	Interpolate(topLeft, bottomLeft, leftSide);
	Interpolate(topRight, bottomRight, rightSide);
	for (int y = 0; y<SCREEN_HEIGHT; ++y)
	{
		vector<vec3> rowColors(SCREEN_WIDTH);
		Interpolate(leftSide[y], rightSide[y], rowColors);
		for (int x = 0; x<SCREEN_WIDTH; ++x)
		{
			sdlAux->putPixel(x, y, rowColors[x]);
		}
	}

	sdlAux->render();
}