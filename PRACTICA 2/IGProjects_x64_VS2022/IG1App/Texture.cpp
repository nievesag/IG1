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
Texture::bind(GLuint mixMode)
{
	// mixMode: modo para la mezcla los colores

	glBindTexture(GL_TEXTURE_2D, mId); // activa la textura

	// el modo de mezcla de colores no queda
	// guardado en el objeto de textura
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mixMode);
		// mixMode: GL_REPLACE, GL_MODULATE, GL_ADD...
}

void
Texture::load(const std::string& BMP_Name, GLubyte alpha)
{
	if (mId == 0) init();

	// variable para cargar la imagen del archivo
	PixMap32RGBA pixMap;

	pixMap.load_bmp24BGR(BMP_Name); // carga y mete alpha = 255

	// carga correcta? -> exception

	if (alpha != 255) pixMap.set_alpha(alpha);

	mWidth = pixMap.width();
	mHeight = pixMap.height();

	glBindTexture(GL_TEXTURE_2D, mId);

	// transferir a GPU
	glTexImage2D(GL_TEXTURE_2D,			// 1D o 2D
				0,						// mipmap level
				GL_RGBA,				// formato interno (GPU) de los datos de la textura
				mWidth,					// Potencias de 2?
				mHeight,				//	   "
				0,						// border
				GL_RGBA,				// formato de los datos de la imagen (data)
				GL_UNSIGNED_BYTE,		// tipo de datos de los datos de data
				pixMap.data());   // puntero a la variable CPU con la imagen

	// la textura queda desactivada?
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
