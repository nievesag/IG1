#include "Entity.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

#pragma region ENTIDADES PADRES WARLORDS
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

void Abs_Entity::load()
{
	mMesh->load();
}

void Abs_Entity::unload()
{
	mMesh->unload();
}

// ---- ENTITY WITH COLORS ----
EntityWithColors::EntityWithColors() 
{
	mShader = Shader::get("vcolors");
}

void EntityWithColors::render(dmat4 const& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		upload(aMat);
		mMesh->render();
	}
}

// ---- SINGLE COLOR ENTITY ----
SingleColorEntity::SingleColorEntity(const vec4& color)
	: mColor(color)
{
	mShader = Shader::get("simple");
}

void SingleColorEntity::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		mShader->setUniform("color", mColor);
		upload(aMat);

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

	}
}

// ---- ENTITY WITH TEXTURE ----
EntityWithTexture::EntityWithTexture(GLboolean modulate, GLboolean alfaActive)
	: mModulate(modulate), mAlfaActive(alfaActive)
{
	if (mAlfaActive) {
		mShader = Shader::get("texture:texture_alpha");
	}
	else {
		mShader = Shader::get("texture");
	}
}

void EntityWithTexture::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (mTexture != nullptr) // si la textura no es nula podemos proceder a renderizarla
		{
			mTexture->bind();	 // activa la textura en la gpu
			mMesh->render();
			mTexture->unbind();  // desactiva la textura en la gpu
		}
	}
}
#pragma endregion

#pragma region PRACTICA 1
// ---- RGB AXES ----
RGBAxes::RGBAxes(GLdouble l)
{
	mMesh = Mesh::createRGBAxes(l);
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

void RGBCube::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
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

void RGBTriangle::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
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

// ---- RGB RECTANGLE ----
RGBRectangle::RGBRectangle(GLdouble w, GLdouble h)
{
	mShader = Shader::get("vcolors");
	mMesh = Mesh::generateRGBRectangle(w, h);
}

void RGBRectangle::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
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
#pragma endregion

#pragma region PRACTICA 2
// ---- GROUND ----
Ground::Ground(GLdouble w, GLdouble h, GLboolean modulate)
	: EntityWithTexture(modulate, false)
{
	mMesh = Mesh::generateRectangleTexCor(w, h, 4, 4);
	mModelMat = translate(glm::dmat4(1), glm::dvec3(0, -0.5, 0)) * rotate(dmat4(1), radians(90.0), glm::dvec3(1, 0, 0));
}

// ---- CAJA SIN TAPAS ----
BoxOutline::BoxOutline(GLdouble length, GLboolean modulate)
	: EntityWithTexture(modulate)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);
}

void BoxOutline::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);

		if (mTexture != nullptr) // si la textura no es nula podemos proceder a renderizarla
		{
			glEnable(GL_CULL_FACE);
				// CARA DE DELANTE
				mTexture->bind();
				glCullFace(GL_BACK);
				mMesh->render();
				mTexture->unbind();

				// CARA DE ATRAS
				mTextureInterior->bind();
				glCullFace(GL_FRONT);
				mMesh->render();
				mTextureInterior->unbind();
			glDisable(GL_CULL_FACE);
		}
	}
}

// ---- CAJA CON TAPAS ----
Box::Box(GLdouble length, GLboolean modulate, int s)
	: EntityWithTexture(modulate), _length(length), scene(s)
{
	mMesh = Mesh::generateBoxOutlineTexCor(_length);
	mMeshTapaAbj = Mesh::generateRectangleTexCor(_length, _length);
	mMeshTapaArr = Mesh::generateRectangleTexCor(_length, _length);
}

Box::~Box()
{
	delete mMesh;
	mMesh = nullptr;

	delete mMeshTapaAbj;
	mMeshTapaAbj = nullptr;

	delete mMeshTapaArr;
	mMeshTapaArr = nullptr;
}

void Box::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr && mTexture != nullptr)
	{
		// ---- Caja Principal ----
		#pragma region Caja Principal
		dmat4 aMat = modelViewMat * mModelMat;
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);

		// culling
		glEnable(GL_CULL_FACE);
			// CARA DE DELANTE
			mTexture->bind(); // activa la textura en la gpu
				glCullFace(GL_BACK);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				mMesh->render();
			mTexture->unbind(); // desactiva la textura en la gpu

			// CARA DE ATRAS
			mTextureInterior->bind(); // activa la textura en la gpu
				glCullFace(GL_FRONT);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				mMesh->render();
			mTextureInterior->unbind(); // desactiva la textura en la gpu
		glDisable(GL_CULL_FACE);
		#pragma endregion
		// ------------

		// ---- Tapa abajo ----
		#pragma region Tapa Abajo
		dmat4 bMat = modelViewMat * mModelMatAbj
		* translate(glm::dmat4(1), glm::dvec3(0, -_length/2.0, 0))
		* rotate(dmat4(1), radians(90.0), dvec3(1.0, 0.0, 0.0));
		upload(bMat);

		// culling
		glEnable(GL_CULL_FACE);
			// CARA DE DELANTE
			mTexture->bind(); // activa la textura en la gpu
				glCullFace(GL_BACK);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				mMeshTapaAbj->render();
			mTexture->unbind(); // desactiva la textura en la gpu

			// CARA DE ATRAS
			mTextureInterior->bind(); // activa la textura en la gpu
				glCullFace(GL_FRONT);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				mMeshTapaAbj->render();
			mTextureInterior->unbind(); // desactiva la textura en la gpu
		glDisable(GL_CULL_FACE);
		#pragma endregion
		// ------------

		// ---- Tapa arriba ----
		#pragma region Tapa Arriba
		dmat4 cMat = modelViewMat * mModelMatArr
			* rotate(dmat4(1), radians(90.0), dvec3(1.0, 0.0, 0.0));
		upload(cMat);

		// culling
		glEnable(GL_CULL_FACE);
			// CARA DE DELANTE
			mTexture->bind(); // activa la textura en la gpu
				glCullFace(GL_BACK);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				mMeshTapaArr->render();
			mTexture->unbind(); // desactiva la textura en la gpu

			// CARA DE ATRAS
			mTextureInterior->bind(); // activa la textura en la gpu
				glCullFace(GL_FRONT);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				mMeshTapaArr->render();
			mTextureInterior->unbind(); // desactiva la textura en la gpu
		glDisable(GL_CULL_FACE);
		#pragma endregion
		// ------------
	}
}

void Box::load()
{
	mMesh->load();
	mMeshTapaAbj->load();
	mMeshTapaArr->load();
}

void Box::unload()
{
	mMesh->unload();
	mMeshTapaAbj->unload();
	mMeshTapaArr->unload();
}

void Box::update()
{
	// esto se haría solo para la tapa de arriba
	mModelMatArr =
		   translate(glm::dmat4(1), glm::dvec3(-65, 30 / 2, -80))		// mueve a donde esta la caja
		 * translate(glm::dmat4(1), glm::dvec3(0, _length / 2, 0))		// "
		 * rotate(dmat4(1), radians(angle), dvec3(0.0, 0.0, 1.0))		// abre y cierra
		 * translate(glm::dmat4(1), glm::dvec3(_length/2, 0, 0));		// mueve para que la visagra sea en el z

	// ---- gestion estado
	// cerrando && angulo <= 0 -> abriendo
	if (openState == 1.0 && angle <= 0.0) openState = 0.0;
	// abriendo && angulo >= 180 -> cerrando
	else if (openState == 0.0 && angle >= 180.0) openState = 1.0;

	// ---- gestion angulo
	// abriendo && angulo <= 180 -> angulo++ (abre)
	if (angle < 180 && openState == 0.0) angle++;
	// cerrando && angulo >= 0	 -> angulo-- (cierra)
	else if (angle > 0 && openState == 1.0) angle--;
}

// ---- ESTRELLA ----
Star3D::Star3D(GLdouble re, GLuint np, GLdouble h, int s, GLboolean modulate)
	: EntityWithTexture(modulate, false), scene(s)
{
	mMesh = Mesh::generateStar3DTexCor(re, np, h);
}

void Star3D::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr && mTexture != nullptr) 
	{
		// texturas
		mTexture->bind(); // activa la textura en la gpu
			// ---- Primera estrella.
			dmat4 aMat = modelViewMat * mModelMat;
			mShader->use();
			mShader->setUniform("modulate", mModulate);
			upload(aMat);

				// culling
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

			// ---- Segunda estrella.
			dmat4 bMat = modelViewMat * mModelMat 
				* rotate(dmat4(1), radians(180.0), dvec3(0.0, 1.0, 0.0));
			mShader->use();
			mShader->setUniform("modulate", mModulate);
			upload(bMat);
			mMesh->render();

		mTexture->unbind(); // desactiva la textura en la gpu
	}
}

void Star3D::update()
{
	if (scene == 3)
	{
		angle += 4.0;
		mModelMat = 
			  translate(glm::dmat4(1), glm::dvec3(0, 0, 80))
			* translate(glm::dmat4(1), glm::dvec3(80, 0, 0))
			* translate(glm::dmat4(1), glm::dvec3(0, 80, 0))
			* rotate(glm::dmat4(1), radians(angle / 2), glm::dvec3(0, 1, 0)) // rotacion sobre y
			* rotate(glm::dmat4(1), radians(angle/2), glm::dvec3(0, 0, 1));		   // rotacion sobre z
	}
}

// ---- CRISTAL ----
GlassParapet::GlassParapet(GLdouble length, GLboolean modulate)
	: EntityWithTexture(modulate, false)
{
	mMesh = Mesh::generateBoxOutlineTexCor(length);
}

void GlassParapet::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);

		if (mTexture != nullptr) // si la textura no es nula podemos proceder a renderizarla
		{
			mTexture->bind(); // activa la textura en la gpu

			// culling
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

			mTexture->unbind(); // activa la textura en la gpu
		}
	}
}

// ---- GRASS ----
Grass::Grass(GLdouble l, GLboolean modulate)
	: EntityWithTexture(modulate, true), length(l)
{
	mMesh = Mesh::generateRectangleTexCor(l, l);
}

void Grass::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr && mTexture != nullptr)
	{
		mTexture->bind();	 // activa la textura en la gpu
			// ---- Primera hierba.
			dmat4 aMat = modelViewMat * mModelMat 
				* translate(glm::dmat4(1), glm::dvec3(0, 0, -80))
				* translate(glm::dmat4(1), glm::dvec3(80, 0, 0))
				* translate(glm::dmat4(1), glm::dvec3(0, length/2, 0))
				* rotate(dmat4(1), radians(-90.0), dvec3(0.0, 0.0, 1.0));
			mShader->use();
			mShader->setUniform("modulate", mModulate);
			upload(aMat);

				// culling
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

			// ---- Segunda hierba.
			dmat4 bMat = modelViewMat * mModelMat 
				* translate(glm::dmat4(1), glm::dvec3(0, 0, -80))
				* translate(glm::dmat4(1), glm::dvec3(80, 0, 0))
				* translate(glm::dmat4(1), glm::dvec3(0, length / 2, 0))
				* rotate(dmat4(1), radians(45.0), dvec3(0.0, 1.0, 0.0))
				* rotate(dmat4(1), radians(-90.0), dvec3(0.0, 0.0, 1.0));

			mShader->use();
			mShader->setUniform("modulate", mModulate);
			upload(bMat);
			mMesh->render();

			// ---- Tercera hierba.
			dmat4 cMat = modelViewMat * mModelMat
				* translate(glm::dmat4(1), glm::dvec3(0, 0, -80))
				* translate(glm::dmat4(1), glm::dvec3(80, 0, 0))
				* translate(glm::dmat4(1), glm::dvec3(0, length / 2, 0))
				* rotate(dmat4(1), radians(-45.0), dvec3(0.0, 1.0, 0.0))
				* rotate(dmat4(1), radians(-90.0), dvec3(0.0, 0.0, 1.0));
			mShader->use();
			mShader->setUniform("modulate", mModulate);
			upload(cMat);
			mMesh->render();

		mTexture->unbind();  // desactiva la textura en la gpu
	}
}

// ---- PHOTO ----
Photo::Photo(GLdouble w, GLdouble h, GLboolean modulate)
	: EntityWithTexture(modulate, false)
{
	mMesh = Mesh::generateRectangleTexCor(w, h);
	mModelMat = rotate(dmat4(1), radians(90.0), glm::dvec3(0, 1, 0)) * rotate(dmat4(1), radians(-90.0), glm::dvec3(1, 0, 0));
}

void Photo::render(const glm::dmat4& modelViewMat) const
{
	if (mMesh != nullptr) {
		dmat4 aMat = modelViewMat * mModelMat; // glm matrix multiplication
		mShader->use();
		mShader->setUniform("modulate", mModulate);
		upload(aMat);

		if (mTexture != nullptr) // si la textura no es nula podemos proceder a renderizarla
		{
			mTexture->bind(); // activa la textura en la gpu

			// culling
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

			mTexture->unbind(); // activa la textura en la gpu
		}
	}
}

void Photo::update()
{
	// actualiza la textura 
	mTexture->loadColorBuffer(800.0, 600.0);
}
#pragma endregion