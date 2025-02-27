#include "Texture.h"

#include "Image.h"

Texture::~Texture()
{
	// gestion de objetos de texturas -> destruirlos
	if (mId != 0)
		glDeleteTextures(1, &mId);
}

void
Texture::init()
{
	// gestion de objetos de texturas -> crearlos
	// genera un identificador para una nueva textura
	glGenTextures(1, &mId);

	// gestion de objetos de texturas -> activarlos para que tengan efecto
	glBindTexture(GL_TEXTURE_2D, mId);

	// gestion de objetos de texturas -> configurarlos (filtros y wrapping)
	// Filters and clamping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // GL_CLAMP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // GL_CLAMP
}

void
Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, mId);
}

void
Texture::load(const std::string& name, GLubyte alpha)
{
	if (mId == 0)
		init();

	Image image;
	image.load(name);

	if (alpha != 255)
		image.setAlpha(alpha);

	mWidth = image.width();
	mHeight = image.height();

	GLint level = 0;  // Base image level
	GLint border = 0; // No border

	glBindTexture(GL_TEXTURE_2D, mId);
	glTexImage2D(GL_TEXTURE_2D,
		level,
		GL_RGBA,
		mWidth,
		mHeight,
		border,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image.data());

	glBindTexture(GL_TEXTURE_2D, 0);
}

void
Texture::setWrap(GLuint wp) // GL_REPEAT, GL_CLAMP_TO_EDGE, ...
{
	glBindTexture(GL_TEXTURE_2D, mId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wp);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// carga el buffer de color (frontal o trasero) dado por el tercer argumento,
// como una textura de dimensiones dadas por los parametros primero y segundo
void 
Texture::loadColorBuffer(GLsizei width, GLsizei height, GLuint buffer)
{
	/*
	if (mId == 0)
		init();

	Texture* tex = new Texture();

	mHeight = height;
	mWidth = width;

	GLint level = 0;  // Base image level
	GLint border = 0; // No border

	glBindTexture(GL_TEXTURE_2D, mId);
	glTexImage2D(GL_TEXTURE_2D,
		level,
		GL_RGBA,
		mWidth,
		mHeight,
		border,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image.data());

	glBindTexture(GL_TEXTURE_2D, 0);
	*/
}