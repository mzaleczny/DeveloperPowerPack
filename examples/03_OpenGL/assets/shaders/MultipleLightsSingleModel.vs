#version 450 core
layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;

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
    FragmentPosition = vec3(Model * vec4(VertexPos, 1.0));
    VertTexCoord = VertexTexCoord;
    VertNormal = mat3(transpose(inverse(Model))) * VertexNormal;
    gl_Position = MVP * vec4(VertexPos, 1.0);
}

