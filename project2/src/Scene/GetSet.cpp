#include "MainScene.h"
#include <iostream>

namespace CG
{
    void MainScene::ResetAction()
	{
		this->action = 0; // idle
	}

	void MainScene::SetAction(int action)
	{
	    if(!edit)
		{
		    ResetJointSet();
		    this->action = action;
		}
		else
		{
		    std::cout << "In Edit Mode\n";
		}
	}

	void MainScene::SetEdit()
	{
	    edit = !edit;
	}

	void MainScene::SetInstanced(bool enable)
	{
	    isInstanced = enable;
	}

	bool MainScene::GetInstanced()
	{
		return isInstanced;
	}

	void MainScene::SetMode(int mode)
	{
		switch (mode)
		{
		case 0:
			this->mode = GL_FILL;
			break;
		case 1:
			this->mode = GL_LINE;
			break;
		default:
			this->mode = GL_FILL;
			break;
		}
	}

	void MainScene::SetJoint(float pitch, float roll, float yaw, int part)
	{
	    switch(part)
		{
		    case 0:
				rendering.core.pitch = pitch;
                   rendering.core.roll = roll;
                   rendering.core.yaw = yaw;
                   break;
			case 1:
                   rendering.head.pitch = pitch;
                   rendering.head.roll = roll;
                   rendering.head.yaw = yaw;
                   break;
               case 2:
                   rendering.upLeftArm.pitch = pitch;
                   rendering.upLeftArm.roll = roll;
                   rendering.upLeftArm.yaw = yaw;
                   break;
               case 3:
                   rendering.downLeftArm = pitch;
                   break;
               case 4:
                   rendering.handRight.pitch = pitch;
                   rendering.handRight.roll = roll;
                   rendering.handRight.yaw = yaw;
                   break;
               case 5:
                   rendering.upRightArm.pitch = pitch;
                   rendering.upRightArm.roll = roll;
                   rendering.upRightArm.yaw = yaw;
                   break;
               case 6:
                   rendering.downRightArm = pitch;
                   break;
               case 7:
                   rendering.handLeft.pitch = pitch;
                   rendering.handLeft.roll = roll;
                   rendering.handLeft.yaw = yaw;
                   break;
               case 8:
                   rendering.downBody.pitch = pitch;
                   rendering.downBody.roll = roll;
                   rendering.downBody.yaw = yaw;
                   break;
               case 9:
                   rendering.upLeftLeg.pitch = pitch;
                   rendering.upLeftLeg.roll = roll;
                   rendering.upLeftLeg.yaw = yaw;
                   break;
               case 10:
                   rendering.downLeftLeg = pitch;
                   break;
               case 11:
                   rendering.footLeft.pitch = pitch;
                   rendering.footLeft.roll = roll;
                   rendering.footLeft.yaw = yaw;
                   break;
               case 12:
                   rendering.upRightLeg.pitch = pitch;
                   rendering.upRightLeg.roll = roll;
                   rendering.upRightLeg.yaw = yaw;
                   break;
               case 13:
                   rendering.downRightLeg = pitch;
                   break;
               case 14:
                   rendering.footRight.pitch = pitch;
                   rendering.footRight.roll = roll;
                   rendering.footRight.yaw = yaw;
                   break;
		}
	}

	void MainScene::SetOffset(float x, float y, float z)
	{
	    UpdateOffset(x,y,z);
	}

	MainScene::JointSet MainScene::GetJoint()
	{
	    return rendering;
	}

	glm::vec3 MainScene::GetOffset()
	{
	    return rootOffset;
	}

	void MainScene::InsertKeyFrame(double timeSpan, int index)
	{
	    keyFrame frame0;
		frame0.offset = rootOffset;
		frame0.time = timeSpan;
		Memory.totalTimeSpan += frame0.time;
		frame0.pose = rendering;
		frame0.chidori = chidoriAttribute;
		frame0.fireball = fireBallAttibute;

        if (index < 0)
        {
            index = 0;
        }
        if (Memory.frames.size()&&Memory.frames.size()-1<index)
		{
		    Memory.frames.push_back(frame0);
		}
        else if (Memory.frames.size())
        {
            Memory.frames.insert(Memory.frames.begin() + index,frame0);
        }
        else
        {
            Memory.frames.push_back(frame0);
        }
        for (int i = 0;i<Memory.frames.size();i++)
        {
            if (i!=Memory.frames.size()-1)
            {
                Memory.frames[i].nextFrameIndex = i+1;
            }
            else
            {
                Memory.frames[i].nextFrameIndex = 0;
            }
        }
	}

	void MainScene::DeleteKeyFrame(int index)
	{
           if (index < 0)
           {
               index = 0;
           }
           if (index < Memory.frames.size())
           {
               Memory.totalTimeSpan -= Memory.frames[index].time;
               Memory.frames.erase(Memory.frames.begin() + index);
           }
           for (int i = 0;i<Memory.frames.size();i++)
           {
               if (i!=Memory.frames.size()-1)
               {
                   Memory.frames[i].nextFrameIndex = i+1;
               }
               else
               {
                   Memory.frames[i].nextFrameIndex = 0;
               }
           }
	}

	void MainScene::AdjustKeyFrame(int index)
	{
           if (index < 0)
           {
               index = 0;
           }
           if (Memory.frames.size()>index)
		{
		    Memory.frames[index].offset = rootOffset;
			Memory.frames[index].pose = rendering;
			Memory.frames[index].offset = rootOffset;
			Memory.frames[index].chidori = chidoriAttribute;
			Memory.frames[index].fireball = fireBallAttibute;
		}
	}

	void MainScene::AdjustTimeSpan(double timeSpan, int index)
	{
	    if (index < 0) index = 0;
		if (timeSpan<=0) timeSpan = 0.01;
           if(index < Memory.frames.size())
           {
               Memory.totalTimeSpan -= Memory.frames[index].time;
               Memory.frames[index].time = timeSpan;
               Memory.totalTimeSpan += Memory.frames[index].time;
           }
	}

	void MainScene::ShowKeyFrame(int index)
	{
	    if (index < 0) index = 0;
    	    if(index < Memory.frames.size())
		{
		    UpdateJointSet(Memory.frames[index].pose);
			UpdateOffset(Memory.frames[index].offset.x,Memory.frames[index].offset.y,Memory.frames[index].offset.z);
		}
	}

	int MainScene::GetMemoryFrameNum()
	{
	    return Memory.frames.size();
	}

	void MainScene::ResetMemory()
    {
        Memory.frames.clear();
        Memory.totalTimeSpan = 0;
    }

    void MainScene::WriteIntoJson()
    {
        SaveKeyFrameSet(&Memory, "res/animations/Memory.json");
    }

	void MainScene::SetAnimationSpeed(double multiplier)
	{
	    if (multiplier>=0)
		{
		    playSpeed = multiplier;
		}
	    else
		{
		    playSpeed = 0;
		}
	}

	void MainScene::ResetJointSet()
	{
            rendering.head.pitch = 0;
            rendering.head.roll = 0;
            rendering.head.yaw = 0;

            rendering.core.pitch = 0;
            rendering.core.roll = 0;
            rendering.core.yaw = 0;

            rendering.downBody.pitch = 0;
            rendering.downBody.roll = 0;
            rendering.downBody.yaw = 0;

            rendering.upRightArm.pitch = 0;
            rendering.upRightArm.roll = 0;
            rendering.upRightArm.yaw = 0;

            rendering.upRightLeg.pitch = 0;
            rendering.upRightLeg.roll = 0;
            rendering.upRightLeg.yaw = 0;

            rendering.upLeftArm.pitch = 0;
            rendering.upLeftArm.roll = 0;
            rendering.upLeftArm.yaw = 0;

            rendering.upLeftLeg.pitch = 0;
            rendering.upLeftLeg.roll = 0;
            rendering.upLeftLeg.yaw = 0;

            rendering.footRight.pitch = 0;
            rendering.footRight.roll = 0;
            rendering.footRight.yaw = 0;

            rendering.handRight.pitch = 0;
            rendering.handRight.roll = 0;
            rendering.handRight.yaw = 0;

            rendering.footLeft.pitch = 0;
            rendering.footLeft.roll = 0;
            rendering.footLeft.yaw = 0;

            rendering.handLeft.pitch = 0;
            rendering.handLeft.roll = 0;
            rendering.handLeft.yaw = 0;

            rendering.downRightArm = 0;
            rendering.downRightLeg = 0;

            rendering.downLeftArm = 0;
            rendering.downLeftLeg = 0;
	}
}
