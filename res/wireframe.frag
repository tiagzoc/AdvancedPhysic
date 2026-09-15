#version 330 core
out vec4 FragColor;

uniform vec3 u_WireFrameColor = vec3(0.0f, 0.0f, 0.0f);
uniform float u_MaxDistance = 10.0f;

in struct VERTEX_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} v;

in struct MAT{
    mat4 World;
    mat4 View;
    mat4 Proj;
    mat4 WVP;
} m;

void main()
{
    vec4 cam_pos = m.View * m.World * vec4(v.position, 1.0f);
    float norm_distance = 1.0f - (-cam_pos.z / u_MaxDistance);
    if (norm_distance < 0.0f) norm_distance = 0.0f;
    if (norm_distance > 1.0f) norm_distance = 1.0f;
    FragColor = vec4(u_WireFrameColor, norm_distance);
}