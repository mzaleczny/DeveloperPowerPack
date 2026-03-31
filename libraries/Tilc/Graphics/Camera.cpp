#include "Camera.h"
#include "Tilc/Utils/Math.h"
#include <iostream>

Tilc::Graphics::TCamera::TCamera()
{
}

Tilc::Graphics::TCamera::TCamera(const Tilc::TVector3D<float>& Position, const Tilc::TVector3D<float>& Right, const Tilc::TVector3D<float>& Up)
{
	m_Position = Position;
	m_Right = Right;
	m_Up = Up;
	Rotate(0.0f, 0.0f);
}

void Tilc::Graphics::TCamera::Rotate(float dx, float dy)
{
	// we want camera move by mouse move as in Unreal Engine
	m_XAngle += MouseSensitivity * dy;
	m_YAngle += MouseSensitivity * dx;

	//Tilc::TQuaternion<float> qx = AngleAxis(DEG_TO_RAD(m_XAngle), { 1.0f, 0.0f, 0.0f });
	//Tilc::TQuaternion<float> qy = AngleAxis(DEG_TO_RAD(m_YAngle), { 0.0f, 1.0f, 0.0f });
	//m_Rotation = normalize(qx * qy);
	glm::quat qx = glm::angleAxis(static_cast<float>(DEG_TO_RAD(m_XAngle)), glm::vec3(m_Right.x, m_Right.y, m_Right.z));
	glm::quat qy = glm::angleAxis(static_cast<float>(DEG_TO_RAD(m_YAngle)), glm::vec3(m_Up.x, m_Up.y, m_Up.z));
	m_Rotation = normalize(qx * qy);
}

void Tilc::Graphics::TCamera::Move(ECameraDirection Direction, float DeltaTime)
{
	Tilc::TVector3D<float> Right{ m_ViewMatrix.GetRow(0) };
	Tilc::TVector3D<float> Forward { m_ViewMatrix.GetRow(2) };
	Tilc::TVector3D<float> DirectionVec;
	switch (Direction)
	{
	case ECameraDirection::Forward:
		DirectionVec = -Forward;
		break;
	case ECameraDirection::Backward:
		DirectionVec = Forward;
		break;
	case ECameraDirection::Left:
		DirectionVec = -Right;
		break;
	case ECameraDirection::Right:
		DirectionVec = Right;
		break;
	}

	m_Position += DirectionVec * DeltaTime * CameraSpeed;
}

Tilc::TMatrix4<float>& Tilc::Graphics::TCamera::UpdateViewMatrix()
{
	/*
	Tilc::TVector3D<float> Right{ m_ViewMatrix.GetRow(0) };
	Tilc::TVector3D<float> Forward{ m_ViewMatrix.GetRow(2) };
	std::cout << "Right=" << Right.x << ", " << Right.y << ", " << Right.z << "      ";
	std::cout << "Forward=" << Forward.x << ", " << Forward.y << ", " << Forward.z << "      \n";
	std::cout << "m_Position: " << m_Position.x << "    " << m_Position.y << "    " << m_Position.z << "\n";
	Tilc::Graphics::PrintMatrix(Translate(TMatrix4<float>(1.0f), -m_Position));
	//Tilc::Graphics::PrintMatrix(QuatToMatrix4(m_Rotation));
	//std::cout << "-\n";
	std::cout << "m_XAngle=" << m_XAngle << "      ";
	std::cout << "m_YAngle=" << m_YAngle << "      ";
	std::cout << "m_Right=" << m_Right.x << ", " << m_Right.y << ", " << m_Right.z << "      ";
	std::cout << "m_Up=" << m_Up.x << ", " << m_Up.y << ", " << m_Up.z << std::endl;
	*/
	m_ViewMatrix = QuatToMatrix4(m_Rotation) * Translate(TMatrix4<float>(1.0f), -m_Position);
	//Tilc::Graphics::PrintMatrix(m_ViewMatrix);
	//std::cout << "\n\n";
	return m_ViewMatrix;
}

Tilc::TMatrix4<float> Tilc::Graphics::TCamera::QuatToMatrix4(glm::quat q)
{
	/*
	glm::mat4 m0 = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3));
	PrintMatrix(m0);
	std::cout << "-------------------------------\n";
	Tilc::TMatrix4<float> m1 = Translate(Tilc::TMatrix4<float>(1.0f), Tilc::TVector3D<float>(1, 2, 3));
	PrintMatrix(m1);
	std::cout << "################################\n";
	*/
	glm::mat4 m = static_cast<glm::mat4>(q);
	return {
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3],
	};
	/*
	return
	{
		q.x, -q.y, -q.z, -q.w,
		q.y, q.x, q.w, -q.z,
		q.z, -q.w, q.x, q.y,
		q.w, q.z, -q.y, q.x
	};
	*/
}

/*
void Tilc::Graphics::PrintMatrix(glm::mat4 m)
{
	int w = 15;
	std::cout << std::setw(w) << m[0][0] << std::setw(w) << m[0][1] << std::setw(w) << m[0][2] << std::setw(w) << m[0][3] << std::endl
			  << std::setw(w) << m[1][0] << std::setw(w) << m[1][1] << std::setw(w) << m[1][2] << std::setw(w) << m[1][3] << std::endl
			  << std::setw(w) << m[2][0] << std::setw(w) << m[2][1] << std::setw(w) << m[2][2] << std::setw(w) << m[2][3] << std::endl
			  << std::setw(w) << m[3][0] << std::setw(w) << m[3][1] << std::setw(w) << m[3][2] << std::setw(w) << m[3][3] << std::endl << std::endl;
}

void Tilc::Graphics::PrintMatrix(Tilc::TMatrix4<float> m)
{
	int w = 15;
	std::cout << std::setw(w) << m[0][0] << std::setw(w) << m[0][1] << std::setw(w) << m[0][2] << std::setw(w) << m[0][3] << std::endl
			  << std::setw(w) << m[1][0] << std::setw(w) << m[1][1] << std::setw(w) << m[1][2] << std::setw(w) << m[1][3] << std::endl
			  << std::setw(w) << m[2][0] << std::setw(w) << m[2][1] << std::setw(w) << m[2][2] << std::setw(w) << m[2][3] << std::endl
			  << std::setw(w) << m[3][0] << std::setw(w) << m[3][1] << std::setw(w) << m[3][2] << std::setw(w) << m[3][3] << std::endl << std::endl;
}
*/