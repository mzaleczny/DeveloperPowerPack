#version 450 core
out vec4 FragColor;

{{ LIGHTING_STUFF }}

void main()
{
    vec3 Normal = normalize(VertNormal);
    vec3 NegativeViewDirection = normalize(CameraPosition - FragmentPosition);

    vec3 ResultColor = vec3(0.0);
    for (int i = 0; i < DIRECTIONAL_LIGHTS_COUNT; ++i)
        ResultColor += DoDirectionalLighting(DirectionalLights[i], Normal, NegativeViewDirection);

    for (int i = 0; i < POINT_LIGHTS_COUNT; ++i)
        ResultColor += DoPointLighting(PointLights[i], Normal, NegativeViewDirection);
    
    for (int i = 0; i < SPOT_LIGHTS_COUNT; ++i)
        ResultColor += DoSpotLighting(SpotLights[i], Normal, NegativeViewDirection);

    FragColor = vec4(ResultColor, 1.0f);
}
