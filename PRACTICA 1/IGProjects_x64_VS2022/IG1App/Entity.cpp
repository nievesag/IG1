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
	mShader = Shader::get("vcolors");
	mMesh = Mesh::createRGBAxes(l);
	load();
}

// ---- SINGLE COLOR ENTITY ----
SingleColorEntity::SingleColorEntity(glm::dvec4 color)
{
	mColor = color;
	mShader = Shader::get("simple");
}

void SingleColorEntity::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->setUniform("color", &mColor);
		mShader->use();
		upload(aMat);
		mMesh->render();
	}
}

// ---- REGULAR POLYGON ----
RegularPolygon::RegularPolygon(GLuint num, GLdouble r, glm::dvec4 color) : SingleColorEntity(color)
{
	mMesh = Mesh::generateRegularPolygon(num, r);
	load();
}

// ---- RGB TRIANGLE ----
RGBTriangle::RGBTriangle()
{
	mShader = Shader::get("vcolors");
	mMesh = Mesh::generateRGBTriangle();
	load();
}

void RGBTriangle::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		//glPolygonMode(GL_BACK, GL_FILL);
		glPolygonMode(GL_BACK, GL_POINT);
		glPolygonMode(GL_FRONT, GL_LINE);
		mShader->use();
		upload(aMat);
		mMesh->render();
	}
}

RGBRectangle::RGBRectangle(GLdouble w, GLdouble h)
{
	mShader = Shader::get("vcolors");
	mMesh = Mesh::generateRGBRectangle(w, h);
	load();
}

void RGBRectangle::render(const glm::mat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		mat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		glPolygonMode(GL_BACK, GL_FILL);
		glPolygonMode(GL_FRONT, GL_POINT);
		mShader->use();
		upload(aMat);
		mMesh->render();
	}
}