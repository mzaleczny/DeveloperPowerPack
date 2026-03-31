#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

out vec3 FragmentPosition;
out vec2 VertTexCoord;
out vec3 VertNormal;

uniform mat4 Model;
uniform mat4 MVP;

void main()
{
    FragmentPosition = vec3(Model * vec4(aPos, 1.0));
    VertTexCoord = aTexCoord;
    VertNormal = mat3(transpose(inverse(Model))) * aNormal;
    gl_Position = MVP * vec4(aPos, 1.0);
}

