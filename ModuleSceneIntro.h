#pragma once
#include "Module.h"
#include "ModulePhysics.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:

	// Constructors & Destructors
	ModuleSceneIntro(Application* app, bool start_enabled = false);
	~ModuleSceneIntro();

	// Main module steps
	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:

	// Lists of physics objects
	PhysBody* ball;

	// Lower ground sensor (will kill all objects passig through it)
	PhysBody* lower_ground_sensor;
	bool sensed;

	// Textures
	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* palaR;
	SDL_Texture* palaL;
	SDL_Texture* Muelle;
	SDL_Texture* Muellent;
	SDL_Texture* Vanish_izq;
	SDL_Texture* Vanish_der;
	SDL_Texture* Boing;
	SDL_Texture* Boing_Star;
	SDL_Texture* Tri;
	SDL_Texture* Tri_;

	//palas
	PhysBody* muelle;
	PhysBody* muelle_max;
	PhysBody* right;
	PhysBody* point_right;
	PhysBody* left;
	PhysBody* point_left;
	PhysBody* vanish_izq;
	PhysBody* vanish_der;
	b2RevoluteJointDef Joint_right;
	b2RevoluteJointDef Joint_left;

	// FX
	uint bonus_fx;

	// Raycast
	p2Point<int> ray;
	bool ray_on;

	//Listeners
	PhysBody* sc1;
	PhysBody* sc2;
	PhysBody* sc3;

	PhysBody* sp1;
	PhysBody* sp2;	
	
	PhysBody* st1;
	PhysBody* st2;

	PhysBody* sr;

	//plus
	bool died = false;
	int vidas = 3;
	bool vida;
	b2Vec2 resetPos;
	b2Vec2 initMPos;
	b2Vec2 initMaxPos;
	bool brilla_1 = false;
	bool brilla_2 = false;
	bool brilla_3 = false;

	// Font score index
	uint score = 000;
	int scoreFont = -1;
	char scoreText[10] = { "\0" };

	uint highscore = 000;
	char HighscoreText[10] = { "\0" };

};
