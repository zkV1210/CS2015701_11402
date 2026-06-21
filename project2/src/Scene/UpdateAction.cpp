#include "MainScene.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CG
{
    void MainScene::UpdateAction(double dt)
	{
		static double _frame = 0;
		static double _frameForSCJ = 0;
		static double _frameForInstance[5] = {0,0,0,0,0};
		if (edit && editFireWork)
		{
		    SetInstanced(false);
			UpdateOffset(rootOffset.x, rootOffset.y, rootOffset.z);

 			if (Memory.frames.size())
 			{
                if (editTimeLine>=Memory.totalTimeSpan)
                {
                    editTimeLine -= Memory.totalTimeSpan;
                }
                JointSet pose = calcutatingJointSet(editTimeLine,Memory);
                UpdateJointSet(pose);
                glm::vec3 position = calcutatingPosition(editTimeLine,Memory);
                UpdateOffset(position.x, position.y, position.z);
                ChidoriSet chidoriAt = calcutatingChidori(editTimeLine,Memory);
                UpdateChidori(chidoriAt);
                fireBallSet fireballAt = calcutatingFireball(editTimeLine,Memory);
                UpdateFireball(fireballAt);

                std::vector<ActiveFirework> newFireworks = EvaluateFireworksAtTime(editTimeLine, Memory);
                if (editCreateFireWork)
                {
                    std::vector<ActiveFirework> tempActive = EvaluateTempFireworksAtTime(editTimeLine);
                    if (tempActive.size() == 1)
                    {
                        newFireworks.push_back(tempActive[0]);
                    }
                }
                UpdateFireWork(newFireworks);
 			}
            else
            {
                ResetJointSet();
                UpdateOffset(0, 16, 0);
            }
		}
		else if (edit)
		{
		    SetInstanced(false);
		    UpdateOffset(rootOffset.x, rootOffset.y, rootOffset.z);
            UpdateJointSet(rendering);
		}
		else
		{
      		if (action == 0)
      		{
                _frameForSCJ = 0;
                SetInstanced(false);
                _frame = 0;
                UpdateOffset(0, 16, 0);
                ResetJointSet();
                std::vector<ActiveFirework> newFireworks;
                UpdateFireWork(newFireworks);
                ChidoriSet chidoriAt;
                UpdateChidori(chidoriAt);
                fireBallSet fireballAt;
                UpdateFireball(fireballAt);
      		}
      		else if(action == 1) //walk
      		{
                SetInstanced(false);
                _frameForSCJ = 0;
                _frame += dt * playSpeed;
                if (_frame>=walk.totalTimeSpan)
                {
                    _frame -= walk.totalTimeSpan;
                }
                JointSet pose = calcutatingJointSet(_frame,walk);
                UpdateJointSet(pose);
                glm::vec3 position = calcutatingPosition(_frame,walk);
                UpdateOffset(position.x, position.y, position.z);
                ChidoriSet chidoriAt = calcutatingChidori(_frame,walk);
                UpdateChidori(chidoriAt);
                std::vector<ActiveFirework> newFireworks = EvaluateFireworksAtTime(_frame, walk);
                UpdateFireWork(newFireworks);
                fireBallSet fireballAt = calcutatingFireball(_frame,walk);
                UpdateFireball(fireballAt);
      		}
      		else if (action == 2) //仰臥起坐
      		{
                SetInstanced(false);
                _frameForSCJ = 0;
      		    _frame += dt * playSpeed;
     			if (_frame>=sitUp.totalTimeSpan)
     			{
     			    _frame -= sitUp.totalTimeSpan;
     			}
      			   JointSet pose = calcutatingJointSet(_frame,sitUp);
                UpdateJointSet(pose);
                glm::vec3 position = calcutatingPosition(_frame,sitUp);
                UpdateOffset(position.x, position.y, position.z);
                ChidoriSet chidoriAt = calcutatingChidori(_frame,sitUp);
                UpdateChidori(chidoriAt);
                std::vector<ActiveFirework> newFireworks = EvaluateFireworksAtTime(_frame, sitUp);
                UpdateFireWork(newFireworks);
                fireBallSet fireballAt = calcutatingFireball(_frame,sitUp);
                UpdateFireball(fireballAt);
      		}
      		else if (action == 3) //伏地挺身
      		{
                SetInstanced(false);
                _frameForSCJ = 0;
                _frame += dt * playSpeed;
                if (_frame>=pushUp.totalTimeSpan)
                {
     			    _frame -= pushUp.totalTimeSpan;
     			}
     			JointSet pose = calcutatingJointSet(_frame,pushUp);
                UpdateJointSet(pose);
                glm::vec3 position = calcutatingPosition(_frame,pushUp);
                UpdateOffset(position.x, position.y, position.z);
                ChidoriSet chidoriAt = calcutatingChidori(_frame,pushUp);
                UpdateChidori(chidoriAt);
                std::vector<ActiveFirework> newFireworks = EvaluateFireworksAtTime(_frame, pushUp);
                UpdateFireWork(newFireworks);
                fireBallSet fireballAt = calcutatingFireball(_frame,pushUp);
                UpdateFireball(fireballAt);
      		}
      		else if (action == 4) // hopak dance
      		{
                SetInstanced(false);
                _frameForSCJ = 0;
                _frame += dt * playSpeed;
                if (_frame>=hopakDance.totalTimeSpan)
                {
                        _frame -= hopakDance.totalTimeSpan;
                }
                JointSet pose = calcutatingJointSet(_frame,hopakDance);
                UpdateJointSet(pose);
                glm::vec3 position = calcutatingPosition(_frame,hopakDance);
                UpdateOffset(position.x, position.y, position.z);
                ChidoriSet chidoriAt = calcutatingChidori(_frame,hopakDance);
                UpdateChidori(chidoriAt);
                std::vector<ActiveFirework> newFireworks = EvaluateFireworksAtTime(_frame, hopakDance);
                UpdateFireWork(newFireworks);
                fireBallSet fireballAt = calcutatingFireball(_frame,hopakDance);
                UpdateFireball(fireballAt);
      		}
            else if (action == 5) // APT dance
      		{
                SetInstanced(false);
                _frameForSCJ = 0;
      		    _frame += dt * playSpeed;
                if (_frame>=AptDance.totalTimeSpan)
                {
                    _frame -= AptDance.totalTimeSpan;
                }
                JointSet pose = calcutatingJointSet(_frame,AptDance);
                UpdateJointSet(pose);
                glm::vec3 position = calcutatingPosition(_frame,AptDance);
                UpdateOffset(position.x, position.y, position.z);
                ChidoriSet chidoriAt = calcutatingChidori(_frame,AptDance);
                UpdateChidori(chidoriAt);
                std::vector<ActiveFirework> newFireworks = EvaluateFireworksAtTime(_frame, AptDance);
                UpdateFireWork(newFireworks);
                fireBallSet fireballAt = calcutatingFireball(_frame,AptDance);
                UpdateFireball(fireballAt);
      		}
      		else if (action == 6) //記憶功能
      		{
                SetInstanced(false);
                _frameForSCJ = 0;
     			_frame += dt * playSpeed;
     			if (_frame>=Memory.totalTimeSpan)
                {
                    _frame -= Memory.totalTimeSpan;
                }
     			if (Memory.frames.size())
     			{
                    JointSet pose = calcutatingJointSet(_frame,Memory);
                    UpdateJointSet(pose);
                    glm::vec3 position = calcutatingPosition(_frame,Memory);
                    UpdateOffset(position.x, position.y, position.z);
                    ChidoriSet chidoriAt = calcutatingChidori(_frame,Memory);
                    UpdateChidori(chidoriAt);
                    fireBallSet fireballAt = calcutatingFireball(_frame,Memory);
                    UpdateFireball(fireballAt);
                    std::vector<ActiveFirework> newFireworks = EvaluateFireworksAtTime(_frame, Memory);
                    UpdateFireWork(newFireworks);

     			}
                else
                {
                    ResetJointSet();
                    UpdateOffset(0, 16, 0);
                }
            }
            else if (action == 7)
            {
                _frameForSCJ += dt * playSpeed;
                if (_frameForSCJ <= SCJ.totalTimeSpan)
                {
                    SetInstanced(false);
                    JointSet pose = calcutatingJointSet(_frameForSCJ,SCJ);
                    UpdateJointSet(pose);
                    glm::vec3 position = calcutatingPosition(_frameForSCJ,SCJ);
                    UpdateOffset(position.x, position.y, position.z);
                    ChidoriSet chidoriAt = calcutatingChidori(_frame,SCJ);
                    UpdateChidori(chidoriAt);
                    fireBallSet fireballAt = calcutatingFireball(_frame,SCJ);
                    UpdateFireball(fireballAt);
                }
                else
                {
                    SetInstanced(true);
                    std::vector<ActiveFirework> newFireworks;
                    UpdateFireWork(newFireworks);
                    ChidoriSet chidoriAt;
                    UpdateChidori(chidoriAt);
                    fireBallSet fireballAt;
                    UpdateFireball(fireballAt);
                    for (int i = 0;i<5;i++)
                    {
                        _frameForInstance[i] += dt * playSpeed;;
                        JointSet pose;
                        glm::vec3 position = {0,0,0};
                        switch (i){
                            case 0:
                                if (_frameForInstance[i]>=walk.totalTimeSpan)
                                {
                                    _frameForInstance[i] -= walk.totalTimeSpan;
                                }

                                pose = calcutatingJointSet(_frameForInstance[i],walk);
                                UpdateJointInstance(pose, i);
                                position = calcutatingPosition(_frameForInstance[i],walk);
                                UpdateOffsetInstance(position.x, position.y, position.z, i);
                                break;
                            case 1:
                                if (_frameForInstance[i]>=sitUp.totalTimeSpan)
                                {
                                    _frameForInstance[i] -= sitUp.totalTimeSpan;
                                }
                                pose = calcutatingJointSet(_frameForInstance[i],sitUp);
                                UpdateJointInstance(pose, i);
                                position = calcutatingPosition(_frameForInstance[i],sitUp);
                                UpdateOffsetInstance(position.x + -10, position.y, position.z + 5, i);
                                break;
                            case 2:
                                if (_frameForInstance[i]>=pushUp.totalTimeSpan)
                                {
                                    _frameForInstance[i] -= pushUp.totalTimeSpan;
                                }
                                pose = calcutatingJointSet(_frameForInstance[i],pushUp);
                                UpdateJointInstance(pose, i);
                                position = calcutatingPosition(_frameForInstance[i],pushUp);
                                UpdateOffsetInstance(position.x + -20, position.y, position.z + 10, i);
                                break;
                            case 3:
                                if (_frameForInstance[i]>=hopakDance.totalTimeSpan)
                                {
                                    _frameForInstance[i] -= hopakDance.totalTimeSpan;
                                }
                                pose = calcutatingJointSet(_frameForInstance[i],hopakDance);
                                UpdateJointInstance(pose, i);
                                position = calcutatingPosition(_frameForInstance[i],hopakDance);
                                UpdateOffsetInstance(position.x + 10, position.y, position.z + 5, i);
                                break;
                            case 4:
                                if (_frameForInstance[i]>=AptDance.totalTimeSpan)
                                {
                                    _frameForInstance[i] -= AptDance.totalTimeSpan;
                                }
                                pose = calcutatingJointSet(_frameForInstance[i],AptDance);
                                UpdateJointInstance(pose, i);
                                position = calcutatingPosition(_frameForInstance[i],AptDance);
                                UpdateOffsetInstance(position.x + 20, position.y, position.z + 10, i);
                                break;
                        }
                    }
                }
            }
		}
	}
}
