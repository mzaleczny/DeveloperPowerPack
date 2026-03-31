#pragma once

#define GLAD_GL_IMPLEMENTATION
#ifdef APIENTRY
# undef APIENTRY
#endif
#include <glad/glad.h>
#ifdef APIENTRY
# undef APIENTRY
#endif
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Vector.h"
#include "Tilc/Utils/Matrix.h"
#include "Tilc/Utils/Quaternion.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Tilc::Graphics
{
	enum class ECameraDirection
	{
		Forward,
		Backward,
		Left,
		Right
	};

	class DECLSPEC TCamera
	{
	public:
		TCamera();
		TCamera(const Tilc::TVector3D<float>& Position, const Tilc::TVector3D<float>& Right, const Tilc::TVector3D<float>& Up);
		void Rotate(float dx, float dy);
		void Move(ECameraDirection Direction, float DeltaTime);
		Tilc::TVector3D<float> GetLookAtDirection()
		{
			return (-m_ViewMatrix.GetRow(2)).Normalize();
		}
		Tilc::TMatrix4<float>& UpdateViewMatrix();
		Tilc::TMatrix4<float> QuatToMatrix4(glm::quat q);
	public:
		Tilc::TVector3D<float> m_Position = { 0.0f, 0.0f, 0.0f };
		Tilc::TVector3D<float> m_Right = { 1.0f, 0.0f, 0.0f };
		Tilc::TVector3D<float> m_Up = { 0.0f, 1.0f, 0.0f };
		Tilc::TMatrix4<float> m_ViewMatrix;
		//Tilc::TQuaternion<float> m_Rotation;
		glm::quat m_Rotation;
		// 
		// looking up/down, angle in degrees
		float m_XAngle = 0.0f;
		// looking left/right, angle in degrees
		float m_YAngle = 0.0f;

		float MouseSensitivity = 0.06f;
		float CameraSpeed = 7.5f;

	};

	//void DECLSPEC PrintMatrix(glm::mat4 m);
	//void DECLSPEC PrintMatrix(Tilc::TMatrix4<float> m);
}
