#include "Shader.h"
// 關鍵：將這顆未爆彈只放在這個 .cpp 裡
#include <Utilty/LoadShaders.h>
#include <iostream>

namespace CG
{
    Shader::Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 呼叫 LoadShaders.h 裡面的結構與函式
        ShaderInfo shaders[] = {
            { GL_VERTEX_SHADER, vertexPath },
            { GL_FRAGMENT_SHADER, fragmentPath },
            { GL_NONE, NULL }
        };
        ID = LoadShaders(shaders);

        if (ID == 0) {
            std::cerr << "Shader 編譯失敗: " << vertexPath << " | " << fragmentPath << std::endl;
        }
    }

    Shader::~Shader()
    {
        glDeleteProgram(ID);
    }

    void Shader::use() const
    {
        glUseProgram(ID);
    }

    //處理把值丟給shader的關係
    void Shader::setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void Shader::bindUniformBlock(const std::string &blockName, GLuint bindingPoint) const {
        GLuint blockIndex = glGetUniformBlockIndex(ID, blockName.c_str());
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(ID, blockIndex, bindingPoint);
        }
    }
}
