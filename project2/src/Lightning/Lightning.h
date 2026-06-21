#pragma once
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <Shader/Shader.h> // 假設你的 Shader 標頭檔在這裡


namespace CG
{
    class Lightning
    {
    public:
        Lightning();
        ~Lightning();

        // 產生閃電形狀
        void Generate(float length, int segments, float jitterAmp = 1.5f);

        void Draw(const std::vector<glm::mat4>& transforms);
        void Draw();

    private:
        GLuint VAO;
        GLuint VBO;
        GLuint instanceSSBO;
        std::vector<glm::vec3> vertices;
    };
}
