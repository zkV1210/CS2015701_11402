#include "MainScene.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CG
{
    void MainScene::UpdateOffset(float x,float y,float z)
	{
        rootOffset = glm::vec3(x, y, z);
	}

	void MainScene::UpdateJointSet(JointSet frame)
	{
	    rendering = frame;
	}

	void MainScene::UpdateChidori(ChidoriSet attribute)
	{
	    chidoriAttribute = attribute;
	}

	void MainScene::UpdateFireball(fireBallSet attribute)
	{
	    fireBallAttibute = attribute;
	}

	void MainScene::UpdateJointInstance(JointSet frame, int index)
		{
			if (index >= 0 && index < 5) {
				instanceRendering[index] = frame;
			}
		}

	void MainScene::UpdateOffsetInstance(float x, float y, float z, int index)
	{
		if (index >= 0 && index < 5) {
			instanceRootOffset[index] = glm::vec3(x, y, z);
		}
	}

	void MainScene::UpdateFireWork(const std::vector<ActiveFirework>& newFireworks)
    {
        fireworkRendering = newFireworks;
    }
}
