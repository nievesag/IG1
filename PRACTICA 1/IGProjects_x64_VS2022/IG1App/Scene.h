#ifndef _H_Scene_H_
#define _H_Scene_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "Entity.h"

#include <vector>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene(); // los hijos al no tener destructora definida van a llamar directamente a la del padre

	Scene(const Scene& s) = delete;            // no copy constructor
	Scene& operator=(const Scene& s) = delete; // no copy assignment

	virtual void init();
	virtual void update();

	void render(Camera const& cam) const;

	// load or unload scene data into the GPU
	void load();
	void unload();

protected:
	void destroy();
	void setGL();
	void resetGL();

	void reset();

	std::vector<Abs_Entity*> gObjects; // Entities (graphic objects) of the scene
};

// --- ESCENAS HIJAS ---
// Scene0, Scene1, Scene2 ...

class Scene0 : public Scene
{
public:
	Scene0() = default;
	void init() override;
};

class Scene1 : public Scene
{
public:
	Scene1() = default;
	void init() override;
};

class Scene2 : public Scene
{
public:
	Scene2() = default;
	void init() override;
};

#endif //_H_Scene_H_