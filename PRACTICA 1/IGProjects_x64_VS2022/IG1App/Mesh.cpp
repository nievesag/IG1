#include "Mesh.h"

using namespace std;
using namespace glm;

// Placeholder for the pending index of a GPU object
constexpr GLuint NONE = numeric_limits<GLuint>::max();



Mesh::Mesh()
 : mVAO(NONE)
 , mVBO(NONE)
 , mCBO(NONE)
{
}

Mesh::~Mesh()
{
	unload();
}

void
Mesh::draw() const
{
	glDrawArrays(
	  mPrimitive,
	  0,
	  size()); // primitive graphic, first index and number of elements to be rendered
}

void
Mesh::load()
{
	assert(mVBO == NONE); // not already loaded

	if (vVertices.size() > 0) { // transfer data
		glGenBuffers(1, &mVBO);
		glGenVertexArrays(1, &mVAO);

		glBindVertexArray(mVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, vVertices.size() * sizeof(vec3), vVertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), nullptr);
		glEnableVertexAttribArray(0);

		if (vColors.size() > 0) {             // upload colors
			glGenBuffers(1, &mCBO);

			glBindBuffer(GL_ARRAY_BUFFER, mCBO);
			glBufferData(GL_ARRAY_BUFFER, vColors.size() * sizeof(vec4), vColors.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), nullptr);
			glEnableVertexAttribArray(1);
		}
	}
}

void
Mesh::unload()
{
	if (mVAO != NONE) {
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(1, &mVBO);
		mVAO = NONE;
		mVBO = NONE;

		if (mCBO != NONE) {
			glDeleteBuffers(1, &mCBO);
			mCBO = NONE;
		}
	}
}

void
Mesh::render() const
{
	assert(mVAO != NONE);

	glBindVertexArray(mVAO);
	draw();
}

Mesh*
Mesh::createRGBAxes(GLdouble l)
{
	Mesh* mesh = new Mesh();

	mesh->mPrimitive = GL_LINES;

	mesh->mNumVertices = 6;
	mesh->vVertices.reserve(mesh->mNumVertices);

	// X axis vertices
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0);
	mesh->vVertices.emplace_back(l, 0.0, 0.0);
	// Y axis vertices
	mesh->vVertices.emplace_back(0, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, l, 0.0);
	// Z axis vertices
	mesh->vVertices.emplace_back(0.0, 0.0, 0.0);
	mesh->vVertices.emplace_back(0.0, 0.0, l);

	mesh->vColors.reserve(mesh->mNumVertices);
	// X axis color: red  (Alpha = 1 : fully opaque)
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	// Y axis color: green
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	// Z axis color: blue
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);

	return mesh;
}

Mesh* Mesh::generateRegularPolygon(GLuint num, GLdouble r)
{
	// Creamos el mesh.
	Mesh* mesh = new Mesh(); // cuando haces new -> puntero.

	// Establecemos primitiva GL_LINE_LOOP.
	mesh->mPrimitive = GL_LINE_LOOP;

	// Establecemos numero de vertices y crea un array de tamano num (para los vertices).
	mesh->mNumVertices = num;
	mesh->vVertices.reserve(mesh->mNumVertices);

	// Se van guardando en sentido antihorario.
	GLdouble alpha = 90.0;
	for (GLuint i = 0; i < num + 2; i++) {
		GLdouble x = r * glm::cos(glm::radians(alpha));
		GLdouble y = r * glm::sin(glm::radians(alpha));
		alpha += 360.0 / num;

		// (x, y, z, 1 -> punto 0 -> vector) -> (Cx + R * cos(alpha), Cy + R * sin(alpha), 0, punto).
		mesh->vVertices.emplace_back(x, y, 0.0);
	}

	return mesh;
}

Mesh* Mesh::generateRGBTriangle()
{
	Mesh* mesh = generateRegularPolygon(3, 200);

	// COLORES
	mesh->vColors.reserve(mesh->mNumVertices);

	// X axis color: red
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	// Y axis color: green
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	// Z axis color: blue
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);

	return mesh;
}

Mesh* Mesh::generateRectangle(GLdouble w, GLdouble h)
{
	Mesh* mesh = new Mesh();

	// Establecemos primitiva GL_TRIANGLE_STRIP
	mesh->mPrimitive = GL_TRIANGLE_STRIP;

	mesh->mNumVertices = 4;
	mesh->vVertices.reserve(mesh->mNumVertices);

	/*
	0---------2
	|    /    |
	1---------3
	*/

	double x = (w / 2);
	double y = (h / 2);

	// 0
	mesh->vVertices.emplace_back(-x, y, 0.0);

	// 1
	mesh->vVertices.emplace_back(-x, -y, 0.0);

	// 2
	mesh->vVertices.emplace_back(x, y, 0.0);

	// 3
	mesh->vVertices.emplace_back(x, -y, 0.0);

	return mesh;
}

Mesh* Mesh::generateRGBRectangle(GLdouble w, GLdouble h)
{
	Mesh* mesh = generateRectangle(w,h);

	/*
	0---------2
	|    /    |
	1---------3
	*/

	// COLORES
	mesh->vColors.reserve(mesh->mNumVertices);

	// 0: red
	mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);
	// 1: green
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	// 2: green
	mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);
	// 3: blue
	mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);

	return mesh;
}