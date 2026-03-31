#include "Tilc/Graphics/Light.h"
#include "Tilc/Graphics/Mesh.h"
#include "Tilc/Graphics/Camera.h"
#include "Tilc/Utils/Math.h"

void Tilc::Graphics::TDirectionalLight::SendLightParametersToPipelineOfTheMesh(TMesh* Mesh, int LightIndex)
{
	std::string LightGLSLVar = "DirectionalLights[" + std::to_string(LightIndex) + "]";
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".Direction", Tilc::TVector3D<float>(-0.2f, -1.0f, -0.3f));
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightComponents.Ambient", m_AmbientColor);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightComponents.Diffuse", m_DiffuseColor);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightComponents.Specular", m_SpecularColor);
}

void Tilc::Graphics::TPointLight::SendLightParametersToPipelineOfTheMesh(TMesh* Mesh, int LightIndex)
{
	std::string LightGLSLVar = "PointLights[" + std::to_string(LightIndex) + "]";
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".Position", m_Position);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightComponents.Ambient", m_AmbientColor);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightComponents.Diffuse", m_DiffuseColor);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightComponents.Specular", m_SpecularColor);
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".ConstantAttenuationFactor", m_ConstantAttenuationFactor);
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LinearAttenuationFactor", m_LinearAttenuationFactor);
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".QuadraticAttenuationFactor", m_QuadraticAttenuationFactor);
}

void Tilc::Graphics::TSpotLight::SendLightParametersToPipelineOfTheMesh(TMesh* Mesh, int LightIndex)
{
	std::string LightGLSLVar = "SpotLights[" + std::to_string(LightIndex) + "]";
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".Direction", m_Direction);
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".CosineInnerConeAngle", glm::cos(DEG_TO_RAD(12.5f)));
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".CosineOuterConeAngle", glm::cos(DEG_TO_RAD(17.5f)));
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightProperties.Position", m_Position);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightProperties.LightComponents.Ambient", m_AmbientColor);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightProperties.LightComponents.Diffuse", m_DiffuseColor);
	Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightProperties.LightComponents.Specular", m_SpecularColor);
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightProperties.ConstantAttenuationFactor", m_ConstantAttenuationFactor);
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightProperties.LinearAttenuationFactor", m_LinearAttenuationFactor);
	Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, LightGLSLVar + ".LightProperties.QuadraticAttenuationFactor", m_QuadraticAttenuationFactor);
}
