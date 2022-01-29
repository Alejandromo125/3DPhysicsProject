#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "Timer.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f
#define JUMP_IMPULSE 1500.0f
#define JUMP_COOLDOWN 3.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	PhysVehicle3D* vehicle;
	btVector3 initialPosition;
	float turn;
	float acceleration;
	float brake;

private:

	Timer jump_coolddown;

	Timer break_timer;
};