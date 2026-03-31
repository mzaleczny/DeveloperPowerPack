#version 450 core
in vec2 VertTexCoord;
out vec4 FragColor;

layout (binding = 0) uniform sampler2D tex;
layout (location = 0) uniform float Alpha;

void main()
{
    if (Alpha > 0.99f)
    {
		FragColor = texture(tex, VertTexCoord).rgba;
    }
    else
    {
		FragColor = vec4(texture(tex, VertTexCoord).rgb, Alpha);
    }
}
