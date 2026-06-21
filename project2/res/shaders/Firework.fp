#version 430 core

in float v_alpha;
in vec2 v_uv; // 範圍是 -1.0 到 1.0

uniform vec3 u_baseColor;
uniform float u_intensity;

out vec4 FragColor;

void main()
{
    // 利用傳過來的 UV 計算距離中心點的距離
    float dist = length(v_uv);

    if (dist > 1.0) discard;

    float coreGlow = exp(-dist * 5.0);
    vec3 finalColor = mix(u_baseColor, vec3(1.0, 1.0, 1.0), coreGlow * 0.9);

    float pointAlpha = smoothstep(1.0, 0.2, dist) * v_alpha;

    FragColor = vec4(finalColor * u_intensity, pointAlpha);
}
