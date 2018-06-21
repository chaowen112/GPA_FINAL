#include "../Externals/Include/Include.h"

#include<vector>
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

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std;
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


int MIN(int a, int b){return a <= b ? a : b;}
int MAX(int a, int b){return a >= b ? a : b;}

float speed = 1;

vector<vec3> border{
	vec3(-100.802498f, -2.116728f, 1.046820f),
	vec3(-219.607971, 64.164497, -39.848606),
	vec3(-94.637405f, -2.448642f, -23.922575f),
	vec3(-138.056793f, 92.501740f, -111.233368f),
	vec3(-94.246925, -2.382619, -111.414291),
	vec3(-135.605576, 82.893936, -138.866928),
	vec3(-135.659821f, -2.488350f, -138.741959f),
	vec3(-252.073196f, 82.959740f, -98.182549f),
	vec3(-261.516510f, -2.051233f, -133.106064f),
	vec3(-345.058472f, 141.929840f, -49.603565f),
	// 5
	vec3(-345.116180f, -2.158473f, -61.040806f),
	vec3(-427.042297f, 140.422089f, -143.101456f),
	vec3(-426.129791f, -1.838198f, -143.561768f),
	vec3(-572.380981f, 108.741440f, -126.294128f),
	vec3(-551.211304f, -2.662150f, -196.066177f),
	vec3(-615.122986f, 44.260509f, -253.173340f),
	vec3(-572.070923f, -2.065657f, -270.255096f),
	vec3(-626.803223f, 34.982220f, -294.126465f),
	vec3(-627.374878f, -2.418805f, -305.500610f),
	vec3(-579.402893, 34.628113, -340.857056),
	//10
	vec3(-574.463928f, -2.410752f, -348.523193f),
	vec3(-758.151306f, 96.127831f, -561.426514f),
	vec3(-219.651245f, -2.671136f, -0.105809f),
	vec3(-100.285927f, 64.838135f, -23.289110f),
	vec3(-252.011871f, -2.494905f, -114.609093f),
	vec3(-261.594940f, 64.881737f, -27.313866f),
	vec3(-534.650391f, -2.386607f, -199.184570f),
	vec3(-460.659637f, 108.356674f, -68.138252f),
	vec3(153.625381f, -3.386777f, -342.958801f),
	vec3(323.996155f, 126.446182f, -301.141693f),
	vec3(218.468735f, -2.420985f, 9.870408f),
	vec3(369.727325f, 98.209724f, -109.700356f),
	//15
	vec3(346.548157f, -3.073323f, -233.488510f),
	vec3(241.938080f, 158.337524f, -113.434631f),
	vec3(153.495743f, -2.913743f, -342.741333f),
	vec3(27.986168f, 126.443588f, -462.500153f),
	vec3(19.059742f, -2.661817f, -355.286407f),
	vec3(-164.519730f, 94.834663f, -460.278687f),
	vec3(-164.480789f, -5.168677f, -353.755951f),
	vec3(-283.788910f, 173.220871f, -443.861359f),
	vec3(-288.431793f, -3.328424f, -352.161743f),
	vec3(-254.093704f, 176.296234f, -463.087250f),
	//20
	//vec3(153.647186, -3.238716, -343.003296),
	//vec3(242.528900f, 126.781853f, -234.959625f),
	vec3(-329.992279f, -3.132721f, -393.011230f),
	vec3(-287.311279f, 161.474884f, -351.979736f),
	vec3(-329.764496f, -3.430237f, -393.701782f),
	vec3(-271.997742f, 160.013504f, -517.565369f),
	vec3(-331.954102f, -3.309258f, -516.749451f),
	vec3(-237.791962f, 138.941452f, -621.525146f),
	vec3(-204.132233f, -1.335818f, -590.753845f),
	vec3(-104.806885f, 244.528427f, -530.563660f),
	//25
	vec3(-71.821915f, -5.079959f, -612.181702f),
	vec3(49.097679f, 98.740616f, -461.777039f),
	vec3(51.893120, -3.106487, -589.223267),
	vec3(172.580597, 159.281021, -483.937683),
	vec3(173.036285f, -3.163212f, -587.479919f),
	vec3(345.955414f, 126.438858f, -602.946655f),
	vec3(173.036285f, -3.163212f, -587.479919f),
	vec3(281.304840f, 126.742302f, -677.245117f),
	vec3(281.608704, -3.226892, -677.247803),
	vec3(297.277802, 126.886528, -814.931213),
	//30
	vec3(-153.889343, -2.576421, -947.077637),
	vec3(-337.560974, 96.839188, -1159.866089),
	vec3(-332.044189, -2.704336, -1167.873169),
	vec3(-284.664520, 34.610046, -1202.874268),
	vec3(-285.440247, -2.630763, -1214.519043),
	vec3(-339.554199, 34.628582, -1237.984619),
	vec3(-342.613220, -2.374616, -1243.614380),
	vec3(-315.737244, 43.647366, -1324.361816),
	vec3(-377.362518, -2.410844, -1309.203369),
	vec3(-451.319153, 108.359085, -1440.309570),
	//35
	vec3(-484.996613, -2.367430, -1447.329224),
	vec3(-566.967712, 140.320206, -1365.197266),
	vec3(-650.160156, -2.495742, -1375.598022),
	vec3(-566.781921, 142.289780, -1459.151489)
	};

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
        cout << "Load Png Fail";
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

void capsule_LoadModels()
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
    string skyfile= "./sky/ss";
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



	camera_first.position.z = -2.0f;
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
		modelR *= 4.0;
		//mat4 modelT = translate(mat4(1.0), vec3(0.0, 0.0, 1.5));
		mat4 model_y = translate(mat4(10.0), vec3(0.0, 39.0, -50.0));
		//mat4 modelT = translate(mat4(1.0), vec3(sin(radians(right_rot)), 0.0, cos(radians(right_rot))));

		mat4 modelS = scale(mat4(1.0), vec3(3.5, 3.5,3.5));
		mat4 modelT = translate(mat4(1.0),models.position);
		modelT *= 2.0;
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
				mat4 modelT_1 = translate(mat4(10.0), vec3(0.0, 0.0, dis_.y*0.3));
				mat4 modelT_2 = translate(mat4(10.0), vec3(dis_.x*0.3, 0.0, 0.0));
				mat4 modelT_3 = translate(mat4(10.0), vec3(0.0 , 0.0, dis_.y*0.3+200));
				mat4 init_rotate = rotate(mat4(), (radians(float(180.0))), vec3(0.0, 10.0, 0.0));
				mat4 model_left_1 = rotate(mat4(), (radians(float(90.0))), vec3(0.0, 10.0, 0.0));
				mat4 model_right_1 = rotate(mat4(), (radians(float(270.0))), vec3(0.0, 10.0, 0.0));
				if (dis < 600.0)
				{
					dis_.y -= 2.5;
					
					glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT_1*init_rotate*modelS));
				}
				else if (dis >= 600.0 && dis <=690.0)
				{   
					dis_.y -= 2.0;
					model_left_1 = rotate(mat4(), (radians(float( dis -600.0))), vec3(0.0, 10.0, 0.0));
					modelT = translate(mat4(10.0), vec3(0.0, 0.0, 10.0));
					glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT_1*model_left_1*init_rotate*modelS));
				}
				else if (dis >= 690.0 && dis <= 1600.0)
				{   
					dis_.x -= 2.5;
					glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT_2*modelT_1*model_left_1*init_rotate*modelS));
				}
				else if (dis >= 1600.0 && dis <= 1690.0)
				{
					dis_.x -= 2.0;
					model_right_1 = rotate(mat4(), (radians(float( 1600.0-dis))), vec3(0.0, 10.0, 0.0));
					glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT_2*modelT_1*model_right_1*model_left_1*init_rotate*modelS));
				}
				else if (dis >= 1690.0 && dis <= 3000.0)
				{
					dis_.y -= 2.5;
					
					glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelT_3*modelT_2*modelT_1*model_right_1*model_left_1*init_rotate*modelS));
				}

			}
			    
			else
				glUniformMatrix4fv(um4mv, 1, GL_FALSE, value_ptr(mouseview*model_y*modelS));
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

	camera_first.ref = vec3(100*cos(pan*PI/180), tilt, 100*sin(pan*PI/180));
	prex = x;
	prey = y;
	printf("%lf, %lf, %lf, %lf, %lf\n", camera_first.ref.x, camera_first.ref.y, camera_first.ref.z, pan, tilt);

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
    float speed = 5;
	vec3 first_goback = normalize(camera_first.ref - camera_first.position);
    vec3 first_goright = normalize(cross(first_goback,camera_first.up_vector));
    vec3 first_goup = normalize(cross(first_goback, first_goright));
    printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	
	switch (key)
	{
	case 'w':
		first_offset += first_goback *vec3(speed);
		//camera_first.position.x +=1.5;
		//camera_first.ref.x +=1.5;
		//camera_third.position.x +=1.5;
		//camera_third.ref.x +=1.5;
		printf("%d\n", camera_third.position.x);
        break;
	case 's':
		first_offset -= first_goback * vec3(speed);
		//camera_first.position.x -= 1.5;
		//camera_first.ref.x -= 1.5;
		//camera_third.position.x -= 1.5;
		//camera_third.ref.x -= 1.5;
		break;
	case 'a':
		first_offset -= first_goright * vec3(speed);
		//camera_first.position.z -=1.5;
		//camera_first.ref.z -= 1.5;
		//camera_third.position.z -=1.5;
		//camera_third.ref.z -= 1.5;
		break;
	case 'd':
		first_offset += first_goright * vec3(speed);
		//camera_first.position.z += 1.5;
		//camera_first.ref.z += 1.5;
		//camera_third.position.z += 1.5;
		//camera_third.ref.z += 1.5;
		break;
	case 'z':
		first_offset += first_goup * vec3(speed);
		//camera_first.position.y -=1.5;
		//camera_first.ref.y -= 1.5;
		//camera_third.position.y -=1.5;
		//camera_third.ref.y -= 1.5;
		break;
	case 'x':
		first_offset -= first_goup * vec3(speed);
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
				models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.25f;
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
				models.position.x -= (float)sin(mod(float(2.0), float(30.0))) * 0.25f;
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
				models.position.x += (float)sin(mod(float(4.0), float(30.0))) * 0.25f;
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
				models.position.x += (float)sin(mod(float(2.0), float(30.0))) * 0.25f;
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
		if (right_rot >= 0 && right_rot <= 30)
		{
			models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.25f;
			models.position.z -= (float)cos(mod(float(4.0), float(30.0))) * 0.75f;
		}
		if (right_rot > 30 && right_rot <= 60)
		{
			models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.45f;
			models.position.z -= (float)cos(mod(float(4.0), float(30.0))) * 0.75f;
		}
		else if (right_rot>60 && right_rot <= 90)
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
			models.position.x -= (float)sin(mod(float(2.0), float(30.0))) * 0.25f;
			models.position.z -= (float)cos(mod(float(2.0), float(30.0))) * 0.75f;
		}
		right_rot += 1;
		break;
	case 'h':
		if (right_rot >= 0 && right_rot <= 30)
		{
			models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.25f;
			models.position.z -= (float)cos(mod(float(4.0), float(30.0))) * 0.75f;
		}
		if (right_rot > 30 && right_rot <= 60)
		{
			models.position.x -= (float)sin(mod(float(4.0), float(30.0))) * 0.45f;
			models.position.z -= (float)cos(mod(float(4.0), float(30.0))) * 0.75f;
		}
		else if (right_rot>60 && right_rot <= 90)
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
			models.position.x -= (float)sin(mod(float(2.0), float(30.0))) * 0.25f;
			models.position.z -= (float)cos(mod(float(2.0), float(30.0))) * 0.75f;
		}
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
