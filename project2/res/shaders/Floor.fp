#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

// 💥 新增：聚光燈參數
uniform vec3 spotDirection;
uniform float cutOff;
uniform float outerCutOff;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    if (fragPosLightSpace.w <= 0.0) return 0.0;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // 超出影子相機視角的地方，預設沒有陰影 (但稍後會被聚光燈的黑色邊緣蓋掉！)
    if(projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    // 地板專用的極小容錯值
    float bias = max(0.00005 * (1.0 - dot(normal, lightDir)), 0.00001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main()
{
    vec3 floorBaseColor = vec3(0.6);
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    // 💥 聚光燈光錐邊緣柔化 (跟 Robot 一模一樣)
    float theta = dot(lightDir, normalize(-spotDirection));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // 💥 漫反射乘上 intensity，光圈外會直接變成 0 (純黑)
    vec3 diffuseLight = diff * vec3(1.0) * intensity;

    float shadow = ShadowCalculation(FragPosLightSpace, normal, lightDir);

    // 環境底光調暗，讓光圈外的地板維持幽暗
    vec3 ambientLight = vec3(0.05);

    vec3 finalColor = (ambientLight + (1.0 - shadow) * diffuseLight) * floorBaseColor;

    FragColor = vec4(finalColor, 1.0);
}
