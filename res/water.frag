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

float mod(float x, float y) {
    while (x >= y) { x -= y; }
    return x;
}

void main()
{
    // float maxHeight = 1.0;

    // float t = clamp(v.position.y + 1.0, 0.0, 2.0) * 0.5;
    // // vec4 high = vec4(1.0, 91.0 / 255.0, 56.0 / 255.0, 1.0);
    // // vec4 low = vec4(18.0 / 255.0, 84.0 / 255.0, 85.0 / 255.0, 1.0);
    // vec4 hsv = vec4(t * 360.0, 0.5, 0.5, 1.0);

    // float C = hsv.z * hsv.y;
    // float X = C * (1 - abs(mod(hsv.x / 60.0, 2.0) - 1));
    // float M = hsv.x- C;


    // vec3 rgb = vec3(1.0, 1.0, 1.0);
    // if(hsv.x < 60.0) rgb        = vec3(C, X, 0);
    // else if(hsv.x < 120.0) rgb  = vec3(X, C, 0);
    // else if(hsv.x < 180.0) rgb  = vec3(0, C, X);
    // else if(hsv.x < 240.0) rgb  = vec3(0, X, C);
    // else if(hsv.x < 300.0) rgb  = vec3(X, 0, C);
    // else rgb                    = vec3(C, 0, X);

    // vec3 rgb2 = vec3(rgb.r + M, rgb.g + M, rgb.b + M);

    float h = (v.position.y - 0.5) * 0.1;
    h = (h + 1.0) * 0.5;

    FragColor = vec4(h, h, h, 1.0) * vec4(0, 0.3, 0.7, 0.5);

    // FragColor = vec4(v.normal.x, v.normal.y, v.normal.z, 1);
}