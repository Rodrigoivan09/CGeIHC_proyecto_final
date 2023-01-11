/*
 * Sphere.h
 *Basado en el código creado por
 *  Created on: 06/09/2016
 *      Author: reynaldo Martell
 */

#ifndef TOROIDE_H_
#define TOROIDE_H_

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include "definition.h"

class Toroide {
public:
	//Sphere(float ratio, int slices, int stacks, MODEL_MODE mode);
	Toroide(float radioCentro, float radioT, int slices, int stacks);
	void init();
	void load();
	void render();
	virtual ~Toroide();
private:
	std::vector<VertexColor> vertexC;
	/*std::vector<VertexLightColor> vertexLC;
	std::vector<VertexLightTexture> vertexLT;*/
	std::vector<GLuint> index;
	float radioCentro;
	float radioT;
	int slices;
	int stacks;
	//MODEL_MODE mode;

	GLuint VAO, VBO, EBO;
};

#endif
