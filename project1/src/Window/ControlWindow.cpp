/*
#include "ControlWindow.h"

#include <imgui.h>

#include <iostream>
#include <vector>
#include <string>

namespace CG
{
	ControlWindow::ControlWindow()
	{
		showDemoWindow = false;
	}

	auto ControlWindow::Initialize() -> bool
	{
		return true;
	}

	void ControlWindow::Display()
	{
		ImGui::Begin("Control");
		{
			ImGui::Checkbox("Demo Window", &showDemoWindow);

			static int actionIndex = 0;
			std::vector<std::string> actions = { "Idle", "Walk", "sitUP",  "pushUP", "hopakDance"};
			ImGui::Text("Action: ");
			ImGui::SameLine(100);
			ImGui::SetNextItemWidth(150);
			if (ImGui::BeginCombo("##Action", actions[actionIndex].c_str()))
			{
				for (int n = 0; n < actions.size(); n++)
				{
					const bool is_selected = (actionIndex == n);
					if (ImGui::Selectable(actions[n].c_str(), is_selected))
					{
						actionIndex = n;
						std::cout << "Set Action " << actionIndex << std::endl;
						targetScene->SetAction(n);
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			static int modeIndex = 0;
			std::vector<std::string> modes = { "Fill", "Line" };
			ImGui::Text("Mode: ");
			ImGui::SameLine(100);
			ImGui::SetNextItemWidth(150);
			if (ImGui::BeginCombo("##Mode", modes[modeIndex].c_str()))
			{
				for (int n = 0; n < modes.size(); n++)
				{
					const bool is_selected = (modeIndex == n);
					if (ImGui::Selectable(modes[n].c_str(), is_selected))
					{
						modeIndex = n;
						std::cout << "Set Mode " << modeIndex << std::endl;
						targetScene->SetMode(n);
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

		}
		ImGui::End();

		// Show the big demo window or not (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
*/
#include "ControlWindow.h"

#include <imgui.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // for std::max

namespace CG
{
	ControlWindow::ControlWindow()
	{
		showDemoWindow = false;
		targetScene = nullptr;

		// 狀態初始化
		editMode = false;
		currentFrameIndex = 0;
		currentFrameTime = 1.0f;
		isPlaying = true;
		playSpeedMultiplier = 1.0f;

		// 位移初始化 (配合 MainScene 預設高度)
		tempOffset[0] = 0.0f;
		tempOffset[1] = 16.0f;
		tempOffset[2] = 0.0f;
	}

	auto ControlWindow::Initialize() -> bool
	{
		return true;
	}

	// ---------------------------------------------------------
	// 雙向同步：讀取 (Read from Model)
	// ---------------------------------------------------------
	void ControlWindow::SyncFromScene()
	{
		if (targetScene)
		{
			tempJoints = targetScene->GetJoint();

			glm::vec3 off = targetScene->GetOffset();
			tempOffset[0] = off.x;
			tempOffset[1] = off.y;
			tempOffset[2] = off.z;
		}
	}

	// ---------------------------------------------------------
	// 雙向同步：寫入 (Write to Model)
	// 嚴格對應 MainScene::SetJoint 的 15 個 part index
	// ---------------------------------------------------------
	void ControlWindow::SyncToScene()
	{
		if (targetScene)
		{
			// 1. 同步 Root Offset
			targetScene->SetOffset(tempOffset[0], tempOffset[1], tempOffset[2]);

			// 2. 同步 Body & Head
			targetScene->SetJoint(tempJoints.core.pitch, tempJoints.core.roll, tempJoints.core.yaw, 0); // Core
			targetScene->SetJoint(tempJoints.head.pitch, tempJoints.head.roll, tempJoints.head.yaw, 1); // Head
			targetScene->SetJoint(tempJoints.downBody.pitch, tempJoints.downBody.roll, tempJoints.downBody.yaw, 8); // DownBody/Pelvis

			// 3. 同步 Left Arm
			targetScene->SetJoint(tempJoints.upLeftArm.pitch, tempJoints.upLeftArm.roll, tempJoints.upLeftArm.yaw, 2); // UpLeftArm
			targetScene->SetJoint(tempJoints.downLeftArm, 0, 0, 3); // DownLeftArm (僅 pitch)
			targetScene->SetJoint(tempJoints.handLeft.pitch, tempJoints.handLeft.roll, tempJoints.handLeft.yaw, 7); // HandLeft

			// 4. 同步 Right Arm
			targetScene->SetJoint(tempJoints.upRightArm.pitch, tempJoints.upRightArm.roll, tempJoints.upRightArm.yaw, 5); // UpRightArm
			targetScene->SetJoint(tempJoints.downRightArm, 0, 0, 6); // DownRightArm (僅 pitch)
			targetScene->SetJoint(tempJoints.handRight.pitch, tempJoints.handRight.roll, tempJoints.handRight.yaw, 4); // HandRight

			// 5. 同步 Left Leg
			targetScene->SetJoint(tempJoints.upLeftLeg.pitch, tempJoints.upLeftLeg.roll, tempJoints.upLeftLeg.yaw, 9); // UpLeftLeg
			targetScene->SetJoint(tempJoints.downLeftLeg, 0, 0, 10); // DownLeftLeg (僅 pitch)
			targetScene->SetJoint(tempJoints.footLeft.pitch, tempJoints.footLeft.roll, tempJoints.footLeft.yaw, 11); // FootLeft

			// 6. 同步 Right Leg
			targetScene->SetJoint(tempJoints.upRightLeg.pitch, tempJoints.upRightLeg.roll, tempJoints.upRightLeg.yaw, 12); // UpRightLeg
			targetScene->SetJoint(tempJoints.downRightLeg, 0, 0, 13); // DownRightLeg (僅 pitch)
			targetScene->SetJoint(tempJoints.footRight.pitch, tempJoints.footRight.roll, tempJoints.footRight.yaw, 14); // FootRight
		}
	}

	// ---------------------------------------------------------
	// 輔助 UI 元件：三軸向與單軸向滑桿
	// ---------------------------------------------------------
	void ControlWindow::DrawJointSlider3(const char* label, float& p, float& r, float& y)
	{
		ImGui::PushID(label);
		ImGui::Text("%s", label);
		if (ImGui::SliderFloat("Pitch", &p, -180.0f, 180.0f) |
			ImGui::SliderFloat("Roll", &r, -180.0f, 180.0f) |
			ImGui::SliderFloat("Yaw", &y, -180.0f, 180.0f))
		{
			SyncToScene();
		}
		ImGui::PopID();
	}

	void ControlWindow::DrawJointSlider1(const char* label, float& angle)
	{
		ImGui::PushID(label);
		ImGui::Text("%s", label);
		if (ImGui::SliderFloat("Angle(Pitch)", &angle, -180.0f, 180.0f))
		{
			SyncToScene();
		}
		ImGui::PopID();
	}

	// ---------------------------------------------------------
	// 核心顯示邏輯
	// ---------------------------------------------------------
	void ControlWindow::Display()
	{
		ImGui::Begin("Animation Editor");
		{
			ImGui::Checkbox("Show ImGui Demo", &showDemoWindow);

			// ========================================================
			// 全域設定：動作與渲染模式
			// ========================================================
			static int actionIndex = 0;
			std::vector<std::string> actions = { "Idle", "Walk", "Sit-Up", "Push-Up", "Hopak", "APT", "Memory", "Shadow Clone" };
			ImGui::Text("Action:");
			ImGui::SameLine(80);
			ImGui::SetNextItemWidth(150);
			if (ImGui::BeginCombo("##Action", actions[actionIndex].c_str())) {
				for (int n = 0; n < actions.size(); n++) {
					if (ImGui::Selectable(actions[n].c_str(), actionIndex == n)) {
						actionIndex = n;
						if (targetScene) targetScene->SetAction(n);
					}
				}
				ImGui::EndCombo();
			}

			static int modeIndex = 0;
			std::vector<std::string> modes = { "Fill", "Line" };
			ImGui::Text("Mode:");
			ImGui::SameLine(80);
			ImGui::SetNextItemWidth(150);
			if (ImGui::BeginCombo("##Mode", modes[modeIndex].c_str())) {
				for (int n = 0; n < modes.size(); n++) {
					if (ImGui::Selectable(modes[n].c_str(), modeIndex == n)) {
						modeIndex = n;
						if (targetScene) targetScene->SetMode(n);
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Separator();

			// ========================================================
			// 雙模式切換核心邏輯
			// ========================================================
			if (ImGui::Checkbox("ENTER EDIT MODE", &editMode)) {
				if (targetScene) {
					targetScene->SetEdit(); // 觸發場景內 Edit Mode 變更
					if (editMode) {
						targetScene->SetAnimationSpeed(0.0); // 編輯模式強制暫停
						SyncFromScene(); // 抓取當前物理狀態到 GUI
					} else {
						targetScene->SetAnimationSpeed(isPlaying ? playSpeedMultiplier : 0.0); // 恢復播放速度
					}
				}
			}

			// ========================================================
			// 非編輯模式：播放器面板
			// ========================================================
			if (!editMode && targetScene) {
				ImGui::Text("--- Playback Controls ---");
				if (ImGui::Button(isPlaying ? "PAUSE ||" : "PLAY >")) {
					isPlaying = !isPlaying;
					targetScene->SetAnimationSpeed(isPlaying ? playSpeedMultiplier : 0.0);
				}
				ImGui::SameLine();
				if (ImGui::SliderFloat("Speed", &playSpeedMultiplier, 0.1f, 3.0f, "%.1f x")) {
					if (isPlaying) targetScene->SetAnimationSpeed(playSpeedMultiplier);
				}
			}

			// ========================================================
			// 編輯模式：時間軸、位移與關節督導員
			// ========================================================
			if (editMode && targetScene) {

				// --- 1. 時間軸 (Timeline) ---
				ImGui::Text("--- Timeline & Memory ---");
				int count = targetScene->GetMemoryFrameNum();

				if (count > 0) {
					// 拖動滑桿即時預覽 (Auto-Load)
					if (ImGui::SliderInt("Frame Index", &currentFrameIndex, 0, count - 1)) {
						targetScene->ShowKeyFrame(currentFrameIndex);
						SyncFromScene();
					}
				} else {
					ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Memory is empty.");
				}

				ImGui::InputFloat("Frame Duration", &currentFrameTime, 0.1f, 0.5f, "%.2f s");

				if (ImGui::Button("Insert New")) {
					targetScene->InsertKeyFrame(currentFrameTime, currentFrameIndex);
					currentFrameIndex = targetScene->GetMemoryFrameNum() - 1;
					targetScene->ShowKeyFrame(currentFrameIndex);
					SyncFromScene();
				}
				ImGui::SameLine();
				if (ImGui::Button("Overwrite")) {
					targetScene->AdjustKeyFrame(currentFrameIndex);
					targetScene->AdjustTimeSpan(currentFrameTime, currentFrameIndex);
				}
				ImGui::SameLine();
				if (ImGui::Button("Delete")) {
					targetScene->DeleteKeyFrame(currentFrameIndex);
					currentFrameIndex = std::max(0, targetScene->GetMemoryFrameNum() - 1);
					if (targetScene->GetMemoryFrameNum() > 0) {
						targetScene->ShowKeyFrame(currentFrameIndex);
						SyncFromScene();
					}
				}

				ImGui::Separator();

				// --- 2. 根節點位移 (Root Offset) ---
				ImGui::Text("--- Root Translation ---");
				if (ImGui::DragFloat3("Offset X/Y/Z", tempOffset, 0.1f, -100.0f, 100.0f)) {
					SyncToScene();
				}

				ImGui::Separator();

				// --- 3. 15 部位關節控制 (Joints) ---
				ImGui::Text("--- Character Joints ---");

				if (ImGui::CollapsingHeader("Body & Head", ImGuiTreeNodeFlags_DefaultOpen)) {
					DrawJointSlider3("Core", tempJoints.core.pitch, tempJoints.core.roll, tempJoints.core.yaw);
					DrawJointSlider3("DownBody (Pelvis)", tempJoints.downBody.pitch, tempJoints.downBody.roll, tempJoints.downBody.yaw);
					DrawJointSlider3("Head", tempJoints.head.pitch, tempJoints.head.roll, tempJoints.head.yaw);
				}

				if (ImGui::CollapsingHeader("Left Arm & Hand")) {
					DrawJointSlider3("L-Upper Arm", tempJoints.upLeftArm.pitch, tempJoints.upLeftArm.roll, tempJoints.upLeftArm.yaw);
					DrawJointSlider1("L-Lower Arm", tempJoints.downLeftArm);
					DrawJointSlider3("L-Hand", tempJoints.handLeft.pitch, tempJoints.handLeft.roll, tempJoints.handLeft.yaw);
				}

				if (ImGui::CollapsingHeader("Right Arm & Hand")) {
					DrawJointSlider3("R-Upper Arm", tempJoints.upRightArm.pitch, tempJoints.upRightArm.roll, tempJoints.upRightArm.yaw);
					DrawJointSlider1("R-Lower Arm", tempJoints.downRightArm);
					DrawJointSlider3("R-Hand", tempJoints.handRight.pitch, tempJoints.handRight.roll, tempJoints.handRight.yaw);
				}

				if (ImGui::CollapsingHeader("Left Leg & Foot")) {
					DrawJointSlider3("L-Thigh", tempJoints.upLeftLeg.pitch, tempJoints.upLeftLeg.roll, tempJoints.upLeftLeg.yaw);
					DrawJointSlider1("L-Shin", tempJoints.downLeftLeg);
					DrawJointSlider3("L-Foot", tempJoints.footLeft.pitch, tempJoints.footLeft.roll, tempJoints.footLeft.yaw);
				}

				if (ImGui::CollapsingHeader("Right Leg & Foot")) {
					DrawJointSlider3("R-Thigh", tempJoints.upRightLeg.pitch, tempJoints.upRightLeg.roll, tempJoints.upRightLeg.yaw);
					DrawJointSlider1("R-Shin", tempJoints.downRightLeg);
					DrawJointSlider3("R-Foot", tempJoints.footRight.pitch, tempJoints.footRight.roll, tempJoints.footRight.yaw);
				}
			}
		}
		ImGui::End();

		if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
