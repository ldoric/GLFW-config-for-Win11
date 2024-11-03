#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


struct Vertex {
    float x, y, z;
};

struct Texture {
    float u, v;
};

struct Normal {
    float nx, ny, nz;
};

struct Face {
    int vertexIndices[3];
    int textureIndices[3];
    int normalIndices[3];
    bool hasTextures = false;
    bool hasNormals = false;
};

std::vector<Vertex> vertices;
std::vector<Texture> textures;
std::vector<Normal> normals;
std::vector<Face> faces;

void loadOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            Vertex vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (type == "vt") {
            Texture texture;
            ss >> texture.u >> texture.v;
            textures.push_back(texture);
        }
        else if (type == "vn") {
            Normal normal;
            ss >> normal.nx >> normal.ny >> normal.nz;
            normals.push_back(normal);
        }
        else if (type == "f") {
            Face face;
            //used later for texture and normal indices
            //char slash;
            //int textureIndex, normalIndex;

            for (int i = 0; i < 3; i++) {
                std::string vertexData;
                ss >> vertexData;
                std::stringstream vertexSS(vertexData);

                std::string vertexIndexStr;
                std::getline(vertexSS, vertexIndexStr, '/');
                face.vertexIndices[i] = std::stoi(vertexIndexStr) - 1;

                if (std::getline(vertexSS, vertexIndexStr, '/')) {
                    if (!vertexIndexStr.empty()) {
                        face.textureIndices[i] = std::stoi(vertexIndexStr) - 1;
                        face.hasTextures = true;
                    }

                    if (std::getline(vertexSS, vertexIndexStr)) {
                        face.normalIndices[i] = std::stoi(vertexIndexStr) - 1;
                        face.hasNormals = true;
                    }
                }
            }
            faces.push_back(face);
        }
    }
    file.close();
}

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    
    std::string filename = "../objects/labTest.obj";  // change .obj
    loadOBJ(filename);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Vertex Shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(0.5, 0.8, 1.0, 1.0);  // Light blue
        }
    )";

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // VAO, VBO setup
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    std::vector<float> vertexData;
    for (const auto& face : faces) {
        for (int i = 0; i < 3; i++) {
            Vertex v = vertices[face.vertexIndices[i]];
            vertexData.push_back(v.x);
            vertexData.push_back(v.y);
            vertexData.push_back(v.z);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // dark bg
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexData.size() / 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
