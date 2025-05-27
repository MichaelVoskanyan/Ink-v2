// shader.cc
#include "shader.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    fs::path projectDir = fs::current_path();


    // Build full paths under source/shaders/
    // I think this is fine to hardcode
    // fs::path vertPath = vertexPath;
    // fs::path fragPath = fragmentPath;

    std::string vertPath = std::string(SHADER_DIR) + vertexPath,
                fragPath = std::string(SHADER_DIR) + fragmentPath;

    std::cerr << "Loading vertex shader from:   " << vertPath << "\n";
    std::cerr << "Loading fragment shader from: " << fragPath << "\n";

    std::string vertexCode, fragmentCode;
    {
        std::ifstream vFile(vertPath), fFile(fragPath);
        if (!vFile || !fFile) {
            throw std::runtime_error("Failed to open shader files:\n  " + std::string(vertPath) +
                                     "\n  " + fragPath);
        }
        std::stringstream vs, fs;
        vs << vFile.rdbuf();
        fs << fFile.rdbuf();
        vertexCode = vs.str();
        fragmentCode = fs.str();
    }

    auto compile = [&](GLenum type, const char *src, const char *tag) {
        GLuint id = glCreateShader(type);
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);
        GLint ok;
        glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetShaderInfoLog(id, 1024, nullptr, log);
            throw std::runtime_error(std::string("ERROR::") + tag + "::COMPILATION_FAILED\n" + log);
        }
        return id;
    };

    GLuint vertID = compile(GL_VERTEX_SHADER, vertexCode.c_str(), "SHADER::VERTEX");
    GLuint fragID = compile(GL_FRAGMENT_SHADER, fragmentCode.c_str(), "SHADER::FRAGMENT");

    rendererID = glCreateProgram();
    glAttachShader(rendererID, vertID);
    glAttachShader(rendererID, fragID);
    glLinkProgram(rendererID);

    {
        GLint ok;
        glGetProgramiv(rendererID, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetProgramInfoLog(rendererID, 1024, nullptr, log);
            throw std::runtime_error(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + log);
        }
    }

    glDeleteShader(vertID);
    glDeleteShader(fragID);
}

void Shader::bind() {
    glUseProgram(rendererID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(rendererID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(rendererID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(rendererID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(rendererID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(rendererID, name.c_str()), x, y, z);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(rendererID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
}
