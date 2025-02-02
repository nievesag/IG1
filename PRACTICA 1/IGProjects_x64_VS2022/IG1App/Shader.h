#ifndef _H_Shader_H_
#define _H_Shader_H_

// - Se encarga de compilar, cargar y pasar parametros a los shaders -

// GUARDA (atributos):
// GLuint mProgram -> referencia al programa en la GPU

// METODOS:
// use -> para activarlo
// setUniform -> cargar parametros
// get(string) -> carga un par de shaders por su nombre de la carpeta assets/shaders
// => Por ejemplo, get("simple") compila los shaders simple_vertex.glsl y simple_fragment.glsl
// de dicha carpeta y carga el programa en la GPU

// COMO FUNCIONAN:
// Los shaders son faciles de comprender, pero en principio los trataremos como cajas negras =>
/*
	#version 330 core
	// posicion del vertice en coordenadas locales
	layout(location = 0) in vec3 aPos;
	uniform mat4 modelView; // matriz de modelado/vista
	uniform mat4 projection; // matriz de proyección
	void main()
	{
		// posicion del vertice en coordenadas de la pantalla
		gl_Position = projection * modelView * vec4(aPos, 1.0);
	}
*/

#include <string>

#include <glm/fwd.hpp>
#include <GL/glew.h>

class Shader
{
public:
	Shader(const std::string& name);
	~Shader();

	Shader(const Shader& m) = delete;            // no copy constructor
	Shader& operator=(const Shader& m) = delete; // no copy assignment

	void use();

	void setUniform(const std::string& name, bool value);
	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, const glm::vec3& value);
	void setUniform(const std::string& name, const glm::vec4& value);
	void setUniform(const std::string& name, const glm::mat4& value);

	static Shader* get(const std::string& name);

	static void setUniform4All(const std::string& name, const glm::mat4& value);

protected:
	GLuint mProgram;

	bool buildShader(GLuint& shader, GLuint type, const std::string& filename);
};

#endif // _H_Shader_H_
