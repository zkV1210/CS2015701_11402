#include "MainScene.h"
#include <imgui.h>
#include <iostream>
#include <GLFW/glfw3.h>

namespace CG
{
    /* 鍵盤滑鼠互動區 */
	void MainScene::OnResize(int width, int height)
	{
        fboWidth = width;
        fboHeight = height;
        if (waterColorTexture != 0) {
            // 更新 FBO 的顏色貼圖 (Color Texture) 大小
            glBindTexture(GL_TEXTURE_2D, waterColorTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        }

        if (waterDepthRBO != 0) {
            // 更新 FBO 的深度緩衝區 (Renderbuffer) 大小
            glBindRenderbuffer(GL_RENDERBUFFER, waterDepthRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        }

        if (mainColorTexture != 0) {
            // 更新 FBO 的顏色貼圖 (Color Texture) 大小
            glBindTexture(GL_TEXTURE_2D, mainColorTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        }

        if (mainDepthRBO != 0) {
            // 更新 FBO 的深度緩衝區 (Renderbuffer) 大小
            glBindRenderbuffer(GL_RENDERBUFFER, mainDepthRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        }

        if (pingpongTexture[0] != 0) {
            for (int i = 0; i < 2; i++)
            {
                glBindTexture(GL_TEXTURE_2D, pingpongTexture[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            }
        }

		std::cout << "MainScene Resize: " << width << " " << height << std::endl;
	}

	void MainScene::OnKeyboard(int key, int action)
	{
		//std::cout << "MainScene OnKeyboard: " << key << " " << action << std::endl;

		if(action == GLFW_REPEAT || action == GLFW_PRESS)
		{
       		switch (key)
       		{
             		case GLFW_KEY_W:
            			camera.UpdatePosition(camera.getFront() * 0.5f);
            			break;
             		case GLFW_KEY_S:
            			camera.UpdatePosition(camera.getFront() * -0.5f);
            			break;
             		case GLFW_KEY_A:
            			camera.UpdatePosition(camera.getRight() * -0.5f);
            			break;
             		case GLFW_KEY_D:
            			camera.UpdatePosition(camera.getRight() * 0.5f);
            			break;
             		case GLFW_KEY_Q:
            			camera.UpdatePosition(camera.getUp() * -0.5f);
            			break;
                   case GLFW_KEY_E:
                       camera.UpdatePosition(camera.getUp() * 0.5f);
                       break;
       		}
		}
	}

       void MainScene::OnMouseButton(int button, int action)
       {
           ImGuiIO& io = ImGui::GetIO();

           //std::cout << movingView << " " << io.WantCaptureMouse << std::endl;

           if (io.WantCaptureMouse)
               return;

           //std::cout << "MainScene OnMouseButton: " << button << " " << action << std::endl;
           MouseButton = button;
           MouseAction = action;

       }

       void MainScene::OnMouseMove(double x, double y)
       {
           ImGuiIO& io = ImGui::GetIO();

           if (io.WantCaptureMouse)
               return;

           if (rotatingView)
           {
               dx = x - lastX;
               dy = y - lastY;
			camera.UpdateYaw(dx * 0.1f, camera.getFront());
			camera.UpdatePitch(-dy * 0.1f, camera.getFront());
			camera.UpdateFront();
               camera.LookAt(camera.getPosition(), camera.getPosition() + camera.getUp(), glm::vec3(0, 1, 0));
           }
           if (movingView)
           {
			dx = x - lastX;
			dy = y - lastY;
			camera.UpdatePosition(camera.getRight() * (dx) * -0.05f + camera.getUp() * dy * 0.05f);
           }
           lastX = x;
           lastY = y;
       }

       void MainScene::OnMouseScroll(double x_offset, double y_offset)
       {
           ImGuiIO& io = ImGui::GetIO();

           if (io.WantCaptureMouse)
               return;

		camera.UpdatePosition(glm::vec3 (0.0f, 0.0f, -y_offset));

       }

}
