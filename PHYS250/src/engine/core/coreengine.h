#ifndef _COREENGINE_
#define _COREENGINE_

#include "../render/window.h"
#include "../input/input.h"
#include "../util/booldef.h"

// TODO: separate into multiple files

/******************** Game Component ********************/

struct GameComponent_;
struct GameObject_;

typedef struct GameComponent_ {
    struct GameObject_* parent;
    void (*update)(struct GameComponent_*, float);
} GameComponent;

/******************** Game Component ********************/

/******************** Game Object ********************/

struct Scene_;

typedef struct GameObject_ {
    struct Scene_* scene;
    struct GameComponent_** gameComponents;
    int nGameComponents;
} GameObject;

extern void GameObject_init(GameObject* gameObject, int size);
extern void GameObject_deinit(GameObject* gameObject);
extern void GameObject_update(GameObject* gameObject, float deltaTime);

/******************** Game Object ********************/

/******************** Scene ********************/

struct Game_;

typedef struct Scene_ {
    struct Game_* game;
    GameObject** gameObjects;
    int nGameObjects;
} Scene;

extern void Scene_init(Scene* scene, int size);
extern void Scene_deinit(Scene* scene);
extern void Scene_update(Scene* scene, float deltaTime);

/******************** Scene ********************/

/******************** Asset ********************/

typedef struct {
    Scene* scenes;
    Surface* surfaces;
    float* variables;
    bool* conditions;
    // TODO: add more asset types as audio and other stuff is added to the engine
} Asset;

/******************** Asset ********************/

/******************** Game ********************/

struct CoreEngine_;

typedef struct Game_ {
    struct CoreEngine_* coreEngine;
    // optional object for holding all the assets of the game, game developer is responsible for managing the assets
    Asset* assets;
    Scene* activeScene;
} Game;

extern void Game_init(Game* game);
extern void Game_update(Game* game, float deltaTime);

/******************** Game ********************/

/******************** CoreEngine ********************/

typedef struct CoreEngine_ {
    Window* window;
    Input* input;
    Game* game;
} CoreEngine;

extern void CoreEngine_init(CoreEngine* coreEngine);
// must have window, input and game already initialized
extern void CoreEngine_run(CoreEngine* coreEngine);

/******************** CoreEngine ********************/

#endif
