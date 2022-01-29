#include "Globals.h"
#include "ModulePlayer.h"
#include "PhysVehicle3D.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "Color.h"

#include <stdio.h>
//#include "Color.cpp"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	Mix_ResumeMusic();
	Mix_SetMusicPosition(0);
	// Load music
	//App->audio->PlayMusic("Assets/capitolio3.ogg");

	App->camera->Move(vec3(0.0f, 4.0f, -12.0f));
	App->camera->LookAt(vec3(0, 3, 0));

	geometryList.add(CreateCube(vec3(-181.0f, 6.5f, -411.212f), vec3(1.0f, 13.0f, 815.0f), Blue, 0, "wall1"));
	geometryList.add(CreateCube(vec3(-151.028f, 6.5f, -394.152f), vec3(1.0f, 13.0f, 719.176f), Blue, 0, "wall2"));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Plane p(0, 1, 0, 0);
	//p.axis = true;
	//p.Render();	


    display(dt);

	return UPDATE_CONTINUE;
}

Cube* ModuleSceneIntro::CreateCube(vec3 pos, vec3 size, Color rgb, float mass, SString name, bool isSensor)
{
	Cube* cube = new Cube();
	cube->SetPos(pos.x, pos.y, pos.z);
	cube->size = size;
	cube->color = rgb;

	physBodies.add(App->physics->AddBodyV2(*cube, mass, isSensor, name));

	return cube;
}

Cube* ModuleSceneIntro::CreateRamp(vec3 pos, vec3 size, Color rgb, float angle, vec3 pivot, SString name, float mass, bool isSensor)
{
	Cube* cube = new Cube();
	cube->SetRotation(angle, pivot);
	cube->SetPos(pos.x, pos.y, pos.z);
	cube->size = size;
	cube->color = rgb;
	physBodies.add(App->physics->AddBodyV2(*cube, mass, isSensor, name));

	return cube;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

void ModuleSceneIntro::Circuit(int* lvlcircuit, int* circuitx, int poscircuit)
{

	//create sensors
	if (pb_limits.Count() != 0 && s_limits.Count() != 0 && s_limits.Count() == pb_limits.Count())
	{
		for (int i = 0; i < s_limits.Count(); i++)
		{
			pb_limits[i]->SetAsSensor(true);
			pb_limits[i]->collision_listeners.add(this);
		}

		for (int i = 0; i < s_endlvl.Count(); i++)
		{
			pb_endlvl[i]->SetAsSensor(true);
			pb_endlvl[i]->collision_listeners.add(this);
		}
	}
}

void ModuleSceneIntro::display(float dt)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindTexture(GL_TEXTURE_2D, texture[1]);

    glDisable(GL_TEXTURE_2D);

    Plane p(0, 1, 0, 0);
    p.axis = true;
    p.Render();

    //l.Render();

    p2List_item<Cube*>* itemCubes = geometryList.getFirst();

    p2List_item<PhysBody3D*>* itemBodies = physBodies.getFirst();

    while (itemBodies != nullptr && itemCubes != nullptr)
    {
        if (itemBodies->data->IsSensor() != true || itemBodies->data->name == "turbo")
        {
            itemCubes->data->Render();
        }
        itemBodies->data->GetTransform(&itemCubes->data->transform);
        itemCubes = itemCubes->next;
        itemBodies = itemBodies->next;
    }

    itemCubes = geometryList.getFirst();
    itemBodies = physBodies.getFirst();
    while (itemBodies != nullptr && itemCubes != nullptr)
    {
        if (itemBodies->data->name == "turbo")
        {
            //itemCubes->data->transform.rotate(angleTurbo, vec3(0.0f, 1.0f, 1.0f));
        }
        itemCubes = itemCubes->next;
        itemBodies = itemBodies->next;
    }
    //angleTurbo += 100.0f * dt;

    itemCubes = lights.getFirst();
    while (itemCubes != nullptr)
    {
        itemCubes->data->Render();
        itemCubes = itemCubes->next;
    }

    /*
    p2List_item<Torus*>* t = torusCheckpointList.getFirst();
    while (t != nullptr)
    {
        t->data->Render();
        t = t->next;
    }

    p2List_item<Cube*>* c = sticksList.getFirst();
    while (c)
    {
        c->data->Render();
        c = c->next;
    }
    */

    //donut.Render();

    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, texture[0]);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 0.0);

    //glVertex3f(-TEX_SIZE, 0.0, TEX_SIZE); // TEX_SIZE is image size

    glTexCoord2f(1.0, 0.0);

    //glVertex3f(TEX_SIZE, 0.0, TEX_SIZE);

    glTexCoord2f(1.0, 1.0);

    //glVertex3f(TEX_SIZE, 0.0, -TEX_SIZE);

    glTexCoord2f(0.0, 1.0);

    //glVertex3f(-TEX_SIZE, 0.0, -TEX_SIZE);

    glDisable(GL_TEXTURE_2D);

    glEnd();

    //glutSwapBuffers();

}