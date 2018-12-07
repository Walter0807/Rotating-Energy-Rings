/*********************************************************
FILE : main.cpp (csci3260 2018-2019 Project)
*********************************************************/
/*********************************************************
Student Information
Student ID: 1155123906
Student Name: Zuowen Wang

Student ID: 1155123308
Student Name: Wentao Zhu
*********************************************************/


//version Dec.3. 17:04
#define _CRT_SECURE_NO_DEPRECATE
#include "C:\Users\cprj2748\Desktop\CG\Rotating-energy-rings-master\Assignment3\Project1\Project1\Dependencies\glew\glew.h"
#include "C:\Users\cprj2748\Desktop\CG\Rotating-energy-rings-master\Assignment3\Project1\Project1\Dependencies\freeglut\freeglut.h"
#include "C:\Users\cprj2748\Desktop\CG\Rotating-energy-rings-master\Assignment3\Project1\Project1\Dependencies\glm\glm.hpp"
#include "C:\Users\cprj2748\Desktop\CG\Rotating-energy-rings-master\Assignment3\Project1\Project1\Dependencies\glm\gtc\matrix_transform.hpp"
#include "C:\Users\cprj2748\Desktop\CG\Rotating-energy-rings-master\Assignment3\Project1\Project1\Dependencies\glm\gtc\type_ptr.hpp"
#include "C:\Users\cprj2748\Desktop\CG\Rotating-energy-rings-master\Assignment3\Project1\Project1\Dependencies\glm\gtx\norm.hpp"
#include "C:\Users\cprj2748\Desktop\CG\Rotating-energy-rings-master\Assignment3\Project1\Project1\Dependencies\glm\gtx\string_cast.hpp"


#include <iostream>
#include <fstream>
#include <vector>
#include "camera.h"


#include <string>


GLuint loadBMP_custom(const char * imagepath);
unsigned char* loadBMP_data(const GLchar* imagepath, int* width, int* height);
//unsigned int loadCubemap(vector<const GLchar*> faces);

using namespace std;
using glm::vec3;
using glm::mat4;

bool firstMouse = true;
int rotateCounter = 0;
Camera cam = Camera(glm::vec3(0.0f, 0.0f, 0.0f));

const int numObj = 7;
GLuint vao[numObj];
GLuint vbo[numObj];
GLuint uvbo[numObj];
GLuint nvbo[numObj];
GLint texture[numObj];
GLint textureEarth[2];
GLint textureWonder[2];
GLint light[numObj];
GLint greenTexture;
GLint textureRedSun;
const int amount = 400;
glm::mat4 modelMatrices[amount];
int drawSize[numObj];

float diffAdjust = 0.1;
float ambAdjust = 0.0;
glm::vec3 specAdjust = glm::vec3(0.2, 0.1, 0.1);

float specularStrength = 0.3;

float smoothCounter = 0;
bool redStarOn = false;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int WIDTH = 1920;
int HEIGHT = 1080;
int counter = 0;

//keyboard control
int cam_up = 0;

vec3 spaceCraftForward;

GLfloat deltaFrame = 5;

//mouse control
int lastX = 0;
int lastY = 0;
int xpos = 0;
int ypos = 0;
bool inRing0 = false;
bool inRing1 = false;
bool inRing2 = false;

bool collision = false;

GLint programID;
GLint skyboxProgramID;
// Could define the Vao&Vbo and interaction parameter here





//a series utilities for setting shader parameters 
void setMat4(const std::string &name, glm::mat4& value)
{
	unsigned int transformLoc = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void setVec4(const std::string &name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setVec3(const std::string &name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void setFloat(const std::string &name, float value)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void setInt(const std::string &name, int value)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	glUseProgram(programID);
}

void installSkyboxShaders()
{
	GLuint skyboxVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint skyboxFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("skyboxvert.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(skyboxVertexShaderID, 1, adapter, 0);
	temp = readShaderCode("skyboxfrag.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(skyboxFragmentShaderID, 1, adapter, 0);

	glCompileShader(skyboxVertexShaderID);
	glCompileShader(skyboxFragmentShaderID);

	if (!checkShaderStatus(skyboxVertexShaderID) || !checkShaderStatus(skyboxFragmentShaderID))
		return;

	skyboxProgramID = glCreateProgram();
	glAttachShader(skyboxProgramID, skyboxVertexShaderID);
	glAttachShader(skyboxProgramID, skyboxFragmentShaderID);
	glLinkProgram(skyboxProgramID);

	if (!checkProgramStatus(skyboxProgramID))
		return;

	glDeleteShader(skyboxVertexShaderID);
	glDeleteShader(skyboxFragmentShaderID);

	glUseProgram(skyboxProgramID);
}

void keyboard(unsigned char key, int x, int y)
{
	//TODO: Use keyboard to do interactive events and animation
	switch (key) {
	case '+':
		ambAdjust += 0.1;
		break;
	case '-':
		ambAdjust -= 0.1;
		break;
	case 'u':
		cam.Position.y++;
		break;
	case 'p':
		cam.Position.y--;
		break;
	case '2':
		specAdjust += glm::vec3(0.1, 0.05, 0.05);
		break;
	case '1':
		specAdjust -= glm::vec3(0.1, 0.05, 0.05);
	case '5':
		diffAdjust += 0.1;
		break;
	case '4':
		diffAdjust -= 0.1;
		break;
	case '9':
		redStarOn = true;
		break;
	case '6':
		redStarOn = false;
		break;
	}

}

void move(int key, int x, int y)
{
	//TODO: Use arrow keys to do interactive events and animation
	switch (key) {
	case GLUT_KEY_LEFT:
		//left_right_press_num -= 0.05;
		cam.ProcessKeyboard(LEFT, deltaFrame);
		cout << "left" << endl;

		break;
	case GLUT_KEY_RIGHT:
		//left_right_press_num += 0.05;
		cam.ProcessKeyboard(RIGHT, deltaFrame);
		cout << "right" << endl;
		break;
	case GLUT_KEY_DOWN:
		cam.ProcessKeyboard(BACKWARD, deltaFrame);
		cout << "down" << endl;
		break;
	case GLUT_KEY_UP:
		cam.ProcessKeyboard(FORWARD, deltaFrame);
		cout << "up" << endl;
		break;

	}
}

void PassiveMouse(int xpos, int ypos)
{
	//TODO: Use Mouse to do interactive events and animation
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	//lastY = ypos;

	cam.ProcessMouseMovement(xoffset, 0);

}

void CreateRand_ModelM() {

	//initialize random seed
	srand(glutGet(GLUT_ELAPSED_TIME));
	GLfloat radius = 18.0f;
	GLfloat offset = 3.4f;
	GLfloat displacement;
	for (GLuint i = 0; i < amount; i++) {
		glm::mat4 model;
		//1. Translation: Randomly displace along circle with radius 'radius' in range [-offset, offset]
		GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
		//x
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat x = sin(angle)*radius + displacement;
		//y
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat y = displacement * 0.4f + 1;
		//z
		displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
		GLfloat z = cos(angle)*radius + displacement;

		glm::vec3 tempCoor = glm::vec3(x, y, z);

		model = glm::translate(model, glm::vec3(x, y, z));
		//2.Scale Scale between 0.05 and 0.25f
		GLfloat scale = (rand() % 10) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));
		//3.Rotation: add random rotation around a semi randomly picked rotation axis vector
		GLfloat rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		//4. Now add to list of matrices
		modelMatrices[i] = model;
	}

}


bool loadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<glm::vec2> & out_uvs, std::vector<glm::vec3> & out_normals) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 6 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	return true;
}
GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = width * height * 3;
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);


	GLuint textureID;
	//TODO: Create one OpenGL texture and set the texture parameter 
	//clarification: for this part we simply copied from the tutorial notes

	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);



	// OpenGL has now copied the data. Free our own version
	delete[] data;
	return textureID;
}
unsigned int loadCubemap(vector<const GLchar*> faces)
{
	int* width = (int *)malloc(sizeof(int *));
	int* height = (int *)malloc(sizeof(int *));
	*width = 0;
	*height = 0;
	unsigned char* imagedata;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++) {

		imagedata = loadBMP_data(faces[i], width, height);

		int temp_width = *width; int temp_height = *height;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, temp_width, temp_height,
			0, GL_BGR, GL_UNSIGNED_BYTE, imagedata);

	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, -1);
	return textureID;
}
unsigned char* loadBMP_data(const GLchar* imagepath, int* width, int* height) {

	printf("Reading image %s\n", imagepath);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	//unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen((char*)imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);
	if (imageSize == 0)    imageSize = (*width) * (*height * 3);
	if (dataPos == 0)      dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, file);
	fclose(file);

	// OpenGL has now copied the data. Free our own version

	return data;
}


void sendDataToOpenGL()
{
	//TODO:
	//Load objects and bind to VAO & VBO
	//To create vao buffers and 3 vbo buffers for each object. 3 indicates we have 3 objects for now.
	glGenVertexArrays(numObj, vao);
	glGenBuffers(numObj, vbo);
	glGenBuffers(numObj, uvbo);
	glGenBuffers(numObj, nvbo);

	//##################  Zeroth Object : Wonderplanet0 #######################
	std::vector<glm::vec3> vertices0;
	std::vector<glm::vec2> uvs0;
	std::vector<glm::vec3> normals0;
	bool obj0 = loadOBJ("./planet.obj", vertices0, uvs0, normals0);
	texture[0] = loadBMP_custom("./sunmap.bmp"); //default plane texture
	textureRedSun = loadBMP_custom("./redgiant.bmp");
	glBindVertexArray(vao[0]);
	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices0.size() * sizeof(glm::vec3), &vertices0[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, uvs0.size() * sizeof(glm::vec2), &uvs0[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, normals0.size() * sizeof(glm::vec3), &normals0[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[0] = (int)vertices0.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[0]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[0]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);

	//##################  Finished Planet  #######################

	//##################  First Object : spaceCraft #######################
	std::vector<glm::vec3> vertices1;
	std::vector<glm::vec2> uvs1;
	std::vector<glm::vec3> normals1;
	bool obj1 = loadOBJ("./spaceCraft.obj", vertices1, uvs1, normals1);
	texture[1] = loadBMP_custom("./spacecraftTexture.bmp"); //default plane texture
	glBindVertexArray(vao[1]);
	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs1.size() * sizeof(glm::vec2), &uvs1[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, normals1.size() * sizeof(glm::vec3), &normals1[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[1] = (int)vertices1.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[1]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[1]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);

	//##################  Finished Plane  #######################

	//##################  Second Object : Earth #######################
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec2> uvs2;
	std::vector<glm::vec3> normals2;
	bool obj2 = loadOBJ("./planet.obj", vertices2, uvs2, normals2);
	textureEarth[0] = loadBMP_custom("./earthTexture.bmp"); //default plane texture
	textureEarth[1] = loadBMP_custom("./earth_normal.bmp");
	glBindVertexArray(vao[2]);
	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[2]);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[2]);
	glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(glm::vec3), &normals2[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[2] = (int)vertices2.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[2]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[2]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);

	//##################  Finished Plane  #######################
	//##################  Third Object : Wonderplanet1 #######################
	std::vector<glm::vec3> vertices3;
	std::vector<glm::vec2> uvs3;
	std::vector<glm::vec3> normals3;
	bool obj3 = loadOBJ("./planet.obj", vertices3, uvs3, normals3);
	textureWonder[0] = loadBMP_custom("./WonderStarTexture.bmp"); //default plane texture
	textureWonder[1] = loadBMP_custom("./brickwall_normal.bmp");
	glBindVertexArray(vao[3]);
	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(glm::vec3), &vertices3[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[3]);
	glBufferData(GL_ARRAY_BUFFER, uvs3.size() * sizeof(glm::vec2), &uvs3[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[3]);
	glBufferData(GL_ARRAY_BUFFER, normals3.size() * sizeof(glm::vec3), &normals3[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[3] = (int)vertices0.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[3]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[3]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);

	//##################  Fourth Object : Rock #######################
	std::vector<glm::vec3> vertices4;
	std::vector<glm::vec2> uvs4;
	std::vector<glm::vec3> normals4;
	bool obj4 = loadOBJ("./rock.obj", vertices4, uvs4, normals4);
	texture[4] = loadBMP_custom("./RockTexture.bmp"); 

	glBindVertexArray(vao[4]);
	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, vertices4.size() * sizeof(glm::vec3), &vertices4[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[4]);
	glBufferData(GL_ARRAY_BUFFER, uvs4.size() * sizeof(glm::vec2), &uvs4[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[4]);
	glBufferData(GL_ARRAY_BUFFER, normals4.size() * sizeof(glm::vec3), &normals4[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[4] = (int)vertices0.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[4]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[4]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);


	//##################  6th : Ring #######################
	std::vector<glm::vec3> vertices6;
	std::vector<glm::vec2> uvs6;
	std::vector<glm::vec3> normals6;
	bool obj6 = loadOBJ("./Ring.obj", vertices6, uvs6, normals6);
	texture[6] = loadBMP_custom("./ringTexture.bmp"); //default plane texture
	greenTexture = loadBMP_custom("./green.bmp");
	glBindVertexArray(vao[6]);
	//send vao of obj0 (plane) to openGL
	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, vertices6.size() * sizeof(glm::vec3), &vertices6[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo[6]);
	glBufferData(GL_ARRAY_BUFFER, uvs6.size() * sizeof(glm::vec2), &uvs6[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, nvbo[6]);
	glBufferData(GL_ARRAY_BUFFER, normals6.size() * sizeof(glm::vec3), &normals6[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glVertexAttribPointer(
		0, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	drawSize[6] = (int)vertices6.size();
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, uvbo[6]);
	glVertexAttribPointer(
		1, // attribute
		2, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, nvbo[6]);
	glVertexAttribPointer(
		2, // attribute
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	glEnableVertexAttribArray(2);

	glBindVertexArray(-1);

	//##################  Finished Ring  #######################




	//##################  Last Object : Skybox  #######################
	GLfloat skyboxVertices[] =
	{
		//left
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		//back
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		//front
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		//right
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		//bottom
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		//up
		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f
	};

	vector<const GLchar*> faces;
	faces.push_back("./skybox/purplenebula_rt.bmp");
	faces.push_back("./skybox/purplenebula_lf.bmp");
	faces.push_back("./skybox/purplenebula_up.bmp");
	faces.push_back("./skybox/purplenebula_dn.bmp");
	faces.push_back("./skybox/purplenebula_ft.bmp");
	faces.push_back("./skybox/purplenebula_bk.bmp");

	texture[5] = loadCubemap(faces);
	glBindVertexArray(vao[5]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(-1); //unbind 
						   //##################  Finished Skybox  #######################

						   //Load texture


}


void paintRing(bool *inRing, glm::vec3 position, glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint modelUniformLocation, GLuint viewUniformLocation, GLuint projectionUniformLocation, GLuint mvpUniformLocation) {
	//#################     Paint Ring   ###############################
	glBindVertexArray(vao[6]);
	GLfloat half = 3.1415926 / 2;

	glm::mat4 rotateHalf = glm::rotate(glm::mat4(1.0f), -half, glm::vec3(1, 0, 0));
	glm::mat4 seflRotate = glm::rotate(glm::mat4(1.0f), (float)(1.5*smoothCounter), glm::vec3(0, 1, 0));
	glm::mat4 modeltranslation6 = glm::mat4();
	modeltranslation6 = glm::translate(glm::mat4(), position);

	glm::mat4 scaleMatrix6;
	scaleMatrix6 = glm::scale(glm::mat4(1.0f), glm::vec3(0.06f));  // the last is scallin coefficience

	model = modeltranslation6  * seflRotate * rotateHalf * scaleMatrix6;

	//model = glm::rotate(model, (float)(smoothCounter), glm::vec3(1, 0, 0));
	glm::mat4 mvp6 = projection * view * model;

	glm::vec3 temp = cam.Position - position + glm::vec3(0.0f, -5.0f, 0.0f) + glm::vec3(10 * cam.Front.x, 10 * cam.Front.y, 10 * cam.Front.z);


	if (glm::length2(temp) > 50) {
		*inRing = false;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[6]);
		glUniform1i(glGetUniformLocation(programID, "texture0"), 0);
		cout << "Colorful!" << endl;
	}
	else {
		*inRing = true;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, greenTexture);
		glUniform1i(glGetUniformLocation(programID, "texture0"), 0);

	}
	
	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp6[0][0]);
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, drawSize[6]);
	glBindVertexArray(-1);
	glBindTexture(GL_TEXTURE_2D, -1);


	//####################finished earth #####################################
}


void paintGL(void)
{
	GLuint modelUniformLocation = glGetUniformLocation(programID, "model");
	GLuint viewUniformLocation = glGetUniformLocation(programID, "view");
	GLuint projectionUniformLocation = glGetUniformLocation(programID, "projection");
	GLuint mvpUniformLocation = glGetUniformLocation(programID, "MVP");



	if (rotateCounter % 100 == 0) {
		smoothCounter += 0.001;
	}
	glUseProgram(programID);
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures

	//different objects have different model matrix
	glm::mat4 model;   //for rotation
	glm::mat4 view;    //for translation
	glm::mat4 projection;


	projection = glm::perspective(cam.Zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 4000.0f);
	view = cam.GetViewMatrix();

	//********************* GIVE ME LIGHT! ********************************
	//eyePosition
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	vec3 eyePositionWorld = cam.Position;
	glUniform3fv(eyePositionUniformLocation, 1, &eyePositionWorld[0]);

	// ambientLight
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	float tempAmpAdjust = ambAdjust;
	//if (tempAmpAdjust <= 0) { tempAmpAdjust = 0.0; }
	vec3 ambientLight(tempAmpAdjust, tempAmpAdjust, tempAmpAdjust);  // RGB light of ambient light
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);

	// sunAmbient
	GLint sunAmbientLightUniformLocation = glGetUniformLocation(programID, "sunAmbient");
	glm::vec3 sunAmbientLight = glm::vec3(1, 1 ,1);  // RGB light of ambient light
	glUniform3fv(ambientLightUniformLocation, 1, &sunAmbientLight[0]);


	//single light source
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	vec3 lightPositionWorld = vec3(-600.0f, -10.0f, -150.0f);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPositionWorld[0]);

	//second light source
	GLint light2PositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld2");
	vec3 light2PositionWorld = vec3(500.0f, 10.0f, 350.0f);
	glUniform3fv(lightPositionUniformLocation, 1, &light2PositionWorld[0]);

	//diffuse
	GLint diffuseStrengthUniformLocation = glGetUniformLocation(programID, "diffuseStrength");
	if (diffAdjust <= 0) { diffAdjust = 0.0; }
	vec3 diffuseStrength(diffAdjust*1.2, diffAdjust, diffAdjust);  
	glUniform3fv(diffuseStrengthUniformLocation, 1, &diffuseStrength[0]);

	//pass specular strength to the shader
	GLint specularStrengthUniformLocation = glGetUniformLocation(programID, "specularStrength");
	//if (specAdjust.x <= 0) { specAdjust.x = 0.0; }
	//if (specAdjust.y <= 0) { specAdjust.y = 0.0; }
	//if (specAdjust.z <= 0) { specAdjust.z = 0.0; }
	vec3 specularStrength = specAdjust; // RGB light of ambient light
	glUniform3fv(specularStrengthUniformLocation, 1, &specularStrength[0]);

	//diffuse 2
	GLint diffuseStrength2UniformLocation = glGetUniformLocation(programID, "diffuseStrength2");
	if (diffAdjust <= 0) { diffAdjust = 0.0; }
	vec3 diffuseStrength2(diffAdjust*1.3, diffAdjust, diffAdjust);
	glUniform3fv(diffuseStrength2UniformLocation, 1, &diffuseStrength2[0]);

	//pass specular strength to the shader 2 
	GLint specularStrength2UniformLocation = glGetUniformLocation(programID, "specularStrength2");
	//if (specAdjust.x <= 0) { specAdjust.x = 0.0; }
	//if (specAdjust.y <= 0) { specAdjust.y = 0.0; }
	//if (specAdjust.z <= 0) { specAdjust.z = 0.0; }
	vec3 specularStrength2 = specAdjust; // RGB light of ambient light
	glUniform3fv(specularStrength2UniformLocation, 1, &specularStrength2[0]);


	//specular light

	//****************************FINISHED LIGHTING*******************************


	//#############some general settings####################
	glClearColor(0.1f, 0.1f, 0.13f, 0.9f); // set the background color

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//TODO:
	//Set lighting information, such as position and color of lighting source
	//Set transformation matrix
	//Bind different textures
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, WIDTH, HEIGHT);




	//****************PAINT FIRST OBJECT spaceCraft*************
	if (!collision) {
		glBindVertexArray(vao[1]);

		glm::mat4 modeltranslation1 = glm::mat4(1.0f);
		glm::mat4 modelTranslateRelativeToCamera = glm::mat4(1.0f);
		modelTranslateRelativeToCamera = glm::translate(glm::mat4(), cam.Position);
		GLfloat step = 10;
		modeltranslation1 = glm::translate(modelTranslateRelativeToCamera, glm::vec3(0.0f, -5.0f, 0.0f) + step * cam.Front);


		glm::mat4 scaleMatrix1;
		scaleMatrix1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.003f));  // the last is scallin coefficience

		glm::mat4 rotateMatrix1;
		rotateMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(-cam.Yaw + 90), vec3(0, 1, 0));

		model = modeltranslation1 * scaleMatrix1*rotateMatrix1;


		glm::mat4 mvp1 = projection * view * model;


		//load and bind texture

		if (inRing0 | inRing1 | inRing2) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, greenTexture);
			glUniform1i(glGetUniformLocation(programID, "texture0"), 0);

		}
		else if (!inRing0 & !inRing1 & !inRing2) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			glUniform1i(glGetUniformLocation(programID, "texture0"), 0);
		}




		glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp1[0][0]);
		glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, drawSize[1]);
		glBindVertexArray(-1);
		glBindTexture(GL_TEXTURE_2D, -1);
	}
	////////******************************************************

	//#################     Paint Sun   ###############################
	GLint sunFlagUniformLocation = glGetUniformLocation(programID, "sunFlag");
	bool sunFlag = true;
	glUniform1i(sunFlagUniformLocation, sunFlag);


	glBindVertexArray(vao[0]);
	glm::mat4 modeltranslation0 = glm::mat4(1.0f);

	glm::mat4 scaleMatrix0;
	if (redStarOn) {
		modeltranslation0 = glm::translate(glm::mat4(), glm::vec3(-600.0f, -10.0f, -50.0f));
		scaleMatrix0 = glm::scale(glm::mat4(1.0f), glm::vec3(100.3f));  // the last is scallin coefficience
					
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRedSun);
		glUniform1i(glGetUniformLocation(programID, "texture6"), 0);//load and bind texture
		
		GLint sunFlagUniformLocation2 = glGetUniformLocation(programID, "redSunFlag");
		glUniform1i(sunFlagUniformLocation2, redStarOn);
	
	}
	else {
		modeltranslation0 = glm::translate(glm::mat4(), glm::vec3(600.0f, 10.0f, 50.0f));
		scaleMatrix0 = glm::scale(glm::mat4(1.0f), glm::vec3(45.3f));  // the last is scallin coefficience
																	   //load and bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(glGetUniformLocation(programID, "texture6"), 0);
	
		GLint sunFlagUniformLocation2 = glGetUniformLocation(programID, "redSunFlag");
		glUniform1i(sunFlagUniformLocation2, redStarOn);

	}




	model = modeltranslation0*scaleMatrix0;


	glm::mat4 mvp0 = projection * view * model;





	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp0[0][0]);
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, drawSize[0]);
	glBindVertexArray(-1);
	glBindTexture(GL_TEXTURE_2D, -1);

	sunFlagUniformLocation = glGetUniformLocation(programID, "sunFlag");
	sunFlag = false;
	glUniform1i(sunFlagUniformLocation, sunFlag);

	//####################finished earth #####################################

	//#################     Paint earth   ###############################
	GLint normalFlagUniformLocation = glGetUniformLocation(programID, "normalMapping_flag");
	bool normalFlag = true;
	glUniform1i(normalFlagUniformLocation, normalFlag);



	glBindVertexArray(vao[2]);
	glm::mat4 modeltranslation2 = glm::mat4(1.0f);
	modeltranslation2 = glm::translate(glm::mat4(), glm::vec3(0.0f, -5.0f, -256.0f));

	glm::vec3 temp2 = cam.Position - glm::vec3(4.3*0.0f, -5.0f, -256.0f) + glm::vec3(0.0f, -5.0f, 0.0f) + glm::vec3(10 * cam.Front.x, 10 * cam.Front.y, 10 * cam.Front.z);

	if (glm::length2(temp2) < 450) { 
		cout << glm::length2(temp2) << endl;
		collision = true; }
	else {
		collision = false;
	}


	glm::mat4 scaleMatrix2;
	scaleMatrix2 = glm::scale(glm::mat4(1.0f), glm::vec3(4.3f));  // the last is scallin coefficience

	model = modeltranslation2 * scaleMatrix2;

	model = glm::rotate(model, (float)(smoothCounter), glm::vec3(0, 1, 0));
	glm::mat4 mvp2 = projection * view * model;


	//load and bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureEarth[0]);
	GLuint TextureID_0 = glGetUniformLocation(programID, "myTextureSampler_0");
	glUniform1i(TextureID_0, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureEarth[1]);
	GLuint TextureID_1 = glGetUniformLocation(programID, "myTextureSampler_1");
	glUniform1i(TextureID_1, 1);


	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp2[0][0]);
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, drawSize[2]);
	glBindVertexArray(-1);
	glBindTexture(GL_TEXTURE_2D, -1);

	normalFlagUniformLocation = glGetUniformLocation(programID, "normalMapping_flag");
	normalFlag = false;
	glUniform1i(normalFlagUniformLocation, normalFlag);

	//####################finished earth #####################################

	//#################     Paint Wonderstar [1]   ###############################
	normalFlagUniformLocation = glGetUniformLocation(programID, "normalMapping_flag");
	normalFlag = true;
	glUniform1i(normalFlagUniformLocation, normalFlag);

	glBindVertexArray(vao[3]);
	glm::mat4 modeltranslation3 = glm::mat4(1.0f);
	modeltranslation3 = glm::translate(glm::mat4(), glm::vec3(-40.0f, -5.0f, -120.0f));

	glm::mat4 scaleMatrix3;
	scaleMatrix3 = glm::scale(glm::mat4(1.0f), glm::vec3(1.8f));  // the last is scallin coefficience

	model =  modeltranslation3 * scaleMatrix3;
	model = glm::rotate(model, (float)(smoothCounter), glm::vec3(0, 1, 0));

	glm::mat4 mvp3 = projection * view * model;


	//load and bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureWonder[0]);
	GLuint TextureID_2 = glGetUniformLocation(programID, "myTextureSampler_0");
	glUniform1i(TextureID_2, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureWonder[1]);
	GLuint TextureID_3 = glGetUniformLocation(programID, "myTextureSampler_1");
	glUniform1i(TextureID_3, 1);


	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp3[0][0]);
	glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, drawSize[3]);
	glBindVertexArray(-1);
	glBindTexture(GL_TEXTURE_2D, -1);

	normalFlagUniformLocation = glGetUniformLocation(programID, "normalMapping_flag");
	normalFlag = false;
	glUniform1i(normalFlagUniformLocation, normalFlag);

	//#######################   Paint Rocks ############################
	glm::mat4 rockOrbitIni = glm::translate(glm::mat4(), glm::vec3(-40.0f, -5.0f, -120.0f));
	glm::mat4 rockOrbit_M = glm::rotate(rockOrbitIni, (float)(smoothCounter), glm::vec3(0, 1, 0));
	
	glm::mat4 scaleMatrix4;
	scaleMatrix4 = glm::scale(glm::mat4(1.0f), glm::vec3(1.8f));  // the last is scallin coefficience


	//load and bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glUniform1i(glGetUniformLocation(programID, "texture0"), 0);
	
	glm::mat4 rockModelMat_temp;
	for (GLuint i = 0; i < amount; i++) {
		rockModelMat_temp = modelMatrices[i];
		rockModelMat_temp = rockOrbit_M * rockModelMat_temp * scaleMatrix4;

		if (i == 0) {
			std::cout << glm::to_string(rockModelMat_temp) << std::endl;
		}


		glm::mat4 mvp4 = projection * view * rockModelMat_temp;

		glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp4[0][0]);
		glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);
		//Draw
		glBindVertexArray(vao[4]);
		glDrawArrays(GL_TRIANGLES, 0, drawSize[4]);
	}
	glBindVertexArray(-1);
	glBindTexture(GL_TEXTURE_2D, -1);


	//################     paint rings   ####################
	paintRing(&inRing0, glm::vec3(10.0f, -4.0f, -55.0f),model, view, projection, modelUniformLocation, viewUniformLocation, projectionUniformLocation, mvpUniformLocation);
	paintRing(&inRing1, glm::vec3(15.0f, -4.0f, -85.0f), model, view, projection, modelUniformLocation, viewUniformLocation, projectionUniformLocation, mvpUniformLocation);
	paintRing(&inRing2, glm::vec3(20.0f, -4.0f, -125.0f), model, view, projection, modelUniformLocation, viewUniformLocation, projectionUniformLocation, mvpUniformLocation);
	//#######################################################





	//#################     Paint SKYBOX   ###############################
	//glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);


	glUseProgram(skyboxProgramID);

	GLuint skb_ModelUniformLocation = glGetUniformLocation(skyboxProgramID, "M");
	glm::mat4 skb_modelMatrix = glm::mat4(1.0f);
	glm::mat4 scaleSkybox;
	scaleSkybox = glm::scale(glm::mat4(1.0f), glm::vec3(2000.0f));  // the last is scalling coefficience

	glm::mat4 modeltranslationNull;
	modeltranslationNull = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f));

	skb_modelMatrix = skb_modelMatrix * scaleSkybox * modeltranslationNull ;

	//remove any translation component of the view matrix
	view = glm::mat4(glm::mat3(view));
	glUniformMatrix4fv(skb_ModelUniformLocation, 1, GL_FALSE, &skb_modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgramID, "projection"), 1, GL_FALSE, &projection[0][0]);

	//skybox cube
	glBindVertexArray(vao[5]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture[5]);
	glUniform1i(glGetUniformLocation(skyboxProgramID, "skybox"), 0);


	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(-1);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	//#################FINISHED  SKYBOX###############################

	glUseProgram(programID);

	counter++;
	if (counter % 100 == 0) {
		cout << "camera front: " << cam.Front.x << "," << cam.Front.y << "," << cam.Front.z << endl;
	}

	glFlush();
	glutPostRedisplay();
}

void initializedGL(void) //run only once
{
	glewInit();
	installShaders();
	installSkyboxShaders();
	sendDataToOpenGL();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Course Project. openGL is so difficult");


	//TODO:
	/*Register different CALLBACK function for GLUT to response
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	initializedGL();
	CreateRand_ModelM();
	glutDisplayFunc(paintGL);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(move);
	glutPassiveMotionFunc(PassiveMouse);


	glutMainLoop();

	return 0;
}
