#ifndef _H_Texture_H_
#define _H_Texture_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Texture
{
	// utiliza la clase PixMap32RGBA para el metodo load
public:
	Texture() = default;
	~Texture();
	Texture(const Texture& tex) = delete;            // no copy constructor
	Texture& operator=(const Texture& tex) = delete; // no copy assignment

	// cargar y transferir a GPU
	void load(const std::string& BMP_Name,GLubyte alpha); // load from file and upload to GPU

	void bind();

	void unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

	GLuint width() const { return mWidth; }
	GLuint height() const { return mHeight; }

	void setWrap(GLuint wp); // GL_REPEAT, GL_CLAMP_TO_EDGE, ...

	void loadColorBuffer(GLsizei width, GLsizei height, GLuint buffer = GL_FRONT);

	void saveScreenshot(const std::string& file);

protected:
	void init();

	GLuint mWidth, mHeight = 0; // dimensiones de la imagen
	GLuint mId = 0;				// identificador interno (GPU) de la textura
								// 0 significa NULL, no es un identificador valido
};

#endif //_H_Texture_H_
