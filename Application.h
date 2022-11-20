#pragma once

#include "p2List.h"
#include "Globals.h"

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModuleSceneIntro;
class Intro;
class Die;
class ModulePlayer;
class ModulePhysics;
class FadeToBlack;
class ModuleFonts;
class Animation;


class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	FadeToBlack* fadetoblack;
	ModuleInput* input;
	ModuleAudio* audio;
	Intro* intro;
	ModuleSceneIntro* scene_intro;
	Die* die;
	ModulePlayer* player;
	ModulePhysics* physics;
	ModuleFonts* fonts;
	Animation* animation;
	

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