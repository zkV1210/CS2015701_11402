#version 420 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 DynamicNormal;

uniform vec3 viewPos;
uniform vec3 waterColor;
uniform vec3 horizonColor;
uniform float fogDensity;
uniform float reflectionRate;
uniform float distortionCoefficient;
uniform sampler2D reflectionTexture;

void main() {
    vec3 N = normalize(DynamicNormal);
    vec3 V = normalize(viewPos - FragPos);

    // ==========================================
    // 光學核心：Schlick 菲涅耳近似 (Fresnel)
    // ==========================================
    // 基礎反射率 (水大約是 0.02，也就是垂直看只有 2% 反光)
    float R0 = reflectionRate;
    // 計算視角與法線的夾角 (cosTheta)
    float cosTheta = max(dot(N, V), 0.0);
    // 代入公式：視線越平，fresnel 越接近 1.0；視線越垂直，越接近 0.02
    float fresnel = R0 + (1.0 - R0) * pow(1.0 - cosTheta, 7.0);

    // ==========================================
    // 2. 螢幕空間反射 (Screen Space Reflection)
    // ==========================================
    // 取得當前 FBO 貼圖的解析度
    vec2 texSize = textureSize(reflectionTexture, 0);

    // 計算當前片段在螢幕上的標準化 UV 座標 (0.0 ~ 1.0)
    vec2 screenUV = gl_FragCoord.xy / texSize;

    // 產生波浪擾動：利用動態法線的 X/Z 分量來扭曲 UV
    // 0.03 決定了水面倒影的破碎程度，可依喜好微調 (例如 0.01 ~ 0.05)
    vec2 distortion = N.xz * distortionCoefficient;

    // 將擾動加上去，並確保不會讀取到貼圖外的奇怪資料
    vec2 reflectUV = clamp(screenUV + distortion, vec2(0.001), vec2(0.999));

    // 從 FBO 貼圖中抽出顏色 (這裡面已經包含了天空和倒置的柯博文！)
    vec3 reflectionColor = texture(reflectionTexture, reflectUV).rgb;

    // 根據視角，讓水底顏色產生深淺變化
    vec3 refractionColor = waterColor * cosTheta;
    vec3 finalWaterColor = refractionColor * (1.0 - fresnel) + reflectionColor * fresnel;





    // ==========================================
    // 遠處霧化 (保持一望無際的視覺幻術)
    // ==========================================
    float distance = length(viewPos - FragPos);
    float fogFactor = exp(-pow((distance * fogDensity), 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 outColor = mix(horizonColor, finalWaterColor, fogFactor);

    FragColor = vec4(outColor, 0.95);
}
