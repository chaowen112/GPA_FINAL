#include "../Externals/Include/Include.h"
//#include "../Include/Common.h"
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

GLfloat offset;
GLfloat offset_value;

GLuint	FBO;
GLuint	depthRBO;
GLuint	FBODataTexture;
GLuint  window_vao;
GLuint	window_buffer;

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
		GLint inv_vp_matrix;
		GLint eye;
	} skybox;
} uniforms;
float front_back, left_right,up_down;
float ref_front_back, ref_left_right, ref_up_down;
bool flag = false;
bool sky_on = false;
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


vector<Shape>shapes;
vector<Material>Materials;

vector<Shape>shape2;
vector<Material>Material2;


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
void My_LoadModel2()
{

	const aiScene *scene = aiImportFile("holodeck.obj", aiProcessPreset_TargetRealtime_MaxQuality);
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
		Material2.push_back(materials);

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

		shape2.push_back(shape);
	}
	cout << "number of myShapes = " << shapes.size() << '\n';

	aiReleaseImport(scene);
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
	bar_on = glGetUniformLocation(program, "bar_on");
	state = glGetUniformLocation(program, "state");
	
	glUseProgram(program);
	front_back = 0.0f;
	left_right = -10.0f;
	up_down = 5.0f;

	ref_front_back = 1.0f;
	ref_left_right = 1.0f;
	ref_up_down = 0.0f;
	My_LoadModels();
	//My_LoadModel2();
	//glGenFramebuffers(1, &FBO);

}

void new_Init()
{
	printf("%x", GL_DRAW_FRAMEBUFFER);
	printf("%x", GL_FRAMEBUFFER);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
	
	glUseProgram(program);

	My_LoadModels();

	
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

	new_Reshape(600, 600);
}

mat4 mouse_rotate;
void My_Display()
{   
	    //printf("%f %f\n", left_right, ref_left_right);
	   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glUseProgram(program);
		mat4 mouseview = view * mouse_rotate;
		
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
        
		if (flag == false)
		{
			for (int i = 0; i < shapes.size(); ++i)
			{   
				

				glBindVertexArray(shapes[i].vao);
				int materialID = shapes[i].materialID;
				glBindTexture(GL_TEXTURE_2D, Materials[materialID].diffuse_tex);
				glActiveTexture(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Materials[materialID].diffuse_tex, 0);
				
			}
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
			//	
			
			
			
		}
		/*if(flag==true)
		{
			for (int i = 0; i < shape2.size(); ++i)
			{
				glBindVertexArray(shape2[i].vao);
				int materialID = shape2[i].materialID;
				glBindTexture(GL_TEXTURE_2D, Material2[materialID].diffuse_tex);
				glActiveTexture(GL_TEXTURE0);
				glDrawElements(GL_TRIANGLES, shape2[i].drawCount, GL_UNSIGNED_INT, 0);
			}
		}*/
		
		glutSwapBuffers();
	
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 1000.0f);
	view = lookAt(vec3(left_right, up_down, front_back), vec3(100.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	//printf("front_back=%f width=%d height=%d\n", front_back,width,height);
	
}
void new_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	projection = perspective(radians(60.0f), viewportAspect, 0.1f, 1000.0f);
	view = lookAt(vec3(left_right, up_down, front_back), vec3(ref_left_right, ref_up_down, ref_front_back), vec3(0.0f, 1.0f, 0.0f));

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
int prex, prey;
void My_MouseMotion(int x, int y) {
	if (!first) {
		prex = x;
		prey = y;
		first = true;
	}
	mouse_rotate = rotate(mat4(), radians((GLfloat)(x - prex) / 3), vec3(0.0, 1.0, 0.0))*rotate(mat4(), radians((GLfloat)(y - prey) / 3), vec3(0.0, 0.0, 1.0));
}
void My_Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
	}
	else if (state == GLUT_UP)
	{
		printf("Mouse %d is released at (%d, %d)\n", button, x, y);
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		left_right += 0.5;
		ref_left_right += 0.5;

		break;
	case 's':
		left_right -= 0.5;
		ref_left_right -= 0.5;
		break;
	case 'a':
		front_back -= 0.5;
		ref_front_back -= 0.5;
		
		break;
	case 'd':
		front_back += 0.5;
		ref_front_back += 0.5;
		break;
	case 'z':
		up_down -= 1.0;
		ref_up_down -= 1.0;
		break;
	case 'x':
		up_down += 1.0;
		ref_up_down += 1.0;
		break;
	case 'r':
		if (flag == false)
		{
            flag = true;
			printf("true\n");
		}
			
		else
		{
			flag = false;
			printf("false\n");
		}
		break;
	case 'c':
		if (bar_value == 0)
			bar_value = 1;
		else
			bar_value = 0;
		printf("%d\n", bar_value);
		break;
	case 'b':
		state_value = 1;
		break;
	case 'e':
		state_value = 2;
		break;
	case 'l':
		state_value = 3;
		break;
	case 'p':
		state_value = 4;
		break;
	case 'v':
		state_value = 5;
		break;
	case 'm':
		state_value = 6;
		break;
	case 'h':
		state_value = 7;
		break;
	case 'o':
	    state_value = 8;
		break;
	
	}
	//printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	//printf("%f %f\n", front_back, left_right);
	view = lookAt(vec3(left_right,up_down,front_back), vec3(ref_left_right, ref_up_down, ref_front_back), vec3(0.0f, 1.0f, 0.0f));
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
	new_Init();
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
