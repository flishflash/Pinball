#pragma once

#include "p2List.h"
#include "Globals.h"

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModulePlayer;
class ModuleSceneIntro;
class Intro;
class Die;
class ModulePhysics;
class FadeToBlack;


class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	FadeToBlack* fadetoblack;
	ModuleInput* input;
	ModuleAudio* audio;
	ModulePlayer* player;
	Intro* intro;
	ModuleSceneIntro* scene_intro;
	Die* die;
	ModulePhysics* physics;
	

private:

	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
};