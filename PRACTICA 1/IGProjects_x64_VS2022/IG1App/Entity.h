#ifndef _H_Entities_H_
#define _H_Entities_H_

// - Abs_Entity -

// GUARDA:
// mModelMat -> matriz de modelado (traslacion, escala y rotaciones)
// mShader -> shader que utiliza
// mMesh -> puntero a su malla

// METODOS:
// render -> void render(const glm::mat4& modelViewMat) const es virtual puro. Las subclases lo definiran.
// carga matriz vista ->
/* void Abs_Entity::upload(const mat4& modelViewMat) const
{
	mShader->setUniform("modelView", modelViewMat);
} */

// - EntityWithColors -
// => Extiende Abs_Entity con un render concreto para entidades con color asociado a cada vertice
// => Su constructor fija mShader = Shader::get("vcolors")
// => Su render se programa asi ->
/* void EntityWithColors::render(
	glm::mat4 const& modelViewMat) const
{
	if (mMesh == nullptr) return;
	mat4 aMat = modelViewMat * mModelMat;
	mShader->use();
	upload(aMat);
	mMesh->render();
}*/

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Shader.h"

class Abs_Entity // abstract class
{
public:
	Abs_Entity()
	  : mModelMat(1.0)  // 4x4 identity matrix
	  , mShader(nullptr) {};
	virtual ~Abs_Entity();

	Abs_Entity(const Abs_Entity& e) = delete;            // no copy constructor
	Abs_Entity& operator=(const Abs_Entity& e) = delete; // no copy assignment

	virtual void render(const glm::mat4& modelViewMat) const = 0; // abstract method

	// modeling matrix
	glm::mat4 const& modelMat() const { return mModelMat; };
	void setModelMat(glm::mat4 const& aMat) { mModelMat = aMat; };

	// load or unload entity data into the GPU
	void load();
	void unload();

protected:
	Mesh* mMesh = nullptr; // the mesh
	glm::mat4 mModelMat;  // modeling matrix
	Shader* mShader; // shader

	// transfers modelViewMat to the GPU
	virtual void upload(const glm::mat4& mModelViewMat) const;
};

class EntityWithColors : public Abs_Entity
{
public:
	explicit EntityWithColors();
	void render(const glm::mat4& modelViewMat) const override;
};

class SingleColorEntity : public Abs_Entity
{
public:
	explicit SingleColorEntity(glm::dvec4 color); // explicit es para que no se pueda hacer una conversion implicita. Ej.: (int)algo.
	glm::dvec4 color() { return mColor; }
	void setColor(const glm::dvec4 &c) { mColor = c; } // & para q no se copie y const porque no se modifica dentro.
	void render(const glm::mat4& modelViewMat) const override;

private:
	glm::dvec4 mColor = { 1.0, 1.0, 1.0, 1.0 }; // predefinido.
};

class RegularPolygon : public SingleColorEntity
{
public:
	explicit RegularPolygon(GLuint num, GLdouble r, const glm::dvec4 &color);
};

class RGBAxes : public EntityWithColors
{
public:
	explicit RGBAxes(GLdouble l);
};

#endif //_H_Entities_H_
