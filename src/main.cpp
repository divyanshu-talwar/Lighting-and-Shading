#include <cstdio>
#include <iostream>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#endif
#include <algorithm>


#include "shader_utils.h"
#include "gl_utils.h"

#define  GLM_FORCE_RADIANS
#define  GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <limits.h>

#define GOURAUD 0
#define PHONG 1
#define DEPTH 2
//Globals
GLuint program, program_light;

GLint vVertex_attrib, vColor_attrib, vNormal_attrib;
GLint vModel_uniform, vView_uniform, vProjection_uniform;

GLint vVertex_attrib_light, vColor_attrib_light;
GLint vModel_uniform_light, vView_uniform_light, vProjection_uniform_light;

int screen_width = 800, screen_height = 600;
double oldX, oldY, currentX, currentY;
bool isDragging=false;

GLuint cube_VAO, sphere_VAO; //Vertex array object for cube

glm::mat4 modelT, viewT, projectionT;//The model, view and projection transformations

GLuint light_VAO; //Vertex array for light source
glm::mat4 modelT_light;

GLuint lightSourceColor, lightSourcePos; //Uniform locations for light data

GLuint cameraPosLocation;	

glm::vec3 LightPos(20.0f, -10.0f, 30.0f);
glm::vec3 cameraPos(60.0f, 60.0f, 50.0f);

GLuint num_latitudes = 64;
GLuint num_longitudes = 64;


// SET SHADING TYPE HERE
int shading_type =  PHONG; //GOURAUD / PHONG / DEPTH

GLFWwindow * setupGlfw(int width, int height, const std::string &title);



void setupModelTransformation()
{
	//Modelling transformations (Model -> World coordinates)
	modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

	//Pass on the modelling matrix to the vertex shader
	glUseProgram(program);
	vModel_uniform = glGetUniformLocation(program, "vModel");
	if(vModel_uniform == -1){
		fprintf(stderr, "Could not bind location: vModel\n");
		exit(0);
	}
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

	//FOR LIGHT SOURCE
	modelT_light = glm::translate(glm::mat4(1.0f), LightPos);
	//Pass on the modelling matrix to the vertex shader
	glUseProgram(program_light);
	vModel_uniform_light = glGetUniformLocation(program_light, "vModel");
	if(vModel_uniform_light == -1){
		fprintf(stderr, "Could not bind location: vModel\n");
		exit(0);
	}
	glUniformMatrix4fv(vModel_uniform_light, 1, GL_FALSE, glm::value_ptr(modelT_light));
}

void setupViewTransformation()
{
	//Viewing transformations (World -> Camera coordinates
	if(shading_type == DEPTH){
		viewT = glm::lookAt(glm::vec3(0.0, 0.0, -150.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	}
	else{
		viewT = glm::lookAt(glm::vec3(40.0, -40.0, 40.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	}
	// for depth
	//Camera at (0, 0, 20) looking down the negative Z-axis in a right handed coordinate system

	//Pass on the viewing matrix to the vertex shader
	glUseProgram(program);
	vView_uniform = glGetUniformLocation(program, "vView");
	if(vView_uniform == -1){
		fprintf(stderr, "Could not bind location: vView\n");
		exit(0);
	}
	glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));

	//FOR LIGHT SOURCE
	glUseProgram(program_light);
	vView_uniform_light = glGetUniformLocation(program_light, "vView");
	if(vView_uniform_light == -1){
		fprintf(stderr, "Could not bind location: vView\n");
		exit(0);
	}
	glUniformMatrix4fv(vView_uniform_light, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation()
{
	//Projection transformation (Orthographic projection)
	float aspect = (float)screen_width/(float)screen_height;
	float view_height = 100.0f;
	//glm::mat4 projection = glm::ortho(-view_height*aspect/2.0f, view_height*aspect/2.0f, -view_height/2.0f, view_height/2.0f, 0.1f, 1000.0f);
	glm::mat4 projectionT = glm::perspective(45.0f, aspect, 0.1f, 1000.0f);

	//Pass on the projection matrix to the vertex shader
	glUseProgram(program);
	vProjection_uniform = glGetUniformLocation(program, "vProjection");
	if(vProjection_uniform == -1){
		fprintf(stderr, "Could not bind location: vProjection\n");
		exit(0);
	}
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));

	// FOR LIGHT SOURCE
	glUseProgram(program_light);
	vProjection_uniform_light = glGetUniformLocation(program_light, "vProjection");
	if(vProjection_uniform_light == -1){
		fprintf(stderr, "Could not bind location: vProjection\n");
		exit(0);
	}
	glUniformMatrix4fv(vProjection_uniform_light, 1, GL_FALSE, glm::value_ptr(projectionT));
}

void setupLightSource(){
	glUseProgram(program);
	lightSourcePos = glGetUniformLocation(program, "lightSourcePos");
	lightSourceColor = glGetUniformLocation(program, "lightSourceColor");
	cameraPosLocation = glGetUniformLocation(program, "cameraPos");
}

void createCubeObject()
{
	glUseProgram(program);

	//Bind shader variables
	vVertex_attrib = glGetAttribLocation(program, "vVertex");
	if(vVertex_attrib == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}
	vColor_attrib = glGetAttribLocation(program, "vColor");
	if(vColor_attrib == -1) {
		fprintf(stderr, "Could not bind location: vColor\n");
		exit(0);
	}

	//Cube data
	GLfloat cube_vertices[] = {10, 10, 10, -10, 10, 10, -10, -10, 10, 10, -10, 10, //Front
							   10, 10, -10, -10, 10, -10, -10, -10, -10, 10, -10, -10}; //Back
	GLushort cube_indices[] = {0, 2, 3, 0, 1, 2, //Front
							   4, 7, 6, 4, 6, 5, //Back
							   5, 2, 1, 5, 6, 2, //Left
							   4, 3, 7, 4, 0, 3, //Right
							   1, 0, 4, 1, 4, 5, //Top
							   2, 7, 3, 2, 6, 7}; //Bottom
	GLfloat cube_colors[] = {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1}; //Unique face colors

	//Generate VAO object
	glGenVertexArrays(1, &cube_VAO);
	glBindVertexArray(cube_VAO);

	//Create VBOs for the VAO
	//Position information (data + format)
	int nVertices = 6*2*3; //(6 faces) * (2 triangles each) * (3 vertices each)
	GLfloat *expanded_vertices = new GLfloat[nVertices*3];
	for(int i=0; i<nVertices; i++) {
		expanded_vertices[i*3] = cube_vertices[cube_indices[i]*3];
		expanded_vertices[i*3 + 1] = cube_vertices[cube_indices[i]*3+1];
		expanded_vertices[i*3 + 2] = cube_vertices[cube_indices[i]*3+2];
	}
	GLuint vertex_VBO;
	glGenBuffers(1, &vertex_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vVertex_attrib);
	glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	delete []expanded_vertices;

	//Color - one for each face
	GLfloat *expanded_colors = new GLfloat[nVertices*3];
	for(int i=0; i<nVertices; i++) {
		int color_index = i / 6;
		expanded_colors[i*3] = cube_colors[color_index*3];
		expanded_colors[i*3+1] = cube_colors[color_index*3+1];
		expanded_colors[i*3+2] = cube_colors[color_index*3+2];
	}
	GLuint color_VBO;
	glGenBuffers(1, &color_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vColor_attrib);
	glVertexAttribPointer(vColor_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	delete []expanded_colors;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0); //Unbind the VAO to disable changes outside this function.
}

void createLightSource(){
	glUseProgram(program_light);

	//Bind shader variables
	vVertex_attrib_light = glGetAttribLocation(program_light, "vVertex");
	if(vVertex_attrib_light == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}
	vColor_attrib_light = glGetAttribLocation(program_light, "vColor");
	if(vColor_attrib_light == -1) {
		fprintf(stderr, "Could not bind location: vColor\n");
		exit(0);
	}

	GLfloat vertex[] = {0, 0, 0};

	GLfloat color[] = {0, 0.6, 0.2};

	int nVertices = 1; 

	glGenVertexArrays(1, &light_VAO);
	glBindVertexArray(light_VAO);

	GLuint vertex_VBO, color_VBO;
	glGenBuffers(1, &vertex_VBO);
	glGenBuffers(1, &color_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), vertex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vVertex_attrib_light);
	glVertexAttribPointer(vVertex_attrib_light, 3, GL_FLOAT,GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), color, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vColor_attrib_light);
	glVertexAttribPointer(vColor_attrib_light, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void createSphereObject(){

	
	glUseProgram(program);
	//Bind shader variables
	vVertex_attrib = glGetAttribLocation(program, "vVertex");
	if(vVertex_attrib == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}
	vColor_attrib = glGetAttribLocation(program, "vColor");
	if(vColor_attrib == -1 && !shading_type) {
		fprintf(stderr, "Could not bind location: vColor\n");
		exit(0);
	}
	vNormal_attrib = glGetAttribLocation(program, "vNormal");
	if(vNormal_attrib == 1){
		fprintf(stderr, "Could not bind location: vNormal\n");
		exit(0);
	}

	GLfloat radius = 16.0f;
	// for depth
	if(shading_type == DEPTH){
		radius = 70.0f;
	}
	GLuint num_lats = num_latitudes;
	GLuint num_longs = num_longitudes;

	GLfloat lat_step = M_PI * (1.0f/(num_lats-  1));
	GLfloat long_step = 2 * M_PI * (1.0f/(num_longs - 1));

	GLuint long_pos = 0; //goes from 0 to 360
	GLuint lat_pos = 0; //goes from 0 to 180

	GLfloat project_radius = 0.0f;

	GLuint nVertices = num_lats * num_longs;
	GLfloat *expanded_vertices = new GLfloat[nVertices * 3];
	GLfloat *expanded_colors = new GLfloat[nVertices * 3];
	GLfloat *normals = new GLfloat[nVertices*3];
	GLuint *indices = new GLuint[nVertices*6];

	int i = 0;

	for(lat_pos = 0; lat_pos < num_lats; lat_pos++){
		project_radius = radius * sin(lat_pos * lat_step);
		for(long_pos = 0; long_pos < num_longs; long_pos++){
			expanded_vertices[i * 3 + 0] = project_radius * cos(long_pos * long_step);
			expanded_vertices[i * 3 + 1] = project_radius * sin(long_pos * long_step);
			expanded_vertices[i * 3 + 2] = radius * cos(lat_pos * lat_step);

			glm::vec3 vertex_vector(expanded_vertices[i*3 + 0], expanded_vertices[i*3 + 1], expanded_vertices[i*3 + 2]);
			glm::vec3 normal_vector = glm::normalize(vertex_vector);

			normals[i * 3 + 0] = normal_vector.x;
			normals[i * 3 + 1] = normal_vector.y;
			normals[i * 3 + 2] = normal_vector.z;

			i++;
		}
	}

	i = 0;
	for(lat_pos = 0; lat_pos < num_lats - 1; lat_pos++){
		for(long_pos = 0; long_pos < num_longs - 1; long_pos++){
			indices[i * 3 + 0] = lat_pos * num_longs + long_pos;
			indices[i * 3 + 1] = lat_pos * num_longs + (long_pos  + 1);
			indices[i * 3 + 2] = (lat_pos + 1) * num_longs + (long_pos + 1);

			indices[(i+1) * 3 + 0] = (lat_pos + 1) * num_longs + (long_pos + 1);
			indices[(i+1) * 3 + 1] = (lat_pos + 1) * num_longs + long_pos;
			indices[(i+1) * 3 + 2] = lat_pos * num_lats + long_pos;
			i += 2;
		}
	}

	for(i = 0; i < nVertices; i++){
		expanded_colors[i * 3 + 0] = 0.0f;
		expanded_colors[i * 3 + 1] = 0.6f;
		expanded_colors[i * 3 + 2] = 0.2f;

	}

	glGenVertexArrays(1, &sphere_VAO);
	glBindVertexArray(sphere_VAO);

	GLuint vertex_VBO, index_EBO;
	GLuint normal_VBO;
	GLuint color_VBO;
	glGenBuffers(1, &vertex_VBO);
	glGenBuffers(1, &index_EBO);

	glGenBuffers(1, &normal_VBO);

	glGenBuffers(1, &color_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vVertex_attrib);
	glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nVertices * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vNormal_attrib);
	glVertexAttribPointer(vNormal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, color_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vColor_attrib);
	glVertexAttribPointer(vColor_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

}

int init_resources()
{
	//Enable certain OpenGL states
	glEnable(GL_DEPTH_TEST); //Enable Z-buffer
	glEnable(GL_MULTISAMPLE); //Draw smoothed polygons

	//Create program
	if(shading_type == PHONG){
		program = createProgram("./res/vshader_phong.vs", "./res/fshader_phong.fs");
	}
	else if (shading_type == GOURAUD){
		program = createProgram("./res/vshader.vs", "./res/fshader.fs");
	}
	else if (shading_type == DEPTH){
		program = createProgram("./res/vshader_depth.vs", "./res/fshader_depth.fs");
	}
	program_light = createProgram("./res/vshader_light.vs", "./res/fshader_light.fs");
	//Create cube VAO
	// createCubeObject();

	// Create sphere VAO
	createSphereObject();

	// Create light VAO
	createLightSource();

	//Setup Transformations
	setupModelTransformation();
	setupViewTransformation();
	setupProjectionTransformation();
	setupLightSource();
	return 1;
}





glm::vec3 getTrackBallVector(double x, double y)
{


	glm::vec3 p = glm::vec3(2.0*x/screen_width - 1.0, 2.0*y/screen_height - 1.0, 0.0); //Normalize to [-1, +1]
	p.y = -p.y; //Invert Y since screen coordinate and OpenGL coordinates have different Y directions.

	float mag2 = p.x*p.x + p.y*p.y;
	if(mag2 <= 1.0f)
		p.z = sqrtf(1.0f - mag2);
	else
		p = glm::normalize(p); //Nearest point, close to the sides of the trackball
	return p;
}

void updateLightColor(glm::vec3 color, glm::vec3 pos){
	glUseProgram(program);
	glUniform3f(lightSourceColor, color.x, color.y, color.z);
	glUniform3f(lightSourcePos, pos.x, pos.y, pos.z);
}


int main(int argc, char* argv[]) {


	using namespace std;

	auto window = setupGlfw(screen_width,screen_height, "Assignment3");
	glfwSetKeyCallback(window,[](GLFWwindow *window, int key, int scancode, int action, int mods){
		if(action == GLFW_PRESS) {
			if (key == GLFW_KEY_ESCAPE) {
				glfwSetWindowShouldClose(window, GL_TRUE);
			}
		}
	});


    #ifndef __APPLE__
    GLenum glew_status = glewInit();
    if(glew_status != GLEW_OK)
    {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return EXIT_FAILURE;
    }
    #endif



    oldX = oldY = currentX = currentY = 0.0;

	if (1 == init_resources())
	{





        glm::mat4 hold = viewT;



        int prevLeftButtonState = GLFW_RELEASE;

		while (!glfwWindowShouldClose(window)) {

			glfwPollEvents();



            int leftButtonState = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
            double x,y;
            glfwGetCursorPos(window,&x,&y);
            if(leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_RELEASE){
                isDragging = true;

                currentX = oldX = x;
                currentY = oldY = y;

            }
            else if(leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_PRESS){
                    currentX = x;
                    currentY = y;

            }
            else if(leftButtonState == GLFW_RELEASE && prevLeftButtonState == GLFW_PRESS){
                isDragging = false;
            }


            prevLeftButtonState = leftButtonState;
            if(isDragging && (currentX !=oldX || currentY != oldY))
            {
                glm::vec3 va = getTrackBallVector(oldX, oldY);
                glm::vec3 vb = getTrackBallVector(currentX, currentY);

                float angle = acos(std::min(1.0f, glm::dot(va,vb)));
                glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
                glm::mat3 camera2object = glm::inverse(glm::mat3(viewT*modelT));
                glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
                modelT = glm::rotate(modelT, angle, axis_in_object_coord);
                glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

                oldX = currentX;
                oldY = currentY;
            }


            /* Clear the background as white */
			glClearColor(1.0, 1.0, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			/*CHANGE LIGHT COLOR HERE*/
			glm::vec3 LightColor(1.0f, 1.0f, 1.0f);
			// glm::vec3 lightColor;
			// lightColor.x = sin(glfwGetTime() * 2.0f);
			// lightColor.y = sin(glfwGetTime() * 0.7f);
			// lightColor.z = sin(glfwGetTime() * 1.3f);
			// glm::vec3 LightPos(60.0f, 60.0f, -10.0f);
			/************************/

			updateLightColor(LightColor, LightPos);

			//Draw light source
			glPointSize(5);
			glUseProgram(program_light);
			glBindVertexArray(light_VAO);
			glDrawArrays(GL_POINTS, 0, 1);
			glBindVertexArray(0);

			//Draw cube
			// glUseProgram(program);
			// glBindVertexArray(cube_VAO);
			// glDrawArrays(GL_TRIANGLES, 0, 6*2*3);
			// glBindVertexArray(0);

			//Draw sphere
			glUseProgram(program); // FOR GOURAUD
			glBindVertexArray(sphere_VAO);
			glDrawElements(GL_TRIANGLES, num_latitudes*num_longitudes*6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glfwSwapBuffers(window);
		}

        glDeleteProgram(program);
	}

	return EXIT_SUCCESS;
}


GLFWwindow * setupGlfw(int width, int height, const std::string &title) {
	glfwInit();

	glfwSetTime(0);

	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 0);
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_SAMPLES,4);
	}
	//
	// Create a GLFWwindow object
	GLFWwindow *window = glfwCreateWindow(width,height,title.c_str(), nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	return window;
}


