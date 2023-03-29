#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include "SDL2auxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const float PI = 3.141592653589793238462;
SDL2Aux* sdlAux;
vector<Triangle> triangles;
float focalLength = SCREEN_WIDTH/2.0; //Usually it's set to one unit, but looking at TestModel.h, the objects are at a z-distance of around 100-300
//Setting the focal length to 1, what I was obtaining was a HUGE "corridor" effect, with the actual boxes being extremelly small at the center
//With a focal length of 1, the objects at the center were so small they could not even been seen
// ---------------------------------
// LIGHT STUFF
vec3 lightPos(0, -0.5, -0.7);
vec3 lightColor = 14.f * vec3(1, 1, 1);
//IDK but I find it nicer to call it ambient light
vec3 ambientLight = 0.5f * vec3(1, 1, 1);
//The color vector describes the power P, i.e. energy per time unit E/t of the emitted light for each
//color component.Each component thus has the physical unit W = J / s.

// CAMERA STUFF
vec3 cameraPos(0.0, 0.0, -2); //This is the origin/look from
mat3 R;
float yaw;
// ---------------------------------
int t;


//Structs
struct Intersection {
	vec3 position;
	float distance;
	int triangleindex;
};

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
);
void update_camera_rotation_matrix(void);
vec3 DirectLight(const Intersection& i);

int main( int argc, char* argv[] )
{
	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.
	LoadTestModel(triangles);
	while(!sdlAux->quitEvent())
	{
		Update();
		Draw();
	}

	sdlAux->saveBMP("screenshot.bmp");
	return 0;
}

namespace Directions {
	static const vec3 UP = glm::vec3(0.0f, 0.0f, 0.1f);
	static const vec3 DOWN = glm::vec3(0.0f, 0.0f, -0.1f);
	static const vec3 LEFT = glm::vec3(-0.1f, 0.0f, 0.0f);
	static const vec3 RIGHT = glm::vec3(0.1f, 0.0f, 0.0f);
}

void update_camera_rotation_matrix() {
	vec3 col1(cos(yaw),		0.0f,		-sin(yaw)	);
	vec3 col2(0.0f,			1.0f,		0.0f		);
	vec3 col3(sin(yaw),		0.0f,		cos(yaw)	);

	R = mat3(col1, col2, col3);
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	//I think this call is specific to SDL2
	//"Use SDL_PumpEvents() to update the state array."
	SDL_PumpEvents();
	const Uint8* keysArray = SDL_GetKeyboardState(NULL);

	if (keysArray != NULL) {
		if (keysArray[SDL_SCANCODE_UP]) { //UNDERSTANDING I HAD TO USE SCANCODE VALUES AND NOT KEYCODE VALUES TOOK A LOT OF TIME (was specified in the wiki luckily)
			//cameraPos += glm::vec3(0.0f, 0.0f, 0.1f);

			//Next line is to move the light source
			lightPos += Directions::UP;
		}
		if (keysArray[SDL_SCANCODE_DOWN]) {
			//cameraPos += glm::vec3(0.0f, 0.0f, -0.1f);

			//Next line is to move the light source
			lightPos += Directions::DOWN;
		}
		if (keysArray[SDL_SCANCODE_LEFT]) {
			//yaw -= 0.1f;
			//update_camera_rotation_matrix();

			//Next line is to move the light source
			lightPos += Directions::LEFT;
		}
		if (keysArray[SDL_SCANCODE_RIGHT]) {
			//yaw += 0.1f;
			//update_camera_rotation_matrix();

			//Next line is to move the light source
			lightPos += Directions::RIGHT;
		}
	}
}

void Draw()
{
	sdlAux->clearPixels();
	float half_width = (float)SCREEN_WIDTH / 2.0f;
	float half_heigth = (float)SCREEN_HEIGHT / 2.0f;
	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 ray_dir(x-half_width, y-half_heigth, focalLength);
			
			//Not sure if normalizing the direction vector is necessary, but usually it's done, so it can't do any harm
			ray_dir = glm::normalize(ray_dir);
			
			//Now we want to rotate the rays direction given the rotation matrix.
			//A mistake I was doing initially was to apply the rotation matrix to the coordinates of the camera directly, but
			//this does not make too much sense, if the camera were in 0,0,0 the rotation would have no effect for example and
			//I believe that applying the rotation to the rays is what was requested (although it was not 100% clear)
			ray_dir = R * ray_dir;
			Intersection rayTriangleIntersection;
			if (ClosestIntersection(cameraPos, ray_dir, triangles, rayTriangleIntersection)) {
				//sdlAux->putPixel(x, y, triangles[rayTriangleIntersection.triangleindex].color);
				//Next line is to obtain figure 4
				vec3 surfaceColor = triangles[rayTriangleIntersection.triangleindex].color;
				vec3 directLight = DirectLight(rayTriangleIntersection);
				vec3 directAndAmbient = directLight + ambientLight;
				sdlAux->putPixel(x, y, surfaceColor );
			}
			else {
				sdlAux->putPixel(x, y, vec3(0, 0, 0)); //black
			}
		}
	}
	sdlAux->render();
}

inline bool doesRayIntersectTriangle(float t, float u, float v) {
	return u >= 0 && v >= 0 && u + v < 1 && t >= 0;
}

bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection
) {
	closestIntersection.distance = std::numeric_limits<float>::max();

	bool found = false;
	for (size_t i = 0; i < triangles.size();i++) {
		Triangle triangle = triangles[i];
		vec3 v0 = triangle.v0;
		vec3 v1 = triangle.v1;
		vec3 v2 = triangle.v2;
		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start - v0;
		mat3 A(-dir, e1, e2);
		vec3 x = glm::inverse(A) * b; //Think of doing it closed form with Cramer's to speed it up
		float t = x.x;
		float u = x.y;
		float v = x.z;
		if (doesRayIntersectTriangle(t,u,v)) {
			found = true;
			if (t < closestIntersection.distance) {
				closestIntersection.distance = t;
				closestIntersection.position = start + (dir*t);
				closestIntersection.triangleindex = i;
			}
		}
	}
	return found; 
	
}

vec3 DirectLight(const Intersection& i) {
	//we have triangle index --> we can retrieve normal
	//Formula: D = B max(r̂.n̂, 0) = (P max(r̂.n̂, 0)) / 4πr2
	//remembering that the color vector given is the power P
	float radius = glm::length(i.position - lightPos);
	//for shadows we cast a ray from the intersection point towards the light, if the t value is smaller than the radius, then it's in shadow
	vec3 shadow_ray = glm::normalize(i.position - lightPos);
	Intersection inters;
	//"When illuminating a surface point we cast another ray from it to the light source."
	//This previous line of explanation in the lab instructions made me think that I needed to cast a ray FROM the intersection to the light source
	
	//What I was doing before --> if (ClosestIntersection(i.position, shadow_ray, triangles, inters)) {

	//This makes more sense in fact
	if (ClosestIntersection(lightPos, shadow_ray, triangles, inters)) {
		//I noticed that if I put <= the black spots increase, so I decided to subtract a threshold and it improved the result
		if (inters.distance < radius - 0.001f) {
			return vec3(0, 0, 0); //It's in shadow, return black
		}
	}

	float area = 4 * M_PI * pow(radius, 2);
	vec3 norm_normal = glm::normalize(triangles[i.triangleindex].normal);
	vec3 norm_r = glm::normalize(lightPos - i.position);
	vec3 P = lightColor;

	return (P * max(glm::dot(norm_r, norm_normal), 0.0f)) / area;
}
