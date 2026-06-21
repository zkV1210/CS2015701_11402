#include "MainScene.h"
#include <Shader/Shader.h>
#include <GLFW/glfw3.h>
#include <Lightning/Lightning.h>
#include <iostream>

namespace CG
{
	void MainScene::Render()
	{
	    static int frameCounter = 0;
		frameCounter++;
		double currentTime = glfwGetTime();

		//Camera
		camera.LookAt(
			camera.getPosition(), // Camera is at (0,0,20), in World Space
			camera.getPosition() + camera.getFront(), // and looks at the origin
			camera.getUp()  // Head is up (set to 0,-1,0 to look upside-down)
		);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO); //打開通往Uniform buffer的通道
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetProjectionMatrix()[0][0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0); //解綁

		updateSSBO();
		RenderShadow();

		if (waterShowcase)
		{
            glBindFramebuffer(GL_FRAMEBUFFER, waterFBO);
            glClearColor(0.1, 0.1, 0.1, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //　鏡像空間
            glm::mat4 normalView = camera.GetViewMatrix();
            glm::mat4 mirrorMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f));
            glm::mat4 reflectionView = normalView * mirrorMatrix;

            // 上傳反轉後的 View 矩陣給 UBO
            glBindBuffer(GL_UNIFORM_BUFFER, UBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &reflectionView[0][0]);
            // Projection 矩陣不變，不需要重傳
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            glFrontFace(GL_CW); //把模型內外倒過來因為上面有換座標系統

            RenderSkybox();
            RenderRobot();
            if (chidoriAttribute.show) RenderChidori(frameCounter);
            if (fireBallAttibute.show) RenderFireball(currentTime);
            RenderFirework();
		}

		// really start

		// =========================================================
		// 1. 背景與緩衝區清理
		// =========================================================
		glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
		glClearColor(0.0, 0.0, 0.0, 1.0);// black screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		//這會在binding = 0上上傳
		glFrontFace(GL_CCW);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO); //打開通往Uniform buffer的通道
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0); //解綁

        //glDepthMask(GL_TRUE);
        RenderSkybox();
        RenderRobot();
        if (chidoriAttribute.show) RenderChidori(frameCounter);

        if (waterShowcase) RenderWater(currentTime);
        else if(dotLightShowcase) RenderFloor();
        if (fireBallAttibute.show)RenderFireball(currentTime);
        RenderFirework();




        glDisable(GL_DEPTH_TEST); // 關閉深度測試，因為我們只要蓋上一層 2D 圖片

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mainColorTexture);

        //post process

        if (motionBlurShowcase)
        {
            // 目標：畫到「這回合」的乒乓緩衝區
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[pingpongIndex]);
            motionBlurShader->use();
            motionBlurShader->setFloat("blurIntensity", 0.75f); // 0.0 ~ 0.99
            motionBlurShader->setInt("currentTexture", 0);

            glActiveTexture(GL_TEXTURE1);
            // 如果現在是 0，上一回合就是 1；反之亦然
            glBindTexture(GL_TEXTURE_2D, pingpongTexture[1 - pingpongIndex]);
            motionBlurShader->setInt("historyTexture", 1);

            // 畫出全螢幕正方形，執行混合！
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, pingpongFBO[pingpongIndex]);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, fboWidth, fboHeight, 0, 0, fboWidth, fboHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            // 3. 乒乓球交換，為下一幀做準備
            pingpongIndex = 1 - pingpongIndex;
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0); // 終於回到預設螢幕！
            glClearColor(1.0, 1.0, 1.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT); // 畫 2D 平面不需要清理深度
            if (mosaicShowcase)
            {
                mosaicShader->use();
                // 設定馬賽克參數 (你可以把這兩個變數放進 ImGui 調整！)
                mosaicShader->setFloat("mosaicSize", 10.0f); // 數字越大越糊
                // 假設你的視窗大小是 1920x1080，請替換成你實際的 glfwGetWindowSize
                mosaicShader->setFloat("screenWidth", fboWidth);
                mosaicShader->setFloat("screenHeight", fboHeight);
                mosaicShader->setInt("screenTexture", 0);
            }
            else
            {
                defaultShader->use();
                defaultShader->setInt("screenTexture", 0);
            }

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

		glFlush();
	}

	void MainScene::RenderSkybox()
	{
        glDepthFunc(GL_LEQUAL);
        skyboxShader->use(); // 切換成天空盒專屬 Shader
        skyboxShader->setInt("skybox", 0); // 告訴它去 0 號貼圖插槽拿圖片
        glDepthMask(GL_FALSE);
        mySkybox->DrawSkybox();    // 裡面會綁定 VAO 並 glDrawArrays
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS); // 畫完天空盒，務必把深度測試恢復成預設值
	}

	void MainScene::RenderRobot()
	{
	    robotShader->use();
  		glBindVertexArray(VAO);


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mySkybox->GetTextureID());
        robotShader->setInt("skybox", 1);
        robotShader->setFloat("reflectionIntensity", envMapState.reflectionIntensity);
        robotShader->setFloat("exposure", envMapState.exposure);
        robotShader->setVec3("lightPos", pointLightPos);
        robotShader->setVec3("lightColor", glm::vec3(1.0f, 0.9f, 0.7f) * lightIntensity);

        glm::vec3 robotPos = GetOffset();
        glm::vec3 targetPos = robotPos; // 瞄準胸口
        glm::vec3 spotDir = glm::normalize(targetPos - pointLightPos);

        robotShader->setVec3("spotDirection", spotDir);
        // 假設內圈 25 度最亮，外圈 35 度產生柔和邊緣 (依照你的 45 度投影範圍內調整)
        robotShader->setFloat("cutOff", glm::cos(glm::radians(25.0f)));
        robotShader->setFloat("outerCutOff", glm::cos(glm::radians(35.0f)));

        // =========================================================
        // 💥 補上 2：影子專用矩陣與深度貼圖
        // =========================================================
        robotShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE3); // 使用第 3 號插槽
        glBindTexture(GL_TEXTURE_2D, depthMap);
        robotShader->setInt("shadowMap", 3);

        robotShader->setInt("useAmbient", ambientShowcase ? 1 : 0);
        robotShader->setInt("useLight", dotLightShowcase ? 1 : 0);
        robotShader->setInt("useToon", toonShowcase ? 1 : 0);

  		// 綁定 SSBO 到 Shader 中設定的 binding = 1
  		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instanceSSBO);

  		// 決定 Draw Call 的呼叫次數
  		int drawCount = isInstanced ? 5 : 1;

  		GLuint offset = 0; // offset for vertices, uvs, normals

  		// 遍歷 15 個零件
  		for (int i = 0; i < PARTSNUM; i++)
  		{
 			// 告訴 Shader 現在準備畫第 i 個零件，請它去 SSBO 自己撈矩陣(動作)
 			robotShader->setInt("u_PartIndex", i);
 			robotShader->setVec3("viewPos", camera.getPosition());

 			int vertexIDoffset = 0;
 			std::string mtlname;

 			// --- 第一階段：畫不透明零件 ---
 			glDepthMask(GL_TRUE); // 允許寫入深度
 			glDisable(GL_BLEND);  // 確保繪製不透明物體時關閉混合

 			for (int j = 0; j < mtls[i].size(); j++) {
				if (Ds[mtls[i][j]] >= 1.0f)
				{
   					mtlname = mtls[i][j];

   					// 把當前材質每個參數傳進去
   					robotShader->setVec3("Material.Kd", KDs[mtlname]);
                            robotShader->setVec3("Material.Ks", KSs[mtlname]);
                            robotShader->setVec3("Material.Ka", KAs[mtlname]);
                            robotShader->setVec3("Material.Ke", KEs[mtlname]);
                            robotShader->setFloat("Material.Ns", NSs[mtlname]);
                            robotShader->setFloat("Material.d",  Ds[mtlname]);

   					// 使用 Instanced 繪製 (支援 1 隻或 5 隻)
   					glDrawArraysInstanced(GL_TRIANGLES, offset + vertexIDoffset, faces[i][j + 1] * 3, drawCount);
				}
				vertexIDoffset += faces[i][j + 1] * 3;
 			}

 			vertexIDoffset = 0;

 			// --- 第二階段：畫透明零件 ---
 			glEnable(GL_BLEND);
 			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 			// 關鍵：這行讓透明物體不「霸佔」深度空間，後面的東西才畫得出來
 			glDepthMask(GL_FALSE);

 			for (int j = 0; j < mtls[i].size(); j++) {
				if (Ds[mtls[i][j]] < 1.0f) // 畫透明的（例如玻璃）
				{
   					mtlname = mtls[i][j];
   					robotShader->setVec3("Material.Kd", KDs[mtlname]);
                    robotShader->setVec3("Material.Ks", KSs[mtlname]);
                    robotShader->setVec3("Material.Ka", KAs[mtlname]);
                    robotShader->setVec3("Material.Ke", KEs[mtlname]);
                    robotShader->setFloat("Material.Ns", NSs[mtlname]);
                    robotShader->setFloat("Material.d",  Ds[mtlname]);

   					// 使用 Instanced 繪製 (支援 1 隻或 5 隻)
   					glDrawArraysInstanced(GL_TRIANGLES, offset + vertexIDoffset, faces[i][j + 1] * 3, drawCount);
				}
				vertexIDoffset += faces[i][j + 1] * 3;
 			}
 			glDepthMask(GL_TRUE); // 畫完透明物體，恢復深度寫入

 			// 換下個零件的 VBO 偏移量
 			offset += vertices_size[i];
  		}
	}

	void MainScene::updateSSBO()
	{
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
        if (isInstanced) {
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(InstanceModels), InstanceModels);
        } else {
            glm::mat4 singleInstanceData[PARTSNUM];
            for(int p = 0; p < PARTSNUM; p++) {
                singleInstanceData[p] = Models[p];
            }
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(singleInstanceData), singleInstanceData);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void MainScene::RenderChidori(int frame)
	{
        if (frame % 4 == 0) {
            // 呼叫物件的 Generate 函式
            chidori->Generate(5.0f, 10, 0.25f);
        }

        std::vector<glm::mat4> lightningTransforms;


        int handIndex = chidoriAttribute.rightHand ? 7 : 4;
        glm::mat4 handMatrix = Models[handIndex];
        glm::vec4 localOffset = glm::vec4(chidoriAttribute.offset, -0.25f, 0.0f, 1.0f);
        glm::vec3 centerPos = glm::vec3(handMatrix * localOffset);

        for(int i = 0; i < 100; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, centerPos);

            float randYaw   = glm::radians((float)(rand() % 360));
            float randPitch = glm::radians((float)(rand() % 360));
            float randRoll  = glm::radians((float)(rand() % 360));

            model = glm::rotate(model, randYaw,   glm::vec3(0, 1, 0));
            model = glm::rotate(model, randPitch, glm::vec3(1, 0, 0));
            model = glm::rotate(model, randRoll,  glm::vec3(0, 0, 1));

            // =========================================================
            // 💥 黑魔法：機率判定 (控制長短的比例)
            // =========================================================
            float randScale = 1.0f;
            int chance = rand() % 100; // 產生 0 ~ 99 的隨機數字 (丟骰子)

            if (chance < 3) {
                // 【突變種】5% 的機率：變成超級長的電弧
                // 基礎長度 2.5 倍，外加 0.0 ~ 2.0 倍的隨機浮動 = 總長 2.5 ~ 4.5 倍
                randScale = 5.0f + ((rand() % 100) / 100.0f) * 2.0f;
            } else if(chance < 5)
            {
                randScale = 1.5f + ((rand() % 100) / 100.0f) * 1.0f;
            }
            else {
                // 【普通兵】85% 的機率：維持在核心周圍的短閃電
                // 基礎長度 0.4 倍，外加 0.0 ~ 0.8 倍的隨機浮動 = 總長 0.4 ~ 1.2 倍
                randScale = 0.4f + ((rand() % 100) / 100.0f) * 0.8f;
            }

            // 把算好的長度乘進矩陣裡
            randScale *= chidoriAttribute.intensity;
            model = glm::scale(model, glm::vec3(randScale, randScale, randScale));
            // =========================================================
            lightningTransforms.push_back(model);
        }

        lightningShader->use();
        lightningShader->setVec3("lightningColor", chidoriAttribute.color);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // 顏色直接相加，越疊越亮！

        chidori->Draw(lightningTransforms); // 畫出 100 條閃電

        // 畫完務必恢復成標準的透明度混合模式，免得影響後面的東西
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
	}

	void MainScene::RenderWater(double cTime)
	{
        waterShader->use();

        // 1. 傳送矩陣與基本參數
        waterShader->setMat4("Model", glm::mat4(1.0f));
        waterShader->setFloat("u_Time", (float)cTime); // 波浪的引擎！
        waterShader->setVec3("viewPos", camera.getPosition());
        waterShader->setVec3("waterColor", waterState.waterColor); // 深海藍色
        waterShader->setFloat("distortionCoefficient", waterState.distortionCo);
        waterShader->setFloat("reflectionRate", waterState.reflectionRate);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, waterColorTexture); // 你在 Initialize 建的那個 ID
        waterShader->setInt("reflectionTexture", 2);

        // 3. 畫出那 6 萬個頂點
        glBindVertexArray(floorVAO);
        // 100 * 100 個格子 * 6 個頂點 = 60000
        glDrawArrays(GL_TRIANGLES, 0, waterVertexCount);
        glBindVertexArray(0);
	}

	void MainScene::RenderRobotDepthOnly()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glBindVertexArray(VAO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instanceSSBO);

        int drawCount = isInstanced ? 5 : 1;
        GLuint offset = 0;

        for (int i = 0; i < PARTSNUM; i++)
        {
            // 告訴 Shader 去 SSBO 拿第 i 個零件的矩陣
            shadowDepthShader->setInt("u_PartIndex", i);

            int vertexIDoffset = 0;
            for (int j = 0; j < mtls[i].size(); j++) {
                glDrawArraysInstanced(GL_TRIANGLES, offset + vertexIDoffset, faces[i][j + 1] * 3, drawCount);
                vertexIDoffset += faces[i][j + 1] * 3;
            }
            offset += vertices_size[i];
        }
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

	void MainScene::RenderShadow()
	{
	    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 500.0f);

        // 2. 讓聚光燈相機盯著柯博文胸口
        glm::vec3 robotPos = GetOffset();
        glm::vec3 targetPos = robotPos;
        glm::vec3 lightPos = GetPointLightPos();
        glm::mat4 lightView = glm::lookAt(lightPos, targetPos, glm::vec3(0.0, 1.0, 0.001f));

        // 把算好的矩陣存進 class 變數，稍後給 RenderRobot 用
        lightSpaceMatrix = lightProjection * lightView;

        // 3. 綁定影子畫布並畫出深度
        shadowDepthShader->use();
        shadowDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); // 切換成高解析度
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glClear(GL_DEPTH_BUFFER_BIT); // 只要清空深度

        RenderRobotDepthOnly(); // 用剛才寫的極簡模式畫

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, fboWidth, fboHeight);
	}

	void MainScene::RenderFloor()
    {
        floorShader->use();

        // 1. 傳送矩陣
        glm::mat4 model = glm::mat4(1.0f); // 放在世界原點
        floorShader->setMat4("model", model);
        floorShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        // 2. 傳送光照參數
        floorShader->setVec3("lightPos", pointLightPos);
        floorShader->setVec3("viewPos", camera.getPosition());

        glm::vec3 targetPos = GetOffset(); // 瞄準胸口
        glm::vec3 spotDir = glm::normalize(targetPos - pointLightPos);

        floorShader->setVec3("spotDirection", spotDir);
        floorShader->setFloat("cutOff", glm::cos(glm::radians(25.0f)));
        floorShader->setFloat("outerCutOff", glm::cos(glm::radians(35.0f)));

        // 3. 綁定剛剛拍好的深度照片 (shadowMap)
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        floorShader->setInt("shadowMap", 3);

        // 4. 畫出你在 SetupFloor 建好的 6 萬個頂點
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, waterVertexCount);
        glBindVertexArray(0);
    }

    void MainScene::RenderFireball(double cTime)
    {

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // 火焰同樣用加法混色最漂亮
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        fireballShader->use();
        glm::mat4 headMatrix = Models[1];
        glm::vec3 headWorldPos = glm::vec3(headMatrix[3]);
        glm::vec3 faceDir = glm::normalize(glm::vec3(headMatrix[2]));
        glm::vec3 mouthWorldPos = headWorldPos + (faceDir * (fireBallAttibute.offset + 21.6f));

        // 2. 傳送你在 UI 上拉動的豪火球屬性
        fireballShader->setVec3("u_position", mouthWorldPos);
        fireballShader->setVec3("u_direction", faceDir);
        fireballShader->setFloat("u_intensity", fireBallAttibute.intensity);
        fireballShader->setFloat("u_speed", fireBallAttibute.speed);

        // 3. 傳送時間
        fireballShader->setFloat("u_time", (float)cTime);

        // 💥 綁定空 VAO，然後直接命令 GPU：「畫 1 個點！」

        glBindVertexArray(VAO); // 繼續借用機器人的 VAO 騙過管線

        // 💥 關鍵修改：我們不再畫點了，我們直接請 GPU 畫 6 個頂點組成正方形！
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);

        // 復原狀態
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

    void MainScene::RenderFirework()
    {
        // 如果當前沒有任何存活的煙火，直接 Return 節省效能
        if (fireworkRendering.empty()) return;


        // --- 1. 設定渲染狀態 ---
        glEnable(GL_PROGRAM_POINT_SIZE); // 允許在 Shader 中修改粒子大小
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // 💥 煙火必須使用加法混色 (Additive Blending) 才能疊加出高溫感

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE); // 關閉深度寫入，避免粒子之間互相遮擋產生黑邊
        glDisable(GL_CULL_FACE);

        fireworkShader->use();
        glBindVertexArray(VAO);

        // --- 2. 遍歷並渲染所有存活的煙火 ---
        for (const auto& fw : fireworkRendering)
        {
            // 防呆：如果煙火還沒爆炸，或壽命已盡，跳過
            if (fw.age < 0.0) continue;

            // 計算標準化壽命 (0.0 到 1.0)
            float lifespan = FIREWORK_DEFAULT / fw.attributes.speed;
            float normalizedLife = glm::clamp((float)(fw.age / lifespan), 0.0f, 1.0f);

            // 傳遞 Uniform
            fireworkShader->setVec3("u_centerPos", fw.attributes.position);
            fireworkShader->setVec3("u_baseColor", fw.attributes.color);
            fireworkShader->setVec3("u_direction", fw.attributes.direction);
            fireworkShader->setFloat("u_age", (float)fw.age);
            fireworkShader->setFloat("u_life", normalizedLife);
            fireworkShader->setFloat("u_speed", fw.attributes.speed);
            fireworkShader->setFloat("u_intensity", fw.attributes.intensity);

            // 💥 暴力發射：直接請 GPU 憑空畫 1500 個點
            glDrawArrays(GL_TRIANGLES, 0, 1500 * 6);
        }

        glBindVertexArray(0);

        // --- 3. 恢復狀態 ---
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }
}
