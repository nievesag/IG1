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
#include "Texture.h"

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
	virtual void update();

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

class SingleColorEntity : public Abs_Entity // clase padre que hereda de abs entity (entidad con color single).
{
public:
	explicit SingleColorEntity(const glm::vec4& color = glm::vec4(1)); // explicit es para que no se pueda hacer una conversion implicita. Ej.: (int)algo.
	void render(const glm::mat4& modelViewMat) const override;

	glm::vec4 getColor() const { return mColor; }
	void setColor(glm::vec4 const &c) { mColor = c; } // & para q no se copie y const porque no se modifica dentro.

private:
	glm::vec4 mColor; // predefinido.
};

class Cube: public SingleColorEntity
{
public:
	explicit Cube(GLdouble length);
};

class RGBCube : public EntityWithColors
{
public:
	explicit RGBCube(GLdouble length, int s);
	void render(const glm::mat4& modelViewMat) const override;
	void update() override;

private:
	int scene = 0;
	double angle = 0.0;
	GLdouble l; // length
	int axisState = 0; // comienza en el EJE X.

	// ESTE ES ORDEN QUE TIENE QUE SEGUIR LA ANIMACI�N.
	// 0 -> eje X.
	// 1 -> eje Z.
	// 2 -> eje Y.
	void rotateOnAxis(GLint n);

};

class RegularPolygon : public SingleColorEntity // clase hija que hereda SingleColorEntity con sus propias cosas de poligono regular.
{
public:
	explicit RegularPolygon(GLuint num, GLdouble r);
};

class RGBTriangle : public EntityWithColors
{
public:
	explicit RGBTriangle(int s);
	void render(const glm::mat4& modelViewMat) const override;
	void update() override;
private:
	int scene = 0;
	double angle = 0.0;
};

class RGBRectangle : public EntityWithColors
{
public:
	explicit RGBRectangle(GLdouble w, GLdouble h);
	void render(const glm::mat4& modelViewMat) const override;
};

class RGBAxes : public EntityWithColors
{
public:
	explicit RGBAxes(GLdouble l);
};

class Ground : public EntityWithColors 
{
public:
	explicit Ground(GLdouble w, GLdouble h);
};

class EntityWithTexture : public Abs_Entity
{
public:
	explicit EntityWithTexture(const Texture* texture, GLboolean modulate = false);
	void render(const glm::mat4& modelViewMat) const override;

protected:
	Texture* mTexture;
	GLboolean mModulate; 
};
#endif //_H_Entities_H_