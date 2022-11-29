/*
Semestre 2023-1
Práctica 5: Optimización y Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;

Texture dadoTexture8Caras;

Model Kitt_M;
Model Llanta_M;

Model kittobj_M;
Model cofrepivote;

Model cocheP;
Model cofreP;

Model Dado_t;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}



void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);
}

void CrearDado()
{
	unsigned int cubo_indices[] = {
		
		//DADO DE 8 CARAS
		0,1,2,  //0-1-2-3
		0,1,3,  
		1,2,3,
		0,2,3,

		4,5,6,  //4-5-6-7
		4,5,7,
		5,6,7,
		4,6,7,

		8,9,10,  //8-9-10-11
		8,9,11,
		9,10,11,
		8,10,11,

		12,13,14,  //12-13-14-15
		12,13,15,
		13,14,15,
		12,14,15,

		16,17,18,   //16-17-18-19
		16,17,19,
		17,18,19,
		16,18,19,

		20,21,22,  //20-21-22-23
		20,21,23,
		21,22,23,
		20,22,23,

		24,25,26,  //24-25-26-27
		24,25,27,
		25,26,27,
		24,26,27,

		28,29,30,  //28-29-30-31
		28,29,31,
		29,30,31,
		28,30,31,
		
	};

	GLfloat cubo_vertices[] = {

				 // DADO DE 8 CARAS
				 //x		y		z	 	S		T			NX		NY		N


				 // 1
				 -3.0f, 0.0f,  0.0f,		0.0f, 0.69f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  3.0f,  0.0f,		0.24f,	0.69f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  3.0f,	    0.14f,	1.00f,		0.0f,	0.0f,	-1.0f,

				 // 2
				 3.0f,  0.0f,  0.0f,		0.5f,  0.69f,		0.0f,	0.0f,	-1.0f,
				 0.0f, -3.0f,  0.0f,		0.747f,	0.69f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f, -3.0f,	    0.63f,	1.0f,		0.0f,	0.0f,	-1.0f,

				 // 3
				 3.0f,  0.0f,  0.0f,		0.5f,  0.35f,		0.0f,	0.0f,	-1.0f,
				 0.0f, -3.0f,  0.0f,		0.75f,	0.35f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  3.0f,	    0.64f,	0.67f,		0.0f,	0.0f,	-1.0f,

				 // 4
				 -3.0f, 0.0f,  0.0f,		0.5f,  0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  3.0f,  0.0f,		0.74f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f, -3.0f,	    0.64f,	0.32f,		0.0f,	0.0f,	-1.0f,


				 // 7
				 3.0f,  0.0f,  0.0f,  		0.26f,	0.05f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  3.0f,  0.0f,		0.5f,	0.04f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  3.0f,	    0.41f,  0.32f,		0.0f,	0.0f,	-1.0f,

				 // 6
				 3.0f,  0.0f,  0.0f,		0.01f,  0.07f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  3.0f,  0.0f,		0.24f,	0.06f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f, -3.0f,	    0.15f,	0.33f,		0.0f,	0.0f,	-1.0f,


				 // 5
				 -3.0f, 0.0f,  0.0f,  		0.014f,	0.41f,		0.0f,	0.0f,	-1.0f,
				 0.0f, -3.0f,  0.0f,		0.24f,	0.41f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  3.0f,	    0.138f,	0.67f,		0.0f,	0.0f,	-1.0f,

				 // 8
				 -3.0f, 0.0f,  0.0f,  		0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,
				 0.0f, -3.0f,  0.0f,		0.5f,	0.34f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,
				 0.0f,  0.0f, -3.0f,	    0.38f,	0.67f,		0.0f,	0.0f,	-1.0f,

		
	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 256, 96);
	meshList.push_back(dado);

}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dadoclase.jpg");
	dadoTexture.LoadTextureA();
	dadoTexture8Caras = Texture("Textures/MK.tga");//DADO DE 8 CARAS
	dadoTexture8Caras.LoadTextureA();

	//---------------------------MODELOS---------------------------------------------

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);

		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 color2 = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 color3 = glm::vec3(1.0f, 1.0f, 0.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		//Dado de 8 caras con personajes de mortal kombat
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 6.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture8Caras.UseTexture();
		meshList[4]->RenderMesh();

		glUseProgram(0);
	   

		mainWindow.swapBuffers();
	}

	return 0;
}
