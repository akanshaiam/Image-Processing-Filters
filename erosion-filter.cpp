// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_DEFINE
#include "stb.h"

// Linear Algebra Library
#include <Eigen/Core>

// Timer
#include <chrono>
#include<iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

// VertexBufferObject wrapper
VertexBufferObject VBO;

// Contains the vertex positions
Eigen::MatrixXf V(2,3);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    // Update the position of the first vertex if the left button is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        V.col(0) << xworld, yworld;

    // Upload the change to the GPU
    VBO.update(V);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {
        case  GLFW_KEY_1:
            V.col(0) << -0.5,  0.5;
            break;
        case GLFW_KEY_2:
            V.col(0) << 0,  0.5;
            break;
        case  GLFW_KEY_3:
            V.col(0) << 0.5,  0.5;
            break;
        default:
            break;
    }

    // Upload the change to the GPU
    VBO.update(V);
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    int width, height, channels;
    char **failed = NULL;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *image = stbi_load("image1.jpg",
                                 &width,
                                 &height,
                                 &channels,
                                 0);

    float vertices[] = {
    // positions          // colors           // texture coords
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, -1.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   -1.0f, -1.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   -1.0f, 1.0f    // top left 
	};

	unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

	unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

	GLuint texture;
	glGenTextures(1, &texture);

	// Specify that we work with a 2D texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(image) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
    }
	stbi_image_free(image);
    
	Program program;
    const GLchar* vertex_shader =
            "#version 330 core\n"
                    "layout (location = 0) in vec3 aPos;"
                    "layout (location = 1) in vec3 aColor;"
                    "layout (location = 2) in vec2 aTexCoord;"
                    "out vec3 ourColor;"
					"out vec2 TexCoord;"
                    "void main() {"
	                "   gl_Position = vec4(aPos, 1.0);"
					"	ourColor = aColor;"
                    "   TexCoord = vec2(aTexCoord.x, aTexCoord.y);"
                    "}";
    const GLchar* fragment_shader =
            "#version 330 core\n"
					"float step = 2.0;"
                    "out vec4 FragColor;"
                    "in vec3 ourColor;"
                    "in vec2 TexCoord;"
					"uniform sampler2D texture1;"
					"const vec2 resolution = vec2(640,480);"
					"vec2 offsets[9] = vec2[](vec2(-1.0,-1.0), vec2(0.0,-1.0), vec2(1.0,-1.0), vec2(-1.0,0.0), vec2(0.0,0.0), vec2(1.0,0.0), vec2(-1.0,1.0), vec2(0.0,1.0), vec2(1.0,1.0));"
                    "void main() {"
					"	float x = 1.0 / resolution.x;"
					"	float y = 1.0 / resolution.y;"
					"	vec2 texOffset = vec2(x,y);"
					"	vec4 minValue = vec4(1.0);"
					"	for (int i = 0; i < 9; i++) {"
					"		minValue = min(texture2D( texture1, TexCoord.xy/resolution.xy + offsets[i]*texOffset), minValue);"
					"	}"
					"	FragColor = minValue;"
                    "}";

    GLuint vertexShader = program.create_shader_helper(GL_VERTEX_SHADER, vertex_shader);
    GLuint fragShader = program.create_shader_helper(GL_FRAGMENT_SHADER, fragment_shader);
    GLuint programShader = glCreateProgram();
    glAttachShader(programShader, vertexShader);
    glAttachShader(programShader, fragShader);

    glBindFragDataLocation(programShader, 0, "FragColor");
    glLinkProgram(programShader);

    GLint status;
    glGetProgramiv(programShader, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        glGetProgramInfoLog(programShader, 512, NULL, buffer);
        std::cerr << "Linker error: " << std::endl << buffer << std::endl;
        programShader = 0;
        return 0;
    }

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    //program.init(vertex_shader,fragment_shader,"outColor");
    //program.bind();
    glUseProgram(programShader);
	glUniform1i(glGetUniformLocation(programShader, "texture1"), 0);
	//glUniform1i(glGetUniformLocation(programShader, "mode_direction"), 1);


    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Bind your VAO (not necessary if you have only one)
        //VAO.bind();

        
        // Clear the framebuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glUseProgram(programShader);
		glUniform1i(glGetUniformLocation(programShader, "texture1"), 0);
		//glUniform1i(glGetUniformLocation(programShader, "mode_direction"), 1);

        // Draw a triangle
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    //VBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
