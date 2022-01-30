#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "glut/glut.h"
#include "Light.h"
#include "SString.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	Cylinder* CreateCylinder(vec3 pos, vec2 heightandAngle, Color rgb, float angle, vec3 pivot, float mass, SString name, bool isSensor = false);
	Cube* CreateCube(vec3 pos, vec3 size, Color rgb, float mass, SString name, bool isSensor = false);
	Cube* CreateRamp(vec3 pos, vec3 size, Color rgb, float angle, vec3 pivot, SString name, float mass, bool isSensor = false);

	void display(float dt);

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	void Floor(vec3 scale, int posX, int posZ, int cir);
	void Circuit(int* lvlcircuit, int* circuitx, int poscircuit);

public:
	/*
	PhysBody3D* pb_snake[MAX_SNAKE];
	Sphere s_snake[MAX_SNAKE];

	PhysBody3D* pb_snake2[MAX_SNAKE];
	Sphere s_snake2[MAX_SNAKE];
	*/
	bool lapSensorActivated = false;

	PhysBody3D* pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;

	p2List<Cube*> lights;

	p2List<Cube*> geometryList;
	p2List<PhysBody3D*> physBodies;

	p2DynArray<PhysBody3D*> pb_limits;
	p2DynArray<Cube>s_limits;

	PhysBody3D* pb_cube[5];
	Cube s_cube[5];

	PhysBody3D* pb_victory;
	Cube sensor_victory;

	PhysBody3D* pb_tricky;
	Cube sensor_tricky;

	p2DynArray<PhysBody3D*> pb_endlvl;
	p2DynArray<Cube>s_endlvl;

	int count = 0;
	int lvlFx = 0;
	int gameWinFx = 0;
	int deadFx = 0;
	int saving = 0;
	int randomize = 0;
	int volume = 10;
};
