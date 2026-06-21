#include "MainScene.h"
#include <fstream>
#include <iostream>
#include <json.hpp>

using json = nlohmann::json;

namespace CG
{
    void MainScene::SaveKeyFrameSet(const keyFrameSet* sourceSet, const std::string& filename)
    {
        if (!sourceSet) return;

        json j;
        j["totalTimeSpan"] = sourceSet->totalTimeSpan;

        // ==========================================
        // 1. 寫入骨架動作與跟隨幀的特效 (離散時間軸)
        // ==========================================
        j["frames"] = json::array();
        for (const auto& frame : sourceSet->frames)
        {
            json jFrame;

            // 基礎屬性
            jFrame["time"] = frame.time;
            jFrame["nextFrameIndex"] = frame.nextFrameIndex;
            jFrame["offset"] = {frame.offset.x, frame.offset.y, frame.offset.z};

            // 骨架姿態 (Pose) - 歐拉角輔助寫入
            auto eulerToJson = [](const eulerAnglesT& e) {
                return json{{"pitch", e.pitch}, {"roll", e.roll}, {"yaw", e.yaw}};
            };

            jFrame["pose"]["core"] = eulerToJson(frame.pose.core);
            jFrame["pose"]["head"] = eulerToJson(frame.pose.head);
            jFrame["pose"]["upRightArm"] = eulerToJson(frame.pose.upRightArm);
            jFrame["pose"]["upLeftArm"] = eulerToJson(frame.pose.upLeftArm);
            jFrame["pose"]["downBody"] = eulerToJson(frame.pose.downBody);
            jFrame["pose"]["upLeftLeg"] = eulerToJson(frame.pose.upLeftLeg);
            jFrame["pose"]["upRightLeg"] = eulerToJson(frame.pose.upRightLeg);
            jFrame["pose"]["handRight"] = eulerToJson(frame.pose.handRight);
            jFrame["pose"]["handLeft"] = eulerToJson(frame.pose.handLeft);
            jFrame["pose"]["footLeft"] = eulerToJson(frame.pose.footLeft);
            jFrame["pose"]["footRight"] = eulerToJson(frame.pose.footRight);

            jFrame["pose"]["downLeftArm"] = frame.pose.downLeftArm;
            jFrame["pose"]["downRightArm"] = frame.pose.downRightArm;
            jFrame["pose"]["downLeftLeg"] = frame.pose.downLeftLeg;
            jFrame["pose"]["downRightLeg"] = frame.pose.downRightLeg;

            // 💥 更新：特效 雷切 (Chidori) - 改為純 offset 與 rightHand 綁定
            jFrame["chidori"]["offset"] = frame.chidori.offset;
            jFrame["chidori"]["intensity"] = frame.chidori.intensity;
            jFrame["chidori"]["color"] = {frame.chidori.color.x, frame.chidori.color.y, frame.chidori.color.z};
            jFrame["chidori"]["show"] = frame.chidori.show;
            jFrame["chidori"]["rightHand"] = frame.chidori.rightHand;

            // 💥 更新：特效 豪火球 (Fireball) - 改為純 offset 綁定
            jFrame["fireball"]["offset"] = frame.fireball.offset;
            jFrame["fireball"]["intensity"] = frame.fireball.intensity;
            jFrame["fireball"]["speed"] = frame.fireball.speed;
            jFrame["fireball"]["show"] = frame.fireball.show;

            j["frames"].push_back(jFrame);
        }

        // ==========================================
        // 2. 寫入煙火發射器 (平行連續時間軸)
        // ==========================================
        j["fireworkTrack"] = json::array();
        for (const auto& trigger : sourceSet->fireworkTrack)
        {
            json jTrigger;
            jTrigger["triggerTime"] = trigger.triggerTime; // 絕對觸發時間

            // 煙火屬性保留完整的 3D 世界設定
            jTrigger["attributes"]["position"] = {trigger.attributes.position.x, trigger.attributes.position.y, trigger.attributes.position.z};
            jTrigger["attributes"]["direction"] = {trigger.attributes.direction.x, trigger.attributes.direction.y, trigger.attributes.direction.z};
            jTrigger["attributes"]["intensity"] = trigger.attributes.intensity;
            jTrigger["attributes"]["color"] = {trigger.attributes.color.x, trigger.attributes.color.y, trigger.attributes.color.z};
            jTrigger["attributes"]["speed"] = trigger.attributes.speed;

            j["fireworkTrack"].push_back(jTrigger);
        }

        // 寫入實體檔案
        std::ofstream file(filename);
        if (file.is_open()) {
            file << j.dump(4);
            file.close();
            std::cout << "[System] 成功儲存動作序列與特效至: " << filename << std::endl;
        } else {
            std::cerr << "[Error] 無法開啟檔案進行寫入: " << filename << std::endl;
        }
    }

    void MainScene::LoadKeyFrameSet(keyFrameSet* targetSet, const std::string& filename)
    {
        if (!targetSet) return;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[Error] 找不到或無法開啟動作檔案: " << filename << std::endl;
            return;
        }

        json j;
        try {
            file >> j;
        } catch (json::parse_error& e) {
            std::cerr << "[Error] JSON 格式錯誤: " << e.what() << std::endl;
            return;
        }

        // 清空原本的資料
        targetSet->frames.clear();
        targetSet->fireworkTrack.clear();
        targetSet->totalTimeSpan = j.value("totalTimeSpan", 0.0);

        // 輔助 Lambda 函式
        auto jsonToEuler = [](const json& jNode) -> eulerAnglesT {
            eulerAnglesT e;
            if (jNode.is_object()) {
                e.pitch = jNode.value("pitch", 0.0f);
                e.roll = jNode.value("roll", 0.0f);
                e.yaw = jNode.value("yaw", 0.0f);
            }
            return e;
        };

        auto jsonToVec3 = [](const json& jNode, glm::vec3 defaultVal = glm::vec3(0.0f)) -> glm::vec3 {
            if (jNode.is_array() && jNode.size() >= 3) {
                return glm::vec3(jNode[0], jNode[1], jNode[2]);
            }
            return defaultVal;
        };

        // ==========================================
        // 1. 讀取骨架動作與跟隨幀的特效
        // ==========================================
        if (j.contains("frames") && j["frames"].is_array())
        {
            for (const auto& jFrame : j["frames"])
            {
                keyFrame frame;
                frame.time = jFrame.value("time", 0.0);
                frame.nextFrameIndex = jFrame.value("nextFrameIndex", 0);
                frame.offset = jsonToVec3(jFrame["offset"], glm::vec3(0,16,0));

                if (jFrame.contains("pose")) {
                    const auto& jPose = jFrame["pose"];
                    frame.pose.core = jsonToEuler(jPose["core"]);
                    frame.pose.head = jsonToEuler(jPose["head"]);
                    frame.pose.upRightArm = jsonToEuler(jPose["upRightArm"]);
                    frame.pose.upLeftArm = jsonToEuler(jPose["upLeftArm"]);
                    frame.pose.downBody = jsonToEuler(jPose["downBody"]);
                    frame.pose.upLeftLeg = jsonToEuler(jPose["upLeftLeg"]);
                    frame.pose.upRightLeg = jsonToEuler(jPose["upRightLeg"]);
                    frame.pose.handRight = jsonToEuler(jPose["handRight"]);
                    frame.pose.handLeft = jsonToEuler(jPose["handLeft"]);
                    frame.pose.footLeft = jsonToEuler(jPose["footLeft"]);
                    frame.pose.footRight = jsonToEuler(jPose["footRight"]);

                    frame.pose.downLeftArm = jPose.value("downLeftArm", 0.0f);
                    frame.pose.downRightArm = jPose.value("downRightArm", 0.0f);
                    frame.pose.downLeftLeg = jPose.value("downLeftLeg", 0.0f);
                    frame.pose.downRightLeg = jPose.value("downRightLeg", 0.0f);
                }

                // 💥 更新：讀取雷切
                if (jFrame.contains("chidori")) {
                    const auto& jC = jFrame["chidori"];
                    frame.chidori.offset = jC.value("offset", 0.0f);
                    frame.chidori.intensity = jC.value("intensity", 1.0f);
                    frame.chidori.color = jsonToVec3(jC["color"], glm::vec3(0.3f, 0.8f, 1.0f));
                    frame.chidori.show = jC.value("show", false);
                    frame.chidori.rightHand = jC.value("rightHand", false);
                }

                // 💥 更新：讀取豪火球
                if (jFrame.contains("fireball")) {
                    const auto& jFb = jFrame["fireball"];
                    frame.fireball.offset = jFb.value("offset", 0.0f);
                    frame.fireball.intensity = jFb.value("intensity", 1.0f);
                    frame.fireball.speed = jFb.value("speed", 1.0f);
                    frame.fireball.show = jFb.value("show", false);
                }

                targetSet->frames.push_back(frame);
            }
        }

        // ==========================================
        // 2. 讀取煙火發射器 (平行連續時間軸)
        // ==========================================
        if (j.contains("fireworkTrack") && j["fireworkTrack"].is_array())
        {
            for (const auto& jTrigger : j["fireworkTrack"])
            {
                FireworkTrigger trigger;
                trigger.triggerTime = jTrigger.value("triggerTime", 0.0);

                if (jTrigger.contains("attributes")) {
                    const auto& jAttr = jTrigger["attributes"];
                    trigger.attributes.position = jsonToVec3(jAttr["position"]);
                    trigger.attributes.direction = jsonToVec3(jAttr["direction"], glm::vec3(0, 1, 0));
                    trigger.attributes.intensity = jAttr.value("intensity", 1.0f);
                    trigger.attributes.color = jsonToVec3(jAttr["color"], glm::vec3(0.3f, 0.8f, 1.0f));
                    trigger.attributes.speed = jAttr.value("speed", 1.0f);
                }

                targetSet->fireworkTrack.push_back(trigger);
            }
        }

        std::cout << "[System] 成功讀取動作序列: " << filename
                  << " (共 " << targetSet->frames.size() << " 幀, "
                  << targetSet->fireworkTrack.size() << " 個煙火觸發點)" << std::endl;
    }
}
