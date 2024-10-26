//**********************************************
// Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1Α της OpenGL
//
// Α.Μ: 5108
// ΟΝΟΜΑ: ΚΟΥΤΣΟΝΙΚΟΛΗΣ ΝΙΚΟΛΑΟΣ
//
// Α.Μ: 4937
// ΟΝΟΜΑ: ΚΩΣΤΟΠΟΥΛΟΣ ΑΛΕΞΑΝΔΡΟΣ
//**********************************************

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

//****************************************
// Οι LoadShaders είναι black box για σας

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

// void processInput(GLFWwindow *window, float &x, float &y) {
//     if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
//         y += 0.01f;
//     if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
//         y -= 0.01f;
//     if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
//         x -= 0.01f;
//     if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
//         x += 0.01f;
// }

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(750, 750, "Άσκηση 1Α - 2024", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the Q key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Grey background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
	//***********************************************
	// Οι shaders σας είναι οι 
    // ProjectVertexShader.vertexshader
    // ProjectFragmentShader.fragmentshader

	GLuint programID = LoadShaders("ProjectVertexShader.vertexshader", "ProjectFragmentShader.fragmentshader");
	
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	
	glm::mat4 Projection = glm::perspective(glm::radians(30.0f), 4.0f / 4.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 30), // Camera  in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // 
	);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model; 
    
	//**************************************************

    // amount of vetrices(every triangle has 3)
    int number_of_maze_vertices = 96; // 32 triangles * 3 vertices

    static const GLfloat maze_vertex_buffer_data[] = {
        // every rect is made out of 2 triangles

        /*1st rectangle - top-left border*/
        // 1st triangle
        -5.0f, 5.0f, 0.0f,
        -4.0f, 3.0f, 0.0f,
        -5.0f, 3.0f, 0.0f,
        
        // 2nd triangle
        -4.0f, 3.0f, 0.0f,
        -5.0f, 5.0f, 0.0f,
        -4.0f, 5.0f, 0.0f,

        /*2nd rectangle - bottom-left border*/
        -5.0f, 1.5f, 0.0f,
        -4.0f, -5.0f, 0.0f,
        -5.0f, -5.0f, 0.0f,
        
        -4.0f, 1.5f, 0.0f,
        -5.0f, 1.5f, 0.0f,
        -4.0f, -5.0f, 0.0f,

        /*3nd rectangle - top border*/
        -4.0f, 5.0f, 0.0f,
        4.0f, 5.0f, 0.0f,
        4.0f, 4.0f, 0.0f,

        -4.0f, 5.0f, 0.0f,
        -4.0f, 4.0f, 0.0f,
        4.0f, 4.0f, 0.0f,

        /*4th rectangle - top-right border*/
        5.0f, 5.0f, 0.0f,
        5.0f, -1.5f, 0.0f,
        4.0f, -1.5f, 0.0f,

        5.0f, 5.0f, 0.0f,
        4.0f, 5.0f, 0.0f,
        4.0f, -1.5f, 0.0f,

        /*5th rectangle - bottom-right border*/
        5.0f, -3.0f, 0.0f,
        4.0f, -3.0f, 0.0f,
        4.0f, -5.0f, 0.0f,

        4.0f, -5.0f, 0.0f,
        5.0f, -5.0f, 0.0f,
        5.0f, -3.0f, 0.0f,

        /*6th rectangle - bottom border*/
        4.0f, -5.0f, 0.0f,
        4.0f, -4.0f, 0.0f,
        -4.0f, -5.0f, 0.0f,
        
        4.0f, -4.0f, 0.0f,
        -4.0f, -4.0f, 0.0f,
        -4.0f, -5.0f, 0.0f,

        /*7th rectangle - bottom bump #1*/
        -3.0f, -4.0f, 0.0f,
        -3.0f, -3.0f, 0.0f,
        -2.0f, -3.0f, 0.0f,
        
        -2.0f, -4.0f, 0.0f,
        -2.0f, -3.0f, 0.0f,
        -3.0f, -4.0f, 0.0f,

        /*8th rectangle - bottom bump #2*/
        -1.0f, -4.0f, 0.0f,
        -1.0f, -3.0f, 0.0f,
        1.0f, -3.0f, 0.0f,
        
        -1.0f, -4.0f, 0.0f,
        1.0f, -3.0f, 0.0f,
        1.0f, -4.0f, 0.0f,

        /*9th rectangle - horizontal mid-left wall*/
        -3.0f, -2.0f, 0.0f,
        -3.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        
        -1.0f, -1.0f, 0.0f,
        -1.0f, -2.0f, 0.0f,
        -3.0f, -2.0f, 0.0f,

        /*10th & 11th rectangles - 90deg mid-top walls*/
        -3.0f, 0.0f, 0.0f,
        -2.0f, 0.0f, 0.0f,
        -3.0f, 3.0f, 0.0f,
        
        -3.0f, 3.0f, 0.0f,
        -2.0f, 3.0f, 0.0f,
        -2.0f, 0.0f, 0.0f,

        -2.0f, 3.0f, 0.0f,
        -2.0f, 2.0f, 0.0f,
        1.0f, 3.0f, 0.0f,
        
        1.0f, 3.0f, 0.0f,
        1.0f, 2.0f, 0.0f,
        -2.0f, 2.0f, 0.0f,

        /*12th to 15th rectangles - zig zag walls*/
        -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        
        -1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        1.0f, 0.0f, 0.0f,
        1.0f, -2.0f, 0.0f,
        0.0f, -2.0f, 0.0f,
        
        0.0f, -2.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        1.0f, -1.0f, 0.0f,
        1.0f, -2.0f, 0.0f,
        3.0f, -1.0f, 0.0f,
        
        3.0f, -1.0f, 0.0f,
        3.0f, -2.0f, 0.0f,
        1.0f, -2.0f, 0.0f,

        3.0f, -2.0f, 0.0f,
        3.0f, -3.0f, 0.0f,
        2.0f, -3.0f, 0.0f,
        
        3.0f, -2.0f, 0.0f,
        2.0f, -2.0f, 0.0f,
        2.0f, -3.0f, 0.0f,

        /*16th rectangle - vertical top-right wall*/
        2.0f, 0.0f, 0.0f,
        3.0f, 0.0f, 0.0f,
        3.0f, 3.0f, 0.0f,
        
        2.0f, 0.0f, 0.0f,
        2.0f, 3.0f, 0.0f,
        3.0f, 3.0f, 0.0f,
    };

    // // buffer of moveable char
    // static const GLfloat char_vertex_buffer_data[] = {
    //     // moveable character
    //     -4.75f, 2.5f, 0.0f,
    //     -4.25f, 2.5f, 0.0f,
    //     -4.75f, 2.0f, 0.0f,

    //     -4.75f, 2.0f, 0.0f,
    //     -4.25f, 2.0f, 0.0f,
    //     -4.25f, 2.5f, 0.0f,
    // };

    // buffer of moveable char
    static const GLfloat char_vertex_buffer_data[] = {
        -4.75f, 2.5f, 0.0f,
        -4.75f, 2.0f, 0.0f,
        -4.25f, 2.5f, 0.0f,
        -4.25f, 2.0f, 0.0f,
    };

    unsigned int char_indices[] = {
        0, 1, 2, // 1st triangle
        1, 2, 3, // 2nd triangle
    };

    // generate the VAOs and VBOs
    GLuint mazevertexbuffer, charvertexbuffer, VAOs[2];
    unsigned int EBO;
	glGenVertexArrays(2, VAOs);
	glGenBuffers(1, &mazevertexbuffer);
	glGenBuffers(1, &charvertexbuffer);
    glGenBuffers(1, &EBO);

    // setup maze
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mazevertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(maze_vertex_buffer_data), maze_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup character
    glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, charvertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(char_vertex_buffer_data), char_vertex_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(char_indices), char_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // on: deixnei perigramma polygwnwn

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); // for camera

        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, number_of_maze_vertices*3);

        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);

        // // firstly draw the maze //
		// glVertexAttribPointer(
		// 	0,                  // attribute 0, must match the layout in the shader.
		// 	3,                  // size
		// 	GL_FLOAT,           // type
		// 	GL_FALSE,           // normalized?
		// 	0,                  // stride
		// 	(void*)0          // array buffer offset
		// );
        // glEnableVertexAttribArray(0);
		// // Draw the maze
        // // glBindVertexArray(VAO0);
        // glBindBuffer(GL_ARRAY_BUFFER, mazevertexbuffer);
		// glDrawArrays(GL_TRIANGLES, 0, number_of_maze_vertices*3);
		// glDisableVertexAttribArray(0);

        // //////////////////////////////////////

        // glVertexAttribPointer(
		// 	0,                  // attribute 0, must match the layout in the shader.
		// 	3,                  // size
		// 	GL_FLOAT,           // type
		// 	GL_FALSE,           // normalized?
		// 	0,                  // stride
		// 	(void*)0          // array buffer offset
		// );
        // glEnableVertexAttribArray(0);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDisableVertexAttribArray(0);
        // // then create the moveable character //
        // // glEnableVertexAttribArray(0);
		// // glBindBuffer(GL_ARRAY_BUFFER, charvertexbuffer);
		// // glVertexAttribPointer(
		// // 	0,                  // attribute 0, must match the layout in the shader.
		// // 	3,                  // size
		// // 	GL_FLOAT,           // type
		// // 	GL_FALSE,           // normalized?
		// // 	0,                  // stride
		// // 	(void*)0          // array buffer offset
		// // );

		// // // Draw the triangle !
        // // glBindVertexArray(VAO1);
		// // glDrawArrays(GL_TRIANGLES, 0, 6); // 2 triangles contained in a rect

		// // glDisableVertexAttribArray(0);
        
        // // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 
	while (glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &mazevertexbuffer);
    glDeleteBuffers(1, &charvertexbuffer);
	glDeleteVertexArrays(1, &VAOs[0]);
    glDeleteVertexArrays(1, &VAOs[1]);
    glDeleteBuffers(1, &EBO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

