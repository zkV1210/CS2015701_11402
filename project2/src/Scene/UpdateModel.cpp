#include "MainScene.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace CG
{
	void MainScene::UpdateModel()
	{
		glm::mat4 Rotatation[PARTSNUM];
		glm::mat4 Translation[PARTSNUM];
		for (int i = 0; i < PARTSNUM; i++)
		{
			Models[i] = glm::mat4(1.0f);
			Rotatation[i] = glm::mat4(1.0f);
			Translation[i] = glm::mat4(1.0f);
		}
		float pitch, roll, yaw;

		glm::mat4 scaleMatrix;
		scaleMatrix = scale(4, 4, 4);

		//Body======================================================
		// 要補把高度補償的
		Rotatation[0] =   rotate(rendering.core.yaw, 0, 1, 0) *  rotate(rendering.core.pitch, 1, 0, 0) * rotate(rendering.core.roll, 0,0,1);
		Translation[0] = translate(	rootOffset.x , rootOffset.y, rootOffset.z);
		Models[0] = Translation[0] * Rotatation[0];


		//頭==========================================================
		pitch = glm::radians(rendering.head.pitch); //pitch
		roll = glm::radians(rendering.head.roll); //roll
		yaw = glm::radians(rendering.head.yaw); //yaw
		glm::vec3 eulerAngles(pitch, yaw, roll);
		Rotatation[1] = glm::mat4_cast(glm::quat(eulerAngles));
		//Rotatation[5] = rotate(yaw, 0,1,0) * rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0);//向前旋轉*向右旋轉
		Translation[1] = translate(0, 2, 0);
		Models[1] = Models[0] * Translation[1] * Rotatation[1];


		//左手=======================================================
		//左上手臂
		pitch = rendering.upLeftArm.pitch; //pitch
		roll = rendering.upLeftArm.roll; //roll
		yaw = rendering.upLeftArm.yaw; //yaw
		Rotatation[2] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Translation[2] = translate(4, 0, 0);
		Models[2] = Models[0] * Translation[2] * Rotatation[2];

		//左下手臂
		pitch = rendering.downLeftArm; // - 20;
		Rotatation[3] = rotate(pitch, 1, 0, 0);
		Translation[3] = translate(0, -2.2, 0);
		Models[3] = Models[2] * Translation[3] * Rotatation[3];

		//左手掌
		pitch = rendering.handLeft.pitch; //pitch
		roll = rendering.handLeft.roll; //roll
		yaw = rendering.handLeft.yaw; //yaw
		Translation[4] = translate(0, -3.7, 0);
		Rotatation[4] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Models[4] = Models[3] * Translation[4] * Rotatation[4];


		//右手=========================================================
		// 右上臂
		pitch = rendering.upRightArm.pitch; //pitch
		roll = rendering.upRightArm.roll; //roll
		yaw = rendering.upRightArm.yaw; //yaw
		Rotatation[5] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Translation[5] = translate(-4, 0, 0);
		Models[5] = Models[0] * Translation[5] * Rotatation[5];

		//右下臂
		pitch = rendering.downRightArm;
		Rotatation[6] = rotate(pitch, 1, 0, 0);
		Translation[6] = translate(0, -2.2, 0);
		Models[6] = Models[5] * Translation[6] * Rotatation[6];

		//右手掌
		pitch = rendering.handRight.pitch; //pitch
		roll = rendering.handRight.roll; //roll
		yaw = rendering.handRight.yaw; //yaw
		Translation[7] = translate(0, -3.7, 0);
		Rotatation[7] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Models[7] = Models[6] * Translation[7] * Rotatation[7];

		//腰===================================================
		pitch = rendering.downBody.pitch; //pitch
		roll = rendering.downBody.roll; //roll
		yaw = rendering.downBody.yaw; //yaw
		Rotatation[8] = rotate(yaw, 0,1,0) * rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0);
		Translation[8] = translate(0, -2.2, 0);

		//glm::mat4 moveBody = translate(0, -1.5, 0);
		Models[8] = Models[0] * Translation[8] * Rotatation[8];
		//=============================================================

		//左腳
		pitch = rendering.upLeftLeg.pitch;
		roll = rendering.upLeftLeg.roll;
		yaw = rendering.upLeftLeg.yaw;
		Rotatation[9] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Translation[9] = translate(2.0, -1.8, 0);
		Models[9] = Models[8] * Translation[9] * Rotatation[9];

		//左小腿
		pitch = rendering.downLeftLeg;
		Translation[10] = translate(0.5, -5.0, 0);
		Rotatation[10] = rotate(pitch, 1, 0, 0);
		Models[10] = Models[9] * Translation[10] * Rotatation[10];

		//腳掌

		pitch = rendering.footLeft.pitch; //pitch
		roll = rendering.footLeft.roll; //roll
		yaw = rendering.footLeft.yaw; //yaw
		Translation[11] = translate(0.5, -5.0, 0);
		Rotatation[11] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Models[11] = Models[10] * Translation[11] * Rotatation[11];
		//=============================================================
		//右腳
		pitch = rendering.upRightLeg.pitch;
		roll = rendering.upRightLeg.roll;
		yaw = rendering.upRightLeg.yaw;
		Rotatation[12] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Translation[12] = translate(-2.0, -1.8, 0);
		Models[12] = Models[8] *  Translation[12] * Rotatation[12];

		pitch = rendering.downRightLeg;
		Rotatation[13] = rotate(pitch, 1, 0, 0);
		Translation[13] = translate(-0.5, -5.0, 0);
		Models[13] =  Models[12] * Translation[13] * Rotatation[13];

		pitch = rendering.footRight.pitch; //pitch
		roll = rendering.footRight.roll; //roll
		yaw = rendering.footRight.yaw; //yaw
		Translation[14] = translate(-0.5, -5.0, 0);
		Rotatation[14] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0,1,0);
		Models[14] = Models[13] * Translation[14] * Rotatation[14];
		//=============================================================
		for(int l = 0;l<PARTSNUM;l++)
		{
		    Models[l] = Models[l] * scaleMatrix;
		}
	}

	void MainScene::UpdateInstanceModel()
    {
       glm::mat4 scaleMatrix;
       scaleMatrix = scale(4, 4, 4);

       for (int inst = 0; inst < 5; inst++)
       {
           glm::mat4 Rotatation[PARTSNUM];
           glm::mat4 Translation[PARTSNUM];

           // 1. 初始化此分身的所有零件矩陣為單位矩陣
           for (int i = 0; i < PARTSNUM; i++)
           {
               InstanceModels[inst][i] = glm::mat4(1.0f);
               Rotatation[i] = glm::mat4(1.0f);
               Translation[i] = glm::mat4(1.0f);
           }

           // 2. 獲取當前分身的專屬狀態
           JointSet pose = instanceRendering[inst];
           glm::vec3 offset = instanceRootOffset[inst];
           float pitch, roll, yaw;

           //Body======================================================
           Rotatation[0] = rotate(pose.core.yaw, 0, 1, 0) * rotate(pose.core.pitch, 1, 0, 0) * rotate(pose.core.roll, 0, 0, 1);
           Translation[0] = translate(offset.x, offset.y, offset.z);
           InstanceModels[inst][0] = Translation[0] * Rotatation[0];

           //頭==========================================================
           pitch = glm::radians(pose.head.pitch);
           roll = glm::radians(pose.head.roll);
           yaw = glm::radians(pose.head.yaw);
           glm::vec3 eulerAngles(pitch, yaw, roll);
           Rotatation[1] = glm::mat4_cast(glm::quat(eulerAngles));
           Translation[1] = translate(0, 2, 0);
           InstanceModels[inst][1] = InstanceModels[inst][0] * Translation[1] * Rotatation[1];

           //左手=======================================================
           //左上手臂
           pitch = pose.upLeftArm.pitch;
           roll = pose.upLeftArm.roll;
           yaw = pose.upLeftArm.yaw;
           Rotatation[2] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           Translation[2] = translate(4, 0, 0);
           InstanceModels[inst][2] = InstanceModels[inst][0] * Translation[2] * Rotatation[2];

           //左下手臂
           pitch = pose.downLeftArm;
           Rotatation[3] = rotate(pitch, 1, 0, 0);
           Translation[3] = translate(0, -2.2, 0);
           InstanceModels[inst][3] = InstanceModels[inst][2] * Translation[3] * Rotatation[3];

           //左手掌
           pitch = pose.handLeft.pitch;
           roll = pose.handLeft.roll;
           yaw = pose.handLeft.yaw;
           Translation[4] = translate(0, -3.7, 0);
           Rotatation[4] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           InstanceModels[inst][4] = InstanceModels[inst][3] * Translation[4] * Rotatation[4];

           //右手=========================================================
           // 右上臂
           pitch = pose.upRightArm.pitch;
           roll = pose.upRightArm.roll;
           yaw = pose.upRightArm.yaw;
           Rotatation[5] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           Translation[5] = translate(-4, 0, 0);
           InstanceModels[inst][5] = InstanceModels[inst][0] * Translation[5] * Rotatation[5];

           //右下臂
           pitch = pose.downRightArm;
           Rotatation[6] = rotate(pitch, 1, 0, 0);
           Translation[6] = translate(0, -2.2, 0);
           InstanceModels[inst][6] = InstanceModels[inst][5] * Translation[6] * Rotatation[6];

           //右手掌
           pitch = pose.handRight.pitch;
           roll = pose.handRight.roll;
           yaw = pose.handRight.yaw;
           Translation[7] = translate(0, -3.7, 0);
           Rotatation[7] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           InstanceModels[inst][7] = InstanceModels[inst][6] * Translation[7] * Rotatation[7];

           //腰===================================================
           pitch = pose.downBody.pitch;
           roll = pose.downBody.roll;
           yaw = pose.downBody.yaw;
           Rotatation[8] = rotate(yaw, 0, 1, 0) * rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0);
           Translation[8] = translate(0, -2.2, 0);
           InstanceModels[inst][8] = InstanceModels[inst][0] * Translation[8] * Rotatation[8];

           //左腳=======================================================
           pitch = pose.upLeftLeg.pitch;
           roll = pose.upLeftLeg.roll;
           yaw = pose.upLeftLeg.yaw;
           Rotatation[9] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           Translation[9] = translate(2.0, -1.8, 0);
           InstanceModels[inst][9] = InstanceModels[inst][8] * Translation[9] * Rotatation[9];

           //左小腿
           pitch = pose.downLeftLeg;
           Translation[10] = translate(0.5, -5.0, 0);
           Rotatation[10] = rotate(pitch, 1, 0, 0);
           InstanceModels[inst][10] = InstanceModels[inst][9] * Translation[10] * Rotatation[10];

           //腳掌
           pitch = pose.footLeft.pitch;
           roll = pose.footLeft.roll;
           yaw = pose.footLeft.yaw;
           Translation[11] = translate(0.5, -5.0, 0);
           Rotatation[11] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           InstanceModels[inst][11] = InstanceModels[inst][10] * Translation[11] * Rotatation[11];

           //右腳=======================================================
           pitch = pose.upRightLeg.pitch;
           roll = pose.upRightLeg.roll;
           yaw = pose.upRightLeg.yaw;
           Rotatation[12] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           Translation[12] = translate(-2.0, -1.8, 0);
           InstanceModels[inst][12] = InstanceModels[inst][8] * Translation[12] * Rotatation[12];

           //右小腿
           pitch = pose.downRightLeg;
           Rotatation[13] = rotate(pitch, 1, 0, 0);
           Translation[13] = translate(-0.5, -5.0, 0);
           InstanceModels[inst][13] = InstanceModels[inst][12] * Translation[13] * Rotatation[13];

           //右腳掌
           pitch = pose.footRight.pitch;
           roll = pose.footRight.roll;
           yaw = pose.footRight.yaw;
           Translation[14] = translate(-0.5, -5.0, 0);
           Rotatation[14] = rotate(roll, 0, 0, 1) * rotate(pitch, 1, 0, 0) * rotate(yaw, 0, 1, 0);
           InstanceModels[inst][14] = InstanceModels[inst][13] * Translation[14] * Rotatation[14];

           //最後全部乘上縮放矩陣=============================================================
           for (int l = 0; l < PARTSNUM; l++)
           {
               InstanceModels[inst][l] = InstanceModels[inst][l] * scaleMatrix;
           }
       }
   }

}
