#version 430 core

struct MaterialInfo {
    vec3 Ka; // 環境光反射率 [cite: 19]
    vec3 Kd; // 漫反射顏色 (主色) [cite: 19]
    vec3 Ks; // 高光顏色 [cite: 20]
    vec3 Ke; // 自發光 [cite: 20]
    float Ns; // 高光係數 (Shininess) [cite: 21]
    float d;  // 透明度 (Alpha) [cite: 21]
};

uniform MaterialInfo Material;

// 從 Vertex Shader 傳入的變數
in vec3 vVaryingNormal;
in vec3 vVaryingLightDir;
in vec3 vVaryingEyeDir; // 用於精確計算高光 [cite: 24]
in vec2 UV;

out vec4 vFragColor;

// 外部光源顏色設定 (沿用原設定)
vec4 ambientLight  = vec4(0.1, 0.1, 0.1, 1.0); // [cite: 22]
vec4 diffuseLight  = vec4(0.8, 0.8, 0.8, 1.0); // [cite: 22]
vec4 specularLight = vec4(1.0, 1.0, 1.0, 1.0); // [cite: 23]

void main(void)
{
    // 將內插過後的法向量與方向向量重新歸一化 (非常重要)
    vec3 N = normalize(vVaryingNormal);
    vec3 L = normalize(vVaryingLightDir);
    vec3 V = normalize(vVaryingEyeDir); // 視線方向 [cite: 25]

    // ==========================================
    // 1. Ambient (環境光)
    // 算法：光源環境色 * 材質環境反射率
    // ==========================================
    vec3 ambient = ambientLight.rgb * Material.Ka; // [cite: 25]

    // ==========================================
    // 2. Diffuse (漫反射)
    // 算法：光源漫反射色 * 材質主色 * 夾角強度
    // ==========================================
    float diffIntensity = max(0.0, dot(N, L)); // [cite: 26]
    vec3 diffuse = diffIntensity * diffuseLight.rgb * Material.Kd; // [cite: 27]

    // ==========================================
    // 3. Specular (高光)
    // 算法：光源高光色 * 材質高光色 * (反射與視線夾角)^Ns
    // ==========================================
    vec3 R = reflect(-L, N); // 反射向量 [cite: 28]
    float specFactor = pow(max(0.0, dot(R, V)), Material.Ns); // [cite: 28]
    vec3 specular = vec3(0.0); // [cite: 28]

    // 只有在被光照到的地方（面對光源）才會有高光
    if(diffIntensity > 0.0) { // [cite: 29]
        specular = specFactor * specularLight.rgb * Material.Ks; // [cite: 29]
    } // [cite: 30]

    // ==========================================
    // 4. Emissive (自發光)
    // ==========================================
    vec3 emissive = Material.Ke; // [cite: 30]

    // ==========================================
    // 5. Final Color 合成
    // 這裡要注意 Alpha 通道要帶入 Material.d 才能正確渲染玻璃
    // ==========================================
    vFragColor = vec4(ambient + diffuse + specular + emissive, Material.d); // [cite: 31]
}
