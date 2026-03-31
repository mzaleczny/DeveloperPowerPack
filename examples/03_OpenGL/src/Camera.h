#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
enum E_DIRECTION
{
    ED_FWD,
    ED_BWD,
    ED_LFT,
    ED_RGT
};


class Camera
{
public:
    glm::vec3 m_position = glm::vec3(0.f, 0.f, 0.f);
    float m_xangle = 0.f;
    float m_yangle = 0.f;
    glm::mat4 m_viewMat;
    glm::quat m_rotation;

    static constexpr float MOUSE_SENSITIVITY = 0.06f;
    static constexpr float SPEED = 7.5f;
public:
    void rotate(float dx, float dy)
    {
        m_xangle += MOUSE_SENSITIVITY * dy;
        m_yangle += MOUSE_SENSITIVITY * dx;

        glm::quat qx = glm::angleAxis(glm::radians(m_xangle), glm::vec3(1.f, 0.f, 0.f));
        glm::quat qy = glm::angleAxis(glm::radians(m_yangle), glm::vec3(0.f, 1.f, 0.f));
        m_rotation = glm::normalize(qx * qy);
    }

    void move(E_DIRECTION direction, float dt)
    {
        glm::vec3 right = glm::vec3(glm::row(m_viewMat, 0));
        glm::vec3 fwd = glm::vec3(glm::row(m_viewMat, 2));
        glm::vec3 dir;
        switch (direction)
        {
        case ED_FWD: dir = -fwd; break;
        case ED_BWD: dir = fwd; break;
        case ED_LFT: dir = -right; break;
        case ED_RGT: dir = right; break;
        }

        m_position += dir * dt * SPEED;
    }

    glm::mat4 updateView()
    {
        return m_viewMat = static_cast<glm::mat4>(m_rotation) * glm::translate(glm::mat4(1.f), -m_position);
    }

    Tilc::TMatrix4<float> toMat4()
    {
        glm::mat4 m = static_cast<glm::mat4>(m_rotation);
        std::cout << "GLM Quat: " << m_rotation.x << "  " << m_rotation.y << "  " << m_rotation.z << "  " << m_rotation.w << std::endl;
        std::cout << "GLM mat:\n";
        std::cout << std::setw(10) << std::setprecision(4) << m[0][0]
            << std::setw(10) << std::setprecision(4) << m[1][0]
            << std::setw(10) << std::setprecision(4) << m[2][0]
            << std::setw(10) << std::setprecision(4) << m[3][0] << std::endl;
        std::cout << std::setw(10) << std::setprecision(4) << m[0][1]
            << std::setw(10) << std::setprecision(4) << m[1][1]
            << std::setw(10) << std::setprecision(4) << m[2][1]
            << std::setw(10) << std::setprecision(4) << m[3][1] << std::endl;
        std::cout << std::setw(10) << std::setprecision(4) << m[0][2]
            << std::setw(10) << std::setprecision(4) << m[1][2]
            << std::setw(10) << std::setprecision(4) << m[2][2]
            << std::setw(10) << std::setprecision(4) << m[3][2] << std::endl;
        std::cout << std::setw(10) << std::setprecision(4) << m[0][3]
            << std::setw(10) << std::setprecision(4) << m[1][3]
            << std::setw(10) << std::setprecision(4) << m[2][3]
            << std::setw(10) << std::setprecision(4) << m[3][3] << std::endl;
        std::cout << "\n------------------" << std::endl;

        m_viewMat = static_cast<glm::mat4>(m_rotation) * glm::translate(glm::mat4(1.f), -m_position);
        return {
            m_viewMat[0][0], m_viewMat[1][0], m_viewMat[2][0], m_viewMat[3][0],
            m_viewMat[0][1], m_viewMat[1][1], m_viewMat[2][1], m_viewMat[3][1],
            m_viewMat[0][2], m_viewMat[1][2], m_viewMat[2][2], m_viewMat[3][2],
            m_viewMat[0][3], m_viewMat[1][3], m_viewMat[2][3], m_viewMat[3][3]
        };
    }
};

#endif