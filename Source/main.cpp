#include "../Externals/Include/Include.h"
//#include "../Include/Common.h"
#include<vector>
#include <math.h>
#include <time.h>

#define turn 0
#define Abstraction 1
#define pixelation 2
#define sin_wave 3
#define red_blue 4
#define bloom_effect 5
#define halftoning 6
#define cool1 7
#define cool2 8
#define MENU_EXIT 9
#define PI 3.1415926525

#define COLLISION_DETECTION_METHOD 2 //-1 for nothing, 0 for nested loop, 1 for grid, 2 for 1 axis sweep and prune, 3 for 3 axis sweep and prune

#define COLLISION_CHECK_INTERVAL_CHANGE 000 //In microseconds (or .000001 seconds)
#define FPS 0.016666666 //Frames per second, less calculation
unsigned int COLLISION_CHECK_INTERVAL = 5000;

using namespace glm;
using namespace std;

vector<vec3> positions;
vector<vec3> velocity;

int *collisions;

//Important variables
GLint numberOfObjects;

//Spatial algo, grid/cell based
#if COLLISION_DETECTION_METHOD == 1

Octree<unsigned int> tree(containerSize);

#endif

//1 Axis Sweep and Prune
#if COLLISION_DETECTION_METHOD == 2

float *xAxisSorted;
int *xAxisElement;

#endif

//3 Axis Sweep and Prune
#if COLLISION_DETECTION_METHOD == 3

float *xAxisSorted;
int *xAxisElement;

float *yAxisSorted;
int *yAxisElement;

float *zAxisSorted;
int *zAxisElement;

#endif

float diameter = 2; //Diameter of sphere

//GCD (Dispatch) Variables
//dispatch_queue_t collisionQueue;
//dispatch_queue_t drawQueue;

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;
int containerSize = 1024; //Size of the box that contains the spheres
float radius = 1.0000001;
static glm::vec3 gravity = glm::vec3(0, -9.81, 0); //Acceleration of velocity of each ball.
vector<int> objId;
void checkCollisions();
void collisionSweepAndPrune();

mat4 view;
mat4 projection;
mat4 model;

GLint um4p;
GLint um4mv;

GLuint program;
GLuint program2;
GLuint skybox_prog;
GLuint tex_envmap;
GLuint skybox_vao;

GLuint bar_on;
GLuint bar_value = 0;

GLuint state;
GLuint state_value = 0;

GLuint iscar;
GLuint car_value = 1;

GLfloat offset;
GLfloat offset_value;

GLfloat xadd=0.0,yadd=40.0,zadd=0.0;
GLfloat x_value, y_value, z_value;

GLuint	FBO;
GLuint	depthRBO;
GLuint	FBODataTexture;
GLuint  window_vao;
GLuint	window_buffer;

int prex, prey;
double pan = 0, tilt = 0;
vec3 first_offset(0.0f,0.0f,0.0f);
vec3 third_offset(0.0f, 0.0f, 0.0f);

vec3 camera_one_view;
bool camera_switch = true;
bool turn_right = false;
void new_Reshape(int width, int height);

vector<vec4>border {vec4(-100.f, 0.0f, -100.0f, -20.8f)};

static const GLfloat window_positions[] =
{
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};

struct
{
	struct
	{
		GLint view_matrix;
		GLint eye;
	} skybox;
} uniforms;

struct Camera
{
	vec3 position;
	vec3 ref;
	vec3 up_vector;
};

Camera camera_first, camera_third;
float front_back, left_right,up_down;
float prev_front_back,prev_left_right,prev_up_down;
float ref_front_back, ref_left_right, ref_up_down;
float z_add = 0.0;
bool flag = false;
bool sky_on = false;

unsigned int numofmesh;
vector<unsigned int>numofvertice;
char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

void freeShaderSource(char** srcp)
{
	delete[] srcp[0];
	delete[] srcp;
}

// define a simple data structure for storing texture image raw data
typedef struct _TextureData
{
	_TextureData(void) :
		width(0),
		height(0),
		data(0)
	{
	}

	int width;
	int height;
	unsigned char* data;
} TextureData;
struct Shape
{
	GLuint vao;
	GLuint vbo_position;
	GLuint vbo_normal;
	GLuint vbo_texcoord;
	GLuint ibo;
	int drawCount;
	int materialID;
};
struct Material
{
	GLuint diffuse_tex;
};

struct Model
{
	vec3 position = vec3(0, 0, 0);
	vec3 scale = vec3(1, 1, 1);
	vec3 rotation = vec3(0, 0.1, 0);	// Euler form
};
Model models;
vector<Shape>shapes;
vector<Shape>car_shapes;
vector<Shape>human_shapes;
vector<Shape>motor_shapes;

vector<Material>Materials;
vector<Material>car_Materials;
vector<Material>human_Materials;
vector<Material>motor_Materials;

vector<Shape>shape2;
vector<Material>Material2;
vec2 dis_;

vector<float> position[10000];
vector<float> texcoord[10000];
vector<float> normal[10000];
vector<unsigned int> indice[10000];
int materialID;
int drawcount;


void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete[] errorLog;
	}
}

// load a png image and return a TextureData structure with raw data
// not limited to png format. works with any image format that is RGBA-32bit
TextureData loadPNG(const char* const pngFilepath)
{
	TextureData texture;
	int components;

	// load the texture with stb image, force RGBA (4 components required)
	stbi_uc *data = stbi_load(pngFilepath, &texture.width, &texture.height, &components, 4);

	// is the image successfully loaded?
	if (data != NULL)
	{
		// copy the raw data
		size_t dataSize = texture.width * texture.height * 4 * sizeof(unsigned char);
		texture.data = new unsigned char[dataSize];
		memcpy(texture.data, data, dataSize);

		// mirror the image vertically to comply with OpenGL convention
		for (size_t i = 0; i < texture.width; ++i)
		{
			for (size_t j = 0; j < texture.height / 2; ++j)
			{
				for (size_t k = 0; k < 4; ++k)
				{
					size_t coord1 = (j * texture.width + i) * 4 + k;
					size_t coord2 = ((texture.height - j - 1) * texture.width + i) * 4 + k;
					std::swap(texture.data[coord1], texture.data[coord2]);
				}
			}
		}

		// release the loaded image
		stbi_image_free(data);
    }else{
        cout << "Load " << pngFilepath << " fails!\n";
    }

	return texture;
}





void My_LoadModels()
{
	const aiScene *scene = aiImportFile("The City.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL) {
		std::cout << "error scene load\n";
	}
	else
		std::cout << "load scene sucess\n";

	//Material material;
	aiString texturePath;
	//total = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial *material = scene->mMaterials[i];
		Material materials;
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{

			//char* filename = (char*)texturePath.C_Str();
			cout << "loading texture\n";
			TextureData tdata = loadPNG(texturePath.C_Str());
			cout << "loading sucess\n";
			cout << texturePath.C_Str() << "\n";
			glGenTextures(1, &materials.diffuse_tex);
			glBindTexture(GL_TEXTURE_2D, materials.diffuse_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		Materials.push_back(materials);

	}
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[i];
		Shape shape;
		glGenVertexArrays(1, &shape.vao);
		glBindVertexArray(shape.vao);
		// create 3 vbos to hold data

		vector<float>vertices;
		vector<float>texCoords;
		vector<float>normals;

		glGenBuffers(1, &shape.vbo_position);
		glGenBuffers(1, &shape.vbo_texcoord);
		glGenBuffers(1, &shape.vbo_normal);
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{

			vertices.push_back(mesh->mVertices[v].x);
			vertices.push_back(mesh->mVertices[v].y);
			vertices.push_back(mesh->mVertices[v].z);
			positions.push_back(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));
			objId.push_back(0);
			numberOfObjects++;
			texCoords.push_back(mesh->mTextureCoords[0][v].x);
			texCoords.push_back(mesh->mTextureCoords[0][v].y);

			normals.push_back(mesh->mNormals[v].x);
			normals.push_back(mesh->mNormals[v].y);
			normals.push_back(mesh->mNormals[v].z);

		}
		
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		// create 1 ibo to hold data

		vector<unsigned int>indices;
		glGenBuffers(1, &shape.ibo);
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{

			indices.push_back(mesh->mFaces[f].mIndices[0]);
			indices.push_back(mesh->mFaces[f].mIndices[1]);
			indices.push_back(mesh->mFaces[f].mIndices[2]);
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


		// glVertexAttribPointer / glEnableVertexArray calls…
		shape.materialID = mesh->mMaterialIndex;
		shape.drawCount = mesh->mNumFaces * 3;
		// save shape…
		
		shapes.push_back(shape);
	}
	cout << "number of myShapes = " << shapes.size() << '\n';

	aiReleaseImport(scene);
}
void motor_LoadModels()
{
	const aiScene *scene = aiImportFile("motorcycle.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL) {
		std::cout << "error scene load\n";
	}
	else
		std::cout << "load scene sucess\n";

	//Material material;
	aiString texturePath;
	//total = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial *material = scene->mMaterials[i];
		Material materials;
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{

			//char* filename = (char*)texturePath.C_Str();
			cout << "loading texture\n";
			TextureData tdata = loadPNG(texturePath.C_Str());
			cout << "loading sucess\n";
			cout << texturePath.C_Str() << "\n";
			glGenTextures(1, &materials.diffuse_tex);
			glBindTexture(GL_TEXTURE_2D, materials.diffuse_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		motor_Materials.push_back(materials);

	}
	numofmesh = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[i];
		Shape shape;
		glGenVertexArrays(1, &shape.vao);
		glBindVertexArray(shape.vao);
		// create 3 vbos to hold data

		vector<float>vertices;
		vector<float>texCoords;
		vector<float>normals;

		glGenBuffers(1, &shape.vbo_position);
		glGenBuffers(1, &shape.vbo_texcoord);
		glGenBuffers(1, &shape.vbo_normal);
		numofvertice.push_back(mesh->mNumVertices);
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			
			vertices.push_back(mesh->mVertices[v].x);
			vertices.push_back(mesh->mVertices[v].y);
			vertices.push_back(mesh->mVertices[v].z);
			positions.push_back(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));
			objId.push_back(1);
			numberOfObjects++;
			texCoords.push_back(mesh->mTextureCoords[0][v].x);
			texCoords.push_back(mesh->mTextureCoords[0][v].y);

			normals.push_back(mesh->mNormals[v].x);
			normals.push_back(mesh->mNormals[v].y);
			normals.push_back(mesh->mNormals[v].z);

			camera_one_view[0] += mesh->mVertices[v].x;
			camera_one_view[1] += mesh->mVertices[v].y;
			camera_one_view[2] += mesh->mVertices[v].x;
		}
		camera_one_view /= mesh->mNumVertices;
		position[i] = vertices;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);


		texcoord[i] = texCoords;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);


		normal[i] = normals;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		// create 1 ibo to hold data

		vector<unsigned int>indices;
		glGenBuffers(1, &shape.ibo);
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{

			indices.push_back(mesh->mFaces[f].mIndices[0]);
			indices.push_back(mesh->mFaces[f].mIndices[1]);
			indices.push_back(mesh->mFaces[f].mIndices[2]);
		}
		indice[i] = indices;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


		// glVertexAttribPointer / glEnableVertexArray calls…
		shape.materialID = mesh->mMaterialIndex;
		shape.drawCount = mesh->mNumFaces * 3;
		// save shape…
		materialID = shape.materialID;
		drawcount = shape.drawCount;
		motor_shapes.push_back(shape);
	}
	camera_one_view /= scene->mNumMeshes;
	cout << "number of myShapes = " << shapes.size() << '\n';

	aiReleaseImport(scene);
}

void car_LoadModels()
{
	const aiScene *scene = aiImportFile("PickUp.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL) {
		std::cout << "error scene load\n";
	}
	else
		std::cout << "load scene sucess\n";

	//Material material;
	aiString texturePath;
	//total = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial *material = scene->mMaterials[i];
		Material materials;
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{

			//char* filename = (char*)texturePath.C_Str();
			cout << "loading texture\n";
			TextureData tdata = loadPNG(texturePath.C_Str());
			cout << "loading sucess\n";
			cout << texturePath.C_Str() << "\n";
			glGenTextures(1, &materials.diffuse_tex);
			glBindTexture(GL_TEXTURE_2D, materials.diffuse_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		car_Materials.push_back(materials);

	}
	numofmesh = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[i];
		Shape shape;
		glGenVertexArrays(1, &shape.vao);
		glBindVertexArray(shape.vao);
		// create 3 vbos to hold data

		vector<float>vertices;
		vector<float>texCoords;
		vector<float>normals;

		glGenBuffers(1, &shape.vbo_position);
		glGenBuffers(1, &shape.vbo_texcoord);
		glGenBuffers(1, &shape.vbo_normal);
		numofvertice.push_back(mesh->mNumVertices);
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{

			vertices.push_back(mesh->mVertices[v].x);
			vertices.push_back(mesh->mVertices[v].y);
			vertices.push_back(mesh->mVertices[v].z);
			positions.push_back(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));
			objId.push_back(2);
			numberOfObjects++;
			texCoords.push_back(mesh->mTextureCoords[0][v].x);
			texCoords.push_back(mesh->mTextureCoords[0][v].y);

			normals.push_back(mesh->mNormals[v].x);
			normals.push_back(mesh->mNormals[v].y);
			normals.push_back(mesh->mNormals[v].z);

			camera_one_view[0] += mesh->mVertices[v].x;
			camera_one_view[1] += mesh->mVertices[v].y;
			camera_one_view[2] += mesh->mVertices[v].x;
		}
		camera_one_view /= mesh->mNumVertices;
		position[i] = vertices;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);


		texcoord[i] = texCoords;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);


		normal[i] = normals;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		// create 1 ibo to hold data

		vector<unsigned int>indices;
		glGenBuffers(1, &shape.ibo);
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{

			indices.push_back(mesh->mFaces[f].mIndices[0]);
			indices.push_back(mesh->mFaces[f].mIndices[1]);
			indices.push_back(mesh->mFaces[f].mIndices[2]);
		}
		indice[i] = indices;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


		// glVertexAttribPointer / glEnableVertexArray calls…
		shape.materialID = mesh->mMaterialIndex;
		shape.drawCount = mesh->mNumFaces * 3;
		// save shape…
		materialID = shape.materialID;
		drawcount = shape.drawCount;
		car_shapes.push_back(shape);
	}
	camera_one_view /= scene->mNumMeshes;
	cout << "number of myShapes = " << shapes.size() << '\n';

	aiReleaseImport(scene);
}

void human_LoadModels()
{
	const aiScene *scene = aiImportFile("Scout strooper.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene == NULL) {
		std::cout << "error scene load\n";
	}
	else
		std::cout << "load scene sucess\n";

	//Material material;
	aiString texturePath;
	//total = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial *material = scene->mMaterials[i];
		Material materials;
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
		{

			//char* filename = (char*)texturePath.C_Str();
			cout << "loading texture\n";
			TextureData tdata = loadPNG(texturePath.C_Str());
			cout << "loading sucess\n";
			cout << texturePath.C_Str() << "\n";
			glGenTextures(1, &materials.diffuse_tex);
			glBindTexture(GL_TEXTURE_2D, materials.diffuse_tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		human_Materials.push_back(materials);

	}
	numofmesh = scene->mNumMeshes;
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[i];
		Shape shape;
		glGenVertexArrays(1, &shape.vao);
		glBindVertexArray(shape.vao);
		// create 3 vbos to hold data

		vector<float>vertices;
		vector<float>texCoords;
		vector<float>normals;

		glGenBuffers(1, &shape.vbo_position);
		glGenBuffers(1, &shape.vbo_texcoord);
		glGenBuffers(1, &shape.vbo_normal);
		numofvertice.push_back(mesh->mNumVertices);
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{

			vertices.push_back(mesh->mVertices[v].x);
			vertices.push_back(mesh->mVertices[v].y);
			vertices.push_back(mesh->mVertices[v].z);
			positions.push_back(vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z));
			objId.push_back(3);
			numberOfObjects++;
			texCoords.push_back(mesh->mTextureCoords[0][v].x);
			texCoords.push_back(mesh->mTextureCoords[0][v].y);

			normals.push_back(mesh->mNormals[v].x);
			normals.push_back(mesh->mNormals[v].y);
			normals.push_back(mesh->mNormals[v].z);

			camera_one_view[0] += mesh->mVertices[v].x;
			camera_one_view[1] += mesh->mVertices[v].y;
			camera_one_view[2] += mesh->mVertices[v].x;
		}
		camera_one_view /= mesh->mNumVertices;
		position[i] = vertices;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);


		texcoord[i] = texCoords;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
		glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);


		normal[i] = normals;
		glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);


		// create 1 ibo to hold data

		vector<unsigned int>indices;
		glGenBuffers(1, &shape.ibo);
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{

			indices.push_back(mesh->mFaces[f].mIndices[0]);
			indices.push_back(mesh->mFaces[f].mIndices[1]);
			indices.push_back(mesh->mFaces[f].mIndices[2]);
		}
		indice[i] = indices;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


		// glVertexAttribPointer / glEnableVertexArray calls…
		shape.materialID = mesh->mMaterialIndex;
		shape.drawCount = mesh->mNumFaces * 3;
		// save shape…
		materialID = shape.materialID;
		drawcount = shape.drawCount;
		human_shapes.push_back(shape);
	}
	camera_one_view /= scene->mNumMeshes;
	cout << "number of myShapes = " << shapes.size() << '\n';

	aiReleaseImport(scene);
}

void load_skybox()
{
    //TextureData envmap_data = loadPNG("../Assets/mountaincube.png");
    vector<TextureData> Map;
    TextureData envap_data;
    vector<string> name;
    //met,miramar,moondust,sandstorm,ss
    string skyfile= "./sky/moondust";
    string skyname;
    skyname = skyfile + "_ft.tga";
    name.push_back(skyname);
    skyname = skyfile + "_bk.tga";
    name.push_back(skyname);
    skyname = skyfile + "_up.tga";
    name.push_back(skyname);
    skyname = skyfile + "_dn.tga";
    name.push_back(skyname);
    skyname = skyfile + "_lf.tga";
    name.push_back(skyname);
    skyname = skyfile + "_rt.tga";
    name.push_back(skyname);
    for(int i =0;i<6;++i){
        envap_data= loadPNG(name[i].c_str());
        cout << name[i].c_str() << endl;
        Map.push_back(envap_data);
    }
    
    glGenTextures(1, &tex_envmap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
    for(int i = 0; i < 6; ++i)
    {
        //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, envmap_data.width, envmap_data.height / 6, 0, GL_RGBA, GL_UNSIGNED_BYTE, envmap_data.data + i * (envmap_data.width * (envmap_data.height / 6) * sizeof(unsigned char) * 4));
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, Map[i].width, Map[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Map[i].data);
        delete[] Map[i].data;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //delete[] enmap_data.data;
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void My_Init()
{
	glClearColor(0.0f, 0.6f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	program = glCreateProgram();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	shaderLog(vertexShader);
	shaderLog(fragmentShader);
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	um4p = glGetUniformLocation(program, "um4p");
	um4mv = glGetUniformLocation(program, "um4mv");
	//bar_on = glGetUniformLocation(program, "bar_on");
	//state = glGetUniformLocation(program, "state");
	
    glUseProgram(program);
	front_back = 1000.0f;
	left_right = 500.0f;
	up_down = 1000.0f;

	ref_front_back = 0.0f;
	ref_left_right = 0.0f;
	ref_up_down = 0.0f;
	My_LoadModels();
	car_LoadModels();
	motor_LoadModels();
	human_LoadModels();
	#if COLLISION_DETECTION_METHOD == 2
    
		xAxisSorted = (float *)malloc(sizeof(float) * numberOfObjects);
		xAxisElement = (int *)malloc(sizeof(int) * numberOfObjects);
		
		for (int i=0;i<numberOfObjects;i++)
		{
			xAxisSorted[i] = positions[i].x;
			xAxisElement[i] = i;
		}
		
		insertionSort(xAxisSorted, xAxisElement, numberOfObjects);
		
		printArray(xAxisSorted, numberOfObjects);
		printA(xAxisElement, numberOfObjects);
    
	#endif
	//My_LoadModel2();
	//glGenFramebuffers(1, &FBO);
    
    //Load SkyBox
    skybox_prog = glCreateProgram();
    GLuint vertexShader1 = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    vertexShaderSource = loadShaderSource("skybox.vs.glsl");
    fragmentShaderSource = loadShaderSource("skybox.fs.glsl");
    glShaderSource(vertexShader1, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader1, 1, fragmentShaderSource, NULL);
    freeShaderSource(vertexShaderSource);
    freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader1);
    glCompileShader(fragmentShader1);
    shaderLog(vertexShader1);
    shaderLog(fragmentShader1);
    glAttachShader(skybox_prog, vertexShader1);
    glAttachShader(skybox_prog, fragmentShader1);
    glLinkProgram(skybox_prog);
    glUseProgram(skybox_prog);
    load_skybox();
    uniforms.skybox.view_matrix = glGetUniformLocation(skybox_prog, "view_matrix");
    glGenVertexArrays(1, &skybox_vao);
    
    
    program2 = glCreateProgram();
    GLuint vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    char** vertexShaderSource2 = loadShaderSource("frame_vertex.vs.glsl");
    char** fragmentShaderSource2 = loadShaderSource("frame_fragment.fs.glsl");
    glShaderSource(vertexShader2, 1, vertexShaderSource2, NULL);
    glShaderSource(fragmentShader2, 1, fragmentShaderSource2, NULL);
    freeShaderSource(vertexShaderSource2);
    freeShaderSource(fragmentShaderSource2);
    glCompileShader(vertexShader2);
    glCompileShader(fragmentShader2);
    shaderLog(vertexShader2);
    shaderLog(fragmentShader2);
    glAttachShader(program2, vertexShader2);
    glAttachShader(program2, fragmentShader2);
    glLinkProgram(program2);
    
    bar_on = glGetUniformLocation(program2, "bar_on");
    state = glGetUniformLocation(program2, "state");
    offset = glGetUniformLocation(program2, "offset");
    glGenVertexArrays(1, &window_vao);
    glBindVertexArray(window_vao);
    
    glGenBuffers(1, &window_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, window_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(window_positions), window_positions, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (const GLvoid*)(sizeof(GL_FLOAT) * 2));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glGenFramebuffers(1, &FBO);

	camera_third.position.z = front_back = -63.0f;
	camera_third.position.x = left_right = -15.0f;
	camera_third.position.y = up_down = 60.0f;
	
	camera_third.ref = vec3(100*cos(pan*PI/180), tilt, 100*sin(pan*PI/180));
	//camera_third.ref.x = ref_left_right = -20.0f;



	camera_first.position.z = 0.0f;
	camera_first.position.x = 0.0f;
	camera_first.position.y = 44.0f;
	

	camera_first.ref = vec3(100*cos(pan*PI/180), tilt, 100*sin(pan*PI/180));
	//camera_first.ref.x  = 0.0f;
	//camera_first.ref.y  = 44.0f;

	camera_first.up_vector = vec3(0.0f, 1.0f, 0.0f);
    
    new_Reshape(600, 600);
}

//mat4 mouse_rotate;
mat4 mouseview;
GLfloat right_rot = 2;
void My_Display()
{   
	    //printf("%f %f\n", left_right, ref_left_right);
	   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    
	    glUseProgram(program);

		//cout << "rotation=" << ' ' << right_rot << endl;
		if(camera_switch)
			mouseview = lookAt(camera_first.position+first_offset, camera_first.ref+first_offset, camera_first.up_vector);
		else
			mouseview = lookAt(camera_third.position, camera_third.ref, camera_third.up_vector);
		
		
		
		mat4 modelR = rotate(mat4(), (radians(right_rot)), models.rotation) ;
		modelR *= 2.0;
		//mat4 modelT = translate(mat4(1.0), vec3(0.0, 0.0, 1.5));
		mat4 model_y = translate(mat4(10.0), vec3(0.0, 39.0, -50.0));
		//mat4 modelT = translate(mat4(1.0), vec3(sin(radians(right_rot)), 0.0, cos(radians(right_rot))));

		mat4 modelS = scale(mat4(1.0), vec3(3.5, 3.5,3.5));
		mat4 modelT = translate(mat4(1.0),models.position);
		
		glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview));
		glUniformMatrix4fv(um4p, 1, GL_FALSE, value_ptr(projection));
		

		//ADD
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
		//glUseProgram(program);
		static const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		static const GLfloat one = 1.0f;
		glClearBufferfv(GL_COLOR, 0, white);
		glClearBufferfv(GL_DEPTH, 0, &one);
		//
        //Draw SkyBox
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex_envmap);
        mat4 view_matrix = mouseview;
        glUseProgram(skybox_prog);
        glBindVertexArray(skybox_vao);
        glUniformMatrix4fv(uniforms.skybox.view_matrix, 1, GL_FALSE, &view_matrix[0][0]);
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glEnable(GL_DEPTH_TEST);
    
        glUseProgram(program);
        glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview));
        glUniformMatrix4fv(um4p, 1, GL_FALSE, value_ptr(projection));
  
			
			for (int i = 0; i < shapes.size(); ++i)
			{
				glBindVertexArray(shapes[i].vao);
				int materialID = shapes[i].materialID;
				glBindTexture(GL_TEXTURE_2D, Materials[materialID].diffuse_tex);
				glActiveTexture(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Materials[materialID].diffuse_tex, 0);
				
			}
			
			glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT*modelR*modelS));
			glUniformMatrix4fv(um4p, 1, GL_FALSE, value_ptr(projection));
			for (int i = 0; i < motor_shapes.size(); ++i)
			{   
				
				glBindVertexArray(motor_shapes[i].vao);
				int materialID = motor_shapes[i].materialID;
				glBindTexture(GL_TEXTURE_2D, motor_Materials[materialID].diffuse_tex);
				glActiveTexture(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, motor_shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Materials[materialID].diffuse_tex, 0);

			}
			modelS = scale(mat4(1.0), vec3(12.0,12.0, 12.0));
			model_y = translate(mat4(10.0), vec3(0.0, 48.0, -50.0)); 
			mat4 modelr = rotate(mat4(), (radians(float(270.0))), vec3(10,0,0));
			glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT*modelR*modelr*modelS));
			for (int i = 0; i < human_shapes.size(); ++i)
			{

				glBindVertexArray(human_shapes[i].vao);
				int materialID = human_shapes[i].materialID;
				glBindTexture(GL_TEXTURE_2D, human_Materials[materialID].diffuse_tex);
				glActiveTexture(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, human_shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Materials[materialID].diffuse_tex, 0);

			}
			
			
			modelS = scale(mat4(1.0), vec3(2.0, 2.0, 2.0));
			model_y = translate(mat4(10.0), vec3(-10.0, 40.0, -50.0));
			if (flag == true)
			{
				float dis = sqrt(dis_.x*dis_.x + dis_.y*dis_.y);
				cout << "dis=" << ' ' << dis << endl;
				if (dis <= 2000.0)
				{
					dis_.y -= 1.0;
					modelT = translate(mat4(10.0), vec3(0.0, 0.0, dis_.y*0.1));
				}
				glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT*modelR*modelS));
			}
			    
			else
				glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelR*modelS));
			for (int i = 0; i < car_shapes.size(); ++i)
			{

				glBindVertexArray(car_shapes[i].vao);
				int materialID = car_shapes[i].materialID;
				glBindTexture(GL_TEXTURE_2D, car_Materials[materialID].diffuse_tex);
				glActiveTexture(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, car_shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Materials[materialID].diffuse_tex, 0);

			}
			//glUniform1f(y_value, zadd);
			//ADD
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, FBODataTexture);
			glBindVertexArray(window_vao);
			glUseProgram(program2);
			glUniform1i(bar_on, bar_value);
			glUniform1i(state, state_value);
			GLfloat move = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 2 * 3.14159 * .75;  // 3/4 of a wave cycle per second
			//printf("move=%d\n", move);
			glUniform1f(offset, move);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			
		glutSwapBuffers();
}


void new_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 10000.0f);
	if(camera_switch)
	    view = lookAt(camera_first.position, camera_first.ref,camera_first.up_vector);
	else
		view = lookAt(camera_third.position, camera_third.ref, camera_third.up_vector);
	glDeleteRenderbuffers(1, &depthRBO);
	glDeleteTextures(1, &FBODataTexture);
	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

	glGenTextures(1, &FBODataTexture);
	glBindTexture(GL_TEXTURE_2D, FBODataTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBODataTexture, 0);
}

void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(timer_speed, My_Timer, val);
}

bool first = false;

void My_MouseMotion(int x, int y) {

	pan -= 0.3*(x-prex);
	tilt += 0.3*(y-prey);

	camera_first.ref = vec3(100*cos(pan*PI/180), 39, 100*sin(pan*PI/180));
	prex = x;
	prey = y;
	printf("%lf, %lf\n", first_offset.x, first_offset.z);

}
void My_Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
		prex = x;
		prey = y;
	}
	else if (state == GLUT_UP)
	{
		printf("Mouse %d is released at (%d, %d)\n", button, x, y);
		prex = x;
		prey = y;
	}
}

vec3 cross(vec3 a, vec3 b)
{
    return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x * b.z, a.x*b.y - a.y * b.x );
}

void My_Keyboard(unsigned char key, int x, int y)
{
    checkCollisions();
    float speed = 1;
	vec3 first_goback = normalize(camera_first.ref - camera_first.position);
    vec3 first_goright = normalize(cross(first_goback,camera_first.up_vector));
    vec3 first_goup = normalize(cross(first_goback, first_goright));
    printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	vec3 tmp;
	double t;
	vec4 b = border[0];
	
	switch (key)
	{
	case 'w':
		tmp = first_offset;
		first_offset += first_goback *vec3(speed);
		first_offset.y = 0;
		printf("%f %f %f %f\n",b.x, b.y, b.z, b.w);
		t = b.x*b.x - b.x*b.z + b.z*first_offset.x - b.x*first_offset.x +
			b.y*b.y - b.y*b.w + b.w*first_offset.y - b.y*first_offset.y;
		if(	
			(b.x+t*(b.z-b.x)-first_offset.x)*(b.x+t*(b.z-b.x)-first_offset.x) + 
			(b.y+t*(b.w-b.y)-first_offset.z)*(b.y+t*(b.w-b.y)-first_offset.z) < 100)
			first_offset = tmp;
		
		cout << t << " " << (b.x+t*(b.z-b.x)-first_offset.x)*(b.x+t*(b.z-b.x)-first_offset.x) + 
		(b.y+t*(b.w-b.y)-first_offset.z)*(b.y+t*(b.w-b.y)-first_offset.z) << endl;
		//camera_first.position.x +=1.5;
		//camera_first.ref.x +=1.5;
		//camera_third.position.x +=1.5;
		//camera_third.ref.x +=1.5;
        break;
	case 's':
		first_offset -= first_goback * vec3(speed);
		first_offset.y = 0;
		//camera_first.position.x -= 1.5;
		//camera_first.ref.x -= 1.5;
		//camera_third.position.x -= 1.5;
		//camera_third.ref.x -= 1.5;
		break;
	case 'a':
		first_offset -= first_goright * vec3(speed);
		first_offset.y = 0;
		//camera_first.position.z -=1.5;
		//camera_first.ref.z -= 1.5;
		//camera_third.position.z -=1.5;
		//camera_third.ref.z -= 1.5;
		break;
	case 'd':
		first_offset += first_goright * vec3(speed);
		first_offset.y = 0;
		//camera_first.position.z += 1.5;
		//camera_first.ref.z += 1.5;
		//camera_third.position.z += 1.5;
		//camera_third.ref.z += 1.5;
		break;
	case 'z':
		first_offset += first_goup * vec3(speed);
		first_offset.y = 0;
		//camera_first.position.y -=1.5;
		//camera_first.ref.y -= 1.5;
		//camera_third.position.y -=1.5;
		//camera_third.ref.y -= 1.5;
		break;
	case 'x':
		first_offset -= first_goup * vec3(speed);
		first_offset.y = 0;
	    //camera_first.position.y += 1.5;
		//camera_first.ref.y += 1.5;
	    //camera_third.position.y += 1.5;
		//camera_third.ref.y += 1.5;
		break;
	
	case 't':
		if(camera_switch)
		{   
			
			right_rot = abs(mod(right_rot, float(360.0)));
			if (right_rot >= 0 && right_rot <= 30)
			{
				models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.45f;
				models.position.z -= (float)cos(mod(float(4.0), float(30.0))) * 0.75f;
			}
			if (right_rot > 30 && right_rot <= 60)
			{
				models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.45f;
				models.position.z -= (float)cos(mod(float(4.0), float(30.0))) * 0.75f;
			}
			else if(right_rot>60 && right_rot<=90)
			{
				models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.75f;
				models.position.z -= (float)cos(mod(float(4.0), float(30.0))) * 0.45f;
			}
			else if (right_rot>90 && right_rot <= 120)
			{
				models.position.x -= (float)sin(mod(float(5.0), float(30.0))) * 0.45f;
				models.position.z -= (float)cos(mod(float(5.0), float(30.0))) * 0.75f;
			}
			else if (right_rot >120 && right_rot <= 150)
			{
				models.position.x -= (float)sin(mod(float(6.0), float(30.0))) * 0.75f;
				models.position.z -= (float)cos(mod(float(6.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >150 && right_rot <= 190)
			{
				models.position.x -= (float)sin(mod(float(6.0), float(30.0))) * 0.45f;
				models.position.z -= (float)cos(mod(float(6.0), float(30.0))) * 0.75f;
			}
			else if (right_rot >190 && right_rot <= 210)
			{
				models.position.x -= (float)sin(mod(float(1.0), float(30.0))) * 0.30f;
				models.position.z -= (float)cos(mod(float(1.0), float(30.0))) * 0.75f;
			}
			else if (right_rot >210 && right_rot <= 240)
			{
				models.position.x -= (float)sin(mod(float(1.0), float(30.0))) * 0.45f;
				models.position.z -= (float)cos(mod(float(1.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >240 && right_rot <= 270)
			{
				models.position.x -= (float)sin(mod(float(1.0), float(30.0))) * 0.75f;
				models.position.z -= (float)cos(mod(float(1.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >270 && right_rot <= 300)
			{
				models.position.x -= (float)sin(mod(float(2.0), float(30.0))) * 0.75f;
				models.position.z -= (float)cos(mod(float(2.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >300 && right_rot <= 330)
			{
				models.position.x -= (float)sin(mod(float(2.0), float(30.0))) * 0.45f;
				models.position.z -= (float)cos(mod(float(2.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >330 && right_rot <= 360)
			{
				models.position.x -= (float)sin(mod(float(2.0), float(30.0))) * 0.45f;
				models.position.z -= (float)cos(mod(float(2.0), float(30.0))) * 0.75f;
			}
		}
		else 
		{   
			//camera_third.position.z += 0.8;
			camera_first.position.z += 1.0;
			//camera_third.ref.z += 0.8;
			camera_first.ref.z += 1.0;
		}
		zadd += 1.0;
		break;
	case 'g':
		
		if (camera_switch)
		{
			right_rot = abs(mod(right_rot, float(360.0)));
			if (right_rot >= 0 && right_rot <= 30)
			{
				models.position.x += (float)sin(mod(float(4.0), float(30.0))) * 0.45f;
				models.position.z += (float)cos(mod(float(4.0), float(30.0))) * 0.75f;
			}
			if (right_rot > 30 && right_rot <= 60)
			{
				models.position.x += (float)sin(mod(float(4.0), float(30.0))) * 0.45f;
				models.position.z += (float)cos(mod(float(4.0), float(30.0))) * 0.45f;
			}
			else if (right_rot>60 && right_rot <= 90)
			{
				models.position.x += (float)sin(mod(float(4.0), float(30.0))) * 0.75f;
				models.position.z += (float)cos(mod(float(4.0), float(30.0))) * 0.45f;
			}
			else if (right_rot>90 && right_rot <= 120)
			{
				models.position.x += (float)sin(mod(float(5.0), float(30.0))) * 0.45f;
				models.position.z += (float)cos(mod(float(5.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >120 && right_rot <= 150)
			{
				models.position.x += (float)sin(mod(float(6.0), float(30.0))) * 0.75f;
				models.position.z += (float)cos(mod(float(6.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >150 && right_rot <= 190)
			{
				models.position.x += (float)sin(mod(float(6.0), float(30.0))) * 0.45f;
				models.position.z += (float)cos(mod(float(6.0), float(30.0))) * 0.75f;
			}
			else if (right_rot >190 && right_rot <= 210)
			{
				models.position.x += (float)sin(mod(float(1.0), float(30.0))) * 0.30f;
				models.position.z += (float)cos(mod(float(1.0), float(30.0))) * 0.75f;
			}
			else if (right_rot >210 && right_rot <= 240)
			{
				models.position.x += (float)sin(mod(float(1.0), float(30.0))) * 0.45f;
				models.position.z += (float)cos(mod(float(1.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >240 && right_rot <= 270)
			{
				models.position.x += (float)sin(mod(float(1.0), float(30.0))) * 0.75f;
				models.position.z += (float)cos(mod(float(1.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >270 && right_rot <= 300)
			{
				models.position.x += (float)sin(mod(float(2.0), float(30.0))) * 0.75f;
				models.position.z += (float)cos(mod(float(2.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >300 && right_rot <= 330)
			{
				models.position.x += (float)sin(mod(float(2.0), float(30.0))) * 0.45f;
				models.position.z += (float)cos(mod(float(2.0), float(30.0))) * 0.45f;
			}
			else if (right_rot >330 && right_rot <= 360)
			{
				models.position.x += (float)sin(mod(float(2.0), float(30.0))) * 0.45f;
				models.position.z += (float)cos(mod(float(2.0), float(30.0))) * 0.75f;
			}
		}
		else
		{
			//camera_third.position.z -= 0.8;
			camera_first.position.z -= 1.0;
			//camera_third.ref.z -= 0.8;
			camera_first.ref.z -= 1.0;
		}
		zadd -= 1.0;
		break;
	case 'f':
		
		right_rot += 1;
		break;
	case 'h':
		
		right_rot -= 1;
		break;
	/*第一人稱*/
	case 'e':
		camera_switch = true;
		break;
	/*第三人稱*/
	case 'r':
		camera_switch = false;
		break;

	case 'o':
		if (flag==true)
			flag = false;
		else
			flag = true;
		break;
	}
	
	/*cout << "first position" <<' '<< camera_first.position.x << ' ' << camera_first.position.y << ' ' << camera_first.position.z << endl;
	cout << "first ref" << ' ' << camera_first.ref.x << ' ' << camera_first.ref.y << ' ' << camera_first.ref.z << endl;
	cout << "third position" << ' ' << camera_third.position.x << ' ' << camera_third.position.y << ' ' << camera_third.position.z << endl;
	cout << "third ref" << ' ' << camera_third.ref.x << ' ' << camera_third.ref.y << ' ' << camera_third.ref.z << endl;*/
	if (camera_switch)
		view = lookAt(camera_first.position, camera_first.ref, vec3(0.0f, 1.0f, 0.0f));
	else
		view = lookAt(camera_third.position, camera_third.ref, vec3(0.0f, 1.0f, 0.0f));
}

void My_SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id)
{
	switch (id)
	{
	case turn:
		if (bar_value == 0)
			bar_value = 1;
		else
			bar_value = 0;
	break;
	case Abstraction:
		state_value = 1;
	break;
	case pixelation:
		state_value = 4;
		break;
	case sin_wave:
		state_value = 5;
		break;
	case red_blue:
		state_value = 2;
		break;
	case bloom_effect:
		state_value = 6;
		break;
	case halftoning:
		state_value = 7;
		break;
	case cool1:
		state_value = 8;
		break;
	case cool2:
		state_value = 3;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("AS2_Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
    glewInit();
	glPrintContextInfo();
	My_Init();
	//new_Init();
	// Create a menu and bind it to mouse right button.
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Filter", menu_timer);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("turn", turn);
	glutAddMenuEntry("Abstraction", Abstraction);
	glutAddMenuEntry("pixelation", pixelation);
	glutAddMenuEntry("sin_wave", sin_wave);
	glutAddMenuEntry("red_blue", red_blue);
	glutAddMenuEntry("bloom_effect", bloom_effect);
	glutAddMenuEntry("halftoning", halftoning);
	glutAddMenuEntry("coolshader1", cool1);
	glutAddMenuEntry("coolshader2", cool2);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	//glutReshapeFunc(My_Reshape);
	glutReshapeFunc(new_Reshape);
	glutMouseFunc(My_Mouse);
	glutMotionFunc(My_MouseMotion);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}

#pragma mark Collisions

//Example right now, change return value later to a vector or smth, or can make another function

void collisionHappened(int objectOne, int objectTwo)
{
    printf("Collision Happened: %i and %i\n", objectOne, objectTwo);
    //Seperate into 3 different dimensions
    //fml
    //never mind you only swap the velocities cuz of the same mass
    /*if (collisions[objectOne] != objectTwo)
    {
        glm::vec3 posOne = positions[objectOne];
        glm::vec3 posTwo = positions[objectTwo];
        
        glm::vec3 velOne = velocity[objectOne];
        glm::vec3 velTwo = velocity[objectTwo];
        
        glm::vec3 newVelTwo = glm::normalize(posTwo - posOne) * glm::length(velTwo);
        glm::vec3 newVelOne = glm::normalize(posOne - posTwo) * glm::length(velOne);

        //velocity[objectOne] = velocity[objectTwo];
        //velocity[objectTwo] = velOne;
        
        velocity[objectOne] = newVelOne;
        velocity[objectTwo] = newVelTwo;
        
    }*/
}

void collided(int objectOne, int objectTwo)
{
    bool collided = false;
    
    glm::vec3 object1 = positions[objectOne];
    glm::vec3 object2 = positions[objectTwo];
    
    if (sqrt((object1.x-object2.x) * (object1.x-object2.x) + ((object1.y-object2.y)*(object1.y-object2.y) + (object1.z-object2.z)*(object1.z-object2.z))) < diameter)
        collided = true;
    else
        collided = false;
    
    if (collided == true)
        collisionHappened(objectOne, objectTwo);
}

//Easiest and shortest, but inefficient. Has runtime of O(n^2)


void collisionNestedLoop()
{
    for (int i=0;i<numberOfObjects;i++)
    {
        for (int j=i+1;j<numberOfObjects;j++)
            collided(i, j);
    }
}

#if COLLISION_DETECTION_METHOD == 1

void collisionOctree()
{
    for (int i=0;i<numberOfObjects;i++)
    {
        glm::vec3 pos = positions[i];
        
        
        printf("X: %f, Y: %f, Z: %f\n", pos.x, pos.y, pos.z);
        
        pos.x = round(pos.x);
        pos.y = round(pos.y);
        pos.z = round(pos.z);
        
        if (tree.at((int)pos.x, (int)pos.y, (int)pos.z)!=i)
        {
            tree.set(pos.x, pos.y, pos.z, i);
            tree.erase(pos.x, pos.y, pos.z);
        }
        
        
        //Loop through all the positions around it, 27 tests per sphere (including itself)
        for (int z=-1;z<=1;z++)
        {
            for (int y=-1;y<=1;y++)
            {
                for (int x=-1;x<=1;x++)
                {
                    //No need to check collision with itself, continue
                    if (x==0 && y==0 && z==0) continue;
                    //Checks that are out of bounds, continue
                    //Can probably improve efficiency by changing the condition in the loop
                    if (pos.x+x < 0 || pos.y+y < 0 || pos.z + z < 0 || pos.x+x >= containerSize || pos.y+y >= containerSize || pos.z+z >= containerSize) continue;
                    
                    int j = tree.at(pos.x+x, pos.y+y, pos.z+z);
                    
                    if (j != -1)
                    {
                        BOOL collide = collided(i, j);
                        if (collide == YES)
                            collisionHappened(i, j);
                    }
                }
            }
        }
    }
    
    
    /*
    int zSize = containerSize;
    int ySize = containerSize;
    int xSize = containerSize;
    
    for ( int z = 0; z < zSize; ++z ) {
        for ( int y = 0; y < ySize; ++y ) {
            for ( int x = 0; x < xSize; ++x ) {
                printf("X: %i Y: %i Z: %i - %i\n", x, y, z, tree.at(x,y,z));
            }
        }
    }
    */
    
}

//I give up on octree :'(
void collisionSpatialGrid()
{
    
}

#endif

#if COLLISION_DETECTION_METHOD == 2


//WHATEVER FUCK THIS CLOSE ENOUGH
void collisionSweepAndPrune()
{
    for (int i=0;i<numberOfObjects;i++)
        xAxisSorted[i] = positions[xAxisElement[i]].x;
    
    //lol forgot to update list
    insertionSort(xAxisSorted, xAxisElement, numberOfObjects);
    
    for (int i=0;i<numberOfObjects;i++)
    {
        int j = i+1;
        
        //SPHERE RADIUS IS 1 NOT 0.5 OH SHIT THAT'S WHY I SCREWED UP MY ALGORITHMS FML
        //Adds 1.001 cuz of float error
        while (xAxisSorted[i] > xAxisSorted[j]-diameter && j < numberOfObjects)
        {
			if(objId[i] == objId[j]) continue;
            collided(xAxisElement[i], xAxisElement[j]);
            j++;
        }
    }
}

#endif

#if COLLISION_DETECTION_METHOD == 3

void collision3AxisSweepAndPrune()
{
    
    for (int i=0;i<numberOfObjects;i++)
    {
        xAxisSorted[i] = positions[xAxisElement[i]].x;
        yAxisSorted[i] = positions[yAxisElement[i]].y;
        zAxisSorted[i] = positions[zAxisElement[i]].z;
    }
    
    
    insertionSort(xAxisSorted, xAxisElement, numberOfObjects);
    insertionSort(yAxisSorted, yAxisElement, numberOfObjects);
    insertionSort(zAxisSorted, zAxisElement, numberOfObjects);
    
    vector<string> xCollision, yCollision, zCollision;
    
    CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();
    for (int i=0;i<numberOfObjects;i++)
    {
        int j = i+1;
        int k = i+1;
        int l = i+1;
        
        while (xAxisSorted[i] > xAxisSorted[j]-diameter && j < numberOfObjects)
        {
            if (xAxisElement[i]<xAxisElement[j])
                xCollision.push_back(std::to_string(xAxisElement[i])+","+std::to_string(xAxisElement[j]));
            else
                xCollision.push_back(to_string(xAxisElement[j])+","+to_string(xAxisElement[i]));
            j++;
        }
        
        while (yAxisSorted[i] > yAxisSorted[k]-diameter && k<numberOfObjects)
        {
            if (yAxisElement[i]<yAxisElement[k])
                yCollision.push_back(std::to_string(yAxisElement[i])+","+std::to_string(yAxisElement[k]));
            else
                yCollision.push_back(to_string(yAxisElement[k])+","+to_string(yAxisElement[i]));
            k++;
        }
        
        while (zAxisSorted[i] > zAxisSorted[l]-diameter && l<numberOfObjects)
        {
            if (zAxisElement[i]<zAxisElement[l])
                zCollision.push_back(std::to_string(zAxisElement[i])+","+std::to_string(zAxisElement[l]));
            else
                zCollision.push_back(to_string(zAxisElement[l])+","+to_string(zAxisElement[i]));
            l++;
        }
    }
    
    /*
    for (int i=0;i<xCollision.size();i++)
    {
        printf("X: %s\n", xCollision[i].c_str());
    }
    
    for (int i=0;i<yCollision.size();i++)
    {
        printf("Y: %s\n", yCollision[i].c_str());
    }
    
    for (int i=0;i<zCollision.size();i++)
    {
        printf("Z: %s\n", zCollision[i].c_str());
    }
    
    */
    
    startTime = CFAbsoluteTimeGetCurrent();
    
    
    sort(xCollision.begin(), xCollision.end());
    sort(yCollision.begin(), yCollision.end());
    sort(zCollision.begin(), zCollision.end());

    printf("sort: %f\n", CFAbsoluteTimeGetCurrent() - startTime);
    
    vector<string>xyIntersection, xyzIntersection;
    set_intersection(xCollision.begin(), xCollision.end(), yCollision.begin(), yCollision.end(), back_inserter(xyIntersection));
    set_intersection(xyIntersection.begin(), xyIntersection.end(), zCollision.begin(), zCollision.end(), back_inserter(xyzIntersection));
    
    for (int i=0;i<xyzIntersection.size();i++)
    {
        string s = xyzIntersection[i];
        
        char_separator<char> sep(",");
        tokenizer<char_separator<char>> tokens(s, sep);
        
        int p[2];
        int q = 0;
        
        BOOST_FOREACH(string t, tokens)
        {
            p[q] = stoi(t);
            q++;
        }
        
        //Probably can switch this to collisionHappened(), but do that later
        //Can't use collisionHappened because it becomes a square, still needs narrowphase check
        //collisionHappened(xAxisElement[p[0]], xAxisElement[p[1]]);
        collided(xAxisElement[p[0]], xAxisElement[p[1]]);
        
        
    }

}

#endif

void checkCollisions() //Main Collision Function
{
    /*
     Problems:
     1. Keeps randomly increasing it's maximum height, presumably because of CPU lag, however suvat still doesn't solve it so
     2. Also slightly decreases it's maximum height
     */
    unsigned long posSize = numberOfObjects;
    
    time_t prev_seconds = time(0);
    time_t curr_seconds = time(0);
    double elapsed_seconds = curr_seconds - prev_seconds;
    
    prev_seconds = curr_seconds;
    
    glm::vec3 gravityFrame = gravity * (float)elapsed_seconds;
    //printf("Elapsed Seconds: %.10f\n", elapsed_seconds);
    //printf("Gravity Frame:  %.10f\n", gravityFrame.y);
    
    for (int i=0;i<numberOfObjects;i++)
    {
        //Temp velocity and position to check for next potential position
        //glm::vec3 tempVelocity = velocity[i]+gravityFrame;
        glm::vec3 tempPos = positions[i];
        
        //tempPos.y += tempVelocity.y*elapsed_seconds;
        
		/*
        //Flip velocity if it hits the bottom of the container
        if (tempPos.y<=0)
        {
            //Still slightly inaccurate, height slightly decreases each time
            
            //Too simple, gonna have to add a slightly more advanced algorithm
            //velocity[i].y*=-1;
            
            //Simple SUVAT to get velocity when it bounces back up
           
            float s = -positions[i].y;
            float u = velocity[i].y;
            float a = gravity.y;
            
            float v = sqrt(u*u + 2*a*s);
            float t = 2*s/(v-u);
            
            //Needs to add gravityFrame.y so the velocity doesn't keep dropping, don't know why..
            //velocity[i].y = v + a*timeRemain - gravityFrame.y;
            //velocity[i].y = v - a*t;
        }
        else //Else apply gravity
        {
            //velocity[i]+=gravityFrame;
        }
        */
        
        /*
        //Checks for wall collisions
        if (tempPos.x<0+radius) //Half of a radius
        {
			printf("collition\n");
            //velocity[i].x=abs(velocity[i].x);
        }
        else if (tempPos.x>containerSize-radius)
        {
			printf("collition\n");
            //velocity[i].x=abs(velocity[i].x)*-1;
        }
        
        if (tempPos.z<0+ radius)
        {
			printf("collition\n");
            //velocity[i].z=abs(velocity[i].z);
        }
        else if (tempPos.z>containerSize-1)
        {
			printf("collition\n");
            //velocity[i].z=abs(velocity[i].z)*-1;
        }*/
        
        
        //positions[i].y+=velocity[i].y*elapsed_seconds;
    	/*
        positions[i] += velocity[i] * (float)elapsed_seconds;
        
        for (int j=0;j<posSize/3;j++)
        {
            posVert[i*posSize + j*3] += velocity[i].x * elapsed_seconds;
            posVert[i*posSize + j*3+1]+=velocity[i].y * elapsed_seconds;
            posVert[i*posSize + j*3+2] += velocity[i].z * elapsed_seconds;
        }*/
    }
    
    //Check collision with each other object
    
#if COLLISION_DETECTION_METHOD == 0
    
    collisionNestedLoop();
    
#endif
    
#if COLLISION_DETECTION_METHOD == 1
    
    collisionOctree();
    
#endif
    
#if COLLISION_DETECTION_METHOD == 2
    
    collisionSweepAndPrune();
    
#endif
    
#if COLLISION_DETECTION_METHOD == 3
    
    collision3AxisSweepAndPrune();
    
#endif
    
    printf("Seconds: %f\n", elapsed_seconds);
    
    //printf("Position: %f\n", positions[10].y);
 //   printf("Velocity: %f\n", velocity[10].y);

}

//Octree implementation at http://nomis80.org/code/octree.html
//Just the data structure, but that's like 90% of the work
/*
#pragma mark GCD Functions

void setupGCD()
{
    collisionQueue = dispatch_queue_create("com.michael.collision.collisionqueue", NULL);
    drawQueue = dispatch_queue_create("com.michael.collision.drawqueue", NULL);
}

void runCollision()
{
    dispatch_async(collisionQueue, ^(void) {
        while(1)
        {
            checkCollisions();
            usleep(COLLISION_CHECK_INTERVAL);
        }
    });
}*/
