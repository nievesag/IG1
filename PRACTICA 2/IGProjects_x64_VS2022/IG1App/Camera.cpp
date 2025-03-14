#include "Shader.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Camera::Camera(Viewport* vp)
  : mViewMat(1.0)
  , mProjMat(1.0)
  , xRight(vp->width() / 2.0)
  , xLeft(-xRight)
  , yTop(vp->height() / 2.0)
  , yBot(-yTop)
  , mViewPort(vp)
{
	setPM();
}

void Camera::setAxes()
{
	// para los ejes right=u, upward=v, front=-n
	/*mRight = glm::row(mViewMat, 0);
	mUpward = row(mViewMat, 1);
	mFront = -row(mViewMat, 2);*/
}

void Camera::moveLR(GLfloat cs) // LEFT / RIGHT.
{
	mEye += mRight * cs;
	mLook += mRight * cs;
	setVM();
}

void Camera::moveFB(GLfloat cs) // FRONT / BACK.
{
	mEye += mFront * cs;
	mLook += mFront * cs;
	setVM();
}

void Camera::moveUD(GLfloat cs) // UP / DOWN.
{
	mEye += mUpward * cs;
	mLook += mUpward * cs;
	setVM();
}

void
Camera::uploadVM() const
{
	Shader::setUniform4All("modelView", mViewMat);
}

void
Camera::setVM()
{
	mViewMat = lookAt(mEye, mLook, mUp); // glm::lookAt defines the view matrix
	setAxes();
}

void
Camera::set2D()
{
	mEye = {0, 0, 500};
	mLook = {0, 0, 0};
	mUp = {0, 1, 0};
	setVM();
}

void
Camera::set3D()
{
	mEye = {500, 500, 500};
	mLook = {0, 10, 0};
	mUp = {0, 1, 0};
	setVM();
}

void
Camera::pitch(GLdouble a)
{
	mViewMat = rotate(mViewMat, glm::radians(a), glm::dvec3(1.0, 0, 0));
	// glm::rotate returns mViewMat * rotationMatrix
}

void
Camera::yaw(GLdouble a)
{
	mViewMat = rotate(mViewMat, glm::radians(a), glm::dvec3(0, 1.0, 0));
	// glm::rotate returns mViewMat * rotationMatrix
}

void
Camera::roll(GLdouble a)
{
	mViewMat = rotate(mViewMat, glm::radians(a), glm::dvec3(0, 0, 1.0));
	// glm::rotate returns mViewMat * rotationMatrix
}

void
Camera::setSize(GLdouble xw, GLdouble yh)
{
	xRight = xw / 2.0;
	xLeft = -xRight;
	yTop = yh / 2.0;
	yBot = -yTop;
	setPM();
}

void
Camera::setScale(GLdouble s)
{
	mScaleFact -= s;
	if (mScaleFact < 0)
		mScaleFact = 0.01;
	setPM();
}

void
Camera::setPM()
{
	if (bOrto) { //  if orthogonal projection
		mProjMat = ortho(xLeft * mScaleFact,
		                 xRight * mScaleFact,
		                 yBot * mScaleFact,
		                 yTop * mScaleFact,
		                 mNearVal,
		                 mFarVal);
		// glm::ortho defines the orthogonal projection matrix
	}
}

void
Camera::uploadPM() const
{
	Shader::setUniform4All("projection", mProjMat);
}

void
Camera::upload() const
{
	mViewPort->upload();
	uploadVM();
	uploadPM();
}
