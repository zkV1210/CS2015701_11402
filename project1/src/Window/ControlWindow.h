/*#pragma once

#include <Scene/MainScene.h>

namespace CG
{
	class ControlWindow
	{
	public:
		ControlWindow();

		auto Initialize() -> bool;
		void Display();

	private:
		bool showDemoWindow;

	private:
		MainScene* targetScene;

	public:
		void SetTargetScene(MainScene* scene)
		{
			targetScene = scene;
		}
	};
}
*/
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

		bool isPlaying;
		float playSpeedMultiplier;

		// === GUI 雙向綁定暫存區 ===
		MainScene::JointSet tempJoints;
		float tempOffset[3]; // 針對 Root Offset 的 X, Y, Z

	private:
		// === 核心同步與 UI 繪製輔助函式 ===
		void SyncFromScene(); // 將場景的真實數據讀取到 GUI 暫存
		void SyncToScene();   // 將 GUI 暫存的數據寫回場景 (含 15 個部位與 Offset)
		void DrawJointSlider3(const char* label, float& pitch, float& roll, float& yaw);
		void DrawJointSlider1(const char* label, float& angle);
	};
}
