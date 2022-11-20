#include "Application.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "FadeToBlack.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleSceneIntro.h"
#include "ModuleFonts.h"
#include "Die.h"


Die::Die(Application* app, bool start_disabled) : Module(app, start_disabled)
{
}

// Destructor
Die::~Die()
{}

// Called before render is available
bool Die::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Die::Start()
{
	img = App->textures->Load("pinball/Pinball_DeathScreen.png");
	init = App->scene_intro;

	return true;
}

// Called each loop iteration
update_status Die::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Called each loop iteration
update_status Die::Update()
{
	// Placeholder not needed any more
	App->renderer->Blit(img, 0, 0);

	App->fonts->BlitText(115, 550, init->scoreFont, init->HighscoreText);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		App->fadetoblack->FadeToblack(this, (Module*)App->scene_intro, 50);
	}
	return UPDATE_CONTINUE;
}

// Called each loop iteration
update_status Die::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool Die::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
