/*
Semestre 2022-1
Práctica: Animación Sencilla y animación compleja
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>

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
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h" 
#include"SpotLight.h"
#include"Sphere.h"
#include"Toroide.h"

#include <irrKlang.h>

// include console I/O methods (conio.h for windows, our wrapper in linux)
#if defined(WIN32)
#include <conio.h>
#else
#include "common/conio.h"
#endif
using namespace irrklang;


#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll


const float toRadians = 3.14159265f / 180.0f;
const float PI = 3.14159265f;
float dirLightx = 0.0f;
float dirLightFx = 0.0f;
float dirLightFz = 0.0f;
bool keys[1024];
bool skyDia = true;
bool skynoche = false;
//bool personaje = true;
bool movPuerta = false;
bool abrirPuerta = true;
bool cerrarPuerta = false;
//Audio
int i = 0;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;
Camera cameraVistaAerea;
Camera cameraVistaCuarto;
Camera cameraVistaSala;
Camera cameraInicial;

Texture brickTexture;
Texture dirtTexture;
Texture pisoTexture;
Texture Tagave;
Texture MunecoNieve;
Texture zanahoria;
Texture boton;
Texture sombrero;
Texture verde;
Texture brazoMuneco;
//materiales
Material Material_brillante;
Material Material_opaco;
Material Timmy;
//luz direccional
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Model Camino_M;
Model casa;
Model sillon;
Model cama;
Model buro;
Model tv;
Model escritorio;
Model compu;
Model silla;
Model puerta;
Model marcoPuerta;
Model arbol;
Model regalo;
Model regaloTapa;
Model regaloCaja;
Model arbolRegalo;
Model pelota;
//Robot
Model Cuerpo;
Model PiernaDer;
Model PiernaIzq;
Model BrazoDer;
Model BrazoIzq;
//Dino
Model Dino;
Model bocaDino;
//Tanque
Model cuerpoTank;
Model torreta;
Model ruedas;
Model llantaGrande;
Model llantaChica;
//Personaje
Model timmy_cabeza;
Model timmy_tronco;
Model pierna_izq;
Model pierna_izq_abajo;
Model pierna_der;
Model pierna_der_abajo;
Model brazo_izq;
Model brazo_der;
Model mano_der;
Model mano_izq;

Sphere sp = Sphere(2.0, 20, 20);
Toroide tr = Toroide(3.0, 2.0, 20, 20);


Skybox skyboxDia;
Skybox skyboxNoche;

glm::vec3 luzNoche(0.4f, 0.4f, 0.8f);
glm::vec3 luzDia(0.9f, 0.9f, 1.0f);
//glm::vec3 PosIni(1.0f, -3.2f, 1.5f);
glm::vec3 PosIni(0.0f, 0.0f, 0.0f);
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0, rotRodDer = 0, rotBraDer = 0, rotBraIzq = 0, rotRodIzqS = 0, rotRodDerS = 0, rotBraDerS = 0, rotBraIzqS = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

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

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
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
	// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main(int argc, const char** argv)
{
	ISoundEngine* engine = irrklang::createIrrKlangDevice();

	//mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow = Window(800, 600); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CreateShaders();

	camera = Camera(glm::vec3(-2.0f, 1.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.5f);
	cameraVistaCuarto = Camera(glm::vec3(7.0f, 5.5f, -13.5f), glm::vec3(0.0f, 1.0f, 0.0f), 180.0f, 0.0f, 5.0f, 0.5f);
	cameraVistaAerea = Camera(glm::vec3(0.0f, 55.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -90.0f, 5.0f, 0.5f);
	cameraVistaSala = Camera(glm::vec3(4.0f, -0.6f, -9.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);
	//cameraInicial = Camera(posTimmy, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso2.tga");
	pisoTexture.LoadTextureA();
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();
	MunecoNieve = Texture("Textures/munecoNieve.png");
	MunecoNieve.LoadTexture();
	zanahoria = Texture("Textures/zanahoria.png");
	zanahoria.LoadTexture();
	boton = Texture("Textures/boton.png");
	boton.LoadTexture();
	sombrero = Texture("Textures/sombrero.png");
	sombrero.LoadTexture();
	verde = Texture("Textures/verde.png");
	verde.LoadTexture();
	brazoMuneco = Texture("Textures/arbolAnimCentro.png");
	brazoMuneco.LoadTexture();
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	Timmy = Material(0.2, 1);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(0.85f, 0.9f, 1.0f, //Noche 0.8f, 0.8f, 1.0f
		1.0f, 1.0f,
		0.0f, 1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		5.0f, 2.0f, -15.0f,
		1.0f, 1.0f, 1.0f);
	pointLightCount++;
	pointLights[1] = PointLight(1.0f, 0.0f, 1.0f,
		0.0f, 1.0f,
		2.0f, 7.8f, -13.0f,
		1.0f, 1.0f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(0.0f, 1.0f, 0.0f,
		0.5f, 2.0f,
		2.09f, 1.51f, -16.0f,
		-0.09f, -0.6f, -0.76f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		//1.2f, 0.9f, -20.0f,
		4.09f, 1.51f, -15.0f,
		0.45f, -0.7f, -56.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	spotLights[2] = SpotLight(1.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		3.09f, 1.51f, -18.0f,
		-0.54f, -0.36f, -0.75f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	std::vector<std::string> skyboxFaces;

	//Dia
	skyboxFaces.push_back("Textures/Skybox3/right.tga");
	skyboxFaces.push_back("Textures/Skybox3/left.tga");
	skyboxFaces.push_back("Textures/Skybox3/bottom.tga");
	skyboxFaces.push_back("Textures/Skybox3/top.tga");
	skyboxFaces.push_back("Textures/Skybox3/back.tga");
	skyboxFaces.push_back("Textures/Skybox3/front.tga");
	std::vector<std::string> skyboxFacesNoche;
	//noche
	skyboxFacesNoche.push_back("Textures/SkyNight/right.tga");
	skyboxFacesNoche.push_back("Textures/SkyNight/left.tga");
	skyboxFacesNoche.push_back("Textures/SkyNight/bottom.tga");
	skyboxFacesNoche.push_back("Textures/SkyNight/top.tga");
	skyboxFacesNoche.push_back("Textures/SkyNight/back.tga");
	skyboxFacesNoche.push_back("Textures/SkyNight/front.tga");

	skyboxDia = Skybox(skyboxFaces);
	skyboxNoche = Skybox(skyboxFacesNoche);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);

	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	tr.init();
	tr.load();

	//engine->play2D("media/Cancion.wav", true);

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		cameraVistaCuarto.keyControl(mainWindow.getsKeys(), deltaTime);
		cameraVistaCuarto.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (skyDia)
		{
			skyboxDia.DrawSkybox(camera.calculateViewMatrix(), projection);
			mainLight.SetDiaNoche(luzDia);
		}
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 dirLight(dirLightx, -1.0f, -0.4);
		spotLights[0].SetDir(dirLight);
		glm::vec3 dirLight1(dirLightFx, -1.0f, -0.4f);
		spotLights[1].SetDir(dirLight1);
		glm::vec3 dirLight2(-0.0, -1.0f, dirLightFz);
		spotLights[2].SetDir(dirLight2);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		//Vistas
		if (mainWindow.getVista() )
		{
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelauxMuneco(1.0);
		glm::mat4 modelauxMunecoBotones(1.0);
		glm::mat4 modelauxMunecoBrazos(1.0);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -6.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		zanahoria.UseTexture();
		tr.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -3.0f, 3.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MunecoNieve.UseTexture();
		sp.render(); //Renderiza esfera
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		modelauxMuneco = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.3f, 2.5f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 1.2f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		zanahoria.UseTexture();
		meshList[1]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.7f, 0.8f, 1.7f));
		model = glm::scale(model, glm::vec3(0.13f, 0.13f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		boton.UseTexture();
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(-0.7f, 0.8f, 1.7f));
		model = glm::scale(model, glm::vec3(0.13f, 0.13f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(-0.7f, 0.0f, 1.85f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(-0.4f, -0.1f, 1.925f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(-0.1f, -0.2f, 1.95f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.2f, -0.2f, 1.95f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.5f, -0.1f, 1.925f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.8f, 0.0f, 1.85f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 2.7f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 2.35f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.0f, -4.0f, 2.7f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 2.6f, 3.8f));
		model = glm::scale(model, glm::vec3(1.2f, 1.0f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		sombrero.UseTexture();
		meshList[4]->RenderMesh();
		model = glm::translate(model, glm::vec3(0.0f, -0.6f, 0.0f));
		model = glm::scale(model, glm::vec3(1.3f, 0.25f, 1.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		meshList[4]->RenderMesh();
		model = glm::translate(model, glm::vec3(0.0f, 0.9f, 0.0f));
		model = glm::scale(model, glm::vec3(0.8f, 0.7f, 0.80f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		verde.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.0f, -1.1f, 1.6f));
		model = glm::scale(model, glm::vec3(3.5f, 0.5f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		verde.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(1.6f, -1.1f, 0.1f));
		model = glm::scale(model, glm::vec3(0.2f, 0.5f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		verde.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(-1.6f, -1.1f, 0.1f));
		model = glm::scale(model, glm::vec3(0.2f, 0.5f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		verde.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(1.4f, -3.0f, 1.7f));
		model = glm::scale(model, glm::vec3(0.5f, 3.0f, 1.2f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		verde.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(0.0f, -1.1f, -1.5f));
		model = glm::scale(model, glm::vec3(3.5f, 0.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		verde.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(-4.0f, -2.0f, 0.5f));
		model = glm::rotate(model, -20 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelauxMunecoBrazos = model;
		model = glm::scale(model, glm::vec3(3.0f, 0.20f, 0.20f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoMuneco.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMunecoBrazos;
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f, 0.08f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		model = glm::translate(model, glm::vec3(4.0f, -2.0f, 0.5f));
		model = glm::rotate(model, 20 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		modelauxMunecoBrazos = model;
		model = glm::scale(model, glm::vec3(3.0f, 0.20f, 0.20f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoMuneco.UseTexture();
		meshList[4]->RenderMesh();
		model = modelauxMunecoBrazos;
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(1.0f, -0.25f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[4]->RenderMesh();
		model = modelauxMuneco;
		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}


