#include "Mesh.h"
#include <queue>
#include <vector>

using namespace std;
using namespace glm;

// Placeholder for the pending index of a GPU object
constexpr GLuint NONE = numeric_limits<GLuint>::max();

Mesh::Mesh()
 : mVAO(NONE)
 , mVBO(NONE)
 , mCBO(NONE)
 , mTCO(NONE)
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

		// cargar en la GPU el array de coordenadas de textura
		if (!vTexCoords.empty())
		{
			glGenBuffers(1, &mTCO);
			glBindBuffer(GL_ARRAY_BUFFER, mTCO);
			glBufferData(GL_ARRAY_BUFFER,
				vTexCoords.size() * sizeof(vec2),
				vTexCoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
				sizeof(vec2), nullptr);
			glEnableVertexAttribArray(2);
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

		// eliminar las coordenadas de textura de la GPU
		if (mTCO != NONE) {
			glDeleteBuffers(1, &mTCO);
			mTCO = NONE;
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
	GLdouble incremento = 360.0 / num;
	for (GLuint i = 0; i < num + 2; i++) {
		GLdouble x = r * glm::cos(glm::radians(alpha));
		GLdouble y = r * glm::sin(glm::radians(alpha));
		alpha += incremento;

		// (x, y, z, 1 -> punto 0 -> vector) -> (Cx + R * cos(alpha), Cy + R * sin(alpha), 0, punto).
		mesh->vVertices.emplace_back(x, y, 0.0);
	}

	return mesh;
}

Mesh* Mesh::generateRGBTriangle()
{
	Mesh* mesh = generateRegularPolygon(3, 20);

	mesh->mPrimitive = GL_TRIANGLES;

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
	Mesh* mesh = generateRectangle(w, h);

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

Mesh* Mesh::generateCube(GLdouble length)
{
	Mesh* mesh = new Mesh();

	/*// Establecemos primitiva GL_TRIANGLE_STRIP
	mesh->mPrimitive = GL_TRIANGLE_STRIP;

	mesh->mNumVertices = 14;
	mesh->vVertices.reserve(mesh->mNumVertices);

	/*
	0---------2
	|    /    |
	1---------3
	

	GLdouble distFromCenter = length/2;

	// 0
	mesh->vVertices.emplace_back(-distFromCenter, -distFromCenter, -distFromCenter);

	// 1
	mesh->vVertices.emplace_back(distFromCenter, -distFromCenter, -distFromCenter);

	// 2
	mesh->vVertices.emplace_back(-distFromCenter, -distFromCenter, distFromCenter);

	// 3
	mesh->vVertices.emplace_back(distFromCenter, -distFromCenter, distFromCenter);

	// 4
	mesh->vVertices.emplace_back(distFromCenter, distFromCenter, distFromCenter);

	// 5 == 1
	mesh->vVertices.push_back(mesh->vVertices[1]);

	// 6
	mesh->vVertices.emplace_back(distFromCenter, distFromCenter, -distFromCenter);

	// 7 == 0
	mesh->vVertices.push_back(mesh->vVertices[0]);

	// 8
	mesh->vVertices.emplace_back(-distFromCenter, distFromCenter, -distFromCenter);

	// 9 == 2
	mesh->vVertices.push_back(mesh->vVertices[2]);

	// 10
	mesh->vVertices.emplace_back(-distFromCenter, distFromCenter, distFromCenter);

	// 11 == 4
	mesh->vVertices.push_back(mesh->vVertices[4]);

	// 12 == 8
	mesh->vVertices.push_back(mesh->vVertices[8]);

	// 13 == 6
	mesh->vVertices.push_back(mesh->vVertices[6]); */

	// ---- VÉRTICES ----.
	// Establecemos primitiva GL_TRIANGLE_STRIP
	mesh->mPrimitive = GL_TRIANGLES;

	mesh->mNumVertices = 36;
	mesh->vVertices.reserve(mesh->mNumVertices);

	GLdouble r = length / 2;

	// ¡¡¡ MIRAR DIBUJO CHULETA PARA ENTENDER !!!

	// ---- CARA 1.
	mesh->vVertices.emplace_back(-r, -r, -r); // 0.
	mesh->vVertices.emplace_back(-r, -r, r); // 1.
	mesh->vVertices.emplace_back(r, -r, -r); // 2.

	mesh->vVertices.emplace_back(r, -r, r); // 3.
	mesh->vVertices.push_back(mesh->vVertices[1]); // 4 (misma pos que el 1).
	mesh->vVertices.push_back(mesh->vVertices[2]); // 5 (misma pos que el 2).

	// ---- CARA 2.
	mesh->vVertices.push_back(mesh->vVertices[2]); // 6 (misma pos que el 2).
	mesh->vVertices.emplace_back(r, r, -r); // 7.
	mesh->vVertices.push_back(mesh->vVertices[0]); // 8 (misma pos que el 0).

	mesh->vVertices.emplace_back(-r, r, -r); // 9.
	mesh->vVertices.push_back(mesh->vVertices[7]); // 10 (misma pos que el 7).
	mesh->vVertices.push_back(mesh->vVertices[0]); // 11 (misma pos que el 0).

	// ---- CARA 3.
	mesh->vVertices.push_back(mesh->vVertices[0]); // 12 (misma pos que el 0).
	mesh->vVertices.push_back(mesh->vVertices[9]); // 13 (misma pos que el 9).
	mesh->vVertices.push_back(mesh->vVertices[1]); // 14 (misma pos que el 1).

	mesh->vVertices.emplace_back(-r, r, r); // 15.
	mesh->vVertices.push_back(mesh->vVertices[9]); // 16 (misma pos que el 9).
	mesh->vVertices.push_back(mesh->vVertices[1]); // 17 (misma pos que el 1).

	// ---- CARA 4 (PARALELA A CARA 1).
	mesh->vVertices.push_back(mesh->vVertices[9]); // 18 (misma pos que el 9).
	mesh->vVertices.push_back(mesh->vVertices[15]); // 19 (misma pos que el 15).
	mesh->vVertices.push_back(mesh->vVertices[7]); // 20 (misma pos que el 7).

	mesh->vVertices.emplace_back(r, r, r); // 21.
	mesh->vVertices.push_back(mesh->vVertices[15]); // 22 (misma pos que el 15).
	mesh->vVertices.push_back(mesh->vVertices[7]); // 23 (misma pos que el 7).

	// ---- CARA 5 (PARALELA A CARA 2).
	mesh->vVertices.push_back(mesh->vVertices[3]); // 24 (misma pos que el 3).
	mesh->vVertices.push_back(mesh->vVertices[21]); // 25 (misma pos que el 21).
	mesh->vVertices.push_back(mesh->vVertices[1]); // 26 (misma pos que el 1).

	mesh->vVertices.push_back(mesh->vVertices[15]); // 27 (misma pos que el 15).
	mesh->vVertices.push_back(mesh->vVertices[21]); // 28 (misma pos que el 21).
	mesh->vVertices.push_back(mesh->vVertices[1]); // 29 (misma pos que el 1).

	// ---- CARA 6 (PARALELA A CARA 3).
	mesh->vVertices.push_back(mesh->vVertices[2]); // 30 (misma pos que el 2).
	mesh->vVertices.push_back(mesh->vVertices[7]); // 31 (misma pos que el 7).
	mesh->vVertices.push_back(mesh->vVertices[3]); // 32 (misma pos que el 3).

	mesh->vVertices.push_back(mesh->vVertices[21]); // 33 (misma pos que el 21).
	mesh->vVertices.push_back(mesh->vVertices[7]); // 34 (misma pos que el 7).
	mesh->vVertices.push_back(mesh->vVertices[3]); // 35 (misma pos que el 3).

	return mesh;
}

Mesh* Mesh::generateRGBCube(GLdouble length)
{
	Mesh* mesh = generateCube(length);

	// ---- COLORES ----.
	/* 0: red (1.0, 0.0, 0.0, 1.0);
	   1: green (0.0, 1.0, 0.0, 1.0);
	   2: green (0.0, 1.0, 0.0, 1.0);
	   3: blue (0.0, 0.0, 1.0, 1.0) */
	mesh->vColors.reserve(mesh->mNumVertices);

	// ---- CARA 1 AZUL.
	for (int i = 0; i < 6; i++)
		mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);

	// ---- CARA 2 ROJO.
	for (int i = 0; i < 6; i++)
		mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);

	// ---- CARA 3 VERDE.
	for (int i = 0; i < 6; i++)
		mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);

	// ---- CARA 4 AZUL.
	for (int i = 0; i < 6; i++)
		mesh->vColors.emplace_back(0.0, 0.0, 1.0, 1.0);

	// ---- CARA 5 ROJO.
	for (int i = 0; i < 6; i++)
		mesh->vColors.emplace_back(1.0, 0.0, 0.0, 1.0);

	// ---- CARA 6 VERDE.
	for (int i = 0; i < 6; i++)
		mesh->vColors.emplace_back(0.0, 1.0, 0.0, 1.0);

	return mesh;
}

Mesh* Mesh::generateRectangleTexCor(GLdouble w, GLdouble h)
{
	Mesh* mesh = generateRectangle(w, h);

	mesh->vTexCoords.reserve(mesh->mNumVertices);

	/*
	0---------2
	|    /    |
	1---------3
	*/

	mesh->vTexCoords.emplace_back(0, 0);
	mesh->vTexCoords.emplace_back(1, 0);
	mesh->vTexCoords.emplace_back(0, 1);
	mesh->vTexCoords.emplace_back(1, 1);

	return mesh;
}

Mesh* Mesh::generateRectangleTexCor(GLdouble w, GLdouble h, GLuint rw, GLuint rh)
{
	Mesh* mesh = generateRectangle(w, h);
	mesh->vTexCoords.reserve(mesh->mNumVertices);

	/*
	0---------2
	|    /    |
	1---------3
	*/

	// repite la textura rw veces a lo ancho y rh a lo alto

	// 0
	mesh->vTexCoords.emplace_back(0, rw);
	// 1
	mesh->vTexCoords.emplace_back(0, 0);
	// 2 
	mesh->vTexCoords.emplace_back(rh, rw);
	// 3
	mesh->vTexCoords.emplace_back(rh, 0);

	return mesh;
}

Mesh* Mesh::generateBoxOutline(GLdouble length)
{
	Mesh* mesh = new Mesh();

	// Establecemos primitiva GL_TRIANGLE_STRIP
	mesh->mPrimitive = GL_TRIANGLE_STRIP;

	mesh->mNumVertices = 10; // !!!! PONER LOS VERTICES JUSTOS
	mesh->vVertices.reserve(mesh->mNumVertices);

	/*
	0---------2
	|    /    |
	1---------3
	*/

	GLdouble r = length/2;

	// CARA 1 (PLANO XY).
	mesh->vVertices.emplace_back(r, r, -r); // 0.
	mesh->vVertices.emplace_back(r, -r, -r); // 1.
	mesh->vVertices.emplace_back(-r, r, -r); // 2.
	mesh->vVertices.emplace_back(-r, -r, -r); // 3.

	// CARA 2 (PLANO YZ).
	mesh->vVertices.emplace_back(-r, r, r); // 4.
	mesh->vVertices.emplace_back(-r, -r, r); // 5.

	// CARA 3 (PARALELO A XY).
	mesh->vVertices.emplace_back(r, r, r); // 6.
	mesh->vVertices.emplace_back(r, -r, r); // 7.

	// CARA 4 (PARALELO A YZ). (vertices de cierre)
	mesh->vVertices.push_back(mesh->vVertices[0]); // 8. = 0 (misma pos que el 0).
	mesh->vVertices.push_back(mesh->vVertices[1]); // 9. = 1 (misma pos que el 1).

	return mesh;
}

Mesh* Mesh::generateBoxOutlineTexCor(GLdouble length)
{
	Mesh* mesh = generateBoxOutline(length);
	mesh->vTexCoords.reserve(mesh->mNumVertices);

	/*
	0---------2
	|    /    |
	1---------3
	*/

	// Cara 1.
	mesh->vTexCoords.emplace_back(0, 1); // 0.
	mesh->vTexCoords.emplace_back(0, 0); // 1.
	mesh->vTexCoords.emplace_back(1, 1); // 2.
	mesh->vTexCoords.emplace_back(1, 0); // 3.

	// Cara 2.
	mesh->vTexCoords.emplace_back(0, 1); // 4.
	mesh->vTexCoords.emplace_back(0, 0); // 5.

	// Cara 3.
	mesh->vTexCoords.emplace_back(1, 1); // 6.
	mesh->vTexCoords.emplace_back(1, 0); // 7.

	// Cara 4.
	mesh->vTexCoords.emplace_back(0, 1); // 8.
	mesh->vTexCoords.emplace_back(0, 0); // 9.

	return mesh;
}

Mesh* Mesh::generateStar3D(GLdouble re, GLuint np, GLdouble h)
{
	// Creamos el mesh.
	Mesh* mesh = new Mesh();

	// Establecemos primitiva GL_TRIANGLE_FAN
	mesh->mPrimitive = GL_TRIANGLE_FAN;

	// el doble de vertices del nupero de puntas (para los vertices interiores) + el vertice del origen + el vertice para cerrar
	mesh->mNumVertices = np * 2 + 2; 
	mesh->vVertices.reserve(mesh->mNumVertices);

	mesh->vVertices.emplace_back(0,0,0);

	// Se van guardando en sentido antihorario.
	GLdouble alpha = 90.0;
	GLdouble incremento = 90.0 / np * 2;
	GLdouble ri = re / 2;
	GLdouble r;
	for (GLuint i = 0; i < np*2+1; i++) 
	{
		if (i % 2 == 0) 
		{
			r = re;
		}
		else 
		{
			r = ri;
		}

		GLdouble x = r * glm::cos(glm::radians(alpha));
		GLdouble y = r * glm::sin(glm::radians(alpha));

		alpha += incremento;

		// (x, y, z, 1 -> punto 0 -> vector) -> (Cx + R * cos(alpha), Cy + R * sin(alpha), 0, punto).
		mesh->vVertices.emplace_back(x, y, h);
	}

	return mesh;
}

Mesh* Mesh::generateStar3DTexCor(GLdouble re, GLuint np, GLdouble h)
{
	/*
	
	(0 , 1)--(0.25 , 1)--(0.5 , 1)--(0.75 , 1)--(1 , 1)
	|                                              |
	(0 , 0.75)                                  (1 , 0.75)
	|                                              |
	(0 , 0.5)                                   (1 , 0.5)       
	|                                              |
	(0 , 0.25)                                  (1 , 0.25)
	|                                              |
	(0 , 0)--(0.25 , 0)--(0.5 , 0)--(0.75 , 0)--(1 , 0)

	*/

	Mesh* mesh = generateStar3D(re, np, h);

	mesh->vTexCoords.reserve(mesh->mNumVertices);
	mesh->vTexCoords.emplace_back(0.5, 0.5); // origen

	mesh->vTexCoords.emplace_back(0.5, 1);   // inicio
	mesh->vTexCoords.emplace_back(0.25, 1);
	mesh->vTexCoords.emplace_back(0, 1);

	mesh->vTexCoords.emplace_back(0, 0.75);
	mesh->vTexCoords.emplace_back(0, 0.5);

	mesh->vTexCoords.emplace_back(0, 0.25);
	mesh->vTexCoords.emplace_back(0, 0);

	mesh->vTexCoords.emplace_back(0.25, 0);
	mesh->vTexCoords.emplace_back(0.5, 0);

	mesh->vTexCoords.emplace_back(0.75, 0);
	mesh->vTexCoords.emplace_back(1, 0);

	mesh->vTexCoords.emplace_back(1, 0.25);
	mesh->vTexCoords.emplace_back(1, 0.5);

	mesh->vTexCoords.emplace_back(1, 0.75);
	mesh->vTexCoords.emplace_back(1, 1);

	mesh->vTexCoords.emplace_back(0.75, 1);
	mesh->vTexCoords.emplace_back(0.5, 1); // final - cierre

	return mesh;
}