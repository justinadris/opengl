
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

char* read_file(const char* filepath) {
    FILE* f = fopen(filepath, "rb"); 

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(len + 1);

    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    
    fclose(f);
    return buffer;
}

GLuint compile_shader(char* source, GLenum type) {
    char* src = read_file(source);

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    free(src);

    return shader;    
}

GLuint compile_program(char* vertexSource, char* fragmentSource) {
    GLuint vertexShader = compile_shader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compile_shader(fragmentSource, GL_FRAGMENT_SHADER);
 
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main() {
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(900, 700, "window", NULL, NULL);
    
    glfwMakeContextCurrent(window);

    glewInit();

    GLuint program = compile_program("vertex.shader", "fragment.shader");

    float vertices[] = {
        0.0f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f 
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate rotation
        float timeValue = (float)glfwGetTime(); // seconds since GLFW started
        float angle = timeValue; // rotate 1 rad/sec
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        // 4x4 rotation matrix around Z
        float transform[16] = {
            cosA, -sinA, 0.0f, 0.0f,
            sinA,  cosA, 0.0f, 0.0f,
            0.0f,  0.0f, 1.0f, 0.0f,
            0.0f,  0.0f, 0.0f, 1.0f
        };

        // Pass matrix to shader
        int transformLoc = glGetUniformLocation(program, "transform");
        glUseProgram(program);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);

        // Draw triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(program);
    glfwTerminate();
}