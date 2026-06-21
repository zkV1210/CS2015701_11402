#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CG
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        void UpdatePosition(glm::vec3 movement);

        void UpdateFront();

        void UpdatePitch(double dy, glm::vec3 nfront);

        void UpdateYaw(double dx, glm::vec3 nfront);

        void UpdateUP();

        void UpdateRight();

        void printInfo();

        void LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

        auto GetProjectionMatrix() -> const glm::mat4
        {
            return projection;
        }

        auto GetViewMatrix() -> const glm::mat4
        {
            return view;
        }

        void SetFov(float degree)
        {
            fov = degree;

            UpdateProjectionMatrix();
        }

        void SetAspect(float ratio)
        {
            aspect = ratio;

            UpdateProjectionMatrix();
        }

        void SetClip(float near, float far)
        {
            clipNear = near;
            clipFar = far;

            UpdateProjectionMatrix();
        }

        glm::vec3 getPosition()
        {
            return position;
        }

        glm::vec3 getFront()
        {
            return front;
        }

        glm::vec3 getUp()
        {
            return up;
        }

        glm::vec3 getRight()
        {
            return right;
        }


    private:
        void UpdateProjectionMatrix();

    private:
        glm::mat4 projection;
        glm::mat4 view;

        float fov;
        float aspect;
        float clipNear;
        float clipFar;
        glm::vec3 position = glm::vec3(0, 5, 30);
        glm::vec3 front = glm::vec3(0, 0, -1);
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = glm::vec3(1, 0, 0);

        double yaw = 0;
        double pitch = 0;
    };
}
