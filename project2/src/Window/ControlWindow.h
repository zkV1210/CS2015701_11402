#pragma once

#include <Scene/MainScene.h>

namespace CG
{
	class ControlWindow
	{
	public:
		ControlWindow();

		auto Initialize() -> bool;
		void Display();

		void SetTargetScene(MainScene* scene)
		{
			targetScene = scene;
		}

	private:
		MainScene* targetScene;
		bool showDemoWindow;

		// === 系統狀態控制 ===
		bool editMode;
		int currentFrameIndex;
		float currentFrameTime;

		int currentFireworkIndex; // 💥 新增：紀錄目前選中的煙火索引

		bool isPlaying;
		float playSpeedMultiplier;

		// === GUI 雙向綁定暫存區 ===
		MainScene::JointSet tempJoints;
		float tempOffset[3];
		MainScene::ChidoriSet tempChidori;
		MainScene::fireBallSet tempFireball;

	private:
		// === 核心同步與 UI 繪製輔助函式 ===
		void SyncFromScene();
		void SyncToScene();
		void DrawJointSlider3(const char* label, float& pitch, float& roll, float& yaw);
		void DrawJointSlider1(const char* label, float& angle);
	};
}
