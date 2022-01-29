#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "Timer.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
	jump_coolddown.Start();
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(3.2, 0.6, 7);
	car.chassis2_size.Set(2.5, 1.7, 3.2);
	car.chassis3_size.Set(1, 1.4, 1.3);
	car.chassis4_size.Set(3.2, 0.8, 6.5);

	car.chassis_offset.Set(0, 0.5, 0);
	car.chassis2_offset.Set(0, 1, -1);
	car.chassis3_offset.Set(0, 0.7, 1.8);
	car.chassis4_offset.Set(0, 0.5, 0);

	car.mass = 130.0f;
	car.suspensionStiffness = 26.10f;
	car.suspensionCompression = 1.42f;
	car.suspensionDamping = 2.35f;
	car.maxSuspensionTravelCm = 510.0f;
	car.frictionSlip = 100.5;
	car.maxSuspensionForce = 1000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 0.8f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 0.8f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.2f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.2f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.2f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.2f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 5, 0);

	
	//vehicle->collision_listeners.add(this);
	vehicle->collision_listeners.add(App->scene_intro);

	vehicle->vehicle->getRigidBody()->setUserPointer(vehicle);

	initialPosition = vehicle->vehicle->getChassisWorldTransform().getOrigin();
	currentPlayerPosition = vehicle->vehicle->getChassisWorldTransform().getOrigin();

	//App->physics->AddConstraintP2P(*decorBody->body, *vehicle->body, car.rear_chassis_offset, car.rear_chassis_offset);

	inDirt = false;
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && (vehicle->GetKmh() < 120))
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && (vehicle->GetKmh() > -30))
	{
		if (vehicle->GetKmh() > 10)
		{
			brake = BRAKE_POWER / 24;
		}

		else
		{
			/*if (slow == true)
			{
				vehicle->body->setLinearVelocity(vehicle->body->getLinearVelocity() / 1.03f);
			}*/

			if (vehicle->GetKmh() < -30)
			{
				acceleration = MAX_ACCELERATION * 5;
			}
			acceleration = -MAX_ACCELERATION * 5;
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if(turn < TURN_DEGREES)
			turn +=  TURN_DEGREES;
	}

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	//if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
	//{
	//	brake = BRAKE_POWER;
	//}

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		acceleration= -MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if ((jump_coolddown.Read() * 0.001) >= JUMP_COOLDOWN)
		{
			vehicle->Push(0.0f, JUMP_IMPULSE, 0.0f);
			jump_coolddown.Start();
		}

	}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	if (!App->input->GetKey(SDL_SCANCODE_UP) && !App->input->GetKey(SDL_SCANCODE_DOWN))
	{
		vehicle->ApplyEngineForce(App->physics->DragForce(vehicle->GetKmh()));
	}

	//mat4x4 decorMatrix;
	//decorBody->GetTransform(&decorMatrix);
	//decor->transform = decorMatrix;
	vehicle->Render();

	float jump_cooldown_calc = 0.0f;
	jump_cooldown_calc = JUMP_COOLDOWN - jump_coolddown.Read() * 0.001f;
	if (jump_cooldown_calc < 0)
		jump_cooldown_calc = 0;

	int tiemer_milisec_read = 0;
	//tiemer_milisec_read = game_timer.Read() - chickens_taken * 2000;

	if (tiemer_milisec_read <= 0)
	{
		tiemer_milisec_read = 0;
	}

	float minutes_f = 0.0f;
	int minutes_i = 0;
	float decimal_minutes = 0.0f;
	float seconds_f = 0.0f;
	int seconds_i = 0;
	float decimal_seconds = 0.0f;
	int miliseconds_i = 0;
	minutes_f = tiemer_milisec_read * 0.001f * 0.0167f;
	minutes_i = minutes_f;
	decimal_minutes = minutes_f - minutes_i;
	seconds_f = decimal_minutes * 60;
	seconds_i = seconds_f;
	decimal_seconds = seconds_f - seconds_i;
	miliseconds_i = decimal_seconds * 1000;



	//AIR CONTROL
	btVector3 airControl;
	airControl = vehicle->vehicle->getChassisWorldTransform().getOrigin();
	if (airControl.getY() > 1)
	{
		Euler angles = vehicle->GetEulerAngles(vehicle->vehicle->getChassisWorldTransform().getRotation());

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			angles.yaw -= (DEGTORAD * 4);
			btQuaternion q;
			q.setEulerZYX(btScalar(angles.yaw), btScalar(angles.pitch), btScalar(angles.roll));
			vehicle->SetRotation(q);
		}
		else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			angles.yaw += (DEGTORAD * 4);
			btQuaternion q;
			q.setEulerZYX(btScalar(angles.yaw), btScalar(angles.pitch), btScalar(angles.roll));
			vehicle->SetRotation(q);
		}
		
	}

	char title[80];
	sprintf_s(title, "%.1f Km/h", vehicle->GetKmh());
	App->window->SetTitle(title);

	currentPlayerPosition = vehicle->vehicle->getChassisWorldTransform().getOrigin();

	return UPDATE_CONTINUE;
}



