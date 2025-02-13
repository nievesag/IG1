#include "IG1App.h"

#include <iostream>

using namespace std;

// static single instance (singleton pattern)
IG1App IG1App::s_ig1app; // default constructor (constructor with no parameters)

// Print OpenGL errors and warnings
void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam)
{
	const char* prefix = (type == GL_DEBUG_TYPE_ERROR)
		? "\x1b[31m[ERROR]\x1b[0m "
		: "\x1b[33m[WARNING]\x1b[0m ";
	cout << prefix << message << endl;
}

void
IG1App::close()
{
	cout << "Closing GLFW...\n";
	glfwSetWindowShouldClose(mWindow, true); // stops main loop

	delete mViewPort;
	delete mCamera;
	for (auto scene : mScenes)
		delete scene;
}

void
IG1App::run() // enters the main event processing loop
{
	if (mWindow == 0) // if not intilialized
		init();

	// IG1App main loop
	while (!glfwWindowShouldClose(mWindow)) 
	{
		// Redisplay the window if needed
		if (mNeedsRedisplay) {
			display();
			mNeedsRedisplay = false;
		}

		// si se permite la actualizacion
		if (mUpdateEnabled) 
		{
			// tiempo transcurrido desde que abres la ventana en segundos
			mStartTime = glfwGetTime();

			// tiempo que se ha tardado en ejecutar lo anterior (si tarda aunque sea mu poquitillo mu poquitillo cosa de na se lo quita 2ms o menos o lo que sea para que sea preciso).
			mNextUpdate = glfwGetTime() - mStartTime;
		
			update(); // llama al metodo update de cada objeto de la escena
			
			// con el tiempo restante para llegar a mNextUpdate
			glfwWaitEventsTimeout(FRAME_DURATION - mNextUpdate);
		}
		else
		{
			// Stop and wait for new events
			glfwWaitEvents();
		}
	}

	destroy();
}

void
IG1App::init()
{
	// create an OpenGL Context
	iniWinOpenGL();

	// create the scene after creating the context
	// allocate memory and resources
	mViewPort = new Viewport(mWinW, mWinH);
	mCamera = new Camera(mViewPort);
	
	// Crea las escenas
	Scene0* scene0 = new Scene0();
	Scene1* scene1 = new Scene1();
	Scene2* scene2 = new Scene2();

	// Mete las escenas en el vector de escenas
	mScenes.push_back(scene0);
	mScenes.push_back(scene1);
	mScenes.push_back(scene2);

	mCamera->set2D();
	
	for (int i = 0; i < mScenes.size(); i++) 
	{
		mScenes[i]->init();
	}
	
	mScenes[mCurrentScene]->load(); // !! HACER LOAD, para cargar los objetos a la escena
}

void IG1App::update()
{
	//mScenes[mCurrentScene]->unload();
	//mScenes[mCurrentScene]->load();
	mScenes[mCurrentScene]->update();
	mNeedsRedisplay = true;
	//display();
}

void
IG1App::iniWinOpenGL()
{ // Initialization
	cout << "Starting GLFW...\n";
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // enable on macOS

	// Create window with its associated OpenGL context, return window's pointer
	mWindow = glfwCreateWindow(mWinW, mWinH, "IG1App", nullptr, nullptr);

	if (mWindow == nullptr) {
		glfwTerminate();
		throw std::logic_error("cannot create GLFW window");
	}

	glfwMakeContextCurrent(mWindow);

	// Initialize OpenGL extension library
	if (GLenum err = glewInit(); err != GLEW_OK) {
		glfwTerminate();
		throw std::logic_error("Error while loading extensions: "s +
			reinterpret_cast<const char*>(glewGetErrorString(err)));
	}

	// Callback registration
	glfwSetWindowSizeCallback(mWindow, s_resize);
	glfwSetCharCallback(mWindow, s_key);
	glfwSetKeyCallback(mWindow, s_specialkey);
	glfwSetWindowRefreshCallback(mWindow, s_display);

	// Error message callback (all messages)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0u, 0, GL_TRUE);
	glDebugMessageCallback(debugCallback, nullptr);

	cout << glGetString(GL_VERSION) << '\n';
	cout << glGetString(GL_VENDOR) << '\n';
}

void
IG1App::destroy()
{ // release memory and resources
	for (Scene* scene : mScenes)
		delete scene;
	mScenes.clear();

	delete mCamera;
	mCamera = nullptr;
	delete mViewPort;
	mViewPort = nullptr;

	glfwTerminate();
}

void
IG1App::display() const
{ // double buffering

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clears the back buffer

	mScenes[mCurrentScene]->render(*mCamera); // uploads the viewport and camera to the GPU

	glfwSwapBuffers(mWindow); // swaps the front and back buffer
}

void
IG1App::resize(int newWidth, int newHeight)
{
	mWinW = newWidth;
	mWinH = newHeight;

	// Resize Viewport to the new window size
	mViewPort->setSize(newWidth, newHeight);

	// Resize Scene Visible Area such that the scale is not modified
	mCamera->setSize(mViewPort->width(), mViewPort->height());
}

void
IG1App::key(unsigned int key)
{
	bool need_redisplay = true;

	switch (key) {
	case '+':
		mCamera->setScale(+0.01); // zoom in  (increases the scale)
		break;
	case '-':
		mCamera->setScale(-0.01); // zoom out (decreases the scale)
		break;
	case 'l':
		mCamera->set3D();
		break;
	case 'o':
		mCamera->set2D();
		break;
	case 'u':
		mUpdateEnabled = !mUpdateEnabled;
		cout << "Update toggled" << endl;
		break;
	default:
		if (key >= '0' && key <= '9' && !changeScene(key - '0')) // -> por que !changeScene(key - '0') ??? si esta negado no se pone a true al cambiar
			cout << "[NOTE] There is no scene " << char(key) << ".\n";
		else
			need_redisplay = false;
		break;
	} // switch

	if (need_redisplay)
		mNeedsRedisplay = true;
	
}

void
IG1App::specialkey(int key, int scancode, int action, int mods)
{
	// Only interested in press events
	if (action == GLFW_RELEASE)
		return;

	bool need_redisplay = true;

	// Handle keyboard input
	// (key reference: https://www.glfw.org/docs/3.4/group__keys.html)
	switch (key) {
	case GLFW_KEY_ESCAPE:                     // Escape key
		glfwSetWindowShouldClose(mWindow, true); // stops main loop
		break;
	case GLFW_KEY_RIGHT:
		if (mods == GLFW_MOD_CONTROL)
			mCamera->pitch(-1); // rotates -1 on the X axis
		else
			mCamera->pitch(1); // rotates 1 on the X axis
		break;
	case GLFW_KEY_LEFT:
		if (mods == GLFW_MOD_CONTROL)
			mCamera->yaw(1); // rotates 1 on the Y axis
		else
			mCamera->yaw(-1); // rotate -1 on the Y axis
		break;
	case GLFW_KEY_UP:
		mCamera->roll(1); // rotates 1 on the Z axis
		break;
	case GLFW_KEY_DOWN:
		mCamera->roll(-1); // rotates -1 on the Z axis
		break;
	default:
		need_redisplay = false;
		break;
	} // switch

	if (need_redisplay)
		mNeedsRedisplay = true;
}

bool
IG1App::changeScene(size_t sceneNr)
{
	// Check whether the scene exists
	if (sceneNr >= mScenes.size())
		return false;

	// Change only if a different scene
	if (sceneNr != mCurrentScene) {
		mScenes[mCurrentScene]->unload(); // descarga cosas internas
		mCurrentScene = sceneNr;
		mScenes[mCurrentScene]->load(); // carga cosas internas
		mNeedsRedisplay = true;
	}

	return true;
}
