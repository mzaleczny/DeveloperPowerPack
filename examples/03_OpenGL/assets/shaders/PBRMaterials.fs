#version 450 core
out vec4 FragColor;

struct PBRMaterialStruct {
    //vec3 Ambient; // <- this is replaced by Diffuse tecture GL_TEXTURE0
    layout (binding = 0) sampler2D Diffuse;     // <- Diffuse tecture GL_TEXTURE0
    layout (binding = 1) sampler2D Specular;    // <- Specular tecture GL_TEXTURE1
    float Shininess;
};

struct LightStruct {
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

in vec3 FragmentPosition;
in vec2 VertTexCoord;
in vec3 VertNormal;

layout (location = 0) uniform vec3 CameraPosition;
layout (location = 1) uniform PBRMaterialStruct Material;
layout (location = 5) uniform LightStruct Light;

void main()
{
    // ambient
    // Warning below we sample with Diffuse texture as it replaces ambient color
    vec3 AmbientColor = Light.Ambient * vec3(texture(Material.Diffuse, VertTexCoord));

    // diffuse
    vec3 Normal = normalize(VertNormal);
    vec3 NegativeLightDirection = normalize(Light.Position - FragmentPosition);
    float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0);
    vec3 DiffuseColor = Light.Diffuse * (DiffuseCosinus * vec3(texture(Material.Diffuse, VertTexCoord)));

    // specular
    vec3 NegativeViewDirection = normalize(CameraPosition - FragmentPosition);
    vec3 ReflectionDirection = reflect(-NegativeLightDirection, Normal);
    float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0);
    float PowSpecularCosinus = pow(SpecularCosinus, Material.Shininess);
    vec3 SpecularColor = Light.Specular * (PowSpecularCosinus * vec3(texture(Material.Specular, VertTexCoord)));
    
    vec3 Result = AmbientColor + DiffuseColor + SpecularColor;
    FragColor = vec4(Result, 1.0);
}
