#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Scene::~Scene()
{
	destroy();
	resetGL();
}

void
Scene::destroy()
{ // release memory and resources

	for (Abs_Entity* el : gObjects)
		delete el;

	gObjects.clear();
}

void
Scene::load()
{
	for (Abs_Entity* obj : gObjects)
		obj->load();
}

void
Scene::unload()
{
	for (Abs_Entity* obj : gObjects)
		obj->unload();
}

void
Scene::setGL()
{
	// OpenGL basic setting
	glClearColor(0.6, 0.7, 0.8, 1.0); // background color (alpha=1 -> opaque)
	glEnable(GL_DEPTH_TEST);          // enable Depth test
}
void
Scene::resetGL()
{
	glClearColor(.0, .0, .0, .0); // background color (alpha=1 -> opaque)
	glDisable(GL_DEPTH_TEST);     // disable Depth test
}

// Para borrar las cosas al cambiar de una escena a otra (ponerla en blanco otra vez).
void Scene::reset()
{
	destroy(); // libera memoria.
	resetGL(); // resetea OpenGL.
	craftScene();
}

void Scene::craftScene()
{
	setGL(); // OpenGL settings

	// allocate memory and load resources
	// Lights
	// Textures

	// Graphics objects (entities) of the scene
	gObjects.push_back(new RGBAxes(400.0)); // EJES XYZ.
}

void
Scene::render(Camera const& cam) const
{
	cam.upload();

	for (Abs_Entity* el : gObjects)
		el->render(cam.viewMat());
}

// --- ESCENAS HIJAS ---
// Scene0, Scene1, Scene2 ... 

// ---- SCENE 0 ----
void Scene0::init()
{
	craftScene();

	// Triangulo cian.
	gObjects.push_back(new RegularPolygon(3, 200, { 0.0, 1.0, 1.0, 1.0 }));

	// Circulo magenta.
	gObjects.push_back(new RegularPolygon(40, 200, { 1.0, 0.0, 1.0, 1.0 }));
}

// ---- SCENE 1 ----
void Scene1::init()
{
	craftScene();

	// Triangulo RGB
	gObjects.push_back(new RGBTriangle());

	// Rectangulo RGB
	gObjects.push_back(new RGBRectangle(200, 100));

	// Circulo blanco.
	gObjects.push_back(new RegularPolygon(40, 100, { 1.0, 1.0, 1.0, 1.0 }));
}

// ---- SCENE 2 ----
void Scene2::init()
{
	craftScene();
}