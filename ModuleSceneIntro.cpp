#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "FadeToBlack.h"
#include "ModuleFonts.h"

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

	resetPos = { 291, 480 };

	vida = false;

	der, izq = false;

	// Set camera position
	App->renderer->camera.x = App->renderer->camera.y = 0;

	// Load textures
	circle = App->textures->Load("pinball/BolaPinball_.png"); 
	box = App->textures->Load("pinball/Pinball.png");
	palaL = App->textures->Load("pinball/Pala_izq.png");
	palaR = App->textures->Load("pinball/Pala_der.png");
	Muelle = App->textures->Load("pinball/Pinball_spring.png");
	Muellent = App->textures->Load("pinball/FIUYM_spring.png");
	Vanish_izq = App->textures->Load("pinball/VanishCollider_izq.png");
	Vanish_der = App->textures->Load("pinball/VanishCollider_der.png");
	Boing = App->textures->Load("pinball/Collider_Star.png");

	// Create a big red sensor on the bottom of the screen.
	// This sensor will not make other objects collide with it, but it can tell if it is "colliding" with something else
	lower_ground_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT+25, SCREEN_WIDTH, 10);

	// Add this module (ModuleSceneIntro) as a listener for collisions with the sensor.
	// In ModulePhysics::PreUpdate(), we iterate over all sensors and (if colliding) we call the function ModuleSceneIntro::OnCollision()
	lower_ground_sensor->listener = this;

	ball = App->physics->CreateCircleDynamic(291, 480, 10);
	App->physics->CreateCircleStatic(107, 269, 13);
	App->physics->CreateCircleStatic(186, 267, 13);
	App->physics->CreateCircleStatic(150, 322, 13);

	vanish_izq = App->physics->CreateRectangleSensor(46, 279, 10, 50);
	vanish_izq->body->SetTransform(vanish_izq->body->GetPosition(), vanish_izq->body->GetAngle()+2.7f);
	vanish_izq->listener = this;

	vanish_der = App->physics->CreateRectangleSensor(248, 279, 10, 50);
	vanish_der->body->SetTransform(vanish_der->body->GetPosition(), vanish_der->body->GetAngle() - 2.75f);
	vanish_der->listener = this;

	//MuelleInit
	initMPos = { 283, 573 };
	initMaxPos = { 288, 590 };
	muelle = App->physics->CreateRectangle(283, 573, 20, 7);
	muelle_max = App->physics->CreateRectangle(288, 590, 20, 7);
	muelle->body->SetType(b2_dynamicBody);
	muelle_max->body->SetType(b2_staticBody);
	muelle_max->body->SetFixedRotation(true);

	//Joint Right 

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

	//Joint Left

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

	char lookupTable[] = { "0123456789abcdefghijklmnopqrstuvwxyz.@'&- " };
	scoreFont = App->fonts->Load("pinball/ui_font.png", lookupTable, 1);
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

	App->renderer->Blit(Boing, 107-13, 269-13);
	App->renderer->Blit(Boing, 186-13, 267-13);
	App->renderer->Blit(Boing, 150-13, 322-13);

	if (izq == false) App->renderer->Blit(Vanish_izq, METERS_TO_PIXELS(vanish_izq->body->GetPosition().x)-29, METERS_TO_PIXELS(vanish_izq->body->GetPosition().y)-5, NULL, NULL, -30);
	if(der==false) App->renderer->Blit(Vanish_der, METERS_TO_PIXELS(vanish_der->body->GetPosition().x)-24, METERS_TO_PIXELS(vanish_der->body->GetPosition().y)-7, NULL, NULL, 22);

	if (died==false)
	{
		App->renderer->Blit(Muelle, METERS_TO_PIXELS(muelle_max->body->GetPosition().x) - 10, METERS_TO_PIXELS(muelle_max->body->GetPosition().y) - 10);
	}
	App->renderer->Blit(Muellent, 277, 619);

	// If user presses SPACE, enable RayCast
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && METERS_TO_PIXELS(muelle_max->body->GetPosition().y) <= 607)
	{
		b2Vec2 almuerzo = { muelle_max->body->GetPosition().x, muelle_max->body->GetPosition().y + 0.005f};
		muelle_max->body->SetTransform(almuerzo, 0);
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		b2Vec2 iner = { 0, -((muelle_max->body->GetPosition().y-PIXEL_TO_METERS(initMPos.y))+5) };
		b2Vec2 centre = { muelle->body->GetLocalCenter() };
		muelle->body->ApplyLinearImpulse(PIXEL_TO_METERS(iner), centre, true);
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
			muelle->body->SetTransform(PIXEL_TO_METERS(initMPos), 0);
			muelle_max->body->SetTransform(PIXEL_TO_METERS(initMaxPos), 0);
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
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		died = true;

		App->fadetoblack->FadeToblack(this, this, 50);

		delete ball;
		ball = NULL;
		delete muelle;
		muelle = NULL;
		delete muelle_max;
		muelle_max = NULL;
		delete left;
		left = NULL;
		delete right;
		right = NULL;
		delete point_left;
		point_left = NULL;
		delete point_right;
		point_right = NULL;
	}
	sprintf_s(scoreText, 10, "%7d", score);
	sprintf_s(HighscoreText, 10, "%7d", highscore);

	App->fonts->BlitText(10, 35, scoreFont, scoreText);

	App->fonts->BlitText(75, 35, scoreFont, "hi");

	//highscore
	if (score > highscore)
	{
		highscore = score;  
	}
	App->fonts->BlitText(107, 35, scoreFont, HighscoreText);


	// Keep playing
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (bodyB->body->GetPosition() == lower_ground_sensor->body->GetPosition() && vidas >= 0)
	{
		if (vida==false)
		{
			vidas--;
			vida = true;
			LOG("vidas: %d", vidas);
			izq = false;
			der = false;
		}

	}

	if (bodyB->body->GetPosition() == vanish_izq->body->GetPosition())
	{
		izq = true;
	}

	if (bodyB->body->GetPosition() == vanish_der->body->GetPosition())
	{
		der = true;
	}



	if (bodyB->body->GetPosition() == lower_ground_sensor->body->GetPosition() && vidas <= 0)
	{
		died = true;

		App->fadetoblack->FadeToblack(this, (Module*)App->die, 50);

		delete muelle;
		muelle = NULL;
		delete muelle_max;
		muelle_max = NULL;
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
