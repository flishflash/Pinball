#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "FadeToBlack.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_disabled) : Module(app, start_disabled)
{

	// Initialise all the internal class variables, at least to NULL pointer
	circle = box = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{
	// You should do some memory cleaning here, if required
}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	died = false;

	vidas = 3;

	resetPos = { 120,200 };

	vida = false;

	// Set camera position
	App->renderer->camera.x = App->renderer->camera.y = 0;

	// Load textures
	circle = App->textures->Load("pinball/BolaPinball_.png"); 
	box = App->textures->Load("pinball/Pinball.png");
	palaL = App->textures->Load("pinball/Pala_izq.png");
	palaR = App->textures->Load("pinball/Pala_der.png");

	// Create a big red sensor on the bottom of the screen.
	// This sensor will not make other objects collide with it, but it can tell if it is "colliding" with something else
	lower_ground_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT+25, SCREEN_WIDTH, 10);

	// Add this module (ModuleSceneIntro) as a listener for collisions with the sensor.
	// In ModulePhysics::PreUpdate(), we iterate over all sensors and (if colliding) we call the function ModuleSceneIntro::OnCollision()
	lower_ground_sensor->listener = this;

	ball = App->physics->CreateCircleDynamic(120, 200, 10);
	App->physics->CreateCircleStatic(107, 269, 13);
	App->physics->CreateCircleStatic(186, 267, 13);
	App->physics->CreateCircleStatic(150, 322, 13);

	//Right 

	right = App->physics->CreateRectangle(226, 559, 84, 20);

	point_right = App->physics->CreateCircleStatic(226, 559, 5);

	Joint_right.bodyA = right->body;
	Joint_right.bodyB = point_right->body;
	Joint_right.referenceAngle = 0 * DEGTORAD;
	Joint_right.enableLimit = true;
	Joint_right.lowerAngle = -25 * DEGTORAD;
	Joint_right.upperAngle = 25 * DEGTORAD;
	Joint_right.localAnchorA.Set(PIXEL_TO_METERS(30), 0);
	Joint_right.localAnchorB.Set(0, 0);
	b2RevoluteJoint* joint_right = (b2RevoluteJoint*)App->physics->world->CreateJoint(&Joint_right);

	//Left

	left = App->physics->CreateRectangle(72, 559, 84, 20);

	point_left = App->physics->CreateCircleStatic(72, 559, 5);

	Joint_left.bodyA = left->body;
	Joint_left.bodyB = point_left->body;
	Joint_left.referenceAngle = 0 * DEGTORAD;
	Joint_left.enableLimit = true;
	Joint_left.lowerAngle = -25 * DEGTORAD;
	Joint_left.upperAngle = 25 * DEGTORAD;
	Joint_left.localAnchorA.Set(PIXEL_TO_METERS(-30), 0);
	Joint_left.localAnchorB.Set(0, 0);
	b2RevoluteJoint* joint_left = (b2RevoluteJoint*)App->physics->world->CreateJoint(&Joint_left);


	return ret;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

update_status ModuleSceneIntro::Update()
{

	App->renderer->Blit(box, 0, 0);

	// If user presses SPACE, enable RayCast
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		// Enable raycast mode
		ray_on = !ray_on;

		// Origin point of the raycast is be the mouse current position now (will not change)
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}


	// The target point of the raycast is the mouse current position (will change over game time)
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();

	// Total distance of the raycast reference segment
	int ray_hit = ray.DistanceTo(mouse);

	// Declare a vector. We will draw the normal to the hit surface (if we hit something)
	fVector normal(0.0f, 0.0f);
	if (died == false)
	{
		if (vida == true)
		{
			ball->body->SetTransform(PIXEL_TO_METERS(resetPos), NULL);
			vida = false;
		}

		App->renderer->Blit(palaL, 58, 545, NULL, 1.0f, left->GetRotation()-5, 13, 13);

		App->renderer->Blit(palaR, 152, 545, NULL, 1.0f, right->GetRotation()+5, 74, 13);

		App->renderer->Blit(circle, METERS_TO_PIXELS(ball->body->GetPosition().x-10), METERS_TO_PIXELS(ball->body->GetPosition().y-10), NULL, 1.0f, ball->GetRotation());

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
			b2Vec2 force = b2Vec2(0, -200);
			right->body->ApplyForceToCenter(force, 1);
			Joint_right.lowerAngle = 25 * DEGTORAD;
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
			b2Vec2 force = b2Vec2(0, -200);
			left->body->ApplyForceToCenter(force, 1);
			Joint_left.lowerAngle = 25 * DEGTORAD;
		}
	}
	// Keep playing
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

	if (vidas >= 0)
	{
		if (vida==false)
		{
			vidas--;
			vida = true;
			LOG("vidas: %d", vidas);
		}
	}

	if (bodyB->body->GetType() == lower_ground_sensor->body->GetType() && vidas <= 0)
	{
		died = true;

		App->fadetoblack->FadeToblack(this, (Module*)App->die, 50);

		delete left;
		left = NULL;
		delete right;
		right = NULL;
		delete point_left;
		point_left = NULL;
		delete point_right;
		point_right = NULL;
	}
}
