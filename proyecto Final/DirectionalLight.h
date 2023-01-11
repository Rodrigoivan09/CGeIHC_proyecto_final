#pragma once
#include "Light.h"

class DirectionalLight :
	public Light //trae la informacion de la clase light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
					GLfloat aIntensity, GLfloat dIntensity,
					GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	void SetDiaNoche(glm::vec3 color);

	~DirectionalLight();

private:
	glm::vec3 direction;
};

