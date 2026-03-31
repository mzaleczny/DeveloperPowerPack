#version 450 core
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec2 VertexPos;
layout (location = 1) in vec2 VertexTexCoord;

out vec2 VertTexCoord;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

layout (location = 0) uniform mat4 Model;

void main()
{
	gl_Position = Model * vec4(VertexPos, 0.0f, 1.0f);
    VertTexCoord = VertexTexCoord;
}
