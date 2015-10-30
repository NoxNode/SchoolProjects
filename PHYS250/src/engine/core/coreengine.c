#include "coreengine.h"
#include "../util/time.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

/******************** Game Object ********************/

void GameObject_init(GameObject* gameObject, int size) {
    gameObject->scene = 0;
    gameObject->nGameComponents = size;
    gameObject->gameComponents = (GameComponent**) malloc(sizeof(GameComponent*) * size);
    for(int i = 0; i < gameObject->nGameComponents; i++) {
        gameObject->gameComponents[i] = 0;
    }
}

void GameObject_deinit(GameObject* gameObject) {
    free(gameObject->gameComponents);
}

void GameObject_update(GameObject* gameObject, float deltaTime) {
    for(int i = 0; i < gameObject->nGameComponents; i++) {
        GameComponent* component = gameObject->gameComponents[i];
        if(component != 0) {
            component->update(component, deltaTime);
        }
    }
}

/******************** Game Object ********************/

/******************** Scene ********************/

void Scene_init(Scene* scene, int size) {
    scene->game = 0;
    scene->nGameObjects = size;
    scene->gameObjects = (GameObject**) malloc(sizeof(GameObject*) * size);
    for(int i = 0; i < scene->nGameObjects; i++) {
        scene->gameObjects[i] = 0;
    }
}

void Scene_deinit(Scene* scene) {
    free(scene->gameObjects);
}

void Scene_update(Scene* scene, float deltaTime) {
    for(int i = 0; i < scene->nGameObjects; i++) {
        GameObject* obj = scene->gameObjects[i];
        if(obj != 0) {
            GameObject_update(obj, deltaTime);
        }
    }
}

/******************** Scene ********************/

/******************** Game ********************/

void Game_init(Game* game) {
    game->coreEngine = 0;
    game->assets = 0;
    game->activeScene = 0;
}

void Game_update(Game* game, float deltaTime) {
    if(game->activeScene != 0) {
        Scene_update(game->activeScene, deltaTime);
    }
}

/******************** Game ********************/

void CoreEngine_init(CoreEngine* coreEngine) {
    coreEngine->window = 0;
    coreEngine->input = 0;
    coreEngine->game = 0;
}

void CoreEngine_run(CoreEngine* coreEngine) {
    unsigned int lastTime = 0;
    unsigned int currentTime;

    // temp code start: fps counter
    unsigned int frames = 0;
    unsigned int lastPrintTime = 0;
    // temp code end: fps counter

    if(coreEngine->input != 0 && coreEngine->window != 0 && coreEngine->game != 0) {
        while(!coreEngine->input->isClosing) {
            Input_handleInput(coreEngine->input);
            Input_update(coreEngine->input);
            Surface_fill(coreEngine->window->surface, 0xFF000000);

            currentTime = getTime();
            unsigned int uint_deltaTime = currentTime - lastTime;
            lastTime = currentTime;
            float deltaTime = ((float) uint_deltaTime) / 1000.0f;
            Game_update(coreEngine->game, deltaTime);

            // temp code start: fps counter
            frames++;
            if(currentTime > lastPrintTime + 1000) {
                printf("%s%d\n", "fps: ", frames);
                frames = 0;
                lastPrintTime = currentTime;
            }
            // temp code end: fps counter

            Window_updateSurface(coreEngine->window);
        }
    }
}
