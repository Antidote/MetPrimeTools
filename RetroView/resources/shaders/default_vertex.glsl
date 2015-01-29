#version 330

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 ambientLight;
uniform vec2 texCoordCoordBias; // for ST animations

// Input
layout(location= 0) in vec3 in_Position;
layout(location= 1) in vec3 in_Normal;
layout(location= 2) in uint in_Color0;
layout(location= 3) in uint in_Color1;
layout(location= 4) in vec2 in_texCoord0;
layout(location= 5) in vec2 in_texCoord1;
layout(location= 6) in vec2 in_texCoord2;
layout(location= 7) in vec2 in_texCoord3;
layout(location= 8) in vec2 in_texCoord4;
layout(location= 9) in vec2 in_texCoord5;
layout(location=10) in vec2 in_texCoord6;
layout(location=11) in vec2 in_texCoord7; // Normally not used, but just in case

// output
out vec3 pos;
out vec3 norm;
out vec4 color0;
out vec4 color1;
out vec2 texCoord0;
out vec2 texCoord1;
out vec2 texCoord2;
out vec2 texCoord3;
out vec2 texCoord4;
out vec2 texCoord5;
out vec2 texCoord6;
out vec2 texCoord7;

void main()
{
        gl_Position = projection * view * model * vec4(in_Position, 1.0);
        pos  = in_Position;
        norm = in_Normal;
        mat4 mv = inverse(transpose(view * model));
        color0 = ambientLight;
        color1 = color0;
//{TEXGEN}
}
