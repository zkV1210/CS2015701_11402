#version 430 core

// ==========================================
// 1. 接收來自 C++ 的材質參數
// ==========================================
struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    vec3 Ke;
    float Ns;
    float d;
};

uniform MaterialInfo Material;

// ==========================================
// 2. 接收從頂點著色器傳來的變數
// ==========================================
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// 💥 新增：來自 Vertex Shader 的光源視角座標
in vec4 FragPosLightSpace;

out vec4 vFragColor;

// ==========================================
// 3. 環境與光源 Uniform 變數
// ==========================================
uniform vec3 viewPos;
uniform samplerCube skybox;
uniform float reflectionIntensity;
uniform float exposure;
uniform int useToon;
uniform int useAmbient;
uniform int useLight;

uniform vec3 lightPos;
uniform vec3 lightColor;

// 💥 新增：聚光燈 (Spotlight) 專用參數
uniform vec3 spotDirection;
uniform float cutOff;
uniform float outerCutOff;

// 💥 新增：深度貼圖 (Shadow Map)
uniform sampler2D shadowMap;


// ==========================================
// 💥 魔法函式：陰影計算 (包含 PCF 邊緣柔化)
// ==========================================
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 1. 執行透視除法，轉換到 NDC 空間 (-1 到 1)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // 2. 將 NDC 座標映射到 0.0 ~ 1.0 的範圍，用來讀取 UV 貼圖
    projCoords = projCoords * 0.5 + 0.5;

    // 如果像素超出了光源相機的拍攝範圍，就不算陰影
    if(projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    // 消除陰影失真 (Shadow Acne) 的偏移量
    float bias = 0 + max(0.0005 * (1.0 - dot(normal, lightDir)), 0.0001);

    // 3. PCF (Percentage-Closer Filtering) 柔化陰影邊緣
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // 採樣周圍 3x3 (共 9 個) 像素的深度來做平均，製造柔和的半影效果
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0; // 取平均值

    return shadow;
}

void main(void)
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    // ==========================================
    // 步驟 A：計算基礎環境反射
    // ==========================================
    vec3 R = reflect(-V, N);
    vec3 reflectionColor = texture(skybox, R).rgb;

    if (useAmbient == 1) {
        reflectionColor *= reflectionIntensity;
    } else {
        reflectionColor = vec3(0.0);
    }

    // ==========================================
    // 步驟 B：參數翻譯
    // ==========================================
    vec3 baseColor = Material.Kd;
    float metalness = (Material.Ks.r + Material.Ks.g + Material.Ks.b) / 3.0;
    metalness = clamp(metalness, 0.0, 1.0);

    float cosTheta = max(dot(N, V), 0.0);
    float fresnel = metalness + (1.0 - metalness) * pow(1.0 - cosTheta, 5.0);
    fresnel = clamp(fresnel, 0.0, 1.0);

    // ==========================================
    // 步驟 C：計算聚光燈 (Spotlight)
    // ==========================================
    vec3 L = normalize(lightPos - FragPos);
    float distance = length(lightPos - FragPos);

    // 距離衰減
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0; // 若你需要距離衰減，可把這行改回公式

    float diff = max(dot(N, L), 0.0);

    // 💥 聚光燈光錐邊緣柔化判定 (Penumbra)
    float theta = dot(L, normalize(-spotDirection));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // 套用光錐邊緣漸層
    attenuation *= intensity;

    // 取得點光源顏色
    vec3 actualLightColor = (useLight == 1) ? lightColor : vec3(0.0);

    // ==========================================
    // 💥 步驟 C-2：計算陰影遮蔽率
    // ==========================================
    // shadow 會回傳 0.0 (全亮) 到 1.0 (全暗)
    float shadow = ShadowCalculation(FragPosLightSpace, N, L);

    // ==========================================
    // 步驟 D：最終光影風格與顏色合成
    // ==========================================
    vec3 finalColor;

    if (useToon == 1)
    {
        // 1. 卡通描邊
        if (cosTheta < 0.22)
        {
            vFragColor = vec4(0.0, 0.0, 0.0, Material.d);
            return;
        }

        // 2. 點光源卡通化
        if (diff > 0.8)      diff = 1.2;
        else if (diff > 0.4) diff = 0.5;
        else                 diff = 0.0;

        // 💥 計算卡通點光源時，乘上 (1.0 - shadow) 應用陰影
        vec3 toonPointLight = actualLightColor * diff * baseColor * attenuation * (1.0 - shadow);

        // 3. 色塊化底色
        vec3 toonBase = baseColor;
        if (useAmbient == 1) {
            if (cosTheta > 0.7)      toonBase *= 1.1;
            else if (cosTheta > 0.3) toonBase *= 0.8;
            else                     toonBase *= 0.5;
        } else {
            toonBase *= 0.15;
        }

        // 4. 色塊化環境反射
        vec3 toonReflection = vec3(0.0);
        if (useAmbient == 1) {
            float reflectionLuminance = dot(reflectionColor, vec3(0.299, 0.587, 0.114));
            if (reflectionLuminance > 0.6)       toonReflection = vec3(1.0);
            else if (reflectionLuminance > 0.2)  toonReflection = reflectionColor * 0.5;
            else                                 toonReflection = vec3(0.05);
        }

        // 陰影只會遮蔽光，不會遮蔽卡通底色與反射
        finalColor = mix(toonBase, toonReflection, metalness * 0.4) + toonPointLight;
    }
    else
    {
        // 💥 寫實渲染：同樣乘上 (1.0 - shadow)
        vec3 realisticPointLight = actualLightColor * diff * baseColor * attenuation * (1.0 - shadow);
        vec3 environmentLight = (useAmbient == 1) ? mix(baseColor, reflectionColor, fresnel) : (baseColor * 0.05);

        // 陰影只會遮蔽光源打出來的光，不會遮蔽環境底光
        finalColor = environmentLight + realisticPointLight;
    }

    // ==========================================
    // 步驟 E：最終後處理
    // ==========================================
    finalColor += Material.Ke;
    finalColor *= exposure;

    // 避免顏色爆掉
    finalColor = clamp(finalColor, 0.0, 1.0);

    vFragColor = vec4(finalColor, Material.d);
    //vFragColor = vec4(vec3(shadow), 1.0);
}
/*#version 430 core

// ==========================================
// 1. 接收來自 C++ 的材質參數
// ==========================================
struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    vec3 Ke;
    float Ns;
    float d;
};

uniform MaterialInfo Material;

// ==========================================
// 2. 接收從頂點著色器傳來的世界座標變數
// ==========================================
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 vFragColor;

// ==========================================
// 3. 環境反射與動態點光源 Uniform 變數
// ==========================================
uniform vec3 viewPos;
uniform samplerCube skybox;
uniform float reflectionIntensity;
uniform float exposure;
uniform int useToon;
uniform int useAmbient;
uniform int useLight;        // 💥 點光源開關

uniform vec3 lightPos;
uniform vec3 lightColor;

void main(void)
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    // ==========================================
    // 步驟 A：計算基礎環境反射
    // ==========================================
    vec3 R = reflect(-V, N);
    vec3 reflectionColor = texture(skybox, R).rgb;

    if (useAmbient == 1) {
        reflectionColor *= reflectionIntensity;
    } else {
        reflectionColor = vec3(0.0);
    }

    // ==========================================
    // 步驟 B：參數翻譯
    // ==========================================
    vec3 baseColor = Material.Kd;
    float metalness = (Material.Ks.r + Material.Ks.g + Material.Ks.b) / 3.0;
    metalness = clamp(metalness, 0.0, 1.0);

    float cosTheta = max(dot(N, V), 0.0);
    float fresnel = metalness + (1.0 - metalness) * pow(1.0 - cosTheta, 5.0);
    fresnel = clamp(fresnel, 0.0, 1.0);

    // ==========================================
    // 步驟 C：計算點光源 (Point Light)
    // ==========================================
    vec3 L = normalize(lightPos - FragPos);
    float distance = length(lightPos - FragPos);

    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0; // 測試中，先不衰減

    float diff = max(dot(N, L), 0.0);

    // 💥 核心修改：如果關閉點光源，實際顏色就是純黑 (無光)
    vec3 actualLightColor = (useLight == 1) ? lightColor : vec3(0.0);

    // ==========================================
    // 步驟 D：最終光影風格與顏色合成
    // ==========================================
    vec3 finalColor;

    if (useToon == 1)
    {
        // 1. 卡通描邊
        if (cosTheta < 0.22)
        {
            vFragColor = vec4(0.0, 0.0, 0.0, Material.d);
            return;
        }

        // 2. 點光源卡通化
        if (diff > 0.8)      diff = 1.2;
        else if (diff > 0.4) diff = 0.5;
        else                 diff = 0.0;

        // 💥 這裡改乘 actualLightColor，關閉時會變 0
        vec3 toonPointLight = actualLightColor * diff * baseColor * attenuation;

        // 3. 色塊化底色
        vec3 toonBase = baseColor;
        if (useAmbient == 1) {
            if (cosTheta > 0.7)      toonBase *= 1.1;
            else if (cosTheta > 0.3) toonBase *= 0.8;
            else                     toonBase *= 0.5;
        } else {
            toonBase *= 0.15;
        }

        // 4. 色塊化環境反射
        vec3 toonReflection = vec3(0.0);
        if (useAmbient == 1) {
            float reflectionLuminance = dot(reflectionColor, vec3(0.299, 0.587, 0.114));
            if (reflectionLuminance > 0.6)       toonReflection = vec3(1.0);
            else if (reflectionLuminance > 0.2)  toonReflection = reflectionColor * 0.5;
            else                                 toonReflection = vec3(0.05);
        }

        finalColor = mix(toonBase, toonReflection, metalness * 0.4) + toonPointLight;
    }
    else
    {
        // 💥 寫實渲染：同樣乘上 actualLightColor
        vec3 realisticPointLight = actualLightColor * diff * baseColor * attenuation;
        vec3 environmentLight = (useAmbient == 1) ? mix(baseColor, reflectionColor, fresnel) : (baseColor * 0.05);

        finalColor = environmentLight + realisticPointLight;
    }

    // ==========================================
    // 步驟 E：最終後處理
    // ==========================================
    finalColor += Material.Ke;
    finalColor *= exposure;

    vFragColor = vec4(finalColor, Material.d);
}
*/
