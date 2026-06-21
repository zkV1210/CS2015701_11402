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

        editMode = false;
        currentFrameIndex = 0;
        currentFrameTime = 1.0f;

        isPlaying = true;
        playSpeedMultiplier = 1.0f;

        tempOffset[0] = 0.0f;
        tempOffset[1] = 16.0f;
        tempOffset[2] = 0.0f;
    }

    auto ControlWindow::Initialize() -> bool
    {
        return true;
    }

    void ControlWindow::SyncFromScene()
    {
        if (targetScene)
        {
            tempJoints = targetScene->GetJoint();
            glm::vec3 off = targetScene->GetOffset();
            tempOffset[0] = off.x;
            tempOffset[1] = off.y;
            tempOffset[2] = off.z;
            tempChidori = targetScene->GetChidoriAttribute();
            tempFireball = targetScene->GetFireBallAttribute();
        }
    }

    void ControlWindow::SyncToScene()
    {
        if (targetScene)
        {
            targetScene->SetOffset(tempOffset[0], tempOffset[1], tempOffset[2]);
            targetScene->SetChidoriAttribute(tempChidori);
            targetScene->SetFireBallAttribute(tempFireball);

            targetScene->SetJoint(tempJoints.core.pitch, tempJoints.core.roll, tempJoints.core.yaw, 0);
            targetScene->SetJoint(tempJoints.head.pitch, tempJoints.head.roll, tempJoints.head.yaw, 1);
            targetScene->SetJoint(tempJoints.downBody.pitch, tempJoints.downBody.roll, tempJoints.downBody.yaw, 8);
            targetScene->SetJoint(tempJoints.upLeftArm.pitch, tempJoints.upLeftArm.roll, tempJoints.upLeftArm.yaw, 2);
            targetScene->SetJoint(tempJoints.downLeftArm, 0, 0, 3);
            targetScene->SetJoint(tempJoints.handLeft.pitch, tempJoints.handLeft.roll, tempJoints.handLeft.yaw, 7);
            targetScene->SetJoint(tempJoints.upRightArm.pitch, tempJoints.upRightArm.roll, tempJoints.upRightArm.yaw, 5);
            targetScene->SetJoint(tempJoints.downRightArm, 0, 0, 6);
            targetScene->SetJoint(tempJoints.handRight.pitch, tempJoints.handRight.roll, tempJoints.handRight.yaw, 4);
            targetScene->SetJoint(tempJoints.upLeftLeg.pitch, tempJoints.upLeftLeg.roll, tempJoints.upLeftLeg.yaw, 9);
            targetScene->SetJoint(tempJoints.downLeftLeg, 0, 0, 10);
            targetScene->SetJoint(tempJoints.footLeft.pitch, tempJoints.footLeft.roll, tempJoints.footLeft.yaw, 11);
            targetScene->SetJoint(tempJoints.upRightLeg.pitch, tempJoints.upRightLeg.roll, tempJoints.upRightLeg.yaw, 12);
            targetScene->SetJoint(tempJoints.downRightLeg, 0, 0, 13);
            targetScene->SetJoint(tempJoints.footRight.pitch, tempJoints.footRight.roll, tempJoints.footRight.yaw, 14);
        }
    }

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

    void ControlWindow::Display()
    {
        ImGui::Begin("Animation & Engine Editor");
        {
            ImGui::Checkbox("Show ImGui Demo", &showDemoWindow);

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

            if (ImGui::Checkbox("ENTER EDIT MODE", &editMode)) {
                if (targetScene) {
                    targetScene->SetEdit();
                    if (editMode) {
                        targetScene->SetAnimationSpeed(0.0);
                        SyncFromScene();
                    } else {
                        targetScene->SetAnimationSpeed(isPlaying ? playSpeedMultiplier : 0.0);
                        targetScene->SetEditFireWork(false); // 退出編輯時關閉煙火編輯
                    }
                }
            }
            ImGui::Separator();

            // ========================================================
            // 環境、光照與後處理面板 (保持原樣)
            // ========================================================
            if (targetScene && ImGui::CollapsingHeader("Environment & Lighting")) {
                bool isWaterOn = targetScene->GetWaterMode();
                if (ImGui::Checkbox("Enable Water Reflection", &isWaterOn)) targetScene->SetWaterMode();
                auto wAttr = targetScene->GetWaterAttribute();
                float wRefl = wAttr.reflectionRate, wDist = wAttr.distortionCo;
                float wCol[3] = { wAttr.waterColor.x, wAttr.waterColor.y, wAttr.waterColor.z };
                bool wChanged = false;
                wChanged |= ImGui::SliderFloat("Water Reflect Rate", &wRefl, 0.0f, 1.0f);
                wChanged |= ImGui::SliderFloat("Water Distortion", &wDist, 0.0f, 1.0f);
                wChanged |= ImGui::ColorEdit3("Water Color", wCol);
                if (wChanged) targetScene->AdjustWaterAttribute(wRefl, wDist, glm::vec3(wCol[0], wCol[1], wCol[2]));
                ImGui::Separator();

                bool isAmbientOn = targetScene->GetAmbientMode();
                if (ImGui::Checkbox("Enable Ambient Light (IBL)", &isAmbientOn)) targetScene->SetAmbientMode();
                auto envAttr = targetScene->GetEnvMapAttribute();
                float envRefl = envAttr.reflectionIntensity, envExp = envAttr.exposure;
                bool envChanged = false;
                envChanged |= ImGui::SliderFloat("Env Reflection", &envRefl, 0.0f, 1.0f);
                envChanged |= ImGui::SliderFloat("Env Exposure", &envExp, 0.0f, 3.0f);
                if (envChanged) targetScene->AdjustEnvMapAttribute(envRefl, envExp);
                ImGui::Separator();

                bool isLightOn = targetScene->GetLightMode();
                if (ImGui::Checkbox("Enable Point Light", &isLightOn)) targetScene->SetLightMode();
                glm::vec3 currentLightPos = targetScene->GetPointLightPos();
                float lightPosArray[3] = { currentLightPos.x, currentLightPos.y, currentLightPos.z };
                if (ImGui::DragFloat3("Light Pos (X/Y/Z)", lightPosArray, 0.5f, -200.0f, 200.0f)) {
                    targetScene->AdjustPointLight(glm::vec3(lightPosArray[0], lightPosArray[1], lightPosArray[2]));
                }
                float currentIntensity = targetScene->GetPointLightIntensity();
                if (ImGui::SliderFloat("Light Intensity", &currentIntensity, 1.0f, 10.0f)) {
                    targetScene->AdjustPointLightIntensity(currentIntensity);
                }
            }

            if (targetScene && ImGui::CollapsingHeader("Post-Processing Effects")) {
                bool isMosaicOn = targetScene->GetMosaic();
                if (ImGui::Checkbox("Enable Mosaic Effect", &isMosaicOn)) targetScene->SetMosaic();
                bool isMotionBlurOn = targetScene->GetMotionBlur();
                if (ImGui::Checkbox("Enable MotionBlur", &isMotionBlurOn)) targetScene->SetMotionBlur();
                bool isToonOn = targetScene->GetToon();
                if (ImGui::Checkbox("Enable Toon Shading", &isToonOn)) targetScene->SetToon();
            }
            ImGui::Separator();

            // ========================================================
            // 播放器面板 (非編輯模式)
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
            // 編輯模式：時間軸、煙火、特效與關節
            // ========================================================
            if (editMode && targetScene) {

                // --------------------------------------------------------
                // 💥 完美呼叫你寫的 API：煙火連續時間軸編輯器 (Firework Editor)
                // --------------------------------------------------------
                ImGui::Text("--- Firework Timeline Editor ---");
                bool fwMode = targetScene->GetEditFireWork();
                if (ImGui::Checkbox("Enable Continuous Firework Edit Mode", &fwMode)) {
                    targetScene->SetEditFireWork(fwMode);
                    if (fwMode && !targetScene->GetEditCreateFireWork()) {
                        targetScene->SetEditTimeLine(0.0); // 進入時時間軸歸零
                    }
                }

                if (fwMode) {
                    // 1. 連續時間軸 (以 Memory.totalTimeSpan 為邊界)
                    double maxTime = targetScene->GetMemoryTotalTimeSpan();
                    if (maxTime <= 0.0) maxTime = 0.1; // 防呆
                    double zero_d = 0.0;
                    double currentTime = targetScene->GetEditTimeLine();

                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.8f, 0.2f, 0.2f, 0.5f));
                    if (ImGui::SliderScalar("Timeline (s)", ImGuiDataType_Double, &currentTime, &zero_d, &maxTime, "%.3f s")) {
                        targetScene->SetEditTimeLine(currentTime); // 拖動時間軸即時更新
                    }
                    ImGui::PopStyleColor();

                    // 2. 切換新建/修改模式
                    bool createMode = targetScene->GetEditCreateFireWork();
                    if (ImGui::Checkbox("Create New Temp Firework", &createMode)) {
                        targetScene->SetEditCreateFireWork(createMode);
                        if (createMode) targetScene->SetFireWorkTime(true, 0, currentTime); // 將暫存煙火的時間對齊目前時間軸
                    }

                    ImGui::Indent();
                    if (createMode) {
                        // --- 💥 呼叫你的 API：處理暫存煙火 (tempMode = true) ---
                        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), ">> Creating Temp Firework");
                        auto fwList = targetScene->GetFireWorkAttribute(true);
                        if (!fwList.empty()) {
                            auto fw = fwList[0];
                            bool changed = false;

                            // 觸發時間
                            float tTime = (float)fw.triggerTime;
                            if (ImGui::SliderFloat("Trigger Time", &tTime, 0.0f, (float)maxTime)) {
                                targetScene->SetFireWorkTime(true, 0, tTime);
                            }

                            // 屬性調整
                            float fPos[3] = { fw.attributes.position.x, fw.attributes.position.y, fw.attributes.position.z };
                            if (ImGui::DragFloat3("Position", fPos, 0.1f, -100.0f, 100.0f)) {
                                fw.attributes.position = glm::vec3(fPos[0], fPos[1], fPos[2]);
                                changed = true;
                            }
                            float fDir[3] = { fw.attributes.direction.x, fw.attributes.direction.y, fw.attributes.direction.z };
                            if (ImGui::SliderFloat3("Direction", fDir, -1.0f, 1.0f)) {
                                fw.attributes.direction = glm::vec3(fDir[0], fDir[1], fDir[2]);
                                changed = true;
                            }
                            float fCol[3] = { fw.attributes.color.x, fw.attributes.color.y, fw.attributes.color.z };
                            if (ImGui::ColorEdit3("Color", fCol)) {
                                fw.attributes.color = glm::vec3(fCol[0], fCol[1], fCol[2]);
                                changed = true;
                            }
                            changed |= ImGui::SliderFloat("Intensity", &fw.attributes.intensity, 0.1f, 10.0f);
                            changed |= ImGui::SliderFloat("Speed", &fw.attributes.speed, 0.1f, 5.0f);

                            if (changed) targetScene->SetFireWorkAttribute(true, 0, fw.attributes);

                            // 💥 呼叫你的 API：存入記憶陣列
                            if (ImGui::Button("SAVE FIREWORK TO MEMORY", ImVec2(-1, 30))) {
                                targetScene->InsertNewFireWork();
                                targetScene->SetEditCreateFireWork(false); // 存檔後回到瀏覽模式
                            }
                        }
                    } else {
                        // --- 💥 呼叫你的 API：編輯已有煙火 (tempMode = false) ---
                        static int currentFireworkIndex = 0;
                        int fwCount = targetScene->GetFireWorkAmount();

                        if (fwCount > 0) {
                            if (currentFireworkIndex >= fwCount) currentFireworkIndex = fwCount - 1;
                            ImGui::SliderInt("Select Memory FW", &currentFireworkIndex, 0, fwCount - 1);

                            auto fwList = targetScene->GetFireWorkAttribute(false);
                            auto fw = fwList[currentFireworkIndex];
                            bool changed = false;

                            float tTime = (float)fw.triggerTime;
                            if (ImGui::SliderFloat("Trigger Time", &tTime, 0.0f, (float)maxTime)) {
                                targetScene->SetFireWorkTime(false, currentFireworkIndex, tTime);
                            }

                            float fPos[3] = { fw.attributes.position.x, fw.attributes.position.y, fw.attributes.position.z };
                            if (ImGui::DragFloat3("Position", fPos, 0.1f, -100.0f, 100.0f)) {
                                fw.attributes.position = glm::vec3(fPos[0], fPos[1], fPos[2]);
                                changed = true;
                            }
                            float fDir[3] = { fw.attributes.direction.x, fw.attributes.direction.y, fw.attributes.direction.z };
                            if (ImGui::SliderFloat3("Direction", fDir, -1.0f, 1.0f)) {
                                fw.attributes.direction = glm::vec3(fDir[0], fDir[1], fDir[2]);
                                changed = true;
                            }
                            float fCol[3] = { fw.attributes.color.x, fw.attributes.color.y, fw.attributes.color.z };
                            if (ImGui::ColorEdit3("Color", fCol)) {
                                fw.attributes.color = glm::vec3(fCol[0], fCol[1], fCol[2]);
                                changed = true;
                            }
                            changed |= ImGui::SliderFloat("Intensity", &fw.attributes.intensity, 0.1f, 10.0f);
                            changed |= ImGui::SliderFloat("Speed", &fw.attributes.speed, 0.1f, 5.0f);

                            if (changed) targetScene->SetFireWorkAttribute(false, currentFireworkIndex, fw.attributes);

                            // 💥 呼叫你的 API：刪除煙火
                            if (ImGui::Button("DELETE SELECTED", ImVec2(-1, 0))) {
                                targetScene->DeleteFireWork(currentFireworkIndex);
                            }
                        } else {
                            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Memory has no fireworks. Create one!");
                        }
                    }
                    ImGui::Unindent();
                }

                ImGui::Separator();

                // --------------------------------------------------------
                // 傳統定格動畫時間軸 (Timeline & Export)
                // --------------------------------------------------------
                if (!fwMode) { // 如果沒有開啟連續煙火編輯，才顯示傳統的影格插入器
                    ImGui::Text("--- Static KeyFrame Memory ---");
                    int count = targetScene->GetMemoryFrameNum();

                    if (count > 0) {
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

                    ImGui::SameLine();
                    if (ImGui::Button("Write JSON")) {
                        targetScene->WriteIntoJson();
                    }
                    ImGui::Separator();
                }

                // --------------------------------------------------------
                // 根節點位移 (Root Offset)
                // --------------------------------------------------------
                ImGui::Text("--- Root Translation ---");
                if (ImGui::DragFloat3("Offset X/Y/Z", tempOffset, 0.1f, -100.0f, 100.0f)) {
                    SyncToScene();
                }
                ImGui::Separator();

                // --------------------------------------------------------
                // 千鳥特效編輯器 (Chidori)
                // --------------------------------------------------------
                ImGui::Text("--- Particle Effect: Chidori ---");
                bool chidoriChanged = false;

                chidoriChanged |= ImGui::Checkbox("Enable Chidori for this Frame", &tempChidori.show);

                // 💥 用 RadioButton 切換左右手
                int handMode = tempChidori.rightHand ? 1 : 0;
                if (ImGui::RadioButton("Left Hand", &handMode, 0)) chidoriChanged = true;
                ImGui::SameLine();
                if (ImGui::RadioButton("Right Hand", &handMode, 1)) chidoriChanged = true;
                tempChidori.rightHand = (handMode == 1);

                // 💥 單一拉桿微調特效位置
                chidoriChanged |= ImGui::SliderFloat("Hand Offset", &tempChidori.offset, -10.0f, 10.0f);

                float cCol[3] = { tempChidori.color.x, tempChidori.color.y, tempChidori.color.z };
                if (ImGui::ColorEdit3("Chidori Color", cCol)) {
                    tempChidori.color = glm::vec3(cCol[0], cCol[1], cCol[2]);
                    chidoriChanged = true;
                }

                chidoriChanged |= ImGui::SliderFloat("Chidori Intensity", &tempChidori.intensity, 0.1f, 10.0f);

                if (chidoriChanged) SyncToScene();
                ImGui::Separator();

                // --------------------------------------------------------
                // 豪火球特效編輯器 (Fireball)
                // --------------------------------------------------------
                ImGui::Text("--- Particle Effect: Fireball ---");
                bool fireballChanged = false;

                // 1. 保留開關
                fireballChanged |= ImGui::Checkbox("Enable Fireball for this Frame", &tempFireball.show);

                // 2. 💥 把原本的 Position 和 Direction 刪掉，換成單一的距離拉桿
                // 讓你可以直覺地調整火球離柯博文嘴巴有多遠
                fireballChanged |= ImGui::SliderFloat("Forward Distance", &tempFireball.offset, 0.0f, 30.0f);

                // 3. 保留強度與速度
                fireballChanged |= ImGui::SliderFloat("Fireball Intensity", &tempFireball.intensity, 0.1f, 5.0f);
                fireballChanged |= ImGui::SliderFloat("Fireball Speed", &tempFireball.speed, 0.1f, 5.0f);

                // 4. 同步與分隔線
                if (fireballChanged) SyncToScene();
                ImGui::Separator();

                // --------------------------------------------------------
                // 15 部位關節控制 (Joints)
                // --------------------------------------------------------
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

        //if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
    }
}
