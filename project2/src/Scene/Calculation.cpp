#include "MainScene.h"
#include <glm/gtc/matrix_transform.hpp>

namespace CG
{
    glm::mat4 MainScene::translate(float x, float y, float z)
    {
    	glm::vec4 t = glm::vec4(x, y, z, 1);//w = 1 ,則x,y,z=0時也能translate
    	glm::vec4 c1 = glm::vec4(1, 0, 0, 0);
    	glm::vec4 c2 = glm::vec4(0, 1, 0, 0);
    	glm::vec4 c3 = glm::vec4(0, 0, 1, 0);
    	glm::mat4 M = glm::mat4(c1, c2, c3, t);
    	return M;
    }

    glm::mat4 MainScene::scale(float x, float y, float z)
    {
    	glm::vec4 c1 = glm::vec4(x, 0, 0, 0);
    	glm::vec4 c2 = glm::vec4(0, y, 0, 0);
    	glm::vec4 c3 = glm::vec4(0, 0, z, 0);
    	glm::vec4 c4 = glm::vec4(0, 0, 0, 1);
    	glm::mat4 M = glm::mat4(c1, c2, c3, c4);
    	return M;
    }

    glm::mat4 MainScene::rotate(float angle, float x, float y, float z)
    {
    	float r = glm::radians(angle);
    	glm::mat4 M = glm::mat4(1);

    	glm::vec4 c1 = glm::vec4(cos(r) + (1 - cos(r)) * x * x, (1 - cos(r)) * y * x + sin(r) * z, (1 - cos(r)) * z * x - sin(r) * y, 0);
    	glm::vec4 c2 = glm::vec4((1 - cos(r)) * y * x - sin(r) * z, cos(r) + (1 - cos(r)) * y * y, (1 - cos(r)) * z * y + sin(r) * x, 0);
    	glm::vec4 c3 = glm::vec4((1 - cos(r)) * z * x + sin(r) * y, (1 - cos(r)) * z * y - sin(r) * x, cos(r) + (1 - cos(r)) * z * z, 0);
    	glm::vec4 c4 = glm::vec4(0, 0, 0, 1);
    	M = glm::mat4(c1, c2, c3, c4);
    	return M;
    }

    MainScene::JointSet MainScene::calcutatingJointSet(double time,keyFrameSet frameSet)
	{
	    double timeL = 0,timeN = 0;
		int frameNow = 0,frameNext = 0;
		//need use while
	    for (int l = 0;l<frameSet.frames.size();l++)
		{
		    if(timeL + frameSet.frames[l].time >= time)
			{
			    frameNow = l;
				frameNext = frameSet.frames[l].nextFrameIndex;
				timeN = frameSet.frames[frameNow].time;
				break;
			}
			timeL += frameSet.frames[l].time;
		}

		JointSet dMovement;

		dMovement.head.pitch = frameSet.frames[frameNow].pose.head.pitch + (frameSet.frames[frameNext].pose.head.pitch-frameSet.frames[frameNow].pose.head.pitch)*((time-timeL)/timeN);
           dMovement.head.roll = frameSet.frames[frameNow].pose.head.roll + (frameSet.frames[frameNext].pose.head.roll-frameSet.frames[frameNow].pose.head.roll)*((time-timeL)/timeN);
           dMovement.head.yaw = frameSet.frames[frameNow].pose.head.yaw + (frameSet.frames[frameNext].pose.head.yaw-frameSet.frames[frameNow].pose.head.yaw)*((time-timeL)/timeN);

           dMovement.core.pitch = frameSet.frames[frameNow].pose.core.pitch + (frameSet.frames[frameNext].pose.core.pitch-frameSet.frames[frameNow].pose.core.pitch)*((time-timeL)/timeN);
           dMovement.core.roll = frameSet.frames[frameNow].pose.core.roll + (frameSet.frames[frameNext].pose.core.roll-frameSet.frames[frameNow].pose.core.roll)*((time-timeL)/timeN);
           dMovement.core.yaw = frameSet.frames[frameNow].pose.core.yaw + (frameSet.frames[frameNext].pose.core.yaw-frameSet.frames[frameNow].pose.core.yaw)*((time-timeL)/timeN);

           dMovement.downBody.pitch = frameSet.frames[frameNow].pose.downBody.pitch + (frameSet.frames[frameNext].pose.downBody.pitch-frameSet.frames[frameNow].pose.downBody.pitch)*((time-timeL)/timeN);
           dMovement.downBody.roll = frameSet.frames[frameNow].pose.downBody.roll + (frameSet.frames[frameNext].pose.downBody.roll-frameSet.frames[frameNow].pose.downBody.roll)*((time-timeL)/timeN);
           dMovement.downBody.yaw = frameSet.frames[frameNow].pose.downBody.yaw + (frameSet.frames[frameNext].pose.downBody.yaw-frameSet.frames[frameNow].pose.downBody.yaw)*((time-timeL)/timeN);

           dMovement.upRightArm.pitch = frameSet.frames[frameNow].pose.upRightArm.pitch + (frameSet.frames[frameNext].pose.upRightArm.pitch-frameSet.frames[frameNow].pose.upRightArm.pitch)*((time-timeL)/timeN);
           dMovement.upRightArm.roll = frameSet.frames[frameNow].pose.upRightArm.roll + (frameSet.frames[frameNext].pose.upRightArm.roll-frameSet.frames[frameNow].pose.upRightArm.roll)*((time-timeL)/timeN);
           dMovement.upRightArm.yaw = frameSet.frames[frameNow].pose.upRightArm.yaw + (frameSet.frames[frameNext].pose.upRightArm.yaw-frameSet.frames[frameNow].pose.upRightArm.yaw)*((time-timeL)/timeN);

           dMovement.upRightLeg.pitch = frameSet.frames[frameNow].pose.upRightLeg.pitch + (frameSet.frames[frameNext].pose.upRightLeg.pitch-frameSet.frames[frameNow].pose.upRightLeg.pitch)*((time-timeL)/timeN);
           dMovement.upRightLeg.roll = frameSet.frames[frameNow].pose.upRightLeg.roll + (frameSet.frames[frameNext].pose.upRightLeg.roll-frameSet.frames[frameNow].pose.upRightLeg.roll)*((time-timeL)/timeN);
           dMovement.upRightLeg.yaw = frameSet.frames[frameNow].pose.upRightLeg.yaw + (frameSet.frames[frameNext].pose.upRightLeg.yaw-frameSet.frames[frameNow].pose.upRightLeg.yaw)*((time-timeL)/timeN);

           dMovement.footRight.pitch = frameSet.frames[frameNow].pose.footRight.pitch + (frameSet.frames[frameNext].pose.footRight.pitch-frameSet.frames[frameNow].pose.footRight.pitch)*((time-timeL)/timeN);
           dMovement.footRight.roll = frameSet.frames[frameNow].pose.footRight.roll + (frameSet.frames[frameNext].pose.footRight.roll-frameSet.frames[frameNow].pose.footRight.roll)*((time-timeL)/timeN);
           dMovement.footRight.yaw = frameSet.frames[frameNow].pose.footRight.yaw + (frameSet.frames[frameNext].pose.footRight.yaw-frameSet.frames[frameNow].pose.footRight.yaw)*((time-timeL)/timeN);

           dMovement.handRight.pitch = frameSet.frames[frameNow].pose.handRight.pitch + (frameSet.frames[frameNext].pose.handRight.pitch-frameSet.frames[frameNow].pose.handRight.pitch)*((time-timeL)/timeN);
           dMovement.handRight.roll = frameSet.frames[frameNow].pose.handRight.roll + (frameSet.frames[frameNext].pose.handRight.roll-frameSet.frames[frameNow].pose.handRight.roll)*((time-timeL)/timeN);
           dMovement.handRight.yaw = frameSet.frames[frameNow].pose.handRight.yaw + (frameSet.frames[frameNext].pose.handRight.yaw-frameSet.frames[frameNow].pose.handRight.yaw)*((time-timeL)/timeN);

           dMovement.upLeftArm.pitch = frameSet.frames[frameNow].pose.upLeftArm.pitch + (frameSet.frames[frameNext].pose.upLeftArm.pitch-frameSet.frames[frameNow].pose.upLeftArm.pitch)*((time-timeL)/timeN);
           dMovement.upLeftArm.roll = frameSet.frames[frameNow].pose.upLeftArm.roll + (frameSet.frames[frameNext].pose.upLeftArm.roll-frameSet.frames[frameNow].pose.upLeftArm.roll)*((time-timeL)/timeN);
           dMovement.upLeftArm.yaw = frameSet.frames[frameNow].pose.upLeftArm.yaw + (frameSet.frames[frameNext].pose.upLeftArm.yaw-frameSet.frames[frameNow].pose.upLeftArm.yaw)*((time-timeL)/timeN);

           dMovement.upLeftLeg.pitch = frameSet.frames[frameNow].pose.upLeftLeg.pitch + (frameSet.frames[frameNext].pose.upLeftLeg.pitch-frameSet.frames[frameNow].pose.upLeftLeg.pitch)*((time-timeL)/timeN);
           dMovement.upLeftLeg.roll = frameSet.frames[frameNow].pose.upLeftLeg.roll + (frameSet.frames[frameNext].pose.upLeftLeg.roll-frameSet.frames[frameNow].pose.upLeftLeg.roll)*((time-timeL)/timeN);
           dMovement.upLeftLeg.yaw = frameSet.frames[frameNow].pose.upLeftLeg.yaw + (frameSet.frames[frameNext].pose.upLeftLeg.yaw-frameSet.frames[frameNow].pose.upLeftLeg.yaw)*((time-timeL)/timeN);

           dMovement.footLeft.pitch = frameSet.frames[frameNow].pose.footLeft.pitch + (frameSet.frames[frameNext].pose.footLeft.pitch-frameSet.frames[frameNow].pose.footLeft.pitch)*((time-timeL)/timeN);
           dMovement.footLeft.roll = frameSet.frames[frameNow].pose.footLeft.roll + (frameSet.frames[frameNext].pose.footLeft.roll-frameSet.frames[frameNow].pose.footLeft.roll)*((time-timeL)/timeN);
           dMovement.footLeft.yaw = frameSet.frames[frameNow].pose.footLeft.yaw + (frameSet.frames[frameNext].pose.footLeft.yaw-frameSet.frames[frameNow].pose.footLeft.yaw)*((time-timeL)/timeN);

           dMovement.handLeft.pitch = frameSet.frames[frameNow].pose.handLeft.pitch + (frameSet.frames[frameNext].pose.handLeft.pitch-frameSet.frames[frameNow].pose.handLeft.pitch)*((time-timeL)/timeN);
           dMovement.handLeft.roll = frameSet.frames[frameNow].pose.handLeft.roll + (frameSet.frames[frameNext].pose.handLeft.roll-frameSet.frames[frameNow].pose.handLeft.roll)*((time-timeL)/timeN);
           dMovement.handLeft.yaw = frameSet.frames[frameNow].pose.handLeft.yaw + (frameSet.frames[frameNext].pose.handLeft.yaw-frameSet.frames[frameNow].pose.handLeft.yaw)*((time-timeL)/timeN);

           dMovement.downRightArm = frameSet.frames[frameNow].pose.downRightArm + (frameSet.frames[frameNext].pose.downRightArm-frameSet.frames[frameNow].pose.downRightArm)*((time-timeL)/timeN);
           dMovement.downRightLeg = frameSet.frames[frameNow].pose.downRightLeg + (frameSet.frames[frameNext].pose.downRightLeg-frameSet.frames[frameNow].pose.downRightLeg)*((time-timeL)/timeN);


           dMovement.downLeftArm = frameSet.frames[frameNow].pose.downLeftArm + (frameSet.frames[frameNext].pose.downLeftArm-frameSet.frames[frameNow].pose.downLeftArm)*((time-timeL)/timeN);
           dMovement.downLeftLeg = frameSet.frames[frameNow].pose.downLeftLeg + (frameSet.frames[frameNext].pose.downLeftLeg-frameSet.frames[frameNow].pose.downLeftLeg)*((time-timeL)/timeN);

           return dMovement;
	}

	glm::vec3 MainScene::calcutatingPosition(double time,keyFrameSet frameSet)
	{
	    double timeL = 0,timeN = 0;
		int frameNow = 0,frameNext = 0;
		//need use while
	    for (int l = 0;l<frameSet.frames.size();l++)
		{
		    if(timeL + frameSet.frames[l].time >= time)
			{
			    frameNow = l;
				frameNext = frameSet.frames[l].nextFrameIndex;
				timeN = frameSet.frames[frameNow].time;
				break;
			}
			timeL += frameSet.frames[l].time;
		}

		glm::vec3 dOffset;
		dOffset.x = frameSet.frames[frameNow].offset.x + (frameSet.frames[frameNext].offset.x-frameSet.frames[frameNow].offset.x)*((time-timeL)/timeN);
           dOffset.y = frameSet.frames[frameNow].offset.y + (frameSet.frames[frameNext].offset.y-frameSet.frames[frameNow].offset.y)*((time-timeL)/timeN);
           dOffset.z = frameSet.frames[frameNow].offset.z + (frameSet.frames[frameNext].offset.z-frameSet.frames[frameNow].offset.z)*((time-timeL)/timeN);

           return dOffset;
	}

	MainScene::ChidoriSet MainScene::calcutatingChidori(double time,keyFrameSet frameSet)
	{
	        double timeL = 0,timeN = 0;
            int frameNow = 0,frameNext = 0;
            for (int l = 0;l<frameSet.frames.size();l++)
            {
                if(timeL + frameSet.frames[l].time >= time)
                {
                    frameNow = l;
                    break;
                }
                timeL += frameSet.frames[l].time;
            }
            ChidoriSet attribute;
            attribute = frameSet.frames[frameNow].chidori;
            return attribute;
	}

	MainScene::fireBallSet MainScene::calcutatingFireball(double time,keyFrameSet frameSet)
	{
	        double timeL = 0,timeN = 0;
            int frameNow = 0,frameNext = 0;
            for (int l = 0;l<frameSet.frames.size();l++)
            {
                if(timeL + frameSet.frames[l].time >= time)
                {
                    frameNow = l;
                    break;
                }
                timeL += frameSet.frames[l].time;
            }
            fireBallSet attribute;
            attribute.offset = frameSet.frames[frameNow].fireball.offset;
    		attribute.intensity = frameSet.frames[frameNow].fireball.intensity;
            attribute.speed = frameSet.frames[frameNow].fireball.speed;
    		attribute.show = frameSet.frames[frameNow].fireball.show;
            return attribute;
	}

	std::vector<MainScene::ActiveFirework> MainScene::EvaluateFireworksAtTime(double currentTime, const keyFrameSet& frameSet)
    {
        std::vector<ActiveFirework> activeList;

        for (const auto& trigger : frameSet.fireworkTrack)
        {
            double age = currentTime - trigger.triggerTime;
            double lifespan = FIREWORK_DEFAULT / trigger.attributes.speed;

            if (age >= 0.0 && age <= lifespan)
            {
                ActiveFirework fw;
                fw.attributes = trigger.attributes;
                fw.age = age;
                activeList.push_back(fw);
            }
        }
        return activeList;
    }

    std::vector<MainScene::ActiveFirework> MainScene::EvaluateTempFireworksAtTime(double currentTime)
    {
        std::vector<ActiveFirework> result;

        double age = currentTime - tempFirework.triggerTime;
        double lifespan = FIREWORK_DEFAULT / tempFirework.attributes.speed;

        // 只有在存活期間內，才生成實體並塞入陣列
        if (age >= 0.0 && age <= lifespan)
        {
            ActiveFirework tempState;
            tempState.attributes = tempFirework.attributes;
            tempState.age = age;
            result.push_back(tempState); // 陣列大小變成 1
        }

        // 如果時間還沒到或已經死了，就會回傳空的陣列 (大小為 0)
        return result;
    }
}
