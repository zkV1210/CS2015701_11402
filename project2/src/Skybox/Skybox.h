#pragma once
#include <string>
#include <GL/glew.h>

namespace CG
{
    class Skybox
    {
    public:
        // 建構子：負責把 Skybox 的大正方體 VAO/VBO 挖好洞
        Skybox();

        // 解構子：負責在程式結束時回收記憶體
        ~Skybox();

        // 載入十字展開圖
        bool LoadFromCross(const std::string& imagePath);

        // 繪製天空盒
        void DrawSkybox();

        // 讓外部 (例如水面 Shader) 可以取得這張貼圖來做反射
        GLuint GetTextureID() const { return textureID; }

    private:
        GLuint textureID;
        GLuint VAO, VBO;

        // 內部函數：初始化天空盒的 36 個頂點
        void SetupMesh();
    };
}
