#version 330

//{GXSHADERINFO}

// Uniforms
uniform sampler2D texs[8];

uniform vec4 konst[4];

uniform float punchThrough;
uniform float texturesEnabled;
uniform float visualizeNormals;

// input
in vec3 norm;
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

    if (texturesEnabled > 0.5)
    {
        //{TEVSTAGES}
        if (punchThrough > 0.5 && prev.a <= .25)
            discard;
        else if (punchThrough > 0.5)
            prev.a = 1;
    }
    else
    {
        prev = vec4(norm * vec3(0.5) + 0.5, 1.0);
    }

    colorOut = prev;
}
