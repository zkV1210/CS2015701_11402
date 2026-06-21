#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float mosaicSize;
uniform float screenWidth;   // 還是靠 C++ 傳進來最穩！
uniform float screenHeight;

void main()
{
    // 1. 使用 C++ 傳入的絕對正確解析度
    vec2 texSize = vec2(screenWidth, screenHeight);

    // 2. 計算畫面上總共能切成幾個馬賽克網格
    vec2 mosaicCounts = texSize / mosaicSize;

    // 3. 將 UV 階梯化 (切成一塊一塊)
    vec2 mosaicUV = floor(TexCoords * mosaicCounts) / mosaicCounts;

    // 修復底線的黑魔法：加上半個網格的偏移量！
    // 讓採樣點移動到方塊正中心
    mosaicUV += 0.5 / mosaicCounts;

    // 4. 輸出顏色
    FragColor = texture(screenTexture, mosaicUV);
}
