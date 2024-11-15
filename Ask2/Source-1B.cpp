// Ονοματεπώνυμα //
/*
    ΑΜ: 5108  Όνομα: Νικόλαος Κουτσονικολής
    ΑΜ: 4937  Όνομα: Αλέξανδρος Κωστόπουλος
*/

// Περιγραφή //
/* 
    Αυτό το αρχείο βασίζεται στο αρχέιο Source-1A.cpp
    της προηγούμενης άσκησης με τροποποιήσεις ώστε να κατασκευάσουμε
    αυτή τη φορά 3Δ γραφικά και να τα αποδώσουμε στην οθόνη
*/

// Include standard headers
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

// βοηθητικές μεταβλητές που θα κρατάνε τις συντεταγμένες
// επειδή θέλουμε κίνηση στους άξονες xyz(x, y και zoom)
float cam_x = 0.0f;
float cam_y = 0.0f;
float cam_z = 20.0f;

// camera function for applying camera movement
// W/X -> changing x coordinate
// Q/Z -> changing y coordinate
// =/- or +/-(numpad) -> zoom in/out(changing z coordinate)
void camera_function() {   
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam_x += 0.01f;
    } else if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        cam_x -= 0.01f;
    } else if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        cam_y += 0.01f;
    } else if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        cam_y -= 0.01f; 
    } else if((glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)) {
        cam_z += 0.01f; // incrementing z coord moves the camera further away from the maze
    } else if((glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)) {
        cam_z -= 0.01f; // decrementing the z coord moves the camera closer to the maze
    }
}

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

// struct to describe collision rectangles
struct Rectangle {
    float minX, maxX, minY, maxY;
};

// function that creates a rectangle from vertex data
Rectangle createRectangle(GLfloat* vertices, int startIdx) {
    return {
        vertices[startIdx], vertices[startIdx + 6], vertices[startIdx + 4], vertices[startIdx + 1]
    };
};

// check if we have surpassed the end of the maze
bool checkIfSurpassedEnd(Rectangle character) {
    if(character.maxX >= 4.75f) {
        return true;
    }
    return false;
}

// check if we have surpassed the start of the maze
bool checkIfSurpassedStart(Rectangle character) {
    if(character.minX <= -5.25f) {
        return true;
    }
    return false;
}

// check if two rectangles overlap/collide
bool checkRectCollision(Rectangle border, Rectangle character) {
    return ((
        (character.minX <= -5.25f || character.maxX >= 4.75f) ||
        (character.minX < border.maxX &&
        character.maxX > border.minX &&
        character.minY < border.maxY &&
        character.maxY > border.minY)
    ));
}

// this function has all the movement logic
// checking for key press and updating the coordinates of the moveable character
void processInput(GLfloat *char_vertex_buffer_data, std::vector<Rectangle> mazeWalls) {
    float moveX, moveY = 0.0f;
    GLfloat new_char_vertex_buffer_data[] = { // temporary array with character coordinates so we don't directly update the original
        // Bottom face(laying on xy-plane as z=0.25f)
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        // Top face(z=0.5f)
        0.0f, 0.0f, 0.50f,
        0.0f, 0.0f, 0.50f,
        0.0f, 0.0f, 0.50f,
        0.0f, 0.0f, 0.50f,
    };
    GLfloat char_starting_vertex_buffer_data[] = { // maze start character coordinates
        // Bottom face(laying on xy-plane as z=0.0f)
        -4.75f, 2.5f, 0.0f,
        -4.75f, 2.0f, 0.0f,
        -4.25f, 2.5f, 0.0f,
        -4.25f, 2.0f, 0.0f,

        // Top face(z=0.50f)
        -4.75f, 2.5f, 0.50f,
        -4.75f, 2.0f, 0.50f,
        -4.25f, 2.5f, 0.50f,
        -4.25f, 2.0f, 0.50f,
    };
    GLfloat char_ending_vertex_buffer_data[] = { // maze end character coordinates
        // Bottom face(laying on xy-plane as z=0.0f)
        4.25f, -2.0f, 0.0f,
        4.25f, -2.5f, 0.0f,
        4.75f, -2.0f, 0.0f,
        4.75f, -2.5f, 0.0f,

        // Top face(z=0.50f)
        4.25f, -2.0f, 0.50f,
        4.25f, -2.5f, 0.50f,
        4.75f, -2.0f, 0.50f,
        4.75f, -2.5f, 0.50f,
    };

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        moveY = 0.001f;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        moveY = -0.001f;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        moveX = -0.001f;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        moveX = 0.001f;

    // calculate new char pos and store them seperately
    for (int i = 0; i < 24; i += 3) {
        new_char_vertex_buffer_data[i] = char_vertex_buffer_data[i] + moveX;
        new_char_vertex_buffer_data[i + 1] = char_vertex_buffer_data[i + 1] + moveY;
    }

    // create bounding box for the character
    Rectangle charRect = createRectangle(new_char_vertex_buffer_data, 0);
    
    // check if the character surpassed the start or the end
    bool surpassedStart = checkIfSurpassedStart(charRect);
    bool surpassedEnd = checkIfSurpassedEnd(charRect);

    // check for collision
    bool collision = false;
    for (const auto& wall : mazeWalls) {
        if (checkRectCollision(wall, charRect)) {
            collision = true;
            break;
        }
    }

    // update pos if no collision
    if (!collision) {
        for (int i = 0; i < 24; i++) {
            char_vertex_buffer_data[i] = new_char_vertex_buffer_data[i];
        }
    }

    // check if surpassed start or end of maze to reposition character
    if(surpassedStart) {
	    for (int i = 0; i < 24; i++) {
            char_vertex_buffer_data[i] = char_ending_vertex_buffer_data[i];
        }
    } else if(surpassedEnd) {
        for (int i = 0; i < 24; i++) {
            char_vertex_buffer_data[i] = char_starting_vertex_buffer_data[i];
        }
    }
}

/**********************************************************************************/
/**********************************************************************************/

int main(void)
{
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

	window = glfwCreateWindow(950, 950, "Άσκηση 1Β - 2024", NULL, NULL);

	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
    glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    /**********************************************************************************/
    /**********************************************************************************/

    // amount of vetrices(every triangle has 3)
    int number_of_maze_vertices = 576; // 192 triangles * 3 vertices

    // vertex data of maze
    GLfloat maze_vertex_buffer_data[] = {
        /*1st rectangle - top-left border*/
        // z = 0
        -5.0f, 5.0f, 0.0f,
        -5.0f, 3.0f, 0.0f,
        -4.0f, 5.0f, 0.0f,
        -4.0f, 3.0f, 0.0f,
        // z = 1
        -5.0f, 5.0f, 1.0f,
        -5.0f, 3.0f, 1.0f,
        -4.0f, 5.0f, 1.0f,
        -4.0f, 3.0f, 1.0f,
        
        ////////////////////////

        /*2nd rectangle - bottom-left border*/
        // z = 0
        -5.0f, 1.5f, 0.0f,
        -5.0f, -5.0f, 0.0f,
        -4.0f, 1.5f, 0.0f,
        -4.0f, -5.0f, 0.0f,
        // z = 1
        -5.0f, 1.5f, 1.0f,
        -5.0f, -5.0f, 1.0f,
        -4.0f, 1.5f, 1.0f,
        -4.0f, -5.0f, 1.0f,

        ////////////////////////

        /*3nd rectangle - top border*/
        // z = 0
        -4.0f, 5.0f, 0.0f,
        -4.0f, 4.0f, 0.0f,
        4.0f, 5.0f, 0.0f,
        4.0f, 4.0f, 0.0f,
        // z = 1
        -4.0f, 5.0f, 1.0f,
        -4.0f, 4.0f, 1.0f,
        4.0f, 5.0f, 1.0f,
        4.0f, 4.0f, 1.0f,

        ////////////////////////

        /*4th rectangle - top-right border*/
        // z = 0
        4.0f, 5.0f, 0.0f,
        4.0f, -1.5f, 0.0f,
        5.0f, 5.0f, 0.0f,
        5.0f, -1.5f, 0.0f,
        // z = 1
        4.0f, 5.0f, 1.0f,
        4.0f, -1.5f, 1.0f,
        5.0f, 5.0f, 1.0f,
        5.0f, -1.5f, 1.0f,

        ////////////////////////

        /*5th rectangle - bottom-right border*/
        // z = 0
        4.0f, -3.0f, 0.0f,
        4.0f, -5.0f, 0.0f,
        5.0f, -3.0f, 0.0f,
        5.0f, -5.0f, 0.0f,
        // z = 1
        4.0f, -3.0f, 1.0f,
        4.0f, -5.0f, 1.0f,
        5.0f, -3.0f, 1.0f,
        5.0f, -5.0f, 1.0f,

        ////////////////////////

        /*6th rectangle - bottom border*/
        // z = 0
        -4.0f, -4.0f, 0.0f,
        -4.0f, -5.0f, 0.0f,
        4.0f, -4.0f, 0.0f,
        4.0f, -5.0f, 0.0f,
        // z = 1
        -4.0f, -4.0f, 1.0f,
        -4.0f, -5.0f, 1.0f,
        4.0f, -4.0f, 1.0f,
        4.0f, -5.0f, 1.0f,

        ////////////////////////

        /*7th rectangle - bottom bump #1*/
        // z = 0
        -3.0f, -3.0f, 0.0f,
        -3.0f, -4.0f, 0.0f,
        -2.0f, -3.0f, 0.0f,
        -2.0f, -4.0f, 0.0f,
        // z = 1
        -3.0f, -3.0f, 1.0f,
        -3.0f, -4.0f, 1.0f,
        -2.0f, -3.0f, 1.0f,
        -2.0f, -4.0f, 1.0f,

        ////////////////////////

        /*8th rectangle - bottom bump #2*/
        // z = 0
        -1.0f, -3.0f, 0.0f,
        -1.0f, -4.0f, 0.0f,
        1.0f, -3.0f, 0.0f,
        1.0f, -4.0f, 0.0f,
        // z = 1
        -1.0f, -3.0f, 1.0f,
        -1.0f, -4.0f, 1.0f,
        1.0f, -3.0f, 1.0f,
        1.0f, -4.0f, 1.0f,

        ////////////////////////

        /*9th rectangle - horizontal mid-left wall*/
        // z = 0
        -3.0f, -1.0f, 0.0f,
        -3.0f, -2.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, -2.0f, 0.0f,
        // z = 1
        -3.0f, -1.0f, 1.0f,
        -3.0f, -2.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -2.0f, 1.0f,

        ////////////////////////

        /*10th & 11th rectangles - 90deg mid-top walls*/
        // z = 0
        -3.0f, 3.0f, 0.0f,
        -3.0f, 0.0f, 0.0f,
        -2.0f, 3.0f, 0.0f,
        -2.0f, 0.0f, 0.0f,
        // z = 1
        -3.0f, 3.0f, 1.0f,
        -3.0f, 0.0f, 1.0f,
        -2.0f, 3.0f, 1.0f,
        -2.0f, 0.0f, 1.0f,

        //////

        // z = 0
        -2.0f, 3.0f, 0.0f,
        -2.0f, 2.0f, 0.0f,
        1.0f, 3.0f, 0.0f,
        1.0f, 2.0f, 0.0f,
        // z = 1
        -2.0f, 3.0f, 1.0f,
        -2.0f, 2.0f, 1.0f,
        1.0f, 3.0f, 1.0f,
        1.0f, 2.0f, 1.0f,

        ////////////////////////


        /*12th to 15th rectangles - zig zag walls*/
        // z = 0
        -1.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        // z = 1
        -1.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        /////
        // z = 0
        0.0f, 0.0f, 0.0f,
        0.0f, -2.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, -2.0f, 0.0f,
        // z = 1
        0.0f, 0.0f, 1.0f,
        0.0f, -2.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, -2.0f, 1.0f,
        /////
        // z = 0
        1.0f, -1.0f, 0.0f,
        1.0f, -2.0f, 0.0f,
        3.0f, -1.0f, 0.0f,
        3.0f, -2.0f, 0.0f,
        // z = 1
        1.0f, -1.0f, 1.0f,
        1.0f, -2.0f, 1.0f,
        3.0f, -1.0f, 1.0f,
        3.0f, -2.0f, 1.0f,
        /////
        // z = 0
        2.0f, -2.0f, 0.0f,
        2.0f, -3.0f, 0.0f,
        3.0f, -2.0f, 0.0f,
        3.0f, -3.0f, 0.0f,
        // z = 1
        2.0f, -2.0f, 1.0f,
        2.0f, -3.0f, 1.0f,
        3.0f, -2.0f, 1.0f,
        3.0f, -3.0f, 1.0f,

        ////////////////////////

        /*16th rectangle - vertical top-right wall*/
        // z = 0
        2.0f, 3.0f, 0.0f,
        2.0f, 0.0f, 0.0f,
        3.0f, 3.0f, 0.0f,
        3.0f, 0.0f, 0.0f,
        // z = 1
        2.0f, 3.0f, 1.0f,
        2.0f, 0.0f, 1.0f,
        3.0f, 3.0f, 1.0f,
        3.0f, 0.0f, 1.0f,
    };
    unsigned int maze_indices[] = {
        // 1st rectangle - top-left border
        0, 1, 2,        1, 2, 3,
        4, 5, 6,        5, 6, 7,
        0, 1, 4,        1, 4, 5,
        2, 3, 6,        3, 6, 7,
        0, 2, 4,        2, 4, 6,
        1, 3, 5,        3, 5, 7,

        // 2nd rectangle - bottom-left border
        8, 9, 10,       9, 10, 11,
        12, 13, 14,     13, 14, 15,
        8, 9, 12,       9, 12, 13,
        10, 11, 14,     11, 14, 15,
        8, 10, 12,      10, 12, 14,
        9, 11, 13,      11, 13, 15,

        // 3rd rectangle - top border
        16, 17, 18,     17, 18, 19,
        20, 21, 22,     21, 22, 23,
        16, 17, 20,     17, 20, 21,
        18, 19, 22,     19, 22, 23,
        16, 18, 20,     18, 20, 22,
        17, 19, 21,     19, 21, 23,

        // 4th rectangle - top-right border
        24, 25, 26,     25, 26, 27,
        28, 29, 30,     29, 30, 31,
        24, 25, 28,     25, 28, 29,
        26, 27, 30,     27, 30, 31,
        24, 26, 28,     26, 28, 30,
        25, 27, 29,     27, 29, 31,

        // 5th rectangle - bottom-right border
        32, 33, 34,     33, 34, 35,
        36, 37, 38,     37, 38, 39,
        32, 33, 36,     33, 36, 37,
        34, 35, 38,     35, 38, 39,
        32, 34, 36,     34, 36, 38,
        33, 35, 37,     35, 37, 39,

        // 6th rectangle - bottom border
        40, 41, 42,     41, 42, 43,
        44, 45, 46,     45, 46, 47,
        40, 41, 44,     41, 44, 45,
        42, 43, 46,     43, 46, 47,
        40, 42, 44,     42, 44, 46,
        41, 43, 45,     43, 45, 47,

        // 7th rectangle - bottom bump #1
        48, 49, 50,     49, 50, 51,
        52, 53, 54,     53, 54, 55,
        48, 49, 52,     49, 52, 53,
        50, 51, 54,     51, 54, 55,
        48, 50, 52,     50, 52, 54,
        49, 51, 53,     51, 53, 55,

        // 8th rectangle - bottom bump #2
        56, 57, 58,     57, 58, 59,
        60, 61, 62,     61, 62, 63,
        56, 57, 60,     57, 60, 61,
        58, 59, 62,     59, 62, 63,
        56, 58, 60,     58, 60, 62,
        57, 59, 61,     59, 61, 63,

        // 9th rectangle - horizontal mid-left wall
        64, 65, 66,     65, 66, 67,
        68, 69, 70,     69, 70, 71,
        64, 65, 68,     65, 68, 69,
        66, 67, 70,     67, 70, 71,
        64, 66, 68,     66, 68, 70,
        65, 67, 69,     67, 69, 71,

        // 10th rectangle - 90deg mid-top walls
        72, 73, 74,     73, 74, 75,
        76, 77, 78,     77, 78, 79,
        72, 73, 76,     73, 76, 77,
        74, 75, 78,     75, 78, 79,
        72, 74, 76,     74, 76, 78,
        73, 75, 77,     75, 77, 79,

        // 11th rectangle - 90deg mid-top walls
        80, 81, 82,     81, 82, 83,
        84, 85, 86,     85, 86, 87,
        80, 81, 84,     81, 84, 85,
        82, 83, 86,     83, 86, 87,
        80, 82, 84,     82, 84, 86,
        81, 83, 85,     83, 85, 87,

        // 12th rectangle - zig zag walls
        88, 89, 90,     89, 90, 91,
        92, 93, 94,     93, 94, 95,
        88, 89, 92,     89, 92, 93,
        90, 91, 94,     91, 94, 95,
        88, 90, 92,     90, 92, 94,
        89, 91, 93,     91, 93, 95,

        // 13th rectangle - zig zag walls
        96, 97, 98,     97, 98, 99,
        100, 101, 102,  101, 102, 103,
        96, 97, 100,    97, 100, 101,
        98, 99, 102,    99, 102, 103,
        96, 98, 100,    98, 100, 102,
        97, 99, 101,    99, 101, 103,

        // 14th rectangle - zig zag walls
        104, 105, 106,  105, 106, 107,
        108, 109, 110,  109, 110, 111,
        104, 105, 108,  105, 108, 109,
        106, 107, 110,  107, 110, 111,
        104, 106, 108,  106, 108, 110,
        105, 107, 109,  107, 109, 111,

        // 15th rectangle - zig zag walls
        112, 113, 114,  113, 114, 115,
        116, 117, 118,  117, 118, 119,
        112, 113, 116,  113, 116, 117,
        114, 115, 118,  115, 118, 119,
        112, 114, 116,  114, 116, 118,
        113, 115, 117,  115, 117, 119,

        // 16th rectangle - vertical top-right wall
        120, 121, 122,  121, 122, 123,
        124, 125, 126,  125, 126, 127,
        120, 121, 124,  121, 124, 125,
        122, 123, 126,  123, 126, 127,
        120, 122, 124,  122, 124, 126,
        121, 123, 125,  123, 125, 127,
    };

    // vertex data of character
    GLfloat char_vertex_buffer_data[] = {
        // Bottom face(laying on xy-plane as z=0.0f)
        -4.75f, 2.5f, 0.0f,
        -4.75f, 2.0f, 0.0f,
        -4.25f, 2.5f, 0.0f,
        -4.25f, 2.0f, 0.0f,

        // Top face(z=1.0f)
        -4.75f, 2.5f, 0.5f,
        -4.75f, 2.0f, 0.5f,
        -4.25f, 2.5f, 0.5f,
        -4.25f, 2.0f, 0.5f,
    };
    unsigned int char_indices[] = {
        // bottom face
        0, 1, 2,
        1, 2, 3,

        // top face
        4, 5, 6,
        5, 6, 7,

        // back face
        1, 3, 5,
        3, 5, 7,

        // front face
        0, 2, 4,
        2, 4, 6,

        // left face
        0, 1, 4,
        1, 4, 5,

        // right face
        2, 3, 6,
        3, 6, 7,
    };

    // color data of maze and character
    GLfloat a = 0.8f;
    static const GLfloat maze_color[] = {
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,

        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,
        0.0f, 0.0f, 0.7f, a,

        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
        0.0f, 0.0f, 1.0f, a,
    };
    static const GLfloat char_color[] = {
        // Top face (Bright)
        0.6f, 0.5f, 0.0f, a,
        0.6f, 0.5f, 0.0f, a,
        0.6f, 0.5f, 0.0f, a,
        0.6f, 0.5f, 0.0f, a,

        // Bottom face (Dark)
        0.85f, 0.75f, 0.0f, a,
        0.85f, 0.75f, 0.0f, a,
        0.85f, 0.75f, 0.0f, a,
        0.85f, 0.75f, 0.0f, a,
    };


    // init vao, ebo, buffers for character and maze
    GLuint mazevertexbuffer, mazeVAO, mazecolorbuffer;
    GLuint charvertexbuffer, charVAO, charcolorbuffer;
    unsigned int mazeEBO, charEBO;

    glGenVertexArrays(1, &charVAO);
    glGenVertexArrays(1, &mazeVAO);
    glGenBuffers(1, &charvertexbuffer);
    glGenBuffers(1, &mazevertexbuffer);
    glGenBuffers(1, &charcolorbuffer);
    glGenBuffers(1, &mazecolorbuffer);
    glGenBuffers(1, &charEBO);
    glGenBuffers(1, &mazeEBO);

    // setup maze
    glBindVertexArray(mazeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mazevertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(maze_vertex_buffer_data), maze_vertex_buffer_data, GL_STATIC_DRAW); // GL_STATIC_DRAW makes buffer immutable
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mazeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(maze_indices), maze_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup maze color
    glBindBuffer(GL_ARRAY_BUFFER, mazecolorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(maze_color), maze_color, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // setup character
    glBindVertexArray(charVAO);
	glBindBuffer(GL_ARRAY_BUFFER, charvertexbuffer);
    // GL_DYNAMIC_DRAW makes the buffer mutable, 
    // and since we move our character that means we need to make it dynamic
	glBufferData(GL_ARRAY_BUFFER, sizeof(char_vertex_buffer_data), char_vertex_buffer_data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, charEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(char_indices), char_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup character color
    glBindBuffer(GL_ARRAY_BUFFER, charcolorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(char_color), char_color, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Create bounding boxes for the maze walls
    // will be used for collision detection later on
    std::vector<Rectangle> mazeWalls = {
        createRectangle(maze_vertex_buffer_data, 0),
        createRectangle(maze_vertex_buffer_data, 24),
        createRectangle(maze_vertex_buffer_data, 48),
        createRectangle(maze_vertex_buffer_data, 72),
        createRectangle(maze_vertex_buffer_data, 96),
        createRectangle(maze_vertex_buffer_data, 120),
        createRectangle(maze_vertex_buffer_data, 144),
        createRectangle(maze_vertex_buffer_data, 180),
        createRectangle(maze_vertex_buffer_data, 204),
        createRectangle(maze_vertex_buffer_data, 228),
        createRectangle(maze_vertex_buffer_data, 252),
        createRectangle(maze_vertex_buffer_data, 276),
        createRectangle(maze_vertex_buffer_data, 300),
        createRectangle(maze_vertex_buffer_data, 324),
        createRectangle(maze_vertex_buffer_data, 348),
        createRectangle(maze_vertex_buffer_data, 372), // need to fix this
    };

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // on: shows polygons

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// Use our shader
		glUseProgram(programID);
        
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
        // Camera matrix
        glm::mat4 View = glm::lookAt(
            glm::vec3(cam_x, cam_y, cam_z), // cam position coordinates - these are the ones we can control
            glm::vec3(0.0f, 0.0f, 0.25f),
            glm::vec3(0.0f, 1.0f, 0.0f) 
        );
        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 MVP = Projection * View * Model;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        camera_function();  // call camera_function to check for camera movement

        // draw maze
        glBindVertexArray(mazeVAO);
        glDrawElements(GL_TRIANGLES, 576, GL_UNSIGNED_INT, 0);

        // draw character + movement
        glBindVertexArray(charVAO);
        glBindBuffer(GL_ARRAY_BUFFER, charvertexbuffer);
        processInput(char_vertex_buffer_data, mazeWalls);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(char_vertex_buffer_data), char_vertex_buffer_data);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);  // Check if the ESC key was pressed or the window was closed

	// Cleanup
    glDeleteVertexArrays(1, &charVAO);
    glDeleteVertexArrays(1, &mazeVAO);
	glDeleteBuffers(1, &charvertexbuffer);
    glDeleteBuffers(1, &mazevertexbuffer);
    glDeleteBuffers(1, &charEBO);
    glDeleteBuffers(1, &mazeEBO);
    glDeleteBuffers(1, &charcolorbuffer);
    glDeleteBuffers(1, &mazecolorbuffer);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}