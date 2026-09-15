#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform vec3 u_BottomColor;
uniform vec3 u_TopColor;

uniform mat4 u_InvViewProj;

void main()
{
    vec2 ndc = vUV * 2.0 - 1.0;
    vec4 clip = vec4(ndc, 1.0, 1.0); 
    vec4 world = u_InvViewProj * clip;
    vec3 dir = normalize(world.xyz / world.w) * 10;

    float t = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 color = mix(u_BottomColor, u_TopColor, t);
    FragColor = vec4(color, 1.0);
}