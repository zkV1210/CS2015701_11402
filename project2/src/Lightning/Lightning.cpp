#include "Lightning.h"
#include <cstdlib> // 給 rand() 用
#include <iostream>

namespace CG
{
    Lightning::Lightning()
    {
        // 建立專屬的 OpenGL 緩衝區
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // 設定座標點格式 (vec3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        glGenBuffers(1, &instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
        // 先挖一個能塞 100 條閃電矩陣的大洞 (依需求調整)
        glBufferData(GL_SHADER_STORAGE_BUFFER, 100 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    Lightning::~Lightning()
    {
        // 釋放 GPU 記憶體
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void Lightning::Generate(float length, int segments, float jitterAmp)
    {
        glm::vec3 start(0.0f);
        glm::vec3 end(0.0f, length, 0.0f);

        vertices.clear();
        vertices.push_back(start);

        glm::vec3 direction = end - start;
        glm::vec3 dirNorm = glm::normalize(direction);

        glm::vec3 up = glm::vec3(0, 1, 0);
        if (abs(glm::dot(dirNorm, up)) > 0.99f) up = glm::vec3(1, 0, 0);
        glm::vec3 right = glm::normalize(glm::cross(dirNorm, up));
        glm::vec3 upCross = glm::normalize(glm::cross(right, dirNorm));

        glm::vec3 currentPos = start;
        float step = length / segments;

        for (int i = 1; i < segments; i++) {
            glm::vec3 basePos = start + dirNorm * (step * i);

            float randRight = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
            float randUp    = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;

            currentPos = basePos + right * randRight * jitterAmp + upCross * randUp * jitterAmp;

            vertices.push_back(currentPos);
            vertices.push_back(currentPos);
        }

        vertices.push_back(end);

        // 傳送新資料到 GPU
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
        glBindVertexArray(0);
    }

    void Lightning::Draw(const std::vector<glm::mat4>& transforms)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);

        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, transforms.size() * sizeof(glm::mat4), transforms.data());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, instanceSSBO);

        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_LINES, 0, vertices.size(), transforms.size());
        glBindVertexArray(0);
    }
}
