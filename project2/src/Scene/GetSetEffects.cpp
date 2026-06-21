#include "MainScene.h"
#include <iostream>
#include <algorithm>

namespace CG
{
    void MainScene::SetWaterMode()
    {
        waterShowcase = !waterShowcase;
    }

	bool MainScene::GetWaterMode()
	{
	    return waterShowcase;
	}

	void MainScene::AdjustWaterAttribute(float R, float D, glm::vec3 color)
	{
        if (R < 0) waterState.reflectionRate = 0;
        else if (R > 1) waterState.reflectionRate = 1;
        else waterState.reflectionRate = R;

        if (D < 0) waterState.distortionCo = 0;
        else if (D > 1) waterState.distortionCo = 1;
        else waterState.distortionCo = D;

        waterState.waterColor.x = std::clamp(color.x, 0.0f, 1.0f);
        waterState.waterColor.y = std::clamp(color.y, 0.0f, 1.0f);
        waterState.waterColor.z = std::clamp(color.z, 0.0f, 1.0f);
	}

	MainScene::waterAttribute MainScene::GetWaterAttribute()
	{
	    return waterState;
	}

	MainScene::ChidoriSet MainScene::GetChidoriAttribute()
	{
	    return chidoriAttribute;
	}

	void MainScene::SetChidoriAttribute(ChidoriSet attribute)
	{

	    chidoriAttribute.offset = attribute.offset;
		chidoriAttribute.intensity = attribute.intensity;
		chidoriAttribute.color = attribute.color;
		chidoriAttribute.show = attribute.show;
		chidoriAttribute.rightHand = attribute.rightHand;
	}

	MainScene::fireBallSet MainScene::GetFireBallAttribute()
	{
	    return fireBallAttibute;
	}

	void MainScene::SetFireBallAttribute(fireBallSet attribute)
	{
	    fireBallAttibute.offset = attribute.offset;
		fireBallAttibute.intensity = attribute.intensity;
		fireBallAttibute.speed = attribute.speed;
		fireBallAttibute.show = attribute.show;
	}

	void MainScene::SetMosaic()
	{
        mosaicShowcase = !mosaicShowcase;
	}

	bool MainScene::GetMosaic()
	{
	    return mosaicShowcase;
	}

	void MainScene::SetMotionBlur()
	{
        motionBlurShowcase = !motionBlurShowcase;
	}

	bool MainScene::GetMotionBlur()
	{
	    return motionBlurShowcase;
	}

	void MainScene::SetToon()
	{
        toonShowcase = !toonShowcase;
	}

	bool MainScene::GetToon()
	{
	    return toonShowcase;
	}

	void MainScene::AdjustPointLight(glm::vec3 pos)
    {
        pointLightPos = pos;
    }

    glm::vec3 MainScene::GetPointLightPos()
    {
        return pointLightPos;
    }

    void MainScene::AdjustPointLightIntensity(float intensity)
    {
        lightIntensity = std::clamp(intensity , 1.0f, 10.0f);
    }
	float MainScene::GetPointLightIntensity()
	{
	    return lightIntensity;
	}


    void MainScene::SetAmbientMode()
    {
        ambientShowcase = !ambientShowcase;
    }

    bool MainScene::GetAmbientMode()
    {
        return ambientShowcase;
    }

    void MainScene::SetLightMode()
    {
        dotLightShowcase = !dotLightShowcase;
    }

    bool MainScene::GetLightMode()
    {
       return dotLightShowcase;
    }

    void MainScene::AdjustEnvMapAttribute(float R, float E)
	{
           if (R < 0) envMapState.reflectionIntensity = 0;
           else if (R > 1) envMapState.reflectionIntensity = 1;
           else envMapState.reflectionIntensity = R;

           if (E < 0) envMapState.exposure = 0;
           else if (E > 3) envMapState.exposure = 3;
           else envMapState.exposure = E;
	}

	MainScene::envMapAttribute MainScene::GetEnvMapAttribute()
	{
	    return envMapState;
	}

	int MainScene::GetFireWorkAmount()
	{
	    return Memory.fireworkTrack.size();
	}

	std::vector<MainScene::FireworkTrigger> MainScene::GetFireWorkAttribute(bool tempMode)
	{
	    std::vector<FireworkTrigger> local;
	    if (tempMode)
		{
		    local.push_back(tempFirework);
			return local;
		}
		else
		{
            return Memory.fireworkTrack;
		}
	}

	void MainScene::SetFireWorkAttribute(bool tempMode, int index, fireWorkSet attribute)
	{
        if (tempMode)
        {
            tempFirework.attributes = attribute;
        }
        else
        {
            if (index < Memory.fireworkTrack.size()&& index>= 0)
            {
                Memory.fireworkTrack[index].attributes = attribute;
            }
        }
	}

	void MainScene::SetFireWorkTime(bool tempMode, int index, double time)
	{
	    double rTime = time;
	    if (time > Memory.totalTimeSpan)
		{
		    rTime = Memory.totalTimeSpan;
		}
		else if (time <0)
		{
		    rTime = 0;
		}
        if (tempMode)
        {
            tempFirework.triggerTime = rTime;
        }
        else
        {
            if (index < Memory.fireworkTrack.size()&& index>= 0)
            {
                Memory.fireworkTrack[index].triggerTime = rTime;
            }
        }
	}

	void MainScene::InsertNewFireWork()
	{
	    Memory.fireworkTrack.push_back(tempFirework);
	}

	void MainScene::DeleteFireWork(int index)
	{
        if (index >= 0 && index < Memory.fireworkTrack.size())
        {
            Memory.fireworkTrack.erase(Memory.fireworkTrack.begin() + index);
        }
	}

	// 在 MainScene.h 宣告，並在 GetSetEffects.cpp 實作
    void MainScene::SetEditFireWork(bool state) { editFireWork = state; }
    bool MainScene::GetEditFireWork() { return editFireWork; }

    void MainScene::SetEditCreateFireWork(bool state) { editCreateFireWork = state; }
    bool MainScene::GetEditCreateFireWork() { return editCreateFireWork; }

    void MainScene::SetEditTimeLine(double time) { editTimeLine = time; }
    double MainScene::GetEditTimeLine() { return editTimeLine; }

    double MainScene::GetMemoryTotalTimeSpan() { return Memory.totalTimeSpan; }
}
