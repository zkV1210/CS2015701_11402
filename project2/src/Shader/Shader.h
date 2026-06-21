#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>


namespace CG
{
    class Shader
    {
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        ~Shader();
        void use() const;

        // Uniform 工具
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        void bindUniformBlock(const std::string &blockName, GLuint bindingPoint) const;

    private:
        GLuint ID;
    };
}
