#version 330 core
out vec4 FragColor;

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

uniform sampler2D u_Texture0;

void main()
{
    FragColor = texture2D(u_Texture0, v.uv);
    // FragColor = vec4(v.normal.x, v.normal.y, v.normal.z, 1);
}