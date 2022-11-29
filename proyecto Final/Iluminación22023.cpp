/*
Semestre 2023-1
Práctica 7: Iluminación 1 
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

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;
int cont = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture DadoTexture;


Model Coche;
Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model Pista;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

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


	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);


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
		//x		y		z	 	S		T			NX		NY		Nz


		// 1
		-3.0f, 0.0f,  0.0f,		0.0f, 0.69f,		1.0f,	1.0f,	-1.0f,
		0.0f,  3.0f,  0.0f,		0.24f,	0.69f,		1.0f,	1.0f,	-1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		1.0f,	1.0f,	-1.0f,
		0.0f,  0.0f,  3.0f,	    0.14f,	1.00f,		1.0f,	1.0f,	-1.0f,

		// 2
		3.0f,  0.0f,  0.0f,		0.5f,  0.69f,		-1.0f,	1.0f,	1.0f,
		0.0f, -3.0f,  0.0f,		0.747f,	0.69f,		-1.0f,	1.0f,	1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		-1.0f,	1.0f,	1.0f,
		0.0f,  0.0f, -3.0f,	    0.63f,	1.0f,		-1.0f,	1.0f,	1.0f,

		// 3
		3.0f,  0.0f,  0.0f,		0.5f,  0.35f,		-1.0f,	-1.0f,	-1.0f,
		0.0f, -3.0f,  0.0f,		0.75f,	0.35f,		-1.0f,	-1.0f,	-1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		-1.0f,	-1.0f,	-1.0f,
		0.0f,  0.0f,  3.0f,	    0.64f,	0.67f,		-1.0f,	-1.0f,	-1.0f,

		// 4
		-3.0f, 0.0f,  0.0f,		0.5f,  0.0f,		1.0f,	-1.0f,	1.0f,
		0.0f,  3.0f,  0.0f,		0.74f,	0.0f,		1.0f,	-1.0f,	1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		1.0f,	-1.0f,	1.0f,
		0.0f,  0.0f, -3.0f,	    0.64f,	0.32f,		1.0f,	-1.0f,	1.0f,


		// 7
		3.0f,  0.0f,  0.0f,  	0.26f,	0.05f,		-1.0f,	-1.0f,	-1.0f,
		0.0f,  3.0f,  0.0f,		0.5f,	0.04f,		-1.0f,	-1.0f,	-1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		-1.0f,	-1.0f,	-1.0f,
		0.0f,  0.0f,  3.0f,	    0.41f,  0.32f,		-1.0f,	-1.0f,	-1.0f,

		// 6
		3.0f,  0.0f,  0.0f,		0.01f,  0.07f,		-1.0f,	1.0f,	1.0f,
		0.0f,  3.0f,  0.0f,		0.24f,	0.06f,		-1.0f,	1.0f,	1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		-1.0f,	1.0f,	1.0f,
		0.0f,  0.0f, -3.0f,	    0.15f,	0.33f,		-1.0f,	1.0f,	1.0f,


		// 5
		-3.0f, 0.0f,  0.0f,  	0.014f,	0.41f,		1.0f,	1.0f,	1.0f,
		0.0f, -3.0f,  0.0f,		0.24f,	0.41f,		1.0f,	1.0f,	1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		1.0f,	1.0f,	1.0f,
		0.0f,  0.0f,  3.0f,	    0.138f,	0.67f,		1.0f,	1.0f,	1.0f,

		// 8
		-3.0f, 0.0f,  0.0f,  	0.26f,  0.34f,		1.0f,	-1.0f,	1.0f,
		0.0f, -3.0f,  0.0f,		0.5f,	0.34f,		1.0f,	-1.0f,	1.0f,
		0.0f,  0.0f,  0.0f,		0.0f,	0.0f,		1.0f,	-1.0f,	1.0f,
		0.0f,  0.0f, -3.0f,	    0.38f,	0.67f,		1.0f,	-1.0f,	1.0f,


	};
	calcAverageNormals(cubo_indices, 96, cubo_vertices, 256, 8, 5);
	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 256, 96);
	meshList.push_back(dado);

}
/*
void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};
	calcAverageNormals(cubo_indices,36,cubo_vertices,192,8,5);
	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);
}
*/

void CreateShaders()
{
	Shader *shader1 = new Shader();
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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	DadoTexture = Texture("Textures/mk.tga");
	DadoTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");
	Coche = Model();
	Coche.LoadModel("Models/Roadster.obj");
	Pista = Model();
	Pista.LoadModel("Models/pista.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		1.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.5f, 0.5f,
		20.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	
	

	//luz coche
	spotLights[0] = SpotLight(0.0f, 0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		.0f, 0.0f, 0.0f,
		0.0f);
	spotLightCount++;
	//luz coche
	spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f);
	spotLightCount++;
	// helicoptero
	spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 8.0f,
		1.0f, 8.0f, 0.0f,
		30.0f);
	spotLightCount++;


	//luz de faro



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
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);



		//luz de helicoptero
		glm::vec3 helicopter(0.0f, 3.0f, -10.0f);
		spotLights[2].SetFlash(helicopter, glm::vec3(0.0f, -1.0f, 0.0f));

		//luz del coche
	


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);




		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);



		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, -10.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-30.0f, 0.0f, -30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista.RenderModel();
		
		

		//luz de helicóptero por tiempo
		if (1) {
			spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f,
				0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f);
			spotLights[2].SetPos(helicopter);
			cont++;

			if (cont > 300) {
				spotLights[2] = SpotLight(1.0f, 0.0f, 0.0f,
					1.0f, 2.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, -1.0f, 0.0f,
					1.0f, 0.0f, 0.0f,
					30.0f);

				spotLights[2].SetPos(helicopter);
				cont++;
				if (cont > 600) {
					spotLights[2] = SpotLight(0.0f, 0.0f, 0.0f,
						8.0f, 0.0f,
						0.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 0.0f,
						0.0f, 6.0f, 0.0f,
						0.0f);
					spotLights[2].SetPos(helicopter);
					cont = 0;
				}
			}
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.53f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Camino_M.RenderModel();


		model = glm::mat4(1.0);
		color = glm::vec3(0.0f, 0.5f, 1.0f);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Coche.RenderModel();
		
		if (mainWindow.getbandera() == 1.0f) {

			spotLights[0] = SpotLight(0.0f, 0.0f, 0.0f,
				0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f);

			spotLights[1] = SpotLight(0.3f, 0.8f, 0.2f,
				0.1f, 5.0f,
				5.0f, 10.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				150.0f);
	
			glm::vec3 cochefront(mainWindow.getmuevex(), 0.0f, 0.0f);
			spotLights[1].SetFlash(cochefront,glm::vec3(1.0f,0.0f,0.0f));

		}

		if (mainWindow.getbandera() == -1.0f) {

			spotLights[0] = SpotLight(0.5f, 0.0f, 0.9f,
				0.1f, 5.0f,
				5.0f, 10.0f, 0.0f,
				0.0f, -1.0f, 0.0f,
				1.0f, 0.0f, 0.0f,
				150.0f);

			spotLights[1] = SpotLight(0.0f, 0.0f, 0.0f,
				0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f);
	
			glm::vec3 cocheback( mainWindow.getmuevex(), 0.0f, 0.0f);
			spotLights[0].SetFlash(cocheback, glm::vec3(-1.0f, 0.0f, 0.0f));
		}
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, 20.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		DadoTexture.UseTexture();
		meshList[4]->RenderMesh();



//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		
				
		
		glDisable(GL_BLEND);


		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	return 0;
}
