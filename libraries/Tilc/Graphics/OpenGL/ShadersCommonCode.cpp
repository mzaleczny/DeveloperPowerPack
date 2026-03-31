#pragma once

#include "Tilc/Graphics/OpenGL/ShadersCommonCode.h"

unsigned int TextureTypeOffset = 2;
Tilc::TExtString CommonLightingStuff = R"(
struct PBRMaterialStruct {
    //vec3 Ambient; // <- this is replaced by Diffuse tecture GL_TEXTURE0
    layout (binding = 0) sampler2D TextureDiffuse1;
    layout (binding = 1) sampler2D TextureDiffuse2;
    layout (binding = 2) sampler2D TextureSpecular1;
    layout (binding = 3) sampler2D TextureSpecular2;
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
    float CosineInnerConeAngle; // cosinus of the angle of the cone of spot light with full light
    float CosineOuterConeAngle; // cosinus of the angle of the cone of spot light to which intensity of the light is smoothely fade off
};

in vec3 FragmentPosition;
in vec2 VertTexCoord;
in vec3 VertNormal;

layout (location = 0) uniform vec3 CameraPosition;
layout (location = 1) uniform PBRMaterialStruct Material;


#define DIRECTIONAL_LIGHTS_COUNT {{DIRECTIONAL_LIGHTS_COUNT}}
uniform DirectionalLightStruct DirectionalLights[DIRECTIONAL_LIGHTS_COUNT];

#define POINT_LIGHTS_COUNT {{POINT_LIGHTS_COUNT}}
uniform PointLightStruct PointLights[POINT_LIGHTS_COUNT];

#define SPOT_LIGHTS_COUNT {{SPOT_LIGHTS_COUNT}}
uniform SpotLightStruct SpotLights[SPOT_LIGHTS_COUNT];

vec3 DoDirectionalLighting(DirectionalLightStruct Light, vec3 Normal, vec3 NegativeViewDirection)
{
    // ambient
    // Warning below we sample with Diffuse texture as it replaces ambient color
    vec3 AmbientColor = Light.LightComponents.Ambient * vec3(texture(Material.TextureDiffuse1, VertTexCoord));

    // diffuse
    // NegativeLightDirection is the direction from FragmentPosition towards LightPosition. LightDirection MUST BE NORMALIZED!!!
    vec3 NegativeLightDirection = -Light.Direction;
    float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0);
    vec3 DiffuseColor = Light.LightComponents.Diffuse * (DiffuseCosinus * vec3(texture(Material.TextureDiffuse1, VertTexCoord)));

    // specular
    vec3 ReflectionDirection = reflect(Light.Direction, Normal);
    float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0);
    float PowSpecularCosinus = pow(SpecularCosinus, Material.Shininess);
    vec3 SpecularColor = Light.LightComponents.Specular * (PowSpecularCosinus * vec3(texture(Material.TextureSpecular1, VertTexCoord)));
    
    return AmbientColor + DiffuseColor + SpecularColor;
}

vec3 DoPointLighting(PointLightStruct Light, vec3 Normal, vec3 NegativeViewDirection)
{
    float Distance = length(Light.Position - FragmentPosition);
    float Attenuation = 1.0f /
                        (Light.ConstantAttenuationFactor + Light.LinearAttenuationFactor * Distance + Light.QuadraticAttenuationFactor * Distance * Distance);
    vec3 LightDirection = normalize(FragmentPosition - Light.Position);

    // ambient
    // Warning below we sample with Diffuse texture as it replaces ambient color
    vec3 AmbientColor = Attenuation * Light.LightComponents.Ambient * vec3(texture(Material.TextureDiffuse1, VertTexCoord));

    // diffuse
    // NegativeLightDirection is the direction from FragmentPosition towards LightPosition. LightDirection MUST BE NORMALIZED!!!
    vec3 NegativeLightDirection = -LightDirection;
    float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0);
    vec3 DiffuseColor = Attenuation * Light.LightComponents.Diffuse * (DiffuseCosinus * vec3(texture(Material.TextureDiffuse1, VertTexCoord)));

    // specular
    vec3 ReflectionDirection = reflect(LightDirection, Normal);
    float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0);
    float PowSpecularCosinus = pow(SpecularCosinus, Material.Shininess);
    vec3 SpecularColor = Attenuation * Light.LightComponents.Specular * (PowSpecularCosinus * vec3(texture(Material.TextureSpecular1, VertTexCoord)));
    
    return AmbientColor + DiffuseColor + SpecularColor;
}

vec3 DoSpotLighting(SpotLightStruct Light, vec3 Normal, vec3 NegativeViewDirection)
{
    float Distance = length(Light.LightProperties.Position - FragmentPosition);
    float Attenuation = 1.0f /
                        (Light.LightProperties.ConstantAttenuationFactor + Light.LightProperties.LinearAttenuationFactor * Distance + Light.LightProperties.QuadraticAttenuationFactor * Distance * Distance);
    // Direction from Light position to fragment position
    vec3 LightDirection = normalize(FragmentPosition - Light.LightProperties.Position);
    vec3 NegativeLightDirection = -LightDirection;

    // spotlight (soft edges)
    float Theta = dot(NegativeLightDirection, normalize(-Light.Direction));
    float Epsilon = (Light.CosineInnerConeAngle - Light.CosineOuterConeAngle);
    float Intensity = clamp((Theta - Light.CosineOuterConeAngle) / Epsilon, 0.0, 1.0);

    // ambient
    // Warning below we sample with Diffuse texture as it replaces ambient color
    vec3 AmbientColor = Attenuation * Light.LightProperties.LightComponents.Ambient * vec3(texture(Material.TextureDiffuse1, VertTexCoord));

    // diffuse
    // NegativeLightDirection is the direction from FragmentPosition towards LightPosition. LightDirection MUST BE NORMALIZED!!!
    float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0);
    vec3 DiffuseColor = Attenuation * Light.LightProperties.LightComponents.Diffuse * (DiffuseCosinus * vec3(texture(Material.TextureDiffuse1, VertTexCoord)));

    // specular
    vec3 ReflectionDirection = reflect(LightDirection, Normal);
    float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0);
    float PowSpecularCosinus = pow(SpecularCosinus, Material.Shininess);
    vec3 SpecularColor = Attenuation * Light.LightProperties.LightComponents.Specular * (PowSpecularCosinus * vec3(texture(Material.TextureSpecular1, VertTexCoord)));
    
    return AmbientColor * Intensity * Attenuation + DiffuseColor * Intensity * Attenuation + SpecularColor * Intensity * Attenuation;
}
)";