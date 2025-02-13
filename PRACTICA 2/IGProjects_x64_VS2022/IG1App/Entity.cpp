#include "Entity.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// ---- ABS ENTITY ----
void
Abs_Entity::upload(const mat4& modelViewMat) const
{
	mShader->setUniform("modelView", modelViewMat);
}

Abs_Entity::~Abs_Entity()
{
	delete mMesh;
	mMesh = nullptr;
}

void Abs_Entity::update()
{
}

void
Abs_Entity::load()
{
	mMesh->load();
}

void
Abs_Entity::unload()
{
	mMesh->unload();
}

// ---- ENTITY WITH COLORS ----
EntityWithColors::EntityWithColors() 
{
	mShader = Shader::get("vcolors");
}

void
EntityWithColors::render(mat4 const& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		upload(aMat);
		mMesh->render();
	}
}

// ---- RGB AXES ----
RGBAxes::RGBAxes(GLdouble l)
{
	mMesh = Mesh::createRGBAxes(l);
}

// ---- SINGLE COLOR ENTITY ----
SingleColorEntity::SingleColorEntity(const vec4& color)
	: mColor(color)
{
	mShader = Shader::get("simple");
}

void SingleColorEntity::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		mShader->setUniform("color", mColor);
		upload(aMat);

		glLineWidth(2);

		glEnable(GL_CULL_FACE);
			// CARA DE DELANTE
			glCullFace(GL_BACK);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mMesh->render();

			// CARA DE ATRAS
			glCullFace(GL_FRONT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mMesh->render();

		glDisable(GL_CULL_FACE);
		glLineWidth(1);
	}
}

// ---- CUBE ----
Cube::Cube(GLdouble length) : SingleColorEntity(vec4(1))
{
	mShader = Shader::get("vcolors");
	mMesh = Mesh::generateCube(length);
}

// ---- RGBCUBE ----.
RGBCube::RGBCube(GLdouble length, int s)
: scene(s), 
  l(length) // para la animación del cubo.
{
	mShader = Shader::get("vcolors");
	mMesh = Mesh::generateRGBCube(length);
}

void RGBCube::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		upload(aMat);
		//mShader->setUniform("modelView", aMat);

		// Hay que poner glFIll en los dos porque con el gl triangles se va generando un triangulo de cara, otro de culo, otro de cara... y así sucesivamente.
		glEnable(GL_CULL_FACE);
		// CARA DE DELANTE
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mMesh->render();

		// CARA DE ATRAS
		glCullFace(GL_FRONT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		mMesh->render();
		glDisable(GL_CULL_FACE);

		//mShader->setUniform("modelView", aMat);
	}
}

void RGBCube::update()
{
	if (scene == 2)
	{
		// Como empieza en 0, hace la animacion en X inicialmente.
		rotateOnAxis(axisState);

		// ¡¡¡OJO!!! como en rotateOnAxis hacemos angle/2 para que sea 180, aquí lo tenemos que hacer 360.
		if (angle >= 360) 
		{ // Cuando llegue a 180 (en la animacion) se reinicia el angulo y se pasa al siguiente estado de animacion.
			angle = 0;
			axisState++;
		}

		// Cuando se complete la animacion se reinicia el estado y vuelta a empezar.
		if (axisState == 3) axisState = 0;

		angle++; // va iterando el angle tio
	}
}

void RGBCube::rotateOnAxis(GLint n)
{
	switch (n)
	{
	case 0: // x
		mModelMat = rotate(glm::dmat4(1), radians(angle / 2), glm::dvec3(1, 0, 0))
			* translate(glm::dmat4(1), glm::dvec3(l / 2, l / 2, -l / 2));
		break;

	case 1: // z
		mModelMat = rotate(glm::dmat4(1), radians(angle / 2), glm::dvec3(0, 0, 1))
			* translate(glm::dmat4(1), glm::dvec3(l / 2, -l / 2, l / 2));
		break;

	case 2: // y
		mModelMat = rotate(glm::dmat4(1), radians(angle / 2), glm::dvec3(0, 1, 0))
			* translate(glm::dmat4(1), glm::dvec3(-l / 2, l / 2, l / 2));
		break;

	default:break;
	}
}


// ---- REGULAR POLYGON ----
RegularPolygon::RegularPolygon(GLuint num, GLdouble r) : SingleColorEntity(vec4(1))
{
	mShader = Shader::get("simple");
	mMesh = Mesh::generateRegularPolygon(num, r);
}

// ---- RGB TRIANGLE ----
RGBTriangle::RGBTriangle(int s)
	: scene(s)
{
	mShader = Shader::get("vcolors");
	mMesh = Mesh::generateRGBTriangle();

	// se usa la matriz de modelado porque es una traslacion
	// mueve el triangulo al punto (R, 0, 0) siendo 
	//mModelMat = translate(glm::dmat4(1), glm::dvec3(100, 0, 0));
}

void RGBTriangle::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		upload(aMat);
		glEnable(GL_CULL_FACE);
			// CARA DE DELANTE
			glCullFace(GL_BACK);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			mMesh->render();
			
			// CARA DE ATRAS
			glCullFace(GL_FRONT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			mMesh->render();
		glDisable(GL_CULL_FACE);
	}
}

void RGBTriangle::update()
{
	if (scene == 1)
	{
		angle += 4.0;
		// se usa la matriz de modelado porque es una rotacion
		mModelMat = rotate(glm::dmat4(1), radians(angle/2), glm::dvec3(0, 0, 1)) // rotacion sobre
			* translate(glm::dmat4(1), glm::dvec3(100, 0, 0)) // traslacioon fuera del origen
			* rotate(glm::dmat4(1), radians(-angle), glm::dvec3(0, 0, 1)); // rotacion sobre si mismo;
	}
}

RGBRectangle::RGBRectangle(GLdouble w, GLdouble h)
{
	mShader = Shader::get("vcolors");
	mMesh = Mesh::generateRGBRectangle(w, h);
}

void RGBRectangle::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		upload(aMat);

		glEnable(GL_CULL_FACE);
			// CARA DE DELANTE
			glCullFace(GL_BACK);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			mMesh->render();

			// CARA DE ATRAS
			glCullFace(GL_FRONT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			mMesh->render();
		glDisable(GL_CULL_FACE);
	}
}