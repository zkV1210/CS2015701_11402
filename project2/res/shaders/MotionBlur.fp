#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D currentTexture; // Pass 2 剛畫好的新鮮畫面 (mainFBO)
uniform sampler2D historyTexture; // 上一幀累積下來的殘影畫面
uniform float blurIntensity;      // 殘影強度 (0.0 ~ 0.99)

void main()
{
    vec3 currentFrame = texture(currentTexture, TexCoords).rgb;
    vec3 pastFrame = texture(historyTexture, TexCoords).rgb;

    // 將現在畫面與過去畫面進行混合
    // blurIntensity 越高，過去畫面的權重越重，殘影就越長
    vec3 finalColor = mix(currentFrame, pastFrame, blurIntensity);

    FragColor = vec4(finalColor, 1.0);
}
