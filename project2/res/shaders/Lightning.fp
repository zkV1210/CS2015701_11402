#version 430 core
out vec4 FragColor;

uniform vec3 lightningColor; // 閃電的核心顏色 (例如青藍色)

void main() {
    // 輸出純色，這會被當作「自發光」
    // 如果你有做 Tone Mapping (ACES)，可以傳入大於 1.0 的顏色值讓它爆亮
    FragColor = vec4(lightningColor, 1.0);
}
