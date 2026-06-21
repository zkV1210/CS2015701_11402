#include <Utilty/LoadShaders.h>
#include <Utilty/OBJLoader.hpp>
#include <imgui.h>

#include "MainScene.h"

//搬移
static glm::mat4 translate(float x, float y, float z)
{
	glm::vec4 t = glm::vec4(x, y, z, 1);//w = 1 ,則x,y,z=0時也能translate
	glm::vec4 c1 = glm::vec4(1, 0, 0, 0);
	glm::vec4 c2 = glm::vec4(0, 1, 0, 0);
	glm::vec4 c3 = glm::vec4(0, 0, 1, 0);
	glm::mat4 M = glm::mat4(c1, c2, c3, t);
	return M;
}
//縮放
static glm::mat4 scale(float x, float y, float z)
{
	glm::vec4 c1 = glm::vec4(x, 0, 0, 0);
	glm::vec4 c2 = glm::vec4(0, y, 0, 0);
	glm::vec4 c3 = glm::vec4(0, 0, z, 0);
	glm::vec4 c4 = glm::vec4(0, 0, 0, 1);
	glm::mat4 M = glm::mat4(c1, c2, c3, c4);
	return M;
}

//旋轉
static glm::mat4 rotate(float angle, float x, float y, float z)
{
	float r = glm::radians(angle);
	glm::mat4 M = glm::mat4(1);

	glm::vec4 c1 = glm::vec4(cos(r) + (1 - cos(r)) * x * x, (1 - cos(r)) * y * x + sin(r) * z, (1 - cos(r)) * z * x - sin(r) * y, 0);
	glm::vec4 c2 = glm::vec4((1 - cos(r)) * y * x - sin(r) * z, cos(r) + (1 - cos(r)) * y * y, (1 - cos(r)) * z * y + sin(r) * x, 0);
	glm::vec4 c3 = glm::vec4((1 - cos(r)) * z * x + sin(r) * y, (1 - cos(r)) * z * y - sin(r) * x, cos(r) + (1 - cos(r)) * z * z, 0);
	glm::vec4 c4 = glm::vec4(0, 0, 0, 1);
	M = glm::mat4(c1, c2, c3, c4);
	return M;
}

namespace CG
{
	MainScene::MainScene()
	{
	}

	MainScene::~MainScene()
	{
	}

	auto MainScene::Initialize() -> bool
	{
		return LoadScene();
	}

	void MainScene::Update(double dt)
	{
		UpdateAction(dt);
		if (isInstanced)
		{
			UpdateInstanceModel(); // 算 5 隻
		}
		else
		{
			UpdateModel();         // 算 1 隻
		}

        if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            rotatingView = true;
            if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                movingView = true;
                rotatingView = false;
            }

            else
                movingView = false;

        }
        else
        {
            rotatingView = false;
			movingView = false;
        }



	}

	void MainScene::Render()
		{
			// =========================================================
			// 1. 背景與緩衝區清理
			// =========================================================
			glClearColor(0.8, 0.8, 0.8, 1); // black screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glPolygonMode(GL_FRONT_AND_BACK, mode);

			glBindVertexArray(VAO); //給格式
			glUseProgram(program);  // uniform參數數值前必須先use shader

			float eyey = glm::radians(eyeAngley);

			// =========================================================
			// 2. 更新攝影機與 MVP 矩陣
			// =========================================================
			camera.LookAt(
				camera.getPosition(), // Camera is at (0,0,20), in World Space
				camera.getPosition() + camera.getFront(), // and looks at the origin
				camera.getUp()  // Head is up (set to 0,-1,0 to look upside-down)
			);

			glBindBuffer(GL_UNIFORM_BUFFER, UBO); //打開通往Uniform buffer的通道
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetProjectionMatrix()[0][0]);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);


			// =========================================================
			// 3. 畫地板 (傳統 Uniform 模式)
			// =========================================================
			glBindVertexArray(floorVAO);

			// 告訴 Shader 現在畫的是地板 (-1 代表觸發傳統 Model 讀取)
			glUniform1i(PartIndexID, -1);

			// 給地板一個簡單的 Model 矩陣
			glm::mat4 floorModel = glm::mat4(1.0f);
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, &floorModel[0][0]);

			// 設定地板材質顏色 (深灰色)
			glm::vec3 gray(0.2f, 0.2f, 0.2f);
			glUniform3fv(M_KdID, 1, &gray[0]);
			glUniform3fv(M_KsID, 1, &gray[0]);
			glUniform1f(M_dID, 1.0f);

			glDrawArrays(GL_TRIANGLES, 0, 6);


			// =========================================================
			// 4. 畫機器人 (SSBO Instanced 模式 - 高效能 Streaming)
			// =========================================================
			glBindVertexArray(VAO);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);

			// 準備傳送 SSBO 的資料包 (使用 glBufferSubData 進行局部更新，不重新挖記憶體)
			if (isInstanced)
			{
				// 影分身模式：將算好的 5 隻矩陣覆蓋進去
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(InstanceModels), InstanceModels);
			}
			else
			{
				// 單機模式：只傳送 1 隻的資料 (15 個 mat4)
				glm::mat4 singleInstanceData[PARTSNUM];
				for(int p = 0; p < PARTSNUM; p++) {
					singleInstanceData[p] = Models[p];
				}
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(singleInstanceData), singleInstanceData);
			}

			// 綁定 SSBO 到 Shader 中設定的 binding = 1
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instanceSSBO);

			// 決定 Draw Call 的呼叫次數
			int drawCount = isInstanced ? 5 : 1;

			GLuint offset = 0; // offset for vertices, uvs, normals

			// 遍歷 15 個零件
			for (int i = 0; i < PARTSNUM; i++)
			{
				// 告訴 Shader 現在準備畫第 i 個零件，請它去 SSBO 自己撈矩陣
				glUniform1i(PartIndexID, i);

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
						glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
						glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
						glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
						glUniform3fv(M_KeID, 1, &KEs[mtlname][0]);
						glUniform1f(M_NsID, NSs[mtlname]);
						glUniform1f(M_dID,  Ds[mtlname]);

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

						glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
						glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
						glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
						glUniform3fv(M_KeID, 1, &KEs[mtlname][0]);
						glUniform1f(M_NsID, NSs[mtlname]);
						glUniform1f(M_dID,  Ds[mtlname]);

						// 使用 Instanced 繪製 (支援 1 隻或 5 隻)
						glDrawArraysInstanced(GL_TRIANGLES, offset + vertexIDoffset, faces[i][j + 1] * 3, drawCount);
					}
					vertexIDoffset += faces[i][j + 1] * 3;
				}
				glDepthMask(GL_TRUE); // 畫完透明物體，恢復深度寫入

				// 換下個零件的 VBO 偏移量
				offset += vertices_size[i];
			}

			// really start
			glFlush();
		}
/*
	void MainScene::Render()
	{
	    //back ground and refresh
		glClearColor(0.8, 0.8, 0.8, 1); //black screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		glBindVertexArray(VAO); //給格式
		glUseProgram(program);//uniform參數數值前必須先use shader

		float eyey = glm::radians(eyeAngley);

		//camera.printInfo();
		camera.LookAt(
			camera.getPosition(), // Camera is at (0,0,20), in World Space
			camera.getPosition() + camera.getFront(), // and looks at the origin
			camera.getUp()  // Head is up (set to 0,-1,0 to look upside-down)
		);

		//update data to UBO for MVP
		// (update camera)
		glBindBuffer(GL_UNIFORM_BUFFER, UBO); //打開通往Uniform buffer的通道
		//把要丟的值丟進去
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetProjectionMatrix()[0][0]);
		//關掉
		glBindBuffer(GL_UNIFORM_BUFFER, 0);


        glBindVertexArray(floorVAO);
        // 給地板一個簡單的 Model 矩陣（單位矩陣即可，或是微調高度避免與 0 點衝突）
        glm::mat4 floorModel = glm::mat4(1.0f);
        glUniformMatrix4fv(ModelID, 1, GL_FALSE, &floorModel[0][0]);

        // 設定地板材質顏色 (例如深灰色，避免蓋過柯博文)
        glm::vec3 gray(0.2f, 0.2f, 0.2f);
        glUniform3fv(M_KdID, 1, &gray[0]);
        glUniform3fv(M_KsID, 1, &gray[0]);
        glUniform1f(M_dID, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(VAO);
    		// =========================實際操作model相關=========================
		GLuint offset = 0;//offset for vertices , uvs , normals
		for (int i = 0; i < PARTSNUM; i++)
		{
		    //sending the data for model, like hand gesture
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Models[i][0][0]);

			//這邊是根據零件配對mtl的過程
			int vertexIDoffset = 0;//glVertexID's offset
			std::string mtlname;//material name

			//針對他的不同材質部分渲染,第一個迴圈先渲不透明的第二個渲透明的
			glDepthMask(GL_TRUE); // 允許寫入深度
            for (int j = 0; j < mtls[i].size(); j++) {
                if (Ds[mtls[i][j]] >= 1.0f)
                {
                    //查這零件總共有什麼材質一個一個更新
                   	mtlname = mtls[i][j];

                   	//把當前材質每個參數傳進去
                   	glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
                   	glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
                  	glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
        			glUniform3fv(M_KeID, 1, &KEs[mtlname][0]);
        			glUniform1f(M_NsID, NSs[mtlname]); // 高光係數 (金屬質感)
                    glUniform1f(M_dID,  Ds[mtlname]);
                    //從這個零件（靠offset表示）開始+vertexIDoffset表示同一材質的從哪裡開始，畫到faces[i][j + 1] * 3
                    glDrawArrays(GL_TRIANGLES, offset+vertexIDoffset, faces[i][j + 1] * 3);
                }
                vertexIDoffset += faces[i][j + 1] * 3;//glVertexID's base offset is face count*3
            }

            vertexIDoffset = 0;
            // --- 第二階段：畫透明零件 ---
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            // 關鍵：這行讓透明物體不「霸佔」深度空間，後面的東西才畫得出來
            glDepthMask(GL_FALSE);


            for (int j = 0; j < mtls[i].size(); j++) {
                if (Ds[mtls[i][j]] < 1.0f) // 畫透明的（玻璃）
                {

                    //查這零件總共有什麼材質一個一個更新
                    mtlname = mtls[i][j];

                    //把當前材質每個參數傳進去
                	glUniform3fv(M_KdID, 1, &KDs[mtlname][0]);
                	glUniform3fv(M_KsID, 1, &KSs[mtlname][0]);
            		glUniform3fv(M_KaID, 1, &KAs[mtlname][0]);
                    glUniform3fv(M_KeID, 1, &KEs[mtlname][0]);
                  	glUniform1f(M_NsID, NSs[mtlname]); // 高光係數 (金屬質感)
                    glUniform1f(M_dID,  Ds[mtlname]);

                    //從這個零件（靠offset表示）開始+vertexIDoffset表示同一材質的從哪裡開始，畫到faces[i][j + 1] * 3
                    glDrawArrays(GL_TRIANGLES, offset+vertexIDoffset, faces[i][j + 1] * 3);
                }
                vertexIDoffset += faces[i][j + 1] * 3;//glVertexID's base offset is face count*3
            }
            glDepthMask(GL_TRUE);
            //換下個零件
			offset += vertices_size[i];
		}
		//really start
		glFlush();
	}
 */

	/* 鍵盤滑鼠互動區 */
	void MainScene::OnResize(int width, int height)
	{
		std::cout << "MainScene Resize: " << width << " " << height << std::endl;
	}

	void MainScene::OnKeyboard(int key, int action)
	{
		//std::cout << "MainScene OnKeyboard: " << key << " " << action << std::endl;

		if(action == GLFW_REPEAT || action == GLFW_PRESS)
		{
    		switch (key)
    		{
          		case GLFW_KEY_W:
         			camera.UpdatePosition(camera.getFront() * 0.5f);
         			break;
          		case GLFW_KEY_S:
         			camera.UpdatePosition(camera.getFront() * -0.5f);
         			break;
          		case GLFW_KEY_A:
         			camera.UpdatePosition(camera.getRight() * -0.5f);
         			break;
          		case GLFW_KEY_D:
         			camera.UpdatePosition(camera.getRight() * 0.5f);
         			break;
          		case GLFW_KEY_Q:
         			camera.UpdatePosition(camera.getUp() * -0.5f);
         			break;
                case GLFW_KEY_E:
                    camera.UpdatePosition(camera.getUp() * 0.5f);
                    break;
    		}
		}
	}

    void MainScene::OnMouseButton(int button, int action)
    {
        ImGuiIO& io = ImGui::GetIO();

        //std::cout << movingView << " " << io.WantCaptureMouse << std::endl;

        if (io.WantCaptureMouse)
            return;

        //std::cout << "MainScene OnMouseButton: " << button << " " << action << std::endl;
        MouseButton = button;
        MouseAction = action;

    }

    void MainScene::OnMouseMove(double x, double y)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (io.WantCaptureMouse)
            return;

        if (rotatingView)
        {
            dx = x - lastX;
            dy = y - lastY;
			camera.UpdateYaw(dx * 0.1f, camera.getFront());
			camera.UpdatePitch(-dy * 0.1f, camera.getFront());
			camera.UpdateFront();
            camera.LookAt(camera.getPosition(), camera.getPosition() + camera.getUp(), glm::vec3(0, 1, 0));
        }
        if (movingView)
        {
			dx = x - lastX;
			dy = y - lastY;
			camera.UpdatePosition(camera.getRight() * (dx) * -0.05f + camera.getUp() * dy * 0.05f);
        }
        lastX = x;
        lastY = y;
    }

    void MainScene::OnMouseScroll(double x_offset, double y_offset)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (io.WantCaptureMouse)
            return;

		camera.UpdatePosition(glm::vec3 (0.0f, 0.0f, -y_offset));

    }


	/* 參數設定交互區 */
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
		//然後把update通通寫進去
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
		}
		//然後把update通通寫進去
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

	/* 初始化的function們 */
	auto MainScene::LoadScene() -> bool
	{
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		//VAO 打開
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//讀shader
		ShaderInfo shaders[] = {
			{ GL_VERTEX_SHADER, "res/shaders/Robot.vp" },//vertex shader
			{ GL_FRAGMENT_SHADER, "res/shaders/Robot.fp" },//fragment shader
			{ GL_NONE, NULL } };
		program = LoadShaders(shaders); //讀取shader

		glUseProgram(program);//uniform參數數值前必須先use shader

		//告訴顯卡到哪個program的shader校正
		MatricesIdx = glGetUniformBlockIndex(program, "MatVP");
		ModelID = glGetUniformLocation(program, "Model"); // 原本的保留給地板用
		PartIndexID = glGetUniformLocation(program, "u_PartIndex");
		M_KaID = glGetUniformLocation(program, "Material.Ka");
        M_KdID = glGetUniformLocation(program, "Material.Kd");
        M_KsID = glGetUniformLocation(program, "Material.Ks");
        M_KeID = glGetUniformLocation(program, "Material.Ke");
        M_NsID = glGetUniformLocation(program, "Material.Ns");
        M_dID  = glGetUniformLocation(program, "Material.d");

		// Camera matrix
		camera.LookAt(glm::vec3(0, 10, 25), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		LoadModel();

        // 1. 定義頂點格式（永遠從大 VBO 的起點 0 開始看）
        // 此時VAO也在紀錄格式 VAO是紀錄怎麼讀Vertex檔案的
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        // 2. 定義 UV 格式
        glBindBuffer(GL_ARRAY_BUFFER, uVBO);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);

        // 3. 定義法向量格式
        glBindBuffer(GL_ARRAY_BUFFER, nVBO);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);

        // 錄製完畢，解綁
        glBindVertexArray(0);

        glGenBuffers(1, &instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
        // 注意最後一個參數傳 NULL，代表只挖洞，不填土
        glBufferData(GL_SHADER_STORAGE_BUFFER, 5 * 15 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        SetupFloor();

		//UBO
		//存VP 把Uniform空間開好
		glGenBuffers(1, &UBO);

		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_DYNAMIC_DRAW);
		//get uniform struct size
		int UBOsize = 0;
		glGetActiveUniformBlockiv(program, MatricesIdx, GL_UNIFORM_BLOCK_DATA_SIZE, &UBOsize);
		//bind UBO to its idx
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, UBOsize);
		glUniformBlockBinding(program, MatricesIdx, 0);

		InitializeStand();
		InitializePushUp();
		InitializeSitUp();
		InitializeWalk();
		InitializeHopakDance();
		InitializeSCJ();
		InitializeAPTDance();

		return true;
	}

	void MainScene::LoadModel()
	{
        //需要針對我們的模型去調整
        //read mtl then read obj by each part to smaller VBOs, and combine all of them to a big VBO
	    //Load MTL
 	    std::vector<glm::vec3> Kds;
    	std::vector<glm::vec3> Kas;
    	std::vector<glm::vec3> Kss;
    	std::vector<glm::vec3> Kes;
        std::vector<float> Nis;
        std::vector<float> Nss;
        std::vector<float> ds;
        std::vector<int> illums;
    	std::vector<std::string> Materials;//mtl-name
    	std::string texture;
        LoadMTL("res/models/model.mtl",
                    Kds, Kas, Kss, Kes, Nis, Nss, ds, illums, Materials, texture
        );
    	for (int i = 0; i < Materials.size(); i++)
    	{
    		std::string mtlname = Materials[i];
    		//  name       vec3
    		KDs[mtlname] = Kds[i];
            KSs[mtlname] = Kss[i];
            KAs[mtlname] = Kas[i];
            KEs[mtlname] = Kes[i];
            NSs[mtlname] = Nss[i];
            NIs[mtlname] = Nis[i];
            Ds[mtlname] = ds[i];
            ILLUMs[mtlname] = illums[i];
    	}
		Load2Buffer("res/models/chest.obj", 0); //胸部
		Load2Buffer("res/models/head.obj", 1); //頭

		Load2Buffer("res/models/l_upperarm.obj", 2); //左上臂
		Load2Buffer("res/models/l_forearm.obj", 3); //左下臂
		Load2Buffer("res/models/l_hand.obj", 4); //左手

		Load2Buffer("res/models/r_upperarm.obj", 5); //右上臂
		Load2Buffer("res/models/r_forearm.obj", 6); //右下臂
		Load2Buffer("res/models/r_hand.obj", 7); //右手

		Load2Buffer("res/models/hip.obj", 8); //腰

		Load2Buffer("res/models/l_DEF-thigh.obj", 9); //左大腿
		Load2Buffer("res/models/l_shin.obj", 10); //左小腿
		Load2Buffer("res/models/l_feet.obj", 11); //左腳

		Load2Buffer("res/models/r_DEF-thigh.obj", 12); //右大腿
		Load2Buffer("res/models/r_shin.obj", 13); //右小腿
		Load2Buffer("res/models/r_feet.obj", 14); //右腳

		GLuint totalSize[3] = { 0,0,0 };
		GLuint offset[3] = { 0,0,0 };

		// calcutating the big VBO's size by sum of all part
		for (int i = 0; i < PARTSNUM; i++)
		{
			totalSize[0] += vertices_size[i] * sizeof(glm::vec3);
			totalSize[1] += uvs_size[i] * sizeof(glm::vec2);
			totalSize[2] += normals_size[i] * sizeof(glm::vec3);
		}

		//generate vbo
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &uVBO);
		glGenBuffers(1, &nVBO);
		//bind vbo ,第一次bind也同等於 create vbo
		glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO的target是GL_ARRAY_BUFFER
		glBufferData(GL_ARRAY_BUFFER, totalSize[0], NULL, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uVBO);//VBO的target是GL_ARRAY_BUFFER
		glBufferData(GL_ARRAY_BUFFER, totalSize[1], NULL, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, nVBO);//VBO的target是GL_ARRAY_BUFFER
		glBufferData(GL_ARRAY_BUFFER, totalSize[2], NULL, GL_STATIC_DRAW);

		for (int i = 0; i < PARTSNUM; i++)
		{
			glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
			glBindBuffer(GL_COPY_READ_BUFFER, VBOs[i]);
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[0], vertices_size[i] * sizeof(glm::vec3));
			offset[0] += vertices_size[i] * sizeof(glm::vec3);
			glInvalidateBufferData(VBOs[i]);//free vbo
			glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

			glBindBuffer(GL_COPY_WRITE_BUFFER, uVBO);
			glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[1], uvs_size[i] * sizeof(glm::vec2));
			offset[1] += uvs_size[i] * sizeof(glm::vec2);
			glInvalidateBufferData(uVBOs[i]);//free vbo
			glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

			glBindBuffer(GL_COPY_WRITE_BUFFER, nVBO);
			glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[2], normals_size[i] * sizeof(glm::vec3));
			offset[2] += normals_size[i] * sizeof(glm::vec3);
			glInvalidateBufferData(nVBOs[i]);//free vbo
			glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		}
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	}

	void MainScene::Load2Buffer(const char* obj, int i)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals; // Won't be used at the moment.

		bool res = LoadOBJ(obj, vertices, uvs, normals, faces[i], mtls[i]);
		if (!res) printf("load failed\n");

		glGenBuffers(1, &VBOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		vertices_size[i] = vertices.size();

		glGenBuffers(1, &uVBOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, uVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
		uvs_size[i] = uvs.size();

		glGenBuffers(1, &nVBOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, nVBOs[i]);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		normals_size[i] = normals.size();
	}

	/* 參數刷新 */
	void MainScene::UpdateAction(double dt)
	{
		static double _frame = 0;
		static double _frameForSCJ = 0;
		static double _frameForInstance[5] = {0,0,0,0,0};
		if (edit)
		{
		    SetInstanced(false);
		    UpdateOffset(rootOffset.x, rootOffset.y, rootOffset.z);
            UpdateJointSet(rendering);
		}
		else {
    		if (action == 0)
    		{
                _frameForSCJ = 0;
                SetInstanced(false);
                _frame = 0;
                UpdateOffset(0, 16, 0);
                ResetJointSet();
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
    			}
                else
                {
                    ResetJointSet();
                    UpdateOffset(0, 16, 0);
                }
    		}
            else if (action = 7)
            {

                _frameForSCJ += dt * playSpeed;

                if (_frameForSCJ <= SCJ.totalTimeSpan)
                {
                    SetInstanced(false);
                    JointSet pose = calcutatingJointSet(_frameForSCJ,SCJ);
                    UpdateJointSet(pose);
                    glm::vec3 position = calcutatingPosition(_frameForSCJ,SCJ);
                    UpdateOffset(position.x, position.y, position.z);
                }
                else
                {
                    SetInstanced(true);
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

	void MainScene::UpdateOffset(float x,float y,float z)
	{
        rootOffset = glm::vec3(x, y, z);
	}

	void MainScene::UpdateJointSet(JointSet frame)
	{
	    rendering = frame;
	}

	void MainScene::UpdateJointInstance(JointSet frame, int index)
		{
			if (index >= 0 && index < 5) {
				instanceRendering[index] = frame;
			}
		}

	void MainScene::UpdateOffsetInstance(float x, float y, float z, int index)
	{
		if (index >= 0 && index < 5) {
			instanceRootOffset[index] = glm::vec3(x, y, z);
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

	/* 協助計算 */
	MainScene::JointSet MainScene::calcutatingJointSet(double time,keyFrameSet frameSet)
	{
	    double timeL = 0,timeN = 0;
		int frameNow = 0,frameNext = 0;
		//need use while
	    for (int l = 0;l<frameSet.frames.size();l++)
		{
		    if(timeL + frameSet.frames[l].time >= time)
			{
			    frameNow = l;
				frameNext = frameSet.frames[l].nextFrameIndex;
				timeN = frameSet.frames[frameNow].time;
				break;
			}
			timeL += frameSet.frames[l].time;
		}

		JointSet dMovement;

		dMovement.head.pitch = frameSet.frames[frameNow].pose.head.pitch + (frameSet.frames[frameNext].pose.head.pitch-frameSet.frames[frameNow].pose.head.pitch)*((time-timeL)/timeN);
        dMovement.head.roll = frameSet.frames[frameNow].pose.head.roll + (frameSet.frames[frameNext].pose.head.roll-frameSet.frames[frameNow].pose.head.roll)*((time-timeL)/timeN);
        dMovement.head.yaw = frameSet.frames[frameNow].pose.head.yaw + (frameSet.frames[frameNext].pose.head.yaw-frameSet.frames[frameNow].pose.head.yaw)*((time-timeL)/timeN);

        dMovement.core.pitch = frameSet.frames[frameNow].pose.core.pitch + (frameSet.frames[frameNext].pose.core.pitch-frameSet.frames[frameNow].pose.core.pitch)*((time-timeL)/timeN);
        dMovement.core.roll = frameSet.frames[frameNow].pose.core.roll + (frameSet.frames[frameNext].pose.core.roll-frameSet.frames[frameNow].pose.core.roll)*((time-timeL)/timeN);
        dMovement.core.yaw = frameSet.frames[frameNow].pose.core.yaw + (frameSet.frames[frameNext].pose.core.yaw-frameSet.frames[frameNow].pose.core.yaw)*((time-timeL)/timeN);

        dMovement.downBody.pitch = frameSet.frames[frameNow].pose.downBody.pitch + (frameSet.frames[frameNext].pose.downBody.pitch-frameSet.frames[frameNow].pose.downBody.pitch)*((time-timeL)/timeN);
        dMovement.downBody.roll = frameSet.frames[frameNow].pose.downBody.roll + (frameSet.frames[frameNext].pose.downBody.roll-frameSet.frames[frameNow].pose.downBody.roll)*((time-timeL)/timeN);
        dMovement.downBody.yaw = frameSet.frames[frameNow].pose.downBody.yaw + (frameSet.frames[frameNext].pose.downBody.yaw-frameSet.frames[frameNow].pose.downBody.yaw)*((time-timeL)/timeN);

        dMovement.upRightArm.pitch = frameSet.frames[frameNow].pose.upRightArm.pitch + (frameSet.frames[frameNext].pose.upRightArm.pitch-frameSet.frames[frameNow].pose.upRightArm.pitch)*((time-timeL)/timeN);
        dMovement.upRightArm.roll = frameSet.frames[frameNow].pose.upRightArm.roll + (frameSet.frames[frameNext].pose.upRightArm.roll-frameSet.frames[frameNow].pose.upRightArm.roll)*((time-timeL)/timeN);
        dMovement.upRightArm.yaw = frameSet.frames[frameNow].pose.upRightArm.yaw + (frameSet.frames[frameNext].pose.upRightArm.yaw-frameSet.frames[frameNow].pose.upRightArm.yaw)*((time-timeL)/timeN);

        dMovement.upRightLeg.pitch = frameSet.frames[frameNow].pose.upRightLeg.pitch + (frameSet.frames[frameNext].pose.upRightLeg.pitch-frameSet.frames[frameNow].pose.upRightLeg.pitch)*((time-timeL)/timeN);
        dMovement.upRightLeg.roll = frameSet.frames[frameNow].pose.upRightLeg.roll + (frameSet.frames[frameNext].pose.upRightLeg.roll-frameSet.frames[frameNow].pose.upRightLeg.roll)*((time-timeL)/timeN);
        dMovement.upRightLeg.yaw = frameSet.frames[frameNow].pose.upRightLeg.yaw + (frameSet.frames[frameNext].pose.upRightLeg.yaw-frameSet.frames[frameNow].pose.upRightLeg.yaw)*((time-timeL)/timeN);

        dMovement.footRight.pitch = frameSet.frames[frameNow].pose.footRight.pitch + (frameSet.frames[frameNext].pose.footRight.pitch-frameSet.frames[frameNow].pose.footRight.pitch)*((time-timeL)/timeN);
        dMovement.footRight.roll = frameSet.frames[frameNow].pose.footRight.roll + (frameSet.frames[frameNext].pose.footRight.roll-frameSet.frames[frameNow].pose.footRight.roll)*((time-timeL)/timeN);
        dMovement.footRight.yaw = frameSet.frames[frameNow].pose.footRight.yaw + (frameSet.frames[frameNext].pose.footRight.yaw-frameSet.frames[frameNow].pose.footRight.yaw)*((time-timeL)/timeN);

        dMovement.handRight.pitch = frameSet.frames[frameNow].pose.handRight.pitch + (frameSet.frames[frameNext].pose.handRight.pitch-frameSet.frames[frameNow].pose.handRight.pitch)*((time-timeL)/timeN);
        dMovement.handRight.roll = frameSet.frames[frameNow].pose.handRight.roll + (frameSet.frames[frameNext].pose.handRight.roll-frameSet.frames[frameNow].pose.handRight.roll)*((time-timeL)/timeN);
        dMovement.handRight.yaw = frameSet.frames[frameNow].pose.handRight.yaw + (frameSet.frames[frameNext].pose.handRight.yaw-frameSet.frames[frameNow].pose.handRight.yaw)*((time-timeL)/timeN);

        dMovement.upLeftArm.pitch = frameSet.frames[frameNow].pose.upLeftArm.pitch + (frameSet.frames[frameNext].pose.upLeftArm.pitch-frameSet.frames[frameNow].pose.upLeftArm.pitch)*((time-timeL)/timeN);
        dMovement.upLeftArm.roll = frameSet.frames[frameNow].pose.upLeftArm.roll + (frameSet.frames[frameNext].pose.upLeftArm.roll-frameSet.frames[frameNow].pose.upLeftArm.roll)*((time-timeL)/timeN);
        dMovement.upLeftArm.yaw = frameSet.frames[frameNow].pose.upLeftArm.yaw + (frameSet.frames[frameNext].pose.upLeftArm.yaw-frameSet.frames[frameNow].pose.upLeftArm.yaw)*((time-timeL)/timeN);

        dMovement.upLeftLeg.pitch = frameSet.frames[frameNow].pose.upLeftLeg.pitch + (frameSet.frames[frameNext].pose.upLeftLeg.pitch-frameSet.frames[frameNow].pose.upLeftLeg.pitch)*((time-timeL)/timeN);
        dMovement.upLeftLeg.roll = frameSet.frames[frameNow].pose.upLeftLeg.roll + (frameSet.frames[frameNext].pose.upLeftLeg.roll-frameSet.frames[frameNow].pose.upLeftLeg.roll)*((time-timeL)/timeN);
        dMovement.upLeftLeg.yaw = frameSet.frames[frameNow].pose.upLeftLeg.yaw + (frameSet.frames[frameNext].pose.upLeftLeg.yaw-frameSet.frames[frameNow].pose.upLeftLeg.yaw)*((time-timeL)/timeN);

        dMovement.footLeft.pitch = frameSet.frames[frameNow].pose.footLeft.pitch + (frameSet.frames[frameNext].pose.footLeft.pitch-frameSet.frames[frameNow].pose.footLeft.pitch)*((time-timeL)/timeN);
        dMovement.footLeft.roll = frameSet.frames[frameNow].pose.footLeft.roll + (frameSet.frames[frameNext].pose.footLeft.roll-frameSet.frames[frameNow].pose.footLeft.roll)*((time-timeL)/timeN);
        dMovement.footLeft.yaw = frameSet.frames[frameNow].pose.footLeft.yaw + (frameSet.frames[frameNext].pose.footLeft.yaw-frameSet.frames[frameNow].pose.footLeft.yaw)*((time-timeL)/timeN);

        dMovement.handLeft.pitch = frameSet.frames[frameNow].pose.handLeft.pitch + (frameSet.frames[frameNext].pose.handLeft.pitch-frameSet.frames[frameNow].pose.handLeft.pitch)*((time-timeL)/timeN);
        dMovement.handLeft.roll = frameSet.frames[frameNow].pose.handLeft.roll + (frameSet.frames[frameNext].pose.handLeft.roll-frameSet.frames[frameNow].pose.handLeft.roll)*((time-timeL)/timeN);
        dMovement.handLeft.yaw = frameSet.frames[frameNow].pose.handLeft.yaw + (frameSet.frames[frameNext].pose.handLeft.yaw-frameSet.frames[frameNow].pose.handLeft.yaw)*((time-timeL)/timeN);

        dMovement.downRightArm = frameSet.frames[frameNow].pose.downRightArm + (frameSet.frames[frameNext].pose.downRightArm-frameSet.frames[frameNow].pose.downRightArm)*((time-timeL)/timeN);
        dMovement.downRightLeg = frameSet.frames[frameNow].pose.downRightLeg + (frameSet.frames[frameNext].pose.downRightLeg-frameSet.frames[frameNow].pose.downRightLeg)*((time-timeL)/timeN);


        dMovement.downLeftArm = frameSet.frames[frameNow].pose.downLeftArm + (frameSet.frames[frameNext].pose.downLeftArm-frameSet.frames[frameNow].pose.downLeftArm)*((time-timeL)/timeN);
        dMovement.downLeftLeg = frameSet.frames[frameNow].pose.downLeftLeg + (frameSet.frames[frameNext].pose.downLeftLeg-frameSet.frames[frameNow].pose.downLeftLeg)*((time-timeL)/timeN);

        return dMovement;
	}

	glm::vec3 MainScene::calcutatingPosition(double time,keyFrameSet frameSet)
	{
	    double timeL = 0,timeN = 0;
		int frameNow = 0,frameNext = 0;
		//need use while
	    for (int l = 0;l<frameSet.frames.size();l++)
		{
		    if(timeL + frameSet.frames[l].time >= time)
			{
			    frameNow = l;
				frameNext = frameSet.frames[l].nextFrameIndex;
				timeN = frameSet.frames[frameNow].time;
				break;
			}
			timeL += frameSet.frames[l].time;
		}

		glm::vec3 dOffset;
		dOffset.x = frameSet.frames[frameNow].offset.x + (frameSet.frames[frameNext].offset.x-frameSet.frames[frameNow].offset.x)*((time-timeL)/timeN);
        dOffset.y = frameSet.frames[frameNow].offset.y + (frameSet.frames[frameNext].offset.y-frameSet.frames[frameNow].offset.y)*((time-timeL)/timeN);
        dOffset.z = frameSet.frames[frameNow].offset.z + (frameSet.frames[frameNext].offset.z-frameSet.frames[frameNow].offset.z)*((time-timeL)/timeN);

        return dOffset;
	}

	/* 初始化動作 */
	void MainScene::InitializeStand()
	{
        stand.pose.upRightArm.roll = -20;
	    stand.pose.upLeftArm.roll = 20;
		stand.pose.upRightLeg.roll = -10;
		stand.pose.upLeftLeg.roll = 10;
	}

	void MainScene::InitializePushUp()
	{
	    keyFrame frame0;
		frame0.offset = {0,4,0};
		frame0.time = 1;
		frame0.nextFrameIndex = 1;
		pushUp.totalTimeSpan += frame0.time;
		frame0.pose.head.pitch = 45;
		frame0.pose.core.pitch = 90;
		frame0.pose.upRightArm.pitch = 0;
        frame0.pose.upLeftArm.pitch = 0;
       	frame0.pose.upRightArm.roll = -60;
       	frame0.pose.upLeftArm.roll = 60;
       	frame0.pose.downRightArm = -90;
       	frame0.pose.downLeftArm = -90;
        frame0.pose.handLeft.pitch = -45;
        frame0.pose.handRight.pitch = -45;
        frame0.pose.handLeft.roll = 85;
        frame0.pose.handRight.roll = -85;
        frame0.pose.footLeft.pitch = 10;
        frame0.pose.footRight.pitch = 10;

        pushUp.frames.push_back(frame0);

        keyFrame frame1;
        frame1.offset = {0,6.2,0};
		frame1.time = 1;
		frame1.nextFrameIndex = 0;
		pushUp.totalTimeSpan += frame1.time;
		frame1.pose.head.pitch = 10;
		frame1.pose.core.pitch = 83;
		frame1.pose.upRightArm.pitch = -90;
        frame1.pose.upLeftArm.pitch = -90;
        frame1.pose.upRightArm.roll = -60;
        frame1.pose.upLeftArm.roll = 60;
        frame1.pose.downRightArm = 0;
        frame1.pose.downLeftArm = 0;
        frame1.pose.handLeft.pitch = -45;
        frame1.pose.handRight.pitch = -45;
        frame1.pose.handLeft.roll = 75;
        frame1.pose.handRight.roll = -75;
        frame1.pose.footLeft.pitch = 10;
        frame1.pose.footRight.pitch = 10;

        pushUp.frames.push_back(frame1);
	}

	void MainScene::InitializeSitUp()
	{
	    keyFrame frame0;
		frame0.offset = {0,2.2,0};
		frame0.time = 1;
		frame0.nextFrameIndex = 1;
		sitUp.totalTimeSpan += frame0.time;

		frame0.pose.head.pitch = 10;
		frame0.pose.core.pitch = -90;
		frame0.pose.downBody.pitch = 0;
		frame0.pose.upRightArm.pitch = -90;
        frame0.pose.upLeftArm.pitch = -90;
       	frame0.pose.upRightArm.roll = -40;
       	frame0.pose.upLeftArm.roll = 40;
       	frame0.pose.downRightArm = -120;
       	frame0.pose.downLeftArm = -120;
        frame0.pose.upLeftLeg.pitch = -45;
        frame0.pose.upRightLeg.pitch = -45;
        frame0.pose.downLeftLeg = 90;
        frame0.pose.downRightLeg = 90;
        frame0.pose.footLeft.pitch = 45;
        frame0.pose.footRight.pitch = 45;
        sitUp.frames.push_back(frame0);

        keyFrame frame1;
		frame1.time = 1;
		frame1.nextFrameIndex = 0;
		sitUp.totalTimeSpan += frame1.time;

		frame1.offset = {0,4.4,2};
		frame1.pose.head.pitch = 45;
		frame1.pose.core.pitch = -15;
		frame1.pose.downBody.pitch = -60;
		frame1.pose.upRightArm.pitch = -75;
        frame1.pose.upLeftArm.pitch = -75;
       	frame1.pose.upRightArm.roll = -40;
       	frame1.pose.upLeftArm.roll = 40;
       	frame1.pose.downRightArm = -120;
       	frame1.pose.downLeftArm = -120;
        frame1.pose.upLeftLeg.pitch = -60;
        frame1.pose.upRightLeg.pitch = -60;
        frame1.pose.downLeftLeg = 90;
        frame1.pose.downRightLeg = 90;
        frame1.pose.footLeft.pitch = 45;
        frame1.pose.footRight.pitch = 45;

        sitUp.frames.push_back(frame1);
	}

	void MainScene::InitializeWalk()
	{
	    keyFrame frame0;
		frame0.time = 1;
		frame0.nextFrameIndex = 1;
		walk.totalTimeSpan += frame0.time;

		frame0.pose.core.pitch = 15;
		frame0.pose.core.roll = -5;
		frame0.pose.upRightArm.pitch = -30;
        frame0.pose.upLeftArm.pitch = 30;
       	frame0.pose.upRightArm.roll = -20;
       	frame0.pose.upLeftArm.roll = 20;
       	frame0.pose.downRightArm = -20;
       	frame0.pose.downLeftArm = -20;
        frame0.pose.upLeftLeg.pitch = -60;
        frame0.pose.upRightLeg.pitch = 30;
        frame0.pose.downLeftLeg = 30;
        frame0.pose.downRightLeg = 0;
        frame0.pose.footLeft.pitch = 15;
        frame0.pose.footRight.pitch = -15;
        walk.frames.push_back(frame0);

        keyFrame frame1;
		frame1.time = 1;
		frame1.nextFrameIndex = 0;
		walk.totalTimeSpan += frame1.time;

		frame1.pose.core.pitch = 15;
		frame1.pose.core.roll = 5;
		frame1.pose.upRightArm.pitch = 30;
        frame1.pose.upLeftArm.pitch = -30;
       	frame1.pose.upRightArm.roll = -20;
       	frame1.pose.upLeftArm.roll = 20;
       	frame1.pose.downRightArm = -20;
       	frame1.pose.downLeftArm = -20;
        frame1.pose.upLeftLeg.pitch = 30;
        frame1.pose.upRightLeg.pitch = -60;
        frame1.pose.downLeftLeg = 0;
        frame1.pose.downRightLeg = 30;
        frame1.pose.footLeft.pitch = -15;
        frame1.pose.footRight.pitch = 15;

        walk.frames.push_back(frame1);
	}

	void MainScene::InitializeHopakDance()
	{
	    keyFrame frame0;
		frame0.offset = {0,8.3,0};
		frame0.time = 0.1;
		frame0.pose.core.pitch = -10;
		frame0.pose.downBody.pitch = -15;
		frame0.pose.upRightArm.pitch = -95;
        frame0.pose.upLeftArm.pitch = -85;
       	frame0.pose.upRightArm.roll = -90;
       	frame0.pose.upLeftArm.roll = 90;
       	frame0.pose.downRightArm = -90;
       	frame0.pose.downLeftArm = -90;
        frame0.pose.upLeftLeg.pitch = -90;
        frame0.pose.upRightLeg.pitch = -95;
        frame0.pose.upLeftLeg.roll = -5;
        frame0.pose.upRightLeg.roll = 5;
        frame0.pose.downLeftLeg = 135;
        frame0.pose.downRightLeg = 20;
        frame0.pose.footLeft.pitch = -3;
        frame0.pose.footRight.pitch = 15;


        keyFrame frame1;
        frame1.offset = {0,8.3,0};
		frame1.time = 0.1;
		frame1.pose.core.pitch = -10;
		frame1.pose.downBody.pitch = -15;
		frame1.pose.upRightArm.pitch = -95;
        frame1.pose.upLeftArm.pitch = -85;
       	frame1.pose.upRightArm.roll = -90;
       	frame1.pose.upLeftArm.roll = 90;
       	frame1.pose.downRightArm = -90;
       	frame1.pose.downLeftArm = -90;
        frame1.pose.upLeftLeg.pitch = -95;
        frame1.pose.upRightLeg.pitch = -90;
        frame1.pose.upLeftLeg.roll = -5;
        frame1.pose.upRightLeg.roll = 5;
        frame1.pose.downLeftLeg = 20;
        frame1.pose.downRightLeg = 135;
        frame1.pose.footLeft.pitch = 15;
        frame1.pose.footRight.pitch = -3;

        keyFrame frame2;
        frame2.offset = {0,8.3,0};
        frame2.time = 0.15;
		frame2.pose.core.pitch = -8;
		frame2.pose.downBody.pitch = -15;
		frame2.pose.upRightArm.pitch = -95;
        frame2.pose.upLeftArm.pitch = -85;
        frame2.pose.upRightArm.roll = -90;
        frame2.pose.upLeftArm.roll = 90;
        frame2.pose.downRightArm = -90;
        frame2.pose.downLeftArm = -90;
        frame2.pose.upLeftLeg.pitch = -100;
        frame2.pose.upRightLeg.pitch = -100;
        frame2.pose.upLeftLeg.roll = -5;
        frame2.pose.upRightLeg.roll = 5;
        frame2.pose.downLeftLeg = 135;
        frame2.pose.downRightLeg = 135;
        frame2.pose.footLeft.pitch = 0;
        frame2.pose.footRight.pitch = 0;

        frame0.nextFrameIndex = 1;
        frame0.offset = {0,8.5,0};
        hopakDance.frames.push_back(frame0);
        hopakDance.totalTimeSpan += frame0.time;
        frame0.nextFrameIndex = 2;
        frame0.offset = {0,8.5,0};
        hopakDance.frames.push_back(frame0);
        hopakDance.totalTimeSpan += frame0.time;
        frame2.nextFrameIndex = 3;
        frame2.offset = {0,9,0};
        hopakDance.frames.push_back(frame2);
        hopakDance.totalTimeSpan += frame2.time;
        frame1.nextFrameIndex = 4;
        frame1.offset = {0,8.5,0};
        hopakDance.frames.push_back(frame1);
        hopakDance.totalTimeSpan += frame1.time;
        frame1.nextFrameIndex = 5;
        frame1.offset = {0,8.5,0};
        hopakDance.frames.push_back(frame1);
        hopakDance.totalTimeSpan += frame1.time;
        frame2.nextFrameIndex = 0;
        frame2.offset = {0,9,0};
        hopakDance.frames.push_back(frame2);
        hopakDance.totalTimeSpan += frame2.time;

	}

	void MainScene::InitializeAPTDance()
	{
	    keyFrame frame0;
		frame0.time = 0.23;
		frame0.pose.core.roll = -15;
		frame0.pose.core.yaw = 0;
		frame0.pose.downBody.pitch = 13;
		frame0.pose.downBody.roll = -10;
		frame0.pose.downBody.yaw = 11;
		frame0.pose.head.pitch = 10;
		frame0.pose.head.roll = 26;
		frame0.pose.upRightArm.pitch = -82;
        frame0.pose.upLeftArm.pitch = -45;
       	frame0.pose.upRightArm.roll = 13;
       	frame0.pose.upLeftArm.roll = -30;
        frame0.pose.upRightArm.yaw = 109;
       	frame0.pose.upLeftArm.yaw = -109;
       	frame0.pose.downRightArm = -60;
       	frame0.pose.downLeftArm = -45;

        frame0.pose.upRightLeg.pitch = -5;
        frame0.pose.upRightLeg.roll = 13;
        frame0.pose.downRightLeg = 6;
        frame0.pose.footRight.pitch = -10;
        frame0.pose.footRight.roll = 7;

        frame0.pose.upLeftLeg.pitch = -45;
        frame0.pose.upLeftLeg.roll = 45;
        frame0.pose.downLeftLeg = 50;
        frame0.pose.footLeft.pitch = -25;
        frame0.pose.footLeft.roll = -16;

   	    keyFrame frame1;
		frame1.time = 0.23;
		frame1.pose.core.roll = 15;
		frame1.pose.core.yaw = 0;
		frame1.pose.downBody.pitch = 13;
		frame1.pose.downBody.roll = 10;
		frame1.pose.downBody.yaw = -11;
		frame1.pose.head.pitch = 10;
		frame1.pose.head.roll = -26;
		frame1.pose.upRightArm.pitch = -45;
        frame1.pose.upLeftArm.pitch = -82;
        frame1.pose.upRightArm.roll = 30;
        frame1.pose.upLeftArm.roll = -13;
        frame1.pose.upRightArm.yaw = 109;
        frame1.pose.upLeftArm.yaw = -109;
        frame1.pose.downRightArm = -45;
        frame1.pose.downLeftArm = -60;

        frame1.pose.upLeftLeg.pitch = -5;
        frame1.pose.upLeftLeg.roll = -13;
        frame1.pose.downLeftLeg = 6;
        frame1.pose.footLeft.pitch = -10;
        frame1.pose.footLeft.roll = -7;

        frame1.pose.upRightLeg.pitch = -45;
        frame1.pose.upRightLeg.roll = -45;
        frame1.pose.downRightLeg = 50;
        frame1.pose.footRight.pitch = -25;
        frame1.pose.footRight.roll = 16;

        keyFrame frame2;
		frame2.time = 0.23;
		frame2.pose.core.roll = 0;
		frame2.pose.core.yaw = 0;
		frame2.pose.downBody.pitch = 13;
		frame2.pose.downBody.roll = 0;
		frame2.pose.downBody.yaw = 0;
		frame2.pose.head.pitch = 13;
		frame2.pose.head.roll = 0;
		frame2.pose.upRightArm.pitch = -67;
        frame2.pose.upLeftArm.pitch = -67;
        frame2.pose.upRightArm.roll = 0;
        frame2.pose.upLeftArm.roll = 0;
        frame2.pose.upRightArm.yaw = 109;
        frame2.pose.upLeftArm.yaw = -109;
        frame2.pose.downRightArm = 0;
        frame2.pose.downLeftArm = 0;

        frame2.pose.upLeftLeg.pitch = -30;
        frame2.pose.upLeftLeg.roll = 15;
        frame2.pose.upLeftLeg.yaw = 5;
        frame2.pose.downLeftLeg = 40;
        frame2.pose.footLeft.pitch = -24;
        frame2.pose.footLeft.roll = -1;

        frame2.pose.upRightLeg.pitch = -30;
        frame2.pose.upRightLeg.roll = -15;
        frame2.pose.upRightLeg.yaw = -5;
        frame2.pose.downRightLeg = 40;
        frame2.pose.footRight.pitch = -24;
        frame2.pose.footRight.roll = -1;


        keyFrame frame3;
        frame3.time = 0.23;
        frame3.pose.core.pitch = -9;
		frame3.pose.core.roll = 0;
		frame3.pose.core.yaw = -80;
		frame3.pose.downBody.pitch = 20;
		frame3.pose.downBody.roll = 0;
		frame3.pose.downBody.yaw = 0;
		frame3.pose.head.pitch = 32;
		frame3.pose.head.roll = 0;

		frame3.pose.upRightArm.pitch = -100;
        frame3.pose.upLeftArm.pitch = -100;
        frame3.pose.upRightArm.roll = 5;
        frame3.pose.upLeftArm.roll = -5;
        frame3.pose.upRightArm.yaw = 21;
        frame3.pose.upLeftArm.yaw = -21;
        frame3.pose.downRightArm = -55;
        frame3.pose.downLeftArm = -55;

        frame3.pose.upLeftLeg.pitch = -40;
        frame3.pose.upLeftLeg.roll = 0;
        frame3.pose.upLeftLeg.yaw = -15;
        frame3.pose.downLeftLeg = 45;
        frame3.pose.footLeft.pitch = -20;
        frame3.pose.footLeft.roll = 0;

        frame3.pose.upRightLeg.pitch = -40;
        frame3.pose.upRightLeg.roll = 0;
        frame3.pose.upRightLeg.yaw = 15;
        frame3.pose.downRightLeg = 45;
        frame3.pose.footRight.pitch = -20;
        frame3.pose.footRight.roll = 0;

        keyFrame frame4;
        frame4.time = 0.23;
        frame4.pose.core.pitch = 10;
		frame4.pose.core.roll = 0;
		frame4.pose.core.yaw = -80;
		frame4.pose.downBody.pitch = -10;
		frame4.pose.downBody.roll = 0;
		frame4.pose.downBody.yaw = 0;
		frame4.pose.head.pitch = 32;
		frame4.pose.head.roll = 0;

		frame4.pose.upRightArm.pitch = -70;
        frame4.pose.upLeftArm.pitch = -70;
        frame4.pose.upRightArm.roll = 6;
        frame4.pose.upLeftArm.roll = -6;
        frame4.pose.upRightArm.yaw = 21;
        frame4.pose.upLeftArm.yaw = -21;
        frame4.pose.downRightArm = -95;
        frame4.pose.downLeftArm = -95;

        frame4.pose.upLeftLeg.pitch = -24;
        frame4.pose.upLeftLeg.roll = 0;
        frame4.pose.upLeftLeg.yaw = -15;
        frame4.pose.downLeftLeg = 45;
        frame4.pose.footLeft.pitch = -15;
        frame4.pose.footLeft.roll = 0;

        frame4.pose.upRightLeg.pitch = -24;
        frame4.pose.upRightLeg.roll = 0;
        frame4.pose.upRightLeg.yaw = 15;
        frame4.pose.downRightLeg = 45;
        frame4.pose.footRight.pitch = -15;
        frame4.pose.footRight.roll = 0;

        int counter = 0;
        for (int i = 0;i<6;i++)
        {
            counter ++;
            frame0.offset = {0,14.5,0};
            frame0.nextFrameIndex = counter;
            AptDance.frames.push_back(frame0);
            AptDance.totalTimeSpan += frame0.time;

            counter ++;
            frame2.offset = {0,14.5,0};
            frame2.nextFrameIndex = counter;
            AptDance.frames.push_back(frame2);
            AptDance.totalTimeSpan += frame2.time;

            counter ++;
            frame1.offset = {0,14.5,0};
            frame1.nextFrameIndex = counter;
            AptDance.frames.push_back(frame1);
            AptDance.totalTimeSpan += frame1.time;

            frame2.offset = {0,14.5,0};
            counter ++;
            frame2.nextFrameIndex = counter;
            AptDance.frames.push_back(frame2);
            AptDance.totalTimeSpan += frame2.time;
        }

        frame3.offset = {-2,14.5,2};
        frame4.offset = {-2,14.5,2};
        for (int i = 0;i<3;i++)
        {
            if (i == 0)
            {
                counter ++;
                frame3.nextFrameIndex = counter;
                AptDance.frames.push_back(frame3);
                AptDance.totalTimeSpan += frame3.time;

                counter ++;
                frame4.time = 0.3;
                frame4.nextFrameIndex = counter;
                AptDance.frames.push_back(frame4);
                AptDance.totalTimeSpan += frame4.time;


                counter ++;
                frame4.time = 0.23;
                frame4.nextFrameIndex = counter;
                AptDance.frames.push_back(frame4);
                AptDance.totalTimeSpan += frame4.time;
            }
            else{
                counter ++;
                frame3.nextFrameIndex = counter;
                AptDance.frames.push_back(frame3);
                AptDance.totalTimeSpan += frame3.time;

                counter ++;
                if (i == 2)
                {
                    frame4.time = 0.3;
                    frame4.nextFrameIndex = counter;
                    AptDance.frames.push_back(frame4);
                    AptDance.totalTimeSpan += frame4.time;

                    counter ++;
                    frame4.nextFrameIndex = counter;
                    frame4.time = 0.23;
                    AptDance.frames.push_back(frame4);
                    AptDance.totalTimeSpan += frame4.time;
                }
                else
                {
                    frame4.nextFrameIndex = counter;
                    AptDance.frames.push_back(frame4);
                    AptDance.totalTimeSpan += frame4.time;
                }
            }
        }

        for (int i = 0;i<6;i++)
        {
            counter ++;
            frame0.offset = {0,14.5,0};
            frame0.nextFrameIndex = counter;
            AptDance.frames.push_back(frame0);
            AptDance.totalTimeSpan += frame0.time;

            counter ++;
            frame2.offset = {0,14.5,0};
            frame2.nextFrameIndex = counter;
            AptDance.frames.push_back(frame2);
            AptDance.totalTimeSpan += frame2.time;

            counter ++;
            frame1.offset = {0,14.5,0};
            frame1.nextFrameIndex = counter;
            AptDance.frames.push_back(frame1);
            AptDance.totalTimeSpan += frame1.time;

            frame2.offset = {0,14.5,0};
            counter ++;
            frame2.nextFrameIndex = counter;
            AptDance.frames.push_back(frame2);
            AptDance.totalTimeSpan += frame2.time;
        }

        frame3.offset = {2,14.5,2};
        frame4.offset = {2,14.5,2};
        frame3.pose.core.yaw = 80;
        frame4.pose.core.yaw = 80;
        for (int i = 0;i<3;i++)
        {
            if (i == 0)
            {
                counter ++;
                frame3.nextFrameIndex = counter;
                AptDance.frames.push_back(frame3);
                AptDance.totalTimeSpan += frame3.time;

                counter ++;
                frame4.time = 0.3;
                frame4.nextFrameIndex = counter;
                AptDance.frames.push_back(frame4);
                AptDance.totalTimeSpan += frame4.time;


                counter ++;
                frame4.time = 0.23;
                frame4.nextFrameIndex = counter;
                AptDance.frames.push_back(frame4);
                AptDance.totalTimeSpan += frame4.time;
            }
            else{
                counter ++;
                frame3.nextFrameIndex = counter;
                AptDance.frames.push_back(frame3);
                AptDance.totalTimeSpan += frame3.time;

                counter ++;
                if (i == 2)
                {
                    frame4.time = 0.3;
                    frame4.nextFrameIndex = counter;
                    AptDance.frames.push_back(frame4);
                    AptDance.totalTimeSpan += frame4.time;

                    frame4.nextFrameIndex = 0;
                    frame4.time = 0.23;
                    AptDance.frames.push_back(frame4);
                    AptDance.totalTimeSpan += frame4.time;
                }
                else
                {
                    frame4.nextFrameIndex = counter;
                    AptDance.frames.push_back(frame4);
                    AptDance.totalTimeSpan += frame4.time;
                }
            }
        }
	}

	void MainScene::InitializeSCJ()
	{
	    keyFrame frame0;
		frame0.time = 1;
		frame0.nextFrameIndex = 1;
		SCJ.totalTimeSpan += frame0.time;

        SCJ.frames.push_back(frame0);

        keyFrame frame1;
		frame1.time = 1;
		frame1.nextFrameIndex = 2;
		SCJ.totalTimeSpan += frame1.time;
		frame1.offset = {0,15.4,0};
		frame1.pose.core.pitch = 10;
		frame1.pose.downBody.pitch = -10;
		frame1.pose.upRightArm.pitch = -95;
		frame1.pose.upRightArm.roll = -88;
		frame1.pose.upRightArm.yaw = 0;
		frame1.pose.downRightArm = -83;

        frame1.pose.upLeftArm.pitch = -47;
       	frame1.pose.upLeftArm.roll = 18;
       	frame1.pose.upLeftArm.yaw = -65;
       	frame1.pose.downLeftArm = -101;
        frame1.pose.handLeft.pitch = -35;
       	frame1.pose.handLeft.roll = 78;
       	frame1.pose.handLeft.yaw = 9;

        frame1.pose.upLeftLeg.roll = 10;
        frame1.pose.upRightLeg.roll = -10;
        frame1.pose.footLeft.pitch = -10;
        frame1.pose.footRight.pitch = 10;
        SCJ.frames.push_back(frame1);
        frame1.nextFrameIndex = 0;
        SCJ.frames.push_back(frame1);
	}

	// 在 Initialize() 或是 LoadModel() 結尾處
    void MainScene::SetupFloor()
    {
        // 定義一個 200x200 的平面，中心在 (0,0,0)
        // 頂點格式：Position (x,y,z), Normal (x,y,z), UV (u,v)
        float floorVertices[] = {
            // Position          // Normal      // UV
            -100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f, // 左下
            100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f, // 右下
            100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,  10.0f, 0.0f,  // 右上

            // Triangle 2 (逆時針: 左下 -> 右上 -> 左上)
            -100.0f, 0.0f,  100.0f,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f, // 左下
            100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,  10.0f, 0.0f,  // 右上
            -100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f   // 左上
        };

        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &floorVBO);

        glBindVertexArray(floorVAO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

        // Position: location 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // UV: location 1
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Normal: location 2
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}
