#version 450 core
out vec4 FragColor;

struct PBRMaterialStruct {
    //vec3 Ambient; // <- this is replaced by Diffuse tecture GL_TEXTURE0
    layout (binding = 0) sampler2D Diffuse;     // <- Diffuse tecture GL_TEXTURE0
    layout (binding = 1) sampler2D Specular;    // <- Specular tecture GL_TEXTURE1
    float Shininess;
};


struct LightStruct {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct DirectionalLightStruct {
    vec3 Direction;
    LightStruct LightComponents;
};

struct PointLightStruct {
    vec3 Position;
    LightStruct LightComponents;
    float ConstantAttenuationFactor;
    float LinearAttenuationFactor;
    float QuadraticAttenuationFactor;
};

struct SpotLightStruct {
    PointLightStruct LightProperties;
    vec3 Direction; // Direction that the light is directed to
    float CosineConeAngle; // cosinus of the angle of the cone of spot light with full light
    float CosineOuterConeAngle; // cosinus of the angle of the cone of spot light to which intensity of the light is smoothely fade off
};

in vec3 FragmentPosition;
in vec2 VertTexCoord;
in vec3 VertNormal;

layout (location = 0) uniform vec3 CameraPosition;
layout (location = 1) uniform PBRMaterialStruct Material;
layout (location = 4) uniform DirectionalLightStruct DirectionalLight;
layout (location = 8) uniform PointLightStruct Light;

vec3 DoDirectionalLighting(DirectionalLightStruct Light)
{
    // ambient
    // Warning below we sample with Diffuse texture as it replaces ambient color
    vec3 AmbientColor = Light.LightComponents.Ambient * vec3(texture(Material.Diffuse, VertTexCoord));

    // diffuse
    vec3 Normal = normalize(VertNormal);
    // NegativeLightDirection is the direction from FragmentPosition towards LightPosition. LightDirection MUST BE NORMALIZED!!!
    vec3 NegativeLightDirection = -Light.Direction;
    float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0);
    vec3 DiffuseColor = Light.LightComponents.Diffuse * (DiffuseCosinus * vec3(texture(Material.Diffuse, VertTexCoord)));

    // specular
    vec3 NegativeViewDirection = normalize(CameraPosition - FragmentPosition);
    vec3 ReflectionDirection = reflect(Light.Direction, Normal);
    float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0);
    float PowSpecularCosinus = pow(SpecularCosinus, Material.Shininess);
    vec3 SpecularColor = Light.LightComponents.Specular * (PowSpecularCosinus * vec3(texture(Material.Specular, VertTexCoord)));
    
    return AmbientColor + DiffuseColor + SpecularColor;
}

vec3 DoPointLighting(PointLightStruct Light)
{
    float Distance = length(Light.Position - FragmentPosition);
    float Attenuation = 1.0f /
                        (Light.ConstantAttenuationFactor + Light.LinearAttenuationFactor * Distance + Light.QuadraticAttenuationFactor * Distance * Distance);
    vec3 LightDirection = normalize(FragmentPosition - Light.Position);

    // ambient
    // Warning below we sample with Diffuse texture as it replaces ambient color
    vec3 AmbientColor = Attenuation * Light.LightComponents.Ambient * vec3(texture(Material.Diffuse, VertTexCoord));

    // diffuse
    vec3 Normal = normalize(VertNormal);
    // NegativeLightDirection is the direction from FragmentPosition towards LightPosition. LightDirection MUST BE NORMALIZED!!!
    vec3 NegativeLightDirection = -LightDirection;
    float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0);
    vec3 DiffuseColor = Attenuation * Light.LightComponents.Diffuse * (DiffuseCosinus * vec3(texture(Material.Diffuse, VertTexCoord)));

    // specular
    vec3 NegativeViewDirection = normalize(CameraPosition - FragmentPosition);
    vec3 ReflectionDirection = reflect(LightDirection, Normal);
    float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0);
    float PowSpecularCosinus = pow(SpecularCosinus, Material.Shininess);
    vec3 SpecularColor = Attenuation * Light.LightComponents.Specular * (PowSpecularCosinus * vec3(texture(Material.Specular, VertTexCoord)));
    
    return AmbientColor + DiffuseColor + SpecularColor;
}


void main()
{
    vec3 DirectionalLightApplied = DoDirectionalLighting(DirectionalLight);
    vec3 PointLightApplied = DoPointLighting(Light);
    FragColor = vec4(DirectionalLightApplied + PointLightApplied, 1.0f);
}
