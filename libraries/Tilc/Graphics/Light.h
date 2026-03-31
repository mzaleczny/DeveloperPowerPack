#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Vector.h"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

namespace Tilc::Graphics
{
	class TMesh;
	class TLight;

	enum class ELightType
	{
		DirectionalLight,
		PointLight,
		SpotLight
	};
	using TLights = std::unordered_map<ELightType, std::vector<Tilc::Graphics::TLight*>>;

	class DECLSPEC TLight
	{
	public:
		TVector3D<float> m_AmbientColor;
		TVector3D<float> m_DiffuseColor;
		TVector3D<float> m_SpecularColor;

		TLight() : m_AmbientColor({}), m_DiffuseColor({}), m_SpecularColor({}) {}
		TLight(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor)
			: m_AmbientColor(AmbientColor), m_DiffuseColor(DiffuseColor), m_SpecularColor(SpecularColor)
		{
		}
		void SetProperties(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor)
		{
			m_AmbientColor = AmbientColor;
			m_DiffuseColor = DiffuseColor;
			m_SpecularColor = SpecularColor;
		}
		void SetProperties(const glm::vec3& AmbientColor, const glm::vec3& DiffuseColor, const glm::vec3& SpecularColor)
		{
			m_AmbientColor = { AmbientColor.x, AmbientColor.y, AmbientColor.z };
			m_DiffuseColor = { DiffuseColor.x, DiffuseColor.y, DiffuseColor.z };
			m_SpecularColor = { SpecularColor.x, SpecularColor.y, SpecularColor.z };
		}
		virtual void SendLightParametersToPipelineOfTheMesh(TMesh* Mesh, int LightIndex) = 0;
	};

	class DECLSPEC TDirectionalLight : public TLight
	{
	public:
		TVector3D<float> m_Direction;

		TDirectionalLight() : TLight({}, {}, {}), m_Direction({})
		{
		}
		TDirectionalLight(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor, const TVector3D<float>& Direction)
			: TLight(AmbientColor, DiffuseColor, SpecularColor), m_Direction(Direction)
		{
		}
		void SetProperties(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor, const TVector3D<float>& Direction)
		{
			TLight::SetProperties(AmbientColor, DiffuseColor, SpecularColor);
			m_Direction = Direction;
		}
		void SetProperties(const glm::vec3& AmbientColor, const glm::vec3& DiffuseColor, const glm::vec3& SpecularColor, const TVector3D<float>& Direction)
		{
			TLight::SetProperties(AmbientColor, DiffuseColor, SpecularColor);
			m_Direction = Direction;
		}
		void SetDirection(const TVector3D<float>& Direction)
		{
			m_Direction = Direction;
		}
		virtual void SendLightParametersToPipelineOfTheMesh(TMesh* Mesh, int LightIndex) override;
	};

	class DECLSPEC TPointLight : public TLight
	{
	public:
		TVector3D<float> m_Position;
		float m_ConstantAttenuationFactor;
		float m_LinearAttenuationFactor;
		float m_QuadraticAttenuationFactor;

		TPointLight()
			: TLight({}, {}, {}), m_Position({}), m_ConstantAttenuationFactor(0.0f), m_LinearAttenuationFactor(0.0f), m_QuadraticAttenuationFactor(0.0f)
		{
		}
		TPointLight(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor, const TVector3D<float>& Position,
			float ConstantAttenuationFactor, float LinearAttenuationFactor, float QuadraticAttenuationFactor)
			: TLight(AmbientColor, DiffuseColor, SpecularColor), m_Position(Position), m_ConstantAttenuationFactor(ConstantAttenuationFactor),
				m_LinearAttenuationFactor(LinearAttenuationFactor), m_QuadraticAttenuationFactor(QuadraticAttenuationFactor)
		{
		}
		void SetProperties(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor, const TVector3D<float>& Position,
			float ConstantAttenuationFactor, float LinearAttenuationFactor, float QuadraticAttenuationFactor)
		{
			TLight::SetProperties(AmbientColor, DiffuseColor, SpecularColor);
			m_Position = Position;
			m_ConstantAttenuationFactor = ConstantAttenuationFactor;
			m_LinearAttenuationFactor = LinearAttenuationFactor;
			m_QuadraticAttenuationFactor = QuadraticAttenuationFactor;
		}
		void SetProperties(const glm::vec3& AmbientColor, const glm::vec3& DiffuseColor, const glm::vec3& SpecularColor, const TVector3D<float>& Position,
			float ConstantAttenuationFactor, float LinearAttenuationFactor, float QuadraticAttenuationFactor)
		{
			TLight::SetProperties(AmbientColor, DiffuseColor, SpecularColor);
			m_Position = Position;
			m_ConstantAttenuationFactor = ConstantAttenuationFactor;
			m_LinearAttenuationFactor = LinearAttenuationFactor;
			m_QuadraticAttenuationFactor = QuadraticAttenuationFactor;
		}
		void SetPosition(const TVector3D<float>& Position)
		{
			m_Position = Position;
		}
		void SetConstantAttenuationFactor(float ConstantAttenuationFactor)
		{
			m_ConstantAttenuationFactor = ConstantAttenuationFactor;
		}
		void SetLinearAttenuationFactor(float LinearAttenuationFactor)
		{
			m_LinearAttenuationFactor = LinearAttenuationFactor;
		}
		void SetQuadraticAttenuationFactor(float QuadraticAttenuationFactor)
		{
			m_QuadraticAttenuationFactor = QuadraticAttenuationFactor;
		}
		virtual void SendLightParametersToPipelineOfTheMesh(TMesh* Mesh, int LightIndex) override;
	};

	class DECLSPEC TSpotLight : public TPointLight
	{
	public:
		TVector3D<float> m_Direction; // Direction that the light is directed to
		float m_CosineInnerConeAngle; // cosinus of the angle of the cone of spot light with full light
		float m_CosineOuterConeAngle; // cosinus of the angle of the cone of spot light to which intensity of the light is smoothely fade off

		TSpotLight()
			: TPointLight({}, {}, {}, {}, 0.0f, 0.0f, 0.0f), m_Direction({}), m_CosineInnerConeAngle(0.0f), m_CosineOuterConeAngle(0.0f)
		{
		}
		TSpotLight(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor, const TVector3D<float>& Position,
			float ConstantAttenuationFactor, float LinearAttenuationFactor, float QuadraticAttenuationFactor,
			const TVector3D<float>& Direction, float CosineInnerConeAngle, float CosineOuterConeAngle)
			: TPointLight(AmbientColor, DiffuseColor, SpecularColor, Position, ConstantAttenuationFactor, LinearAttenuationFactor, QuadraticAttenuationFactor),
				m_Direction(Direction), m_CosineInnerConeAngle(CosineInnerConeAngle), m_CosineOuterConeAngle(CosineOuterConeAngle)
		{
		}
		void SetProperties(const TVector3D<float>& AmbientColor, const TVector3D<float>& DiffuseColor, const TVector3D<float>& SpecularColor, const TVector3D<float>& Position,
			float ConstantAttenuationFactor, float LinearAttenuationFactor, float QuadraticAttenuationFactor,
			const TVector3D<float>& Direction, float CosineInnerConeAngle, float CosineOuterConeAngle)
		{
			TPointLight::SetProperties(AmbientColor, DiffuseColor, SpecularColor, Position, ConstantAttenuationFactor, LinearAttenuationFactor, QuadraticAttenuationFactor);
			m_Direction = Direction;
			m_CosineInnerConeAngle = CosineInnerConeAngle;
			m_CosineOuterConeAngle = CosineOuterConeAngle;
		}
		void SetProperties(const glm::vec3& AmbientColor, const glm::vec3& DiffuseColor, const glm::vec3& SpecularColor, const TVector3D<float>& Position,
			float ConstantAttenuationFactor, float LinearAttenuationFactor, float QuadraticAttenuationFactor,
			const TVector3D<float>& Direction, float CosineInnerConeAngle, float CosineOuterConeAngle)
		{
			TPointLight::SetProperties(AmbientColor, DiffuseColor, SpecularColor, Position, ConstantAttenuationFactor, LinearAttenuationFactor, QuadraticAttenuationFactor);
			m_Direction = Direction;
			m_CosineInnerConeAngle = CosineInnerConeAngle;
			m_CosineOuterConeAngle = CosineOuterConeAngle;
		}
		void SetDirection(const TVector3D<float>& Direction)
		{
			m_Direction = Direction;
		}
		void SetCosineInnerConeAngle(float CosineInnerConeAngle)
		{
			m_CosineInnerConeAngle = CosineInnerConeAngle;
		}
		void SetCosineOuterConeAngle(float CosineOuterConeAngle)
		{
			m_CosineOuterConeAngle = CosineOuterConeAngle;
		}
		virtual void SendLightParametersToPipelineOfTheMesh(TMesh* Mesh, int LightIndex) override;
	};
}
