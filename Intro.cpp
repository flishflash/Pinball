#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "FadeToBlack.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "Intro.h"


Intro::Intro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
Intro::~Intro()
{}

// Called before render is available
bool Intro::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Intro::Start()
{
	intro = App->textures->Load("pinball/Pinball_StartScreen.png");

	return true;
}

// Called each loop iteration
update_status Intro::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Called each loop iteration
update_status Intro::Update()
{
	// Placeholder not needed any more
	App->renderer->Blit(intro, 0, 0);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		App->fadetoblack->FadeToblack(this, (Module*)App->scene_intro, 50);
	}

	return UPDATE_CONTINUE;
}

// Called each loop iteration
update_status Intro::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool Intro::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
