#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDL2auxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::ivec2;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float focal_length = 500;
SDL2Aux* sdlAux;
int t;
vector<Triangle> triangles;


// CAMERA STUFF

vec3 cameraPos(0, 0, -3.001);
mat3 cameraRotationMatrix(	1.0,	0.0,	0.0,
							0.0,	1.0,	0.0,
							0.0,	0.0,	1.0); //Initialized as identity matrix

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
void VertexShader(const vec3& v, ivec2& p);
void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result);
void DrawLineSDL(ivec2 a, ivec2 b, vec3 color);
void DrawPolygonEdges(const vector<vec3>& vertices);

int main( int argc, char* argv[] )
{
	LoadTestModel( triangles );
	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.

	while( !sdlAux->quitEvent() )
	{
		Update();
		Draw();
	}

	sdlAux->saveBMP("screenshot.bmp");
	return 0;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	SDL_PumpEvents(); 
	const Uint8* keysArray = SDL_GetKeyboardState(NULL);
	if (keysArray != NULL){
		if( keysArray[SDL_SCANCODE_UP] )
			;

		if( keysArray[SDL_SCANCODE_DOWN] )
			;

		if( keysArray[SDL_SCANCODE_RIGHT] )
			;

		if( keysArray[SDL_SCANCODE_LEFT] )
			;

		if( keysArray[SDL_SCANCODE_RSHIFT] )
			;

		if( keysArray[SDL_SCANCODE_RCTRL] )
			;

		if( keysArray[SDL_SCANCODE_W] )
			;

		if( keysArray[SDL_SCANCODE_S] )
			;

		if( keysArray[SDL_SCANCODE_D] )
			;

		if( keysArray[SDL_SCANCODE_A] )
			;

		if( keysArray[SDL_SCANCODE_E] )
			;

		if( keysArray[SDL_SCANCODE_Q] )
			;
	}
}

void Draw()
{
	sdlAux->clearPixels();
	
	for (int i = 0; i < triangles.size(); ++i)
	{
		vector<vec3> vertices(3);
		vertices[0] = triangles[i].v0;
		vertices[1] = triangles[i].v1;
		vertices[2] = triangles[i].v2;
		DrawPolygonEdges(vertices);
	}
	
	sdlAux->render();
}

void VertexShader(const vec3& v, ivec2& p) {
	vec3 v_in_camera_coordinate_system = (v - cameraPos) * cameraRotationMatrix;
	p.x = (focal_length * (v_in_camera_coordinate_system.x / v_in_camera_coordinate_system.z)) + (SCREEN_WIDTH / 2);
	p.y = (focal_length * (v_in_camera_coordinate_system.y / v_in_camera_coordinate_system.z)) + (SCREEN_HEIGHT / 2);
}

void Interpolate(ivec2 a, ivec2 b, vector<ivec2>& result)
{
	//NOTE: Bresenham's line algorithm could do this faster, consider implementing after lab finished.
	int N = result.size();
	float number_of_steps = float(glm::max(N - 1, 1));
	// ivec2 vecDiff = ivec2(b - a);//I was going mad and then realized IVEC is an !!! INTEGER !!! vector, so values were always casted... :(
	//To avoid having to do component-wise operations, I'll use glm::vec2
	glm::vec2 stepVec((float)(b.x - a.x) / number_of_steps, (float)(b.y - a.y) / number_of_steps);

	glm::vec2 current(a);
	for (int i = 0; i < N; ++i)
	{
		result[i] = current;
		current += stepVec;
	}
}

void DrawLineSDL(ivec2 a, ivec2 b, vec3 color) {
	glm::vec2 delta(0,0);
	delta.x = glm::abs(a.x - b.x);
	delta.y = glm::abs(a.y - b.y);

	int pixels = glm::max(delta.x, delta.y) + 1; //We get the max+1 so we cannot have holes
	vector<ivec2> line(pixels);
	Interpolate(a, b, line);
	for (auto& pixel : line) {
		sdlAux->putPixel(pixel.x, pixel.y, color);
	}
}

void DrawPolygonEdges(const vector<vec3>& vertices)
{
	int V = vertices.size();
	// Transform each vertex from 3D world position to 2D image position:
	vector<ivec2> projectedVertices(V);
	for (int i = 0; i < V; ++i)
	{
		VertexShader(vertices[i], projectedVertices[i]);
	}
	// Loop over all vertices and draw the edge from it to the next vertex:
	for (int i = 0; i < V; ++i)
	{
		int j = (i + 1) % V; // The next vertex, so avoid exceeding V size and it "wraps" around
		vec3 color(1, 1, 1);
		DrawLineSDL(projectedVertices[i], projectedVertices[j], color);
	}
}
