#version 330
#extension GL_ARB_shading_language_420pack : enable

//{GXSHADERINFO}

// Uniforms
uniform mat4 model;

layout(binding = 0) uniform sampler2D tex0;
layout(binding = 1) uniform sampler2D tex1;
layout(binding = 2) uniform sampler2D tex2;
layout(binding = 3) uniform sampler2D tex3;
layout(binding = 4) uniform sampler2D tex4;
layout(binding = 5) uniform sampler2D tex5;
layout(binding = 6) uniform sampler2D tex6;
layout(binding = 7) uniform sampler2D tex7;

uniform vec4 konst[4];

uniform bool punchThrough;
// input
smooth in vec3 norm;
in vec4 color0;
in vec4 color1;
in vec3 texCoord0;
in vec3 texCoord1;
in vec3 texCoord2;
in vec3 texCoord3;
in vec3 texCoord4;
in vec3 texCoord5;
in vec3 texCoord6;
in vec3 texCoord7;

// output
out vec4 colorOut;
void main() {
    vec4 tevin_a  = vec4(0, 0, 0, 1), tevin_b = tevin_a, tevin_c = tevin_a, tevin_d = tevin_a;
    vec4 c0       = vec4(1, 1, 1, 1), c1 = c0, c2 = c0;
    vec4 prev     = vec4(0.5, 0.5, 0.5, 1);
    vec4 rast     = vec4(0, 0, 0, 1), tex = vec4(0);
    vec4 konstc   = vec4(1, 1, 1, 1);
    vec2 tevCoord = vec2(0);

    if (!gl_FrontFacing)
        discard;

//{TEVSTAGES}

    if (punchThrough && prev.a < 0.25)
        discard;

    colorOut = prev;
}
