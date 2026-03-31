#version 450 core
out vec4 FragColor;

struct MaterialStruct {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shininess;
};

struct LightStruct {
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

in vec3 FragmentPosition;
in vec3 VertNormal;

uniform vec3 CameraPosition;
uniform MaterialStruct Material;
uniform LightStruct Light;

void main()
{
    // ambient
    vec3 AmbientColor = Light.Ambient * Material.Ambient;

    // diffuse
    vec3 Normal = normalize(VertNormal);
    vec3 NegativeLightDirection = normalize(Light.Position - FragmentPosition);
    float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0);
    vec3 DiffuseColor = Light.Diffuse * (DiffuseCosinus * Material.Diffuse);

    // specular
    vec3 NegativeViewDirection = normalize(CameraPosition - FragmentPosition);
    vec3 ReflectionDirection = reflect(-NegativeLightDirection, Normal);
    float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0);
    float PowSpecularCosinus = pow(SpecularCosinus, Material.Shininess);
    vec3 SpecularColor = Light.Specular * (PowSpecularCosinus * Material.Specular);

    vec3 result = AmbientColor + DiffuseColor + SpecularColor;
    FragColor = vec4(result, 1.0);
}
