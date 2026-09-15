#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vUV;

uniform mat4 u_World;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_WVP;

out struct VERTEX_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} v;

out struct MAT{
    mat4 World;
    mat4 View;
    mat4 Proj;
    mat4 WVP;
} m;

void main()
{
    gl_Position = u_WVP * (vec4(vPosition, 1.0));
    v.position = vPosition;
    v.normal = vNormal;
    v.uv = vUV;

    m.World = u_World;
    m.View = u_View;
    m.Proj = u_Proj;
    m.WVP = u_WVP;
}