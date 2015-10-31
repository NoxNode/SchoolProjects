#include "engine/render/window.h"
#include "engine/input/input.h"
#include "engine/core/coreengine.h"
#include <stdio.h>
#include "engine/util/booldef.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600

/******************** Custom Game Component code ********************/

float x = 0, y = 0;
float speed = 500;

void testUpdate(GameComponent* gameComponent, float deltaTime) {
	Keyboard* keyboard = gameComponent->parent->scene->game->coreEngine->input->keyboard;
	if(Keyboard_isKeyHeld(keyboard, SDLK_w)) {
		y -= deltaTime * speed;
	}
	if(Keyboard_isKeyHeld(keyboard, SDLK_a)) {
		x -= deltaTime * speed;
	}
	if(Keyboard_isKeyHeld(keyboard, SDLK_s)) {
		y += deltaTime * speed;
	}
	if(Keyboard_isKeyHeld(keyboard, SDLK_d)) {
		x += deltaTime * speed;
	}

	Surface* windowSurface = gameComponent->parent->scene->game->coreEngine->window->surface;
	Surface_fillRect(windowSurface, 0xFFFF0000, (int) x, (int) y, 50, 50);
}

/******************** Custom Game Component code ********************/

#undef main
int main(int argc, char** argv)
{
	Window window;
	Window_init(&window, "testing", DISPLAY_WIDTH, DISPLAY_HEIGHT);

	Input input;
	Input_init(&input);
	Input_enableKeyboard(&input, 255);

	CoreEngine coreEngine;
	CoreEngine_init(&coreEngine);
	coreEngine.window = &window;
	coreEngine.input = &input;

	Game game;
	Game_init(&game);
	coreEngine.game = &game;
	game.coreEngine = &coreEngine;

	Scene activeScene;
	Scene_init(&activeScene, 1);
	game.activeScene = &activeScene;
	activeScene.game = &game;

	GameObject gameObject;
	GameObject_init(&gameObject, 1);
	gameObject.scene = &activeScene;
	activeScene.gameObjects[0] = &gameObject;

	GameComponent gameComponent;
	gameComponent.parent = &gameObject;
	gameComponent.update = testUpdate;
	gameObject.gameComponents[0] = &gameComponent;

	CoreEngine_run(&coreEngine);

	GameObject_deinit(&gameObject);
	Scene_deinit(&activeScene);
	Input_deinit(&input);
	Window_deinit(&window);

	return 0;
}
