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
void Texture::loadColorBuffer(GLsizei width, GLsizei height, GLuint buffer) // buffer puede ser GL_FRONT o GL_BACK
{
	if (mId == 0)
		init();

	// Inicializa los atributos de la textura
	mWidth = width;
	mHeight = height;

	// --- Setea
	glReadBuffer(buffer);			   // Para modificar el buffer de lectura activo 
	// -> las instrucciones siguientes afectan al buffer que se cargue aqui
	glBindTexture(GL_TEXTURE_2D, mId); // Bindea textura a mId

	// Los datos se copian del buffer de lectura activo: GL_FRONT o GL_BACK
	// Copiar en la textura activa parte de la imagen del Color Buffer (en coordenadas de pantalla, como el viewport)
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, mWidth, mHeight, 0);

	// --- Resetea
	glBindTexture(GL_TEXTURE_2D, 0); // Bindea textura a 0
	glReadBuffer(GL_FRONT);			 // por defecto GL_FRONT
}

void Texture::saveScreenshot(const std::string& file)
{
	Image img;

	img.reserve(800, 600); // Reserve

	glBindTexture(GL_TEXTURE_2D, mId); // Bindea textura a mId

	// Obtener (de GPU a CPU) la imagen de la textura activa:
	// pixels: array donde guardar los datos (de tipo y tamanio adecuado)
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data());

	img.save(file); // gestiona el fichero
}