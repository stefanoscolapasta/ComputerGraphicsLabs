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

using namespace std;
using glm::vec3;

// ---------------------------------------------------------
// GLOBAL VARIABLES
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
vec3 topLeft(1, 0, 0); // red
vec3 topRight(0, 0, 1); // blue
vec3 bottomLeft(1, 1, 0); // yellow
vec3 bottomRight(0, 1, 0); // green //I swapped these two to obtain exactly the image shown in the lab instructions
SDL2Aux *sdlAux;

// ---------------------------------------------------------
// FUNCTION DECLARATIONS
void Draw();
void Interpolate(float a, float b, vector<float>& result);
void Interpolate(vec3 a, vec3 b, vector<vec3>& result);
// ---------------------------------------------------------

// FUNCTION DEFINITIONS
int main(int argc, char* argv[])
{
	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
	vector<vec3> result(4);
	vec3 a(1, 4, 9.2);
	vec3 b(4, 1, 9.8);
	Interpolate(a, b, result);
	for (int i = 0; i < result.size(); ++i)
	{
		cout << "( "
			<< result[i].x << ", "
			<< result[i].y << ", "
			<< result[i].z << " ) ";
	}

	while (!sdlAux->quitEvent()) {
		Draw();
	}
	sdlAux->saveBMP("screenshot.bmp");
	return 0;
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

void Draw()
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