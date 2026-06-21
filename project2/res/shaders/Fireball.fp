#version 430 core

out vec4 FragColor;

in vec3 v_FragPosWorld;
in vec3 v_CenterWorld;

uniform float u_time;
uniform float u_intensity;
uniform float u_speed;
uniform vec3 u_direction;

layout(std140, binding = 0) uniform MatVP {
    mat4 View;
    mat4 Projection;
};

float hash(vec3 p) {
    p = fract(p * 0.3183099 + 0.1); p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}
float noise3D(vec3 x) {
    vec3 i = floor(x); vec3 f = fract(x); f = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(i + vec3(0,0,0)), hash(i + vec3(1,0,0)), f.x), mix(hash(i + vec3(0,1,0)), hash(i + vec3(1,1,0)), f.x), f.y), mix(mix(hash(i + vec3(0,0,1)), hash(i + vec3(1,0,1)), f.x), mix(hash(i + vec3(0,1,1)), hash(i + vec3(1,1,1)), f.x), f.y), f.z);
}
float fbm(vec3 p) {
    float v = 0.0; float a = 0.5;
    for (int i = 0; i < 4; i++) { v += a * noise3D(p); p *= 2.0; a *= 0.5; }
    return v;
}

void main()
{
    vec3 camPos = transpose(mat3(View)) * -View[3].xyz;
    vec3 rayDir = normalize(v_FragPosWorld - camPos);
    vec3 dir = normalize(u_direction + vec3(0.0001));

    // =========================================================
    // 1. 建立更巨大的邊界球體 (Bounding Sphere)
    // =========================================================
    // 球心往後退一點點，半徑加大到 26.0
    vec3 sphereCenter = v_CenterWorld - dir * 8.0;
    float sphereRadius = 26.0;

    vec3 oc = camPos - sphereCenter;
    float b = dot(oc, rayDir);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float h_disc = b * b - c;

    if(h_disc < 0.0) discard;

    float t1 = max(-b - sqrt(h_disc), 0.0);
    float t2 = -b + sqrt(h_disc);
    if (t2 < 0.0) discard;

    // =========================================================
    // 2. 光線步進 (Ray-Marching)
    // =========================================================
    // 體積變大，採樣次數稍微提升到 30 保持畫質
    int steps = 30;
    float dt = (t2 - t1) / float(steps);
    float t = t1 + dt * 0.5;

    vec3 finalCol = vec3(0.0);
    float finalAlpha = 0.0;
    float firstHitT = -1.0;

    for(int i = 0; i < steps; i++)
    {
        vec3 p = camPos + rayDir * t;
        vec3 diff = p - v_CenterWorld;
        float h = dot(diff, dir);

        // 💥 安全的水滴形狀公式 (解決前方的白色殘影 Bug)
        // 尾巴從 -22.0 長到 -3.0，頭部從 -3.0 縮到 6.0，絕不允許反向 smoothstep
        float profile = smoothstep(-22.0, -3.0, h) * sqrt(max(1.0 - smoothstep(-3.0, 1.0, h), 0.0));

        if (profile > 0.001)
        {
            float r = length(diff - dir * h);
            // 💥 火球最大半徑提升到 10.5！
            float maxR = 11.5 * profile;

            if (r < maxR)
            {
                float shapeDensity = smoothstep(maxR, maxR * 0.1, r);

                vec3 scroll = -dir * u_time * u_speed * 12.0;
                // 因為火球變大，雜訊的採樣縮放稍微調小 (0.25)，讓火焰紋理更粗獷
                float n = fbm(p * 0.25 + scroll);

                float localDensity = shapeDensity * n * 2.5;

                if (localDensity > 0.05)
                {
                    if (firstHitT < 0.0) firstHitT = t;

                    vec3 c = vec3(0.0);

                    // 💥 更紅的顏色映射 (Red-Dominant Palette)
                    // 把高溫白光壓在極限核心，大面積留給血紅色與亮橘色
                    if (localDensity > 0.85)
                        c = vec3(1.0, 0.9, 0.7); // 核心：偏黃的白光
                    else if (localDensity > 0.6)
                        c = mix(vec3(1.0, 0.25, 0.0), vec3(1.0, 0.9, 0.7), (localDensity - 0.6) / 0.25); // 中層：鮮明的橘色
                    else if (localDensity > 0.2)
                        c = mix(vec3(0.75, 0.0, 0.0), vec3(1.0, 0.25, 0.0), (localDensity - 0.2) / 0.4); // 外層：濃烈的深紅色
                    else
                        c = mix(vec3(0.0), vec3(0.75, 0.0, 0.0), localDensity / 0.2); // 邊緣：深紅消散至黑

                    c *= u_intensity;
                    float weight = localDensity * 0.5; // 稍微增加疊加權重，讓火球看起來更厚實
                    finalCol += c * weight;
                    finalAlpha += weight;
                }
            }
        }
        t += dt;
    }

    if (finalAlpha <= 0.01) discard;

    // =========================================================
    // 3. 寫入真實 3D 深度
    // =========================================================
    vec3 hitPos = camPos + rayDir * firstHitT;
    vec4 clipPos = Projection * View * vec4(hitPos, 1.0);
    gl_FragDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;

    FragColor = vec4(finalCol, smoothstep(0.0, 1.0, finalAlpha));
}
