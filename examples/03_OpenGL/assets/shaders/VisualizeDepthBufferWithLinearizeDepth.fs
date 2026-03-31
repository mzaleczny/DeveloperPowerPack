#version 450 core
out vec4 FragColor;

float Near = 0.1;
float Far = 100.0;

float LinearizeDepth(float Depth)
{
    float z = Depth * 2.0 - 1.0; // backo to NDC
    return (2.0 * Near * Far) / (Far + Near - z * (Far - Near));
}

void main()
{
    float Depth = LinearizeDepth(gl_FragCoord.z) / Far;
    FragColor = vec4(vec3(Depth), 1.0);
}
