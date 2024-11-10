/*************/
// Αυτό το αρχείο θα το χρησιμοποιήσετε
// για να υλοποιήσετε την άσκηση 1Α της OpenGL
//
// Α.Μ: 5108
// ΟΝΟΜΑ: ΚΟΥΤΣΟΝΙΚΟΛΗΣ ΝΙΚΟΛΑΟΣ
//
// Α.Μ: 4937
// ΟΝΟΜΑ: ΚΩΣΤΟΠΟΥΛΟΣ ΑΛΕΞΑΝΔΡΟΣ
/*************/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <chrono>

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

/*************/
// Οι LoadShaders είναι black box για σας
/*************/

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

/**********************************************************************************/
/**********************************************************************************/

// finished flag global var
bool finishedFlag = false;

// struct to describe collision rectangles
struct Rectangle { 
    float minX, maxX, minY, maxY;
};

// Function to create a rectangle from vertex data
Rectangle createRectangle(GLfloat* vertices, int startIdx) {
    return {
        vertices[startIdx], vertices[startIdx + 6], vertices[startIdx + 4], vertices[startIdx + 1]
    };
};

// check if we have reached the end of the maze
// if yes set the flag to true so we can terminate the window
bool checkIfReachedEnd(Rectangle character) {
    if(character.maxX >= 5.0f) { // we have reached the end
        finishedFlag = true;
        return true;
    }
    return false;
}

// Function to check if two rectangles overlap/collide
bool checkRectCollision(Rectangle border, Rectangle character) {
    checkIfReachedEnd(character);
    return ((
        (character.minX < -5.0f || character.maxX > 5.0f) ||
        (character.minX < border.maxX &&
        character.maxX > border.minX &&
        character.minY < border.maxY &&
        character.maxY > border.minY)
    ));
}

// this function does all the movement
// checking for key pressing and setting the next coordinates of the moveable character
void processInput(GLFWwindow *window, GLfloat *char_vertex_buffer_data, GLfloat *maze_vertex_buffer_data) {
    float moveX, moveY = 0.0f;
    GLfloat new_char_vertex_buffer_data[12];

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        moveY = 0.001f;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        moveY = -0.001f;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        moveX = -0.001f;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        moveX = 0.001f;

    // calculate new char pos and store them seperately
    for (int i = 0; i < 12; i += 3) {
        new_char_vertex_buffer_data[i] = char_vertex_buffer_data[i] + moveX;
        new_char_vertex_buffer_data[i + 1] = char_vertex_buffer_data[i + 1] + moveY;
    }

    // Create bounding box for the character
    Rectangle charRect = createRectangle(new_char_vertex_buffer_data, 0);

    // Create bounding boxes for the maze walls
    std::vector<Rectangle> mazeWalls = {
        createRectangle(maze_vertex_buffer_data, 0),
        createRectangle(maze_vertex_buffer_data, 12),
        createRectangle(maze_vertex_buffer_data, 24),
        createRectangle(maze_vertex_buffer_data, 36),
        createRectangle(maze_vertex_buffer_data, 48),
        createRectangle(maze_vertex_buffer_data, 60),
        createRectangle(maze_vertex_buffer_data, 72),
        createRectangle(maze_vertex_buffer_data, 84),
        createRectangle(maze_vertex_buffer_data, 96),
        createRectangle(maze_vertex_buffer_data, 108),
        createRectangle(maze_vertex_buffer_data, 120),
        createRectangle(maze_vertex_buffer_data, 132),
        createRectangle(maze_vertex_buffer_data, 144),
        createRectangle(maze_vertex_buffer_data, 156),
        createRectangle(maze_vertex_buffer_data, 168),
        createRectangle(maze_vertex_buffer_data, 180),
        createRectangle(maze_vertex_buffer_data, 192)
    };

    // Check collision
    bool collision = false;
    for (const auto& wall : mazeWalls) {
        if (checkRectCollision(wall, charRect)) {
            collision = true;
            break;
        }
    }

    // update pos if no collision
    if (!collision) {
        for (int i = 0; i < 12; i++) {
            char_vertex_buffer_data[i] = new_char_vertex_buffer_data[i];
        }
    }
}

/**********************************************************************************/
/**********************************************************************************/

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

	// Black background(r=0,g=0,b=0,alpha=0)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
	/*************/
	// Οι shaders σας είναι οι 
    // ProjectVertexShader.vertexshader
    // ProjectFragmentShader.fragmentshader
    /*************/

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
    
	/*************/

    // amount of vetrices(every triangle has 3)
    int number_of_maze_vertices = 96; // 32 triangles * 3 vertices

    GLfloat maze_vertex_buffer_data[] = {
        /*1st rectangle - top-left border*/
        -5.0f, 5.0f, 0.0f,
        -5.0f, 3.0f, 0.0f,
        -4.0f, 5.0f, 0.0f,
        -4.0f, 3.0f, 0.0f,

        /*2nd rectangle - bottom-left border*/
        -5.0f, 1.5f, 0.0f,
        -5.0f, -5.0f, 0.0f,
        -4.0f, 1.5f, 0.0f,
        -4.0f, -5.0f, 0.0f,

        /*3nd rectangle - top border*/
        -4.0f, 5.0f, 0.0f,
        -4.0f, 4.0f, 0.0f,
        4.0f, 5.0f, 0.0f,
        4.0f, 4.0f, 0.0f,

        /*4th rectangle - top-right border*/
        4.0f, 5.0f, 0.0f,
        4.0f, -1.5f, 0.0f,
        5.0f, 5.0f, 0.0f,
        5.0f, -1.5f, 0.0f,

        /*5th rectangle - bottom-right border*/
        4.0f, -3.0f, 0.0f,
        4.0f, -5.0f, 0.0f,
        5.0f, -3.0f, 0.0f,
        5.0f, -5.0f, 0.0f,

        /*6th rectangle - bottom border*/
        -4.0f, -4.0f, 0.0f,
        -4.0f, -5.0f, 0.0f,
        4.0f, -4.0f, 0.0f,
        4.0f, -5.0f, 0.0f,

        /*7th rectangle - bottom bump #1*/
        -3.0f, -3.0f, 0.0f,
        -3.0f, -4.0f, 0.0f,
        -2.0f, -3.0f, 0.0f,
        -2.0f, -4.0f, 0.0f,

        /*8th rectangle - bottom bump #2*/
        -1.0f, -3.0f, 0.0f,
        -1.0f, -4.0f, 0.0f,
        1.0f, -3.0f, 0.0f,
        1.0f, -4.0f, 0.0f,

        /*9th rectangle - horizontal mid-left wall*/
        -3.0f, -1.0f, 0.0f,
        -3.0f, -2.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, -2.0f, 0.0f,

        /*10th & 11th rectangles - 90deg mid-top walls*/
        -3.0f, 3.0f, 0.0f,
        -3.0f, 0.0f, 0.0f,
        -2.0f, 3.0f, 0.0f,
        -2.0f, 0.0f, 0.0f,
        //////
        -2.0f, 3.0f, 0.0f,
        -2.0f, 2.0f, 0.0f,
        1.0f, 3.0f, 0.0f,
        1.0f, 2.0f, 0.0f,

        /*12th to 15th rectangles - zig zag walls*/
        -1.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        /////
        0.0f, 0.0f, 0.0f,
        0.0f, -2.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, -2.0f, 0.0f,
        /////
        1.0f, -1.0f, 0.0f,
        1.0f, -2.0f, 0.0f,
        3.0f, -1.0f, 0.0f,
        3.0f, -2.0f, 0.0f,
        /////
        2.0f, -2.0f, 0.0f,
        2.0f, -3.0f, 0.0f,
        3.0f, -2.0f, 0.0f,
        3.0f, -3.0f, 0.0f,

        /*16th rectangle - vertical top-right wall*/
        2.0f, 3.0f, 0.0f,
        2.0f, 0.0f, 0.0f,
        3.0f, 3.0f, 0.0f,
        3.0f, 0.0f, 0.0f,
    };

    unsigned int maze_indices[] = {
        0, 1, 2,
        1, 2, 3,

        4, 5, 6,
        5, 6, 7,

        8, 9, 10,
        9, 10, 11,

        12, 13, 14,
        13, 14, 15,

        16, 17, 18,
        17, 18, 19,

        20, 21, 22,
        21, 22, 23,

        24, 25, 26,
        25, 26, 27,

        28, 29, 30,
        29, 30, 31,

        32, 33, 34,
        33, 34, 35,

        36, 37, 38,
        37, 38, 39,

        40, 41, 42,
        41, 42, 43,

        44, 45, 46,
        45, 46, 47,

        48, 49, 50,
        49, 50, 51,

        52, 53, 54,
        53, 54, 55,

        56, 57, 58,
        57, 58, 59,

        60, 61, 62,
        61, 62, 63,
    };

    /*************/

    // vertex buffer of moveable char
    GLfloat char_vertex_buffer_data[] = {
        -4.75f, 2.5f, 0.0f, // top-left
        -4.75f, 2.0f, 0.0f, // bottom-left
        -4.25f, 2.5f, 0.0f, // top-right
        -4.25f, 2.0f, 0.0f, // bottom-right
    };

    unsigned int char_indices[] = {
        0, 1, 2,
        1, 2, 3,
    };

    /*************/

    // generate the VAOs, VBOs EBOs
    GLuint mazevertexbuffer, charvertexbuffer, VAOs[2]; // VAOs[0] for maze and VAOs[1] for character
    unsigned int EBOs[2]; // EBOs[0] for maze and EBOs[1] for character
	glGenVertexArrays(2, VAOs);
	glGenBuffers(1, &mazevertexbuffer);
	glGenBuffers(1, &charvertexbuffer);
    glGenBuffers(2, EBOs);

    // setup maze
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mazevertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(maze_vertex_buffer_data), maze_vertex_buffer_data, GL_STATIC_DRAW); // GL_STATIC_DRAW makes buffer immutable
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(maze_indices), maze_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup moveable character
    glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, charvertexbuffer);
    // GL_DYNAMIC_DRAW makes the buffer mutable, 
    // and since we move our character that means we need to make it dynamic
	glBufferData(GL_ARRAY_BUFFER, sizeof(char_vertex_buffer_data), char_vertex_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(char_indices), char_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // on: deixnei ta polygwna

    // render loop
	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); // for camera

        // draw maze
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);

        // draw moveable character + deal with movement
        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ARRAY_BUFFER, charvertexbuffer);
        processInput(window, char_vertex_buffer_data, maze_vertex_buffer_data);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(char_vertex_buffer_data), char_vertex_buffer_data);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 
	while (!finishedFlag && glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &mazevertexbuffer);
    glDeleteBuffers(1, &charvertexbuffer);
	glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, EBOs);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}