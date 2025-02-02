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
	virtual void draw() const;

	GLuint mVAO;  // vertex array object

private:
	GLuint mVBO;  // vertex buffer object
	GLuint mCBO;  // color buffer object
};

#endif //_H_Scene_H_
