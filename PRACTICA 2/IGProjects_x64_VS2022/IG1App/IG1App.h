#ifndef _H_IG1App_H_
#define _H_IG1App_H_

// - Se crea directamente en el main y se encarga de manejar la ventana, la entrada / salida y el control general del programa -

// GUARDA (atributos):
// mWindow de tipo GLFWwindow* -> el puntero a la ventana de GLFW
// mWinW / mWinH -> dimensiones de la ventana
// mViewport -> puntero al viewport
// mCamera -> puntero a la camara
// mNeedsResdisplay -> para indicar a los manejadores de eventos si se ha de repintar la ventana
// mScenes -> vector de escenas de tipo vector<Scene*>
// mCurrentScene -> indice de escena actual
// => Las teclas numericas estan configuradas para seleccionar las escenas de este vector de escenas

// METODOS:
// run -> ejecuta el bucle principal
// init e iniWinOpengGL -> inicializan OpenGL, sus extensiones con GLEW y la ventana con GLFW
// resize -> maneja cambios en la dimension de la ventana
// key ->  maneja pulsaciones de caracteres en el teclado y specialkey hace lo propio con teclas especiales(flechas, escape, etc.)
// destroy -> cierra ordenadamente el programa


// ---- EJEMPLOS PROGRAMAS ----
// - SIMPLE -
/*
=> El programa simple se ocupa de renderizar entidades con un unico color fijo.
   Este se pasa a traves del parametro uniforme vec4 color.

=> El shader de vertices se limita a multiplicar el vertice por la
   matriz de modelado/vista y la de proyeccion.

=> El shader de fragmentos devuelve como color el parametro

	// codigo:
   uniform vec4 color; // uniform color for all vertices
   out vec4 FragColor; // fixed-name color variable
					   // used by OpenGL
   void main()
   {
	  FragColor = color;
   }
 */

// - VCOLORS-
/*
=> El programa vcolors se ocupa de renderizar entidades con colores
asociados a cada vertice (como el RGBAxis).

=> El shader de vertices transforma el vertice como simple ys
pasa el color al shader de fragmentos.

	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec4 aColor;
	uniform mat4 modelView; // model-view matrix
	uniform mat4 projection;
	out vec4 vertexColor;
	void main()
	{
		gl_Position = projection * modelView * vec4(aPos, 1.0);
		vertexColor = aColor;
	}

=> El shader de fragmentos toma el color recibido como color del fragmento.

	in vec4 vertexColor; // color assigned to this vertex
	out vec4 FragColor; // fixed-name color variable
	// used by OpenGL
	void main()
	{
		FragColor = vertexColor;
	}
*/

#include <GL/glew.h>     // OpenGL Extension Wrangler
#include <GLFW/glfw3.h>  // OpenGL Library
#include <glm/glm.hpp>   // OpenGL Mathematics

#include "Camera.h"
#include "Scene.h"
#include "Viewport.h"

class IG1App
{
public:
	// static single instance (singleton pattern)
	static IG1App s_ig1app;

	IG1App(IG1App const& J) = delete;         // no copy constructor
	void operator=(IG1App const& J) = delete; // no copy assignment

	// Viewport position and size
	Viewport const& viewPort() { return *mViewPort; };
	// Camera position, view volume and projection
	Camera const& camera() { return *mCamera; };
	// Graphics objects of the scene
	Scene const& scene() { return *mScenes[mCurrentScene]; };

	// Change to the given scene
	bool changeScene(size_t sceneNr);

	void run();   // the main event processing loop
	void close(); // the application

protected:
	IG1App() = default;
	~IG1App() { close(); };

	void init();
	void update();

	void iniWinOpenGL();
	void destroy();

	void display() const;                      // the scene
	void resize(int newWidth, int newHeight);  // the viewport (without changing the scale)
	void key(unsigned int codepoint); // keypress event
	void specialkey(int key, int scancode, int action, int mods); // special keypress event

	// static callbacks
	static void s_display(GLFWwindow*) { s_ig1app.display(); };
	static void s_resize(GLFWwindow*, int newWidth, int newHeight) { s_ig1app.resize(newWidth, newHeight); };
	static void s_key(GLFWwindow* win, unsigned int codepoint) { s_ig1app.key(codepoint); };
	static void s_specialkey(GLFWwindow* win, int key, int scancode, int action, int mods) { s_ig1app.specialkey(key, scancode, action, mods); };

	// sacar capturas
	void captura();

	// Viewport position and size
	Viewport* mViewPort = nullptr;
	// Camera position, view volume and projection
	Camera* mCamera = nullptr;
	// Graphics objects are in the scenes
	std::vector<Scene*> mScenes;
	size_t mCurrentScene = 0;

	bool mNeedsRedisplay = true;  // main event processing loop
	GLFWwindow* mWindow = nullptr; // window's handle
	int mWinW = 800;               // window's width
	int mWinH = 600;               // window's height

	// Control de tiempo
	const GLdouble FRAME_DURATION = 0.01;
	bool mUpdateEnabled = true;
	double mNextUpdate = 0.0;
	double mStartTime = 0.0;
};

#endif //_H_IG1App_H_