#include "Camera.h"
#include <iostream>

namespace CG
{
    Camera::Camera()
    {
        projection = glm::mat4(1.0);
        view = glm::mat4(1.0);

        fov = 80.0f;
        aspect = 4.0f / 3.0f;
        clipNear = 0.01f;
        clipFar = 2000.0f;

        UpdateProjectionMatrix();
    }

    Camera::~Camera()
    {

    }

    void Camera::LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
    {
        view = glm::lookAt(eye, center, up);
    }


    void Camera::UpdateProjectionMatrix()
    {
        projection = glm::perspective(glm::radians(fov), aspect, clipNear, clipFar);
    }

    void Camera::UpdatePosition(glm::vec3 movement)
    {
        position += movement;
    }



    void Camera::UpdateFront()
    {
        front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        front = normalize(front);
        UpdateRight();
        UpdateUP();
    }

    void Camera::UpdatePitch(double dy, glm::vec3 nfront)
    {
        if (dy>=90)
        {
            dy = 89;
        }else if (dy<=-90)
        {
            dy = -89;
        }
        pitch = glm::degrees(asin(nfront.y)) + dy;
    }

    void Camera::UpdateYaw(double dx, glm::vec3 nfront)
    {
        yaw = glm::degrees(atan2(nfront.z, nfront.x)) + dx;
    }

    void Camera::UpdateRight()
    {
        right = normalize(cross(front, glm::vec3(0, 1, 0)));
    }

    void Camera::UpdateUP()
    {
        up = normalize(cross(right, front));
    }

    void Camera::printInfo()
    {
        std::cout << "Position: " << position.x << " " << position.y << " " << position.z << std::endl;
        std::cout << "Front: " << front.x << " " << front.y << " " << front.z << std::endl;
        std::cout << "up: " << up.x << " " << up.y << " " << up.z << std::endl;
    }
}
