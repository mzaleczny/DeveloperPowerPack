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

uniform mat4 Model[10];
uniform mat4 MVP[10];

void main()
{
    FragmentPosition = vec3(Model[gl_InstanceID] * vec4(aPos, 1.0));
    VertTexCoord = aTexCoord;
    VertNormal = mat3(transpose(inverse(Model[gl_InstanceID]))) * aNormal;
    gl_Position = MVP[gl_InstanceID] * vec4(aPos, 1.0);
}

