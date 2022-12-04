/*
Semestre 2023-1
Animaci�n:
1.- Simple o b�sica:Por banderas y condicionales
2.- Compleja: Por medio de funciones y algoritmos,Textura Animada.
4.- T�cnicas de Animaci�n: Por Keyframes
*/
//para cargar imagen	
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>r
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

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;
const float PI = 3.14159265f;

//variables para animaci�n
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;

float rotheli;
float rotheliOffset;
float movheli;
float movheliOffset;

bool avanza = true;
bool avanzaH = true;

float toffsetu = 0.0f;
float toffsetv = 0.0f;

glm::vec3 PosIni(0.0f, 0.0f, 0.0f);
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0, rotRodDer = 0, rotBraDer = 0, rotBraIzq = 0, rotRodIzqS = 0, rotRodDerS = 0, rotBraDerS = 0, rotBraIzqS = 0;
float rotJimmy = 0.0f;
bool giroDer = false;
bool giroIzq = false;
bool recorrido1 = true;
bool recorrido2 = false;
bool recorrido3 = false;
bool recorrido4 = false;
bool auxMovJimmy = true;
bool keys[1024];


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture albercaTexture;

Texture mariscos;
Texture puesto;
Texture mariscos_comida;
Texture helados_caja;
Texture tortas_rotulo;
Texture tortas_front;
Texture rotulo_represion;
Texture tacos_rotulo;
Texture tacos_front;
Texture tacos_rotulo_mini;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
Model Coche;
Model jimmy;
Model aku;
Model shrek;


Model jimmy_cabeza;
Model jimmy_tronco;
Model pierna_izq;
Model pierna_izq_abajo;
Model pierna_der;
Model pierna_der_abajo;
Model brazo_izq;
Model brazo_der;
Model mano_der;
Model mano_izq;


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

//Esfera
Sphere sp = Sphere(1.0, 10, 10); // esfera radio 1, 20 slices, 20 meridianos

//c�lculo del promedio de las normales para sombreado de Phong
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
	
	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

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

	GLfloat cubo_vertices[] = {

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
		-0.5f, -0.5f, -0.5f,	0.0f,   0.0f,		0.0f,	0.0f,	1.0f,
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

	unsigned int piramidecuadrangular_indices[] = {
			0,3,4,
			3,2,4,
			2,1,4,
			1,0,4,
			0,1,2,
			0,2,4

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
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

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);
	
	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(cubo_vertices, cubo_indices, 256, 96);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(obj7);
}


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
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/luna.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	mariscos = Texture("Textures/mariscos.jpg");
	mariscos.LoadTextureA();
	puesto = Texture("Textures/puesto_sin_rotulo.jpg");
	puesto.LoadTextureA();
	mariscos_comida = Texture("Textures/mariscos_comida.jpg");
	mariscos_comida.LoadTextureA();
	helados_caja = Texture("Textures/helados_caja.jpg");
	helados_caja.LoadTextureA();

	tortas_rotulo = Texture("Textures/tortas_rotulo.jpg");
	tortas_rotulo.LoadTextureA();
	tortas_front = Texture("Textures/tortas_front.png");
	tortas_front.LoadTextureA();
	rotulo_represion = Texture("Textures/rotulo_SC.png");
	rotulo_represion.LoadTextureA();

	tacos_front = Texture("Textures/tacos_front.png");
	tacos_front.LoadTextureA();
	tacos_rotulo = Texture("Textures/tacos_rotulo.png");
	tacos_rotulo.LoadTextureA();
	tacos_rotulo_mini = Texture("Textures/rotulo_tacos.png");
	tacos_rotulo_mini.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");
	Coche = Model();
	Coche.LoadModel("Models/coche_p5.obj");

	jimmy = Model();
	jimmy.LoadModel("Models/jimmy.obj");

	aku = Model();
	aku.LoadModel("Models/Aku_Aku_READY.fbx");

	shrek = Model();
	shrek.LoadModel("Models/CHARACTER_Shrek.obj");

	//Personaje
	jimmy_tronco = Model();
	jimmy_tronco.LoadModel("Models/Jimmy/torzo.obj");
	pierna_der = Model();
	pierna_der.LoadModel("Models/Jimmy/pierna_der.obj");
	pierna_der_abajo = Model();
	pierna_der_abajo.LoadModel("Models/Jimmy/pie_der.obj");
	pierna_izq = Model();
	pierna_izq.LoadModel("Models/Jimmy/pierna_izq.obj");
	pierna_izq_abajo = Model();
	pierna_izq_abajo.LoadModel("Models/Jimmy/pie_izq.obj");
	brazo_der = Model();
	brazo_der.LoadModel("Models/Jimmy/brazo_der.obj");
	brazo_izq = Model();
	brazo_izq.LoadModel("Models/Jimmy/Brazo_izq.obj");
	mano_der = Model();
	mano_der.LoadModel("Models/Jimmy/antebrazo_der.obj");
	mano_izq = Model();
	mano_izq.LoadModel("Models/Jimmy/antebrazo_izq.obj");

	/*for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotRodDer = 0;
		KeyFrame[i].rotBraDer = 0;
		KeyFrame[i].rotBraIzq = 0;
		KeyFrame[i].rotRodIzqS = 0;
		KeyFrame[i].rotRodDerS = 0;
		KeyFrame[i].rotBraDerS = 0;
		KeyFrame[i].rotBraIzqS = 0;
		KeyFrame[i].rotIncS = 0;
		KeyFrame[i].rotInc2S = 0;
		KeyFrame[i].rotInc3S = 0;
		KeyFrame[i].rotInc4S = 0;
		KeyFrame[i].rotInc = 0;
		KeyFrame[i].rotInc2 = 0;
		KeyFrame[i].rotInc3 = 0;
		KeyFrame[i].rotInc4 = 0;

	}*/

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/skyRt.tga");
	skyboxFaces.push_back("Textures/Skybox/skyLf.tga");
	skyboxFaces.push_back("Textures/Skybox/skyDwn.tga");
	skyboxFaces.push_back("Textures/Skybox/skyUp.tga");
	skyboxFaces.push_back("Textures/Skybox/skyBk.tga");
	skyboxFaces.push_back("Textures/Skybox/skyFront.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, -1.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz de helic�ptero
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		50.0f);
	spotLightCount++;

	//luz de faro



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	//coche y llantas
	movCoche = 0.0f;
	movOffset = 0.3f; // velocidad
	rotllanta = 0.0f;
	rotllantaOffset = 3.0f;

	//helicopter helicopter;
	movheli = 0.0f;
	movheliOffset = 0.2f;
	rotheli = 0.0f;
	rotheliOffset = 4.0f;


	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader


	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		// animacion coche
		if (avanza) {
			if (!(movCoche > -300.0f))
				avanza = false;

			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
			rotllanta += rotllantaOffset * deltaTime;


		}
		else {
			if (!(movCoche < 300.0f))
				avanza = true;

			movCoche += movOffset * deltaTime;
			rotllanta -= rotllantaOffset * deltaTime;

		}



// animacion helicopter
	GLfloat angleH = -30.0f; //angulo de inclinaci�n al frente

	if (avanzaH) {
		if (!(movheli > -300.0f)) 
			avanzaH = false;
		
		movheli -= movheliOffset * deltaTime;
		
		if (movheli < -290.0f && rotheli < 180.0f && angleH < 30.0f) {
			rotheli += rotheliOffset * deltaTime;
			angleH += rotheliOffset * deltaTime;
		}
	
	}
	else {

		if (!(movheli < 0)) 
			avanzaH = true;

		movheli += movheliOffset * deltaTime;

		if (movheli > -10.0f && rotheli > 0.0f && angleH < 30.0f) {
			rotheli -= rotheliOffset * deltaTime;
			angleH -= rotheliOffset * deltaTime;
		}
	}


	//Recorrido Jimmy

	if (mainWindow.getrecorrido())
	{
		if (recorrido1)
		{
			//posX += 0.5f;
			rotJimmy = 90.0f;
			posZ -= 0.009;
			if (auxMovJimmy)
			{
				rotRodDer = 0.0f;
				rotRodDerS += 0.4f;
				rotRodIzqS -= 0.4f;
				rotRodIzq -= 0.5f;
				rotBraDerS -= 0.4f;
				rotBraIzqS += 0.4f;
				if (rotRodDerS > 30)
				{
					auxMovJimmy = false;

				}
			}
			if (!auxMovJimmy)
			{
				rotRodDerS -= 0.4f;
				rotRodIzqS += 0.4f;
				rotRodIzq = 0.0f;
				rotRodDer -= 0.5f;
				rotBraDerS += 0.4f;
				rotBraIzqS -= 0.4f;
				if (rotRodDerS < -30)
				{
					auxMovJimmy = true;
				}
			}
			if (posZ < -7.7f)
			{
				rotRodDerS = 0.0f;
				rotRodIzqS = 0.0f;
				rotRodIzq = 0.0f;
				rotRodDer = 0.0f;
				rotBraDerS = 0.0f;
				rotBraIzqS = 0.0f;
				recorrido1 = false;
				//engine->play2D("media/puertaAbre.wav",
				/*	false, false, true);*/
				//movPuerta = true;
				recorrido2 = true;
			}
		}
	/*	if (movPuerta)
		{

			if (abrirPuerta)
			{
				rotPuerta -= 20.0f * deltaTime;
				if (rotPuerta < -90)
				{
					abrirPuerta = false;
					recorrido2 = true;
				}
			}
		}*/
		if (recorrido2)
		{
			rotRodDerS = 60.0f;
			rotRodDer = -30.0f;
			posY += 0.008f;
			posZ -= 0.008f;
			printf("Pos y = %f", posY);
			if (posY > 0.3f)
			{
				rotRodDer = 0.0f;
				rotRodDerS = 0.0f;
				rotRodIzqS = 60.0f;
				rotRodIzq = -30.0f;
				posZ -= 0.008f;
				posY += 0.008f;
				printf("pos y = %f", posY);
				if (posY > 0.75)
				{
					rotRodIzqS = 0.0f;
					rotRodIzq = 0.0f;
					posZ -= 0.008f;
					printf("pos z = %f", posZ);
					if (posZ < -0.5f)
					{
						recorrido2 = false;
						recorrido3 = true;
					}
				}
			}
		}
		if (recorrido3)
		{
			posZ -= 0.009;
			if (auxMovJimmy)
			{
				rotRodDerS += 0.4f;
				rotRodIzqS -= 0.4f;
				rotBraDerS -= 0.4f;
				rotBraIzqS += 0.4f;
				if (rotRodDerS > 20)
				{
					auxMovJimmy = false;

				}
			}
			if (!auxMovJimmy)
			{
				//movTimmyZ -+ 0.006f;
				rotRodDerS -= 0.4f;
				rotRodIzqS += 0.4f;
				rotBraDerS += 0.4f;
				rotBraIzqS -= 0.4f;
				if (rotRodDerS < -20)
				{
					auxMovJimmy = true;
				}
			}
			if (posZ < -11.7f)
			{
				rotRodDerS = 0.0f;
				rotRodIzqS = 0.0f;
				rotBraDerS = 0.0f;
				rotBraIzqS = 0.0f;
				posY = 0.3f;
				recorrido3 = false;
				//EspectaculoLuces = true;
				//cerrarPuerta = true;
				recorrido4 = true;
			}
		}
		if (recorrido4)
		{
			posZ -= 0.009;
			if (auxMovJimmy)
			{
				rotRodDerS += 0.4f;
				rotRodIzqS -= 0.4f;
				rotBraDerS -= 0.4f;
				rotBraIzqS += 0.4f;
				if (rotRodDerS > 20)
				{
					auxMovJimmy = false;

				}
			}
			if (!auxMovJimmy)
			{
				rotRodDerS -= 0.4f;
				rotRodIzqS += 0.4f;
				rotBraDerS += 0.4f;
				rotBraIzqS -= 0.4f;
				if (rotRodDerS < -20)
				{
					auxMovJimmy = true;
				}
			}
			if (posZ < -17.7f)
			{
				rotRodDerS = 0.0f;
				rotRodIzqS = 0.0f;
				rotBraDerS = 0.0f;
				rotBraIzqS = 0.0f;
				recorrido4 = false;
				/*engine->play2D("media/puertaCierra.wav",
					false, false, true);
				cerrarPuerta = true;*/
			}
		}
		/*if (cerrarPuerta)
		{
			rotPuerta += 20.0f * deltaTime;
			if (rotPuerta > 0)
			{
				cerrarPuerta = false;
				recorrido = false;
			}
		}*/
	}


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
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());


		glm::vec3 helicopter(0.0f, 3.0f, movheli);
		spotLights[2].SetFlash(helicopter, glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[2].SetPos(helicopter);

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		glm::mat4 modelauxJimmy(1.0);
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();



		//�###############################3  JiMMY

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(30, -2.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		jimmy.RenderModel();

		// ######################3      Jimmy Articulado ###############################3

		//Personaje

		//tonco
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(posX + 1.8f, -0.5f + posY, posZ + 1.9f));
		//model = glm::translate(model, glm::vec3(1.8f, 0.0f,  1.9f));
		model = glm::rotate(model, rotJimmy * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Timmy.UseMaterial(uniformSpecularIntensity, uniformShininess);
		jimmy_tronco.RenderModel();
		modelauxJimmy = model;
		model = glm::translate(model, glm::vec3(0.052f, 0.45f, 2.6f));
		model = glm::rotate(model, rotRodDerS * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna_der.RenderModel();
		model = glm::translate(model, glm::vec3(-0.08f, -0.7f, 0.07f));
		model = glm::rotate(model, rotRodDer * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna_der_abajo.RenderModel();
		model = modelauxJimmy;
		model = glm::translate(model, glm::vec3(0.1, 0.537f, 1.731));
		model = glm::rotate(model, rotRodIzqS * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna_izq.RenderModel();
		model = glm::translate(model, glm::vec3(-0.07f, -0.82f, -0.08));
		model = glm::rotate(model, rotRodIzq * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pierna_izq_abajo.RenderModel();
		model = modelauxJimmy;
		model = glm::translate(model, glm::vec3(0.0f, 2.65f, 3.0f));
		model = glm::rotate(model, rotBraDerS * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, giroDer * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo_der.RenderModel();
		model = glm::translate(model, glm::vec3(-0.054f, -0.7f, 0.32f));
		model = glm::rotate(model, rotBraDer * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, giroIzq * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mano_der.RenderModel();
		model = modelauxJimmy;
		model = glm::translate(model, glm::vec3(0.05f, 2.65f, 1.4f));
		model = glm::rotate(model, rotBraIzqS * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazo_izq.RenderModel();
		model = glm::translate(model, glm::vec3(0.04f, -0.6f, -0.47f));
		model = glm::rotate(model, rotBraIzq * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mano_izq.RenderModel();
		glDisable(GL_BLEND);

		//######################  AKU @@@@@@@@@

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(40, 10.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//aku.RenderModel();

		//###################  SHREK

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(60, -2.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		shrek.RenderModel();


// puesto  tortas

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, -1.0f, -10.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		puesto.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// parte superior
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		tortas_rotulo.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// barra
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 2.0f));
		model = glm::scale(model, glm::vec3(5.0f, 0.1f, 1.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		puesto.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// interior
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 1.505f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 0.001f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		tortas_front.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// rotulo sandra cuevas
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5001f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		rotulo_represion.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();



// puesto  mariscos

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, -1.0f, -10.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		puesto.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// parte superior
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		mariscos.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// barra
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 2.0f));
		model = glm::scale(model, glm::vec3(5.0f, 0.1f, 1.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		puesto.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// interior
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 1.505f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 0.001f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		mariscos_comida.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();



// puesto  tacos

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -10.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		puesto.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// parte superior
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		tacos_rotulo.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// barra
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 2.0f));
		model = glm::scale(model, glm::vec3(5.0f, 0.1f, 1.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		puesto.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// interior
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 1.505f));
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 0.001f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		tacos_front.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();
		// rotulo sandra cuevas
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.5001f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f, 2.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		tacos_rotulo_mini.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[5]->RenderMesh();



		// Carro de helados
		//rectangulo 1 (caja)
		model = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::translate(model, glm::vec3(movCoche, -1.0f, 20.0f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.5f, 0.25f, -5.0f));
		model = glm::scale(model, glm::vec3(2.5f, 1.25f, 1.0f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		helados_caja.UseTexture();
		meshList[5]->RenderMesh(); // dibujando cubo


// rectangulo 3 (motor)
		color = glm::vec3(0.3f, 0.6f, 0.5f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.2f, 0.01f, -5.0f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(1.2f, 0.65f, 1.0f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh(); // dibujando cubo

// rectangulo 4 (cabina)
		color = glm::vec3(0.3f, 0.6f, 0.5f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 0.4f, -5.0f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(0.7f, 0.6f, 1.0f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh(); // dibujando cubo

// rectangulo 5 (ventanas)
		color = glm::vec3(0.0f, 0.1f, 1.0f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 0.45f, -5.0f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(0.35f, 0.3f, 1.001f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh(); // dibujando cubo

// rectangulo 6 (parabrisas)
		color = glm::vec3(0.0f, 0.1f, 1.0f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.4f, 0.45f, -5.0f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(0.01f, 0.3f, 0.8f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh(); // dibujando cubo

// rectangulo 7 (luz izq)
		color = glm::vec3(1.0f, 1.0f, 0.0f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.8f, 0.1f, -5.28f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(0.01f, 0.25f, 0.25f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh(); // dibujando cubo

// rectangulo 8 (luz der)
		color = glm::vec3(1.0f, 1.0f, 0.0f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.8f, 0.1f, -4.72f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(0.01f, 0.25f, 0.25f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[5]->RenderMesh(); // dibujando cubo

//cono helado
		color = glm::vec3(0.6f, 0.29f, 0.0f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.3f, 1.0f, -5.0f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(0.6f, 1.2f, 0.6f)); // escalando figuras
		model = glm::rotate(model, 180 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[6]->RenderMesh(); // dibujando piramide

//  helado
		color = glm::vec3(1.0f, 0.4f, 0.69f);  // declarando el color magenta al objeto
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.3f, 1.8f, -5.0f)); //posicionando al escenario, retrocedemos la camara para ver la figura
		model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f)); // escalando figuras
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render(); //Renderiza esfera

		//llantas coche de helados
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.1f, -0.25f, -4.45f));
		model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.006f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.1f, -0.25f, -5.5f));
		model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.006f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, -0.25f, -4.45f));
		model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.006f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, -0.25f, -5.5f));
		model = glm::scale(model, glm::vec3(0.004f, 0.004f, 0.006f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();

		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

//  Coche
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(movCoche, 3.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Coche.RenderModel();



		model = modelaux;
		model = glm::translate(model, glm::vec3(-8.0, -1.5f, 3.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.03f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-8.0, -1.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.03f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(5.5, -1.5f, 3.5f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.03f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(5.5, -1.5f, -3.0f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.03f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Llanta_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, movheli));
		model = glm::translate(model, glm::vec3(0.0f, 3.0f + 4 * sin((0.3 * rotllanta) * toRadians), 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotheli * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, angleH * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.53f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f, 2.0f, 3.0f));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Camino_M.RenderModel();

		//Agave �qu� sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		

		//textura con movimiento
		//Importantes porque la variable uniform no podemos modificarla directamente
		toffsetu += 0.001 * deltaTime;
		toffsetv += 0.0	  * deltaTime;
		//para que no se desborde la variable
		if (toffsetu > 1.0)
			toffsetu = 0.0;
		if (toffsetv > 1.0)
			toffsetv = 0;
		//printf("\ntfosset %f \n", toffsetu);
		//pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetu, toffsetv);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -6.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		FlechaTexture.UseTexture();
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		glDisable(GL_BLEND);
		
		



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
