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
	score = 0;

	resetPos = { 291, 480 };

	vida = false;

	brilla_1 = false;
	brilla_2 = false;
	brilla_3 = false;

	// Set camera position
	App->renderer->camera.x = App->renderer->camera.y = 0;
	//Music and sounds
	App->audio->LoadFx("pinball/Audio/Boing.ogg");
	palanca = App->audio->LoadFx("pinball/Audio/Palanca_Mine.ogg");
	boing = App->audio->PlayMusic("pinball/Audio/Jetpack.ogg");
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
	Boing_Star = App->textures->Load("pinball/Collider_Star_Brillando.png");
	Tri = App->textures->Load("pinball/Collider_triangular.png");
	Tri_ = App->textures->Load("pinball/Collider_triangular_reverso.png");

	// Create a big red sensor on the bottom of the screen.
	// This sensor will not make other objects collide with it, but it can tell if it is "colliding" with something else
	lower_ground_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT+25, SCREEN_WIDTH, 10);

	//Listener points
	sc1 = App->physics->CreateRectangleSensor(107, 269, 27, 27);
	sc1->listener = this;
	sc2 = App->physics->CreateRectangleSensor(186, 267, 27, 27);
	sc2->listener = this;
	sc3 = App->physics->CreateRectangleSensor(150, 322, 27, 27);
	sc3->listener = this;
	sp1 = App->physics->CreateRectangleSensor(45, 279, 42, 12);
	sp1->listener = this;
	sp1->body->SetTransform(sp1->body->GetPosition(), -0.5235);
	sp2 = App->physics->CreateRectangleSensor(245, 275, 38, 10);
	sp2->listener = this;
	sp2->body->SetTransform(sp2->body->GetPosition(), 0.4235);
	sr = App->physics->CreateRectangleSensor(45, 390, 13, 100);
	sr->listener = this;

	st1 = App->physics->CreateRectangleSensor(65, 495, 5, 50);
	st1->listener = this;	
	st1->body->SetTransform(st1->body->GetPosition(), -0.5);
	st2 = App->physics->CreateRectangleSensor(225, 495, 5, 50);
	st2->listener = this;
	st2->body->SetTransform(st2->body->GetPosition(), 0.5);

	// Add this module (ModuleSceneIntro) as a listener for collisions with the sensor.
	// In ModulePhysics::PreUpdate(), we iterate over all sensors and (if colliding) we call the function ModuleSceneIntro::OnCollision()
	lower_ground_sensor->listener = this;

	ball = App->physics->CreateCircleDynamic(291, 480, 10);
	App->physics->CreateCircleStatic(107, 269, 13);
	App->physics->CreateCircleStatic(186, 267, 13);
	App->physics->CreateCircleStatic(150, 322, 13);

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

	char lookupTable[] = { "ABCDEFGHIJKLNOPKRSTUVXYZ0123456789: " };
	scoreFont = App->fonts->Load("pinball/ABC.png", lookupTable, 1);
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

	App->renderer->Blit(Tri, 48, 470);
	App->renderer->Blit(Tri_, 210, 470);

	if (vidas == 3) App->renderer->Blit(circle, 80, 658);
	if (vidas >= 2) App->renderer->Blit(circle, 55, 658);
	if (vidas >= 1) App->renderer->Blit(circle, 30, 658);

	if (brilla_1)
	{
		App->renderer->Blit(Boing_Star, 107 - 13, 269 - 13);
		brilla_1 = false;
	}
	if (brilla_2)
	{
		App->renderer->Blit(Boing_Star, 186 - 13, 267 - 13);
		brilla_2 = false;
	}
	if (brilla_3)
	{
		App->renderer->Blit(Boing_Star, 150 - 13, 322 - 13);
		brilla_3 = false;
	}

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
			App->audio->PlayFx(palanca);
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
			b2Vec2 force = b2Vec2(0, -200);
			left->body->ApplyForceToCenter(force, 1);
			Joint_left.lowerAngle = 25 * DEGTORAD;
			App->audio->PlayFx(palanca);
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

	App->fonts->BlitText(75, 35, scoreFont, "HI");

	//highscore
	if (score > highscore)
	{
		highscore = score;  
	}
	App->fonts->BlitText(107, 35, scoreFont, HighscoreText);

	App->renderer->Blit(Vanish_izq, 49 - 29, 279 - 5, NULL, NULL, -30);
	App->renderer->Blit(Vanish_der, 248 - 24, 279 - 7, NULL, NULL, 22);

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
		}

	}
	if (bodyB == sc1 || bodyB == sc2 || bodyB == sc3) {
		score += 50;
		if (bodyB == sc1) brilla_1 = true;
		if (bodyB == sc2) brilla_2 = true;
		if (bodyB == sc3) brilla_3 = true;
	}
	if (bodyB == sp1 || bodyB == sp2) {
		score += 75;
	}
	if (bodyB == sr) {
		score += 5;
	}
	if (bodyB == st1)
	{
		ball->body->ApplyLinearImpulse((b2Vec2{ PIXEL_TO_METERS(5), -PIXEL_TO_METERS(20)}), ball->body->GetLocalCenter(), true);
		App->audio->PlayFx(boing);
	}
	if (bodyB == st2)
	{
		ball->body->ApplyLinearImpulse((b2Vec2{ -PIXEL_TO_METERS(5), -PIXEL_TO_METERS(20) }), ball->body->GetLocalCenter(), true);
		App->audio->PlayFx(boing);
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
