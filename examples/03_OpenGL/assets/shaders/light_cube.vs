#version 450 core
layout (location = 0) in vec3 aPos;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(aPos, 1.0);
}

