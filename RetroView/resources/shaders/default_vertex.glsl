#version 330

//{GXSHADERINFO}

// Input
layout (location =  0) in vec3 in_Position;
layout (location =  1) in vec3 in_Normal;
layout (location =  2) in vec4 in_Color0;
layout (location =  3) in vec4 in_Color1;
layout (location =  4) in vec2 in_TexCoord0;
layout (location =  5) in vec2 in_TexCoord1;
layout (location =  6) in vec2 in_TexCoord2;
layout (location =  7) in vec2 in_TexCoord3;
layout (location =  8) in vec2 in_TexCoord4;
layout (location =  9) in vec2 in_TexCoord5;
layout (location = 10) in vec2 in_TexCoord6;
layout (location = 11) in vec2 in_TexCoord7; // Normally not used, but just in case

// output
out vec3 pos;
out vec3 norm;
out vec4 color0;
out vec4 color1;
out vec3 texCoord0;
out vec3 texCoord1;
out vec3 texCoord2;
out vec3 texCoord3;
out vec3 texCoord4;
out vec3 texCoord5;
out vec3 texCoord6;
out vec3 texCoord7;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 ambientLight;

uniform mat4 texMtx[11];
uniform mat4 postMtx[21];

void main()
{
    gl_Position = projection * view * model * vec4(in_Position, 1.0);
    pos  = in_Position;
    norm = transpose(inverse(mat3(view * model))) * in_Normal;

    color0 = ambientLight;
    color1 = color0;
//{TEXGEN}
}
