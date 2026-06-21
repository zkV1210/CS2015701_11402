#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    // 什麼運算都不做，直接把原本那張照片的像素顏色貼上去！
    FragColor = texture(screenTexture, TexCoords);
}

/*
#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

// 接收你的深度貼圖
uniform sampler2D screenTexture;

void main()
{
    // 1. 深度圖是一張單通道貼圖，所以我們只拿 .r (Red) 分量
    float depthValue = texture(screenTexture, TexCoords).r;

    // 2. 💥 魔法放大鏡：因為深度值太接近 1.0 了，我們用 pow 把它加深
    // 數字 50.0 可以自己調，數字越大，對比越強，越容易看到灰黑色的輪廓
    float contrastDepth = pow(depthValue, 50.0);

    // 3. 把這個灰階值輸出到螢幕上
    FragColor = vec4(vec3(contrastDepth), 1.0);
}
*/
