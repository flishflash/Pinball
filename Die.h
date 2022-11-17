#ifndef __DIE_H__
#define __DIE_H__

#include "Module.h"

struct SDL_Texture;

class Die : public Module
{
public:

	Die(Application* app, bool start_enabled = false);

	// Destructor
	virtual ~Die();

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
	SDL_Texture* img;

};

#endif // __DIE_H__