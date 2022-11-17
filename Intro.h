#ifndef __INTRO_H__
#define __INTRO_H__

#include "Module.h"

struct SDL_Texture;

class Intro : public Module
{
public:

	Intro(Application* app, bool start_enabled = true);

	// Destructor
	~Intro();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	update_status PreUpdate();

	// Called each loop iteration
	update_status Update();

	// Called before all Updates
	update_status PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

private:
	SDL_Texture* intro;

};

#endif // __INTRO_H__