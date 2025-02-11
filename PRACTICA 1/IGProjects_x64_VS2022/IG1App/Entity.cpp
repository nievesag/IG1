#include "Entity.h"

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
	setModelMat(mModelMat);
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

// ---- REGULAR POLYGON ----
RegularPolygon::RegularPolygon(GLuint num, GLdouble r) : SingleColorEntity(vec4(1))
{
	mShader = Shader::get("vcolors");
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
	mModelMat = translate(glm::dmat4(1), glm::dvec3(100, 0, 0));
}

void RGBTriangle::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		
		//mShader->setUniform("modelView", aMat);
		
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

		//mShader->setUniform("modelView", aMat);
	}
}

void RGBTriangle::update()
{
	//if(scene == 1)
	{
		// se usa la matriz de modelado porque es una rotacion
		mModelMat = rotate(glm::dmat4(1), radians(45.0), glm::dvec3(0, 0, 1));
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