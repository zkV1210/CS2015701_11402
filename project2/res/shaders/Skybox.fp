#version 420 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main() {
    // 直接用 3D 向量去 CubeMap 裡面抓顏色
    FragColor = texture(skybox, TexCoords);
}
