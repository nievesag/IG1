#ifndef _H_Mesh_H_
#define _H_Mesh_H_

// - Se encarga de manejar las mallas del programa -
// Malla (mesh): Coordenadas de los vertices, componentes del color,
// coordenadas de vectores normales, coordenadas de textura

// GUARDA (atributos):
// mPrimitive -> primitiva con la que trazar la malla (punto, linea, triangulo, ...)
// mNumVertices -> numero de vertices
// vVertices de tipo vec3 -> arrays de vertices
// vColors de tipo vec4 -> colores
// mVAO -> el identificador del vertex array object(VAO) de la malla en la GPU
// mVBO -> el identificador de los vertex buffer objects(VBO) de vertices
// mCBO -> el identificador de los vertex buffer objects(VBO) de colores

// La malla puede estar o no cargada en la GPU (no cargada -> identificadores NONE)

// METODOS:
// load / unload -> carga / descarga
// render / draw -> pintar
// createRGBAxes -> construccion de mallas

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
	// Tipicamente construyen un objeto malla vacio (new Mesh) y modifican sus atributos
	static Mesh* createRGBAxes(GLdouble l); // creates a new 3D-RGB axes mesh
	
	// Genera los num vertices que forman el poligono regular inscrito en la circunferencia de radio ??, sobre el plano z = 0, centrada en el origen.
	static Mesh* generateRegularPolygon(GLuint num, GLdouble r);

	// Genera un triangulo de colores RGB de radio 200 sobre el plano z = 0, centrado en el origen de 
	static Mesh* generateRGBTriangle();

	// Genera los cuatro vertices del rectangulo centrado en el origen, sobre el plano z = 0, de ancho w y alto h
	static Mesh* generateRectangle(GLdouble w, GLdouble h);

	// Mete un color primario a cada vertice de un rectangle
	static Mesh* generateRGBRectangle(GLdouble w, GLdouble h);

	static Mesh* generateCube(GLdouble length);

	static Mesh* generateRGBCube(GLdouble length);

	static Mesh* generateRectangleTexCor(GLdouble w, GLdouble h);

	static Mesh* generateRectangleTexCor(GLdouble w, GLdouble h, GLuint rw, GLuint rh);

	static Mesh* generateBoxOutline(GLdouble length);

	static Mesh* generateBoxOutlineTexCor(GLdouble length);

	static Mesh* generateStar3D(GLdouble re, GLuint np, GLdouble h);

	static Mesh* generateStar3DTexCor(GLdouble re, GLuint np, GLdouble h);

	Mesh();
	virtual ~Mesh();

	Mesh(const Mesh& m) = delete;            // no copy constructor
	Mesh& operator=(const Mesh& m) = delete; // no copy assignment

	virtual void render() const;

	GLuint size() const { return mNumVertices; }; // number of elements
	std::vector<glm::vec3> const& vertices() const { return vVertices; };
	std::vector<glm::vec4> const& colors() const { return vColors; };

	void load();
	void unload();


protected:
	GLuint mPrimitive =
	  GL_TRIANGLES;          // graphic primitive: GL_POINTS, GL_LINES, GL_TRIANGLES, ...
	GLuint mNumVertices = 0; // number of elements ( = vVertices.size())
	std::vector<glm::vec3> vVertices; // vertex array
	std::vector<glm::vec4> vColors;   // color array
	std::vector<glm::vec2> vTexCoords; // texture vertex array
	virtual void draw() const;

	GLuint mVAO;  // vertex array object

private:
	GLuint mVBO;  // vertex buffer object
	GLuint mCBO;  // color buffer object
	GLuint mTCO;  // texture color buffer object
};

#endif //_H_Scene_H_
