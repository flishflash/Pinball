#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	// Initialise all the internal class variables, at least to NULL pointer
	circle = box = rick = NULL;
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

	// Set camera position
	App->renderer->camera.x = App->renderer->camera.y = 0;

	// Load textures
	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/Pinball.png");

	// Create a big red sensor on the bottom of the screen.
	// This sensor will not make other objects collide with it, but it can tell if it is "colliding" with something else
	lower_ground_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	// Add this module (ModuleSceneIntro) as a listener for collisions with the sensor.
	// In ModulePhysics::PreUpdate(), we iterate over all sensors and (if colliding) we call the function ModuleSceneIntro::OnCollision()
	lower_ground_sensor->listener = this;

	ball = App->physics->CreateCircleDynamic(100, 200, 10);
	App->physics->CreateCircleStatic(107, 269, 13);
	App->physics->CreateCircleStatic(186, 267, 13);
	App->physics->CreateCircleStatic(150, 322, 13);

	//Right 

	right = App->physics->CreateRectangle(0, 500, 84, 20);

	point_right = App->physics->CreateCircleStatic(236, 589, 5);

	Joint_right.bodyA = right->body;
	Joint_right.bodyB = point_right->body;
	Joint_right.referenceAngle = 0 * DEGTORAD;
	Joint_right.enableLimit = true;
	Joint_right.lowerAngle = -30 * DEGTORAD;
	Joint_right.upperAngle = 25 * DEGTORAD;
	Joint_right.localAnchorA.Set(PIXEL_TO_METERS(30), 0);
	Joint_right.localAnchorB.Set(0, 0);
	b2RevoluteJoint* joint_right = (b2RevoluteJoint*)App->physics->world->CreateJoint(&Joint_right);

	//Left

	left = App->physics->CreateRectangle(0, 500, 84, 20);

	point_left = App->physics->CreateCircleStatic(82, 589, 5);

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

	App->renderer->Blit(box, 10, 30);

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

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		b2Vec2 force = b2Vec2(0, -200);
		right->body->ApplyForceToCenter(force, 1);
		revoluteJointDef_right.lowerAngle = 30 * DEGTORAD;
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		b2Vec2 force = b2Vec2(0, -200);
		left->body->ApplyForceToCenter(force, 1);
		revoluteJointDef_left.lowerAngle = 30 * DEGTORAD;
	}

	// Keep playing
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	// Play Audio FX on every collision, regardless of who is colliding
	App->audio->PlayFx(bonus_fx);

	// Do something else. You can also check which bodies are colliding (sensor? ball? player?)
}
