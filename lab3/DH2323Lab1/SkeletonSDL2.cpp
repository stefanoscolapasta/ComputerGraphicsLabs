#include <iostream>
#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDL2auxiliary.h"
#include "TestModel.h"
#include <vector>

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
float depthBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

struct Pixel {
	int x;
	int y;
	float zinv;
	vec3 illumination;
};

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec3 reflectance;
};

// CAMERA STUFF
float yaw = 0; //camera y-rotation
vec3 cameraPos(0, 0, -3.001);
mat3 R(	cos(yaw),	0.0,	sin(yaw),
		0.0,		1.0,	0.0,
		-sin(yaw),	0.0,	cos(yaw)); //Initialized as identity matrix

// LIGHTING AND COLOR STUFF
vec3 currentColor;
vec3 lightPos(0, -0.5, -0.7);
vec3 lightPower = 15.0f * vec3(1, 1, 1);
vec3 indirectLightPowerPerArea = 0.5f * vec3(1, 1, 1);

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
void VertexShader(const Vertex& v, Pixel& p);
void Interpolate(Pixel a, Pixel b, vector<Pixel>& result);
void DrawLineSDL(Pixel a, Pixel b, vec3 color);
void DrawPolygonEdges(const vector<Vertex>& vertices);
void UpdateCameraRotation(float val);
void ComputePolygonRows(const vector<Pixel>& vertexPixels, vector<Pixel>& leftPixels, vector<Pixel>& rightPixels);
void DrawPolygonRows(const vector<Pixel>& leftPixels, const vector<Pixel>& rightPixels);
void DrawPolygon(const vector<Vertex>& vertices);
void PixelShader(const Pixel& p);

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

	//I AM NOT ENTIRELY SURE WHY, BUT UNLESS YOU FIRST BACK UP SLIGHTLY BEFORE ROTATING, IT BREAKS DOWN.
	//I SUPPOSE ITS RELATED TO THE CULLING ISSUE
	if (keysArray != NULL){
		if (keysArray[SDL_SCANCODE_UP]) {
			cameraPos.z += 0.02;
		}

		if (keysArray[SDL_SCANCODE_DOWN]) {
			cameraPos.z -= 0.02;
		}

		if (keysArray[SDL_SCANCODE_RIGHT]) {
			yaw -= 0.01f;
			UpdateCameraRotation(yaw);
		}

		if (keysArray[SDL_SCANCODE_LEFT]) {
			yaw += 0.01f;
			UpdateCameraRotation(yaw);
		}

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
	//Let's reset the depth buffer
	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			depthBuffer[y][x] = 0.0f;
		}
	}
	sdlAux->clearPixels();

	for (int i = 0; i < triangles.size(); ++i)
	{
		currentColor = triangles[i].color;
		vector<Vertex> vertices(3);
		vertices[0].position = triangles[i].v0;
		vertices[1].position = triangles[i].v1;
		vertices[2].position = triangles[i].v2;

		for (Vertex& vert : vertices) {
			vert.normal = triangles[i].normal;
			vert.reflectance = triangles[i].color;
		}

		DrawPolygon(vertices);
	}
	
	sdlAux->render();
}

void VertexShader(const Vertex& v, Pixel & p) {
	vec3 v_in_camera_coordinate_system = (v.position - cameraPos) * R;
	p.zinv = 1.0f / v_in_camera_coordinate_system.z;
	p.x = (focal_length * v_in_camera_coordinate_system.x * p.zinv) + (SCREEN_WIDTH / 2);
	p.y = (focal_length * v_in_camera_coordinate_system.y * p.zinv) + (SCREEN_HEIGHT / 2);

	float radius = glm::length(v.position - lightPos);

	float area = 4 * M_PI * (radius * radius);
	vec3 directLighting = lightPower * SDL_max(glm::dot(glm::normalize(v.normal), glm::normalize(lightPos -  v.position)), 0.0f) / area;
	vec3 indirectLighting = indirectLightPowerPerArea;
	p.illumination = v.reflectance * (directLighting + indirectLighting);
}

void PixelShader(const Pixel& p) {
	int x = p.x;
	int y = p.y;
	if (p.zinv > depthBuffer[y][x])
	{
		depthBuffer[y][x] = p.zinv;
		sdlAux->putPixel(x, y, p.illumination);
	}
}

void Interpolate(Pixel a, Pixel b, vector<Pixel>& result)
{
	int N = result.size();
	float maxV = (float)(glm::max(N - 1, 1));
	float xStep = (b.x - a.x) / maxV;
	float yStep = (b.y - a.y) / maxV;
	float zStep = (b.zinv - a.zinv) / maxV;
	vec3 lightStep = (b.illumination - a.illumination) / maxV;

	Pixel current(a);
	for (int i = 0; i < N; ++i)
	{
		current.x = a.x + i * xStep;
		current.y = a.y + i * yStep;
		current.zinv = a.zinv + i * zStep;
		current.illumination += lightStep;

		result[i] = current;
	}
}



void DrawLineSDL(Pixel a, Pixel b, vec3 color) {
	Pixel delta;
	delta.x = glm::abs(a.x - b.x);
	delta.y = glm::abs(a.y - b.y);

	int pixels = glm::max(delta.x, delta.y) + 1; //We get the max+1 so we cannot have holes
	
	vector<Pixel> line(pixels);
	Interpolate(a, b, line);

	for (auto& pixel : line) {
		PixelShader(pixel);
	}

}

void DrawPolygonEdges(const vector<Vertex>& vertices)
{
	int V = vertices.size();
	// Transform each vertex from 3D world position to 2D image position:
	vector<Pixel> projectedVertices(V);
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

void UpdateCameraRotation(float yaw_val) {
	vec3 col1(cos(yaw), 0.0f, -sin(yaw));
	vec3 col2(0.0f, 1.0f, 0.0f);
	vec3 col3(sin(yaw), 0.0f, cos(yaw));

	R = mat3(col1, col2, col3);
}

void DrawPolygon(const vector<Vertex>& vertices) {
	int V = vertices.size();
	vector<Pixel> vertexPixels(V);

	for (int i = 0; i < V; ++i) {
		VertexShader(vertices[i], vertexPixels[i]);
	}
		
	vector<Pixel> leftPixels;
	vector<Pixel> rightPixels;
	ComputePolygonRows(vertexPixels, leftPixels, rightPixels);
	DrawPolygonRows(leftPixels, rightPixels);
}

void ComputePolygonRows(const vector<Pixel>& vertexPixels, vector<Pixel>& leftPixels,
	vector<Pixel>& rightPixels)
{
	// 1. Find max and min y-value of the polygon
	// and compute the number of rows it occupies.
	int max = numeric_limits<int>::min();
	int min = numeric_limits<int>::max();

	for (auto& vertex : vertexPixels) {
		if (vertex.y > max) max = vertex.y;
		if (vertex.y < min) min = vertex.y;
	}
	// 2. Resize leftPixels and rightPixels
	// so that they have an element for each row.
	int ROWS = max - min + 1;

	leftPixels.resize(ROWS);
	rightPixels.resize(ROWS);
	// 3. Initialize the x-coordinates in leftPixels
	// to some really large value and the x-coordinates
	// in rightPixels to some really small value.
	for (int i = 0; i < ROWS; ++i)
	{
		leftPixels[i].x = SCREEN_WIDTH;
		leftPixels[i].y = min + i;

		rightPixels[i].x = 0;
		rightPixels[i].y = min + i;
	}
	// 4. Loop through all edges of the polygon and use
	// linear interpolation to find the x-coordinate for
	// each row it occupies. Update the corresponding
	// values in rightPixels and leftPixels.
	for (size_t i = 0; i < vertexPixels.size(); i++) {
		size_t vertex_to_interpolate_with_i_vertex = (i + 1) % vertexPixels.size();

		int steps_to_interpolate = std::abs(vertexPixels[i].y - vertexPixels[vertex_to_interpolate_with_i_vertex].y) + 1;

		vector<Pixel> line(steps_to_interpolate);

		Interpolate(vertexPixels[i], vertexPixels[vertex_to_interpolate_with_i_vertex], line);

		for (auto& pixel : line) {
			int i = pixel.y - min;

			if (pixel.x < leftPixels[i].x) {
				leftPixels[i].x = pixel.x;
				leftPixels[i].zinv = pixel.zinv;
				leftPixels[i].illumination = pixel.illumination;
			}
			if (pixel.x >= rightPixels[i].x) {
				rightPixels[i].x = pixel.x;
				rightPixels[i].zinv = pixel.zinv;
				rightPixels[i].illumination = pixel.illumination;
			}
		}

	}
}


void DrawPolygonRows(const vector<Pixel>& leftPixels, const vector<Pixel>& rightPixels) {
	for (size_t i = 0; i < leftPixels.size(); i++) {
		DrawLineSDL(leftPixels[i], rightPixels[i], currentColor);
	}
}