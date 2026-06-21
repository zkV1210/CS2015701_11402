#include "MainScene.h"

#include <GLFW/glfw3.h>

namespace CG
{
	MainScene::MainScene()
	{
	}

	MainScene::~MainScene()
	{
	}

	auto MainScene::Initialize() -> bool
	{
		return LoadScene();
	}

	void MainScene::Update(double dt)
	{
		UpdateAction(dt);
		if (isInstanced)
		{
			UpdateInstanceModel(); // ºâ 5 °¦
		}
		else
		{
			UpdateModel();         // ºâ 1 °¦
		}

        if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            rotatingView = true;
            if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                movingView = true;
                rotatingView = false;
            }
            else
                movingView = false;
        }
        else
        {
            rotatingView = false;
			movingView = false;
        }
	}
}
