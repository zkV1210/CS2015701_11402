#include <Utilty/OBJLoader.hpp>
#include <Shader/Shader.h>
#include "MainScene.h"
#include <Lightning/Lightning.h>
#include <iostream>

namespace CG
{
	auto MainScene::LoadScene() -> bool
	{
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

		camera.LookAt(glm::vec3(0, 10, 25), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		//load shader
		defaultShader = std::make_unique<Shader>("res/shaders/Default.vp", "res/shaders/Default.fp");

		//environment map
        robotShader = std::make_unique<Shader>("res/shaders/Robot.vp", "res/shaders/Robot.fp");

        floorShader = std::make_unique<Shader>("res/shaders/Floor.vp", "res/shaders/Floor.fp");

        LoadModel();

        SetupVertexObject();
        SetupMovementUniform();
        SetupUBO();
        SetupFloor();

		SetupSkybox();
        SetupWater();
        SetupMosaic();
        SetupMotionBlur();
        SetupShadowFBO();
        SetupChidori();
        SetupFirework();

        //fireball
        glGenVertexArrays(1, &fireballVAO);
        fireballShader = std::make_unique<Shader>("res/shaders/Fireball.vp", "res/shaders/Fireball.fp");

        GLfloat range[2]; // 宣告一個可以裝兩個浮點數的陣列
        glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, range);

        /*
		InitializeStand();
		InitializePushUp();
		InitializeSitUp();
		InitializeWalk();
		InitializeHopakDance();
		InitializeSCJ();
		InitializeAPTDance();

		SaveKeyFrameSet(&walk, "res/animations/walk.json");
		SaveKeyFrameSet(&pushUp, "res/animations/pushUp.json");
		SaveKeyFrameSet(&sitUp, "res/animations/sitUp.json");
		SaveKeyFrameSet(&AptDance, "res/animations/AptDance.json");
		SaveKeyFrameSet(&hopakDance, "res/animations/hopakDance.json");
		SaveKeyFrameSet(&SCJ, "res/animations/SCJ.json");
		 */
		LoadKeyFrameSet(&walk, "res/animations/walk.json");
		LoadKeyFrameSet(&pushUp, "res/animations/pushUp.json");
		LoadKeyFrameSet(&sitUp, "res/animations/sitUp.json");
		LoadKeyFrameSet(&AptDance, "res/animations/AptDance.json");
		LoadKeyFrameSet(&hopakDance, "res/animations/hopakDance.json");
		LoadKeyFrameSet(&SCJ, "res/animations/SCJ.json");
		LoadKeyFrameSet(&Memory, "res/animations/FireChidori.json");

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
			glDeleteBuffers(1, &VBOs[i]); //free vbo original
			glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

			glBindBuffer(GL_COPY_WRITE_BUFFER, uVBO);
			glBindBuffer(GL_COPY_READ_BUFFER, uVBOs[i]);
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[1], uvs_size[i] * sizeof(glm::vec2));
			offset[1] += uvs_size[i] * sizeof(glm::vec2);
			glDeleteBuffers(1, &uVBOs[i]); //free vbo
			glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

			glBindBuffer(GL_COPY_WRITE_BUFFER, nVBO);
			glBindBuffer(GL_COPY_READ_BUFFER, nVBOs[i]);
			glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, offset[2], normals_size[i] * sizeof(glm::vec3));
			offset[2] += normals_size[i] * sizeof(glm::vec3);
			glDeleteBuffers(1, &nVBOs[i]);
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

	void MainScene::SetupUBO()
	{
        //存VP 把Uniform空間開好
        // 挖洞給 UBO 裝 Camera 的 View 和 Projection 矩陣
        glGenBuffers(1, &UBO);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
            // 把這個 UBO 實體，插在 GPU 的「0 號共用插座 (Binding Point 0)」上 （對應shader）
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	}

	void MainScene::SetupVertexObject()
	{
	    //VAO 打開,紀錄格式
       	glGenVertexArrays(1, &VAO);
       	glBindVertexArray(VAO);
        // 1. 定義頂點格式（永遠從大 VBO 的起點 0 開始看）
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //格式
        glEnableVertexAttribArray(0);

        // 2. 定義 UV 格式
        glBindBuffer(GL_ARRAY_BUFFER, uVBO);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); //格式
        glEnableVertexAttribArray(1);

        // 3. 定義法向量格式
        glBindBuffer(GL_ARRAY_BUFFER, nVBO);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); //格式
        glEnableVertexAttribArray(2);

        // 錄製完畢，解綁
        glBindVertexArray(0);
	}

	void MainScene::SetupMovementUniform()
	{
        glGenBuffers(1, &instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
        // 注意最後一個參數傳 NULL，代表只挖洞，不填土
        glBufferData(GL_SHADER_STORAGE_BUFFER, 5 * 15 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
	// 在 Initialize() 或是 LoadModel() 結尾處
    void MainScene::SetupFloor()
    {
        std::vector<float> waterVertices;

        int gridSize = 100;           // 把地板切成 100x100 的格子
        float physicalSize = 2000.0f;  // 地板的真實物理大小 (從 -100 到 100)
        float startX = -physicalSize / 2.0f;
        float startZ = -physicalSize / 2.0f;
        float step = physicalSize / gridSize;

        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                // 計算這一個小格子的四個角落 (X, Z)
                float x0 = startX + j * step;
                float z0 = startZ + i * step;
                float x1 = x0 + step;
                float z1 = z0 + step;

                // 計算這一個小格子的 UV 座標
                float u0 = (float)j / gridSize;
                float v0 = (float)i / gridSize;
                float u1 = (float)(j + 1) / gridSize;
                float v1 = (float)(i + 1) / gridSize;

                // 共用的基本法向量 (朝上)，Shader 會自己扭曲它
                float nx = 0.0f, ny = 1.0f, nz = 0.0f;

                // 第一個三角形 (左上 -> 左下 -> 右下)
                // 頂點格式：Position(3) + UV(2) + Normal(3) = 8 個 float
                float p1[] = { x0, 0.0f, z0,   u0, v0,   nx, ny, nz };
                float p2[] = { x0, 0.0f, z1,   u0, v1,   nx, ny, nz };
                float p3[] = { x1, 0.0f, z1,   u1, v1,   nx, ny, nz };

                // 第二個三角形 (左上 -> 右下 -> 右上)
                float p4[] = { x0, 0.0f, z0,   u0, v0,   nx, ny, nz };
                float p5[] = { x1, 0.0f, z1,   u1, v1,   nx, ny, nz };
                float p6[] = { x1, 0.0f, z0,   u1, v0,   nx, ny, nz };

                // 把這 6 個頂點的資料塞進大陣列裡
                waterVertices.insert(waterVertices.end(), p1, p1 + 8);
                waterVertices.insert(waterVertices.end(), p2, p2 + 8);
                waterVertices.insert(waterVertices.end(), p3, p3 + 8);
                waterVertices.insert(waterVertices.end(), p4, p4 + 8);
                waterVertices.insert(waterVertices.end(), p5, p5 + 8);
                waterVertices.insert(waterVertices.end(), p6, p6 + 8);
            }
        }

        glGenVertexArrays(1, &floorVAO);
        glGenBuffers(1, &floorVBO);

        // 綁定到floorVAO 和 VBO
        glBindVertexArray(floorVAO);
        glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
        glBufferData(GL_ARRAY_BUFFER, waterVertices.size() * sizeof(float), waterVertices.data(), GL_STATIC_DRAW);
        waterVertexCount = waterVertices.size() / 8;
        // 設定屬性指針 (注意 Stride 變成 8 * sizeof(float) 了)
        // 0: 座標 (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        // 1: UV (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        // 2: 法線 (vec3)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

        glBindVertexArray(0);
    }

    void MainScene ::SetupShadowFBO()
    {
        shadowDepthShader = std::make_unique<Shader>("res/shaders/ShadowDepth.vp", "res/shaders/ShadowDepth.fp");
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        // 注意格式是 GL_DEPTH_COMPONENT
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // 💥 黑魔法：把貼圖邊界設為純白 (深度 1.0)
        // 這樣當我們的畫面超出光源照射範圍時，才不會出現詭異的黑色殘影

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // 2. 建立 FBO 並掛載深度貼圖
        glGenFramebuffers(1, &shadowFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

        // 3. 告訴 OpenGL：我們不打算在這個 FBO 畫任何顏色！
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解除綁定
    }

    void MainScene::SetupMosaic()
    {
        mosaicShader = std::make_unique<Shader>("res/shaders/Mosaic.vp", "res/shaders/Mosaic.fp");
        glGenFramebuffers(1, &mainFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);

		glGenTextures(1, &mainColorTexture);
		glBindTexture(GL_TEXTURE_2D, mainColorTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainColorTexture, 0);

        glGenRenderbuffers(1, &mainDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, mainDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mainDepthRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FBO is not complete!" << std::endl;

        float quadVertices[] = {
            // 座標 (x, y)       // 貼圖座標 (u, v)
            -1.0f,  1.0f,       0.0f, 1.0f, // 左上角
            -1.0f, -1.0f,       0.0f, 0.0f, // 左下角
             1.0f, -1.0f,       1.0f, 0.0f, // 右下角

            -1.0f,  1.0f,       0.0f, 1.0f, // 左上角
             1.0f, -1.0f,       1.0f, 0.0f, // 右下角
             1.0f,  1.0f,       1.0f, 1.0f  // 右上角
        };

        // 2. 建立 VAO 與 VBO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // 3. 設定頂點屬性通道
        // 通道 0：位置 (x, y)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // 通道 1：貼圖座標 (u, v)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // 解除綁定
        glBindVertexArray(0);

    }

    void MainScene::SetupSkybox()
    {
        skyboxShader = std::make_unique<Shader>("res/shaders/Skybox.vp", "res/shaders/Skybox.fp");
		mySkybox = std::make_unique<CG::Skybox>();
        mySkybox->LoadFromCross("res/Skybox/skybox_night.png");
    }

    void MainScene::SetupWater()
    {
        waterShader = std::make_unique<Shader>("res/shaders/Water.vp", "res/shaders/Water.fp");

        glGenFramebuffers(1, &waterFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, waterFBO);

		glGenTextures(1, &waterColorTexture);
		glBindTexture(GL_TEXTURE_2D, waterColorTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, waterColorTexture, 0); //把這個綁上waterFBO

        glGenRenderbuffers(1, &waterDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, waterDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, waterDepthRBO);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FBO is not complete!" << std::endl;
    }

    void MainScene::SetupMotionBlur()
    {
        motionBlurShader = std::make_unique<Shader>("res/shaders/MotionBlur.vp", "res/shaders/MotionBlur.fp");
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongTexture);

        for (int i = 0; i < 2; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongTexture[i]);
            // 給定跟螢幕一樣的大小
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // 歷史紀錄區不需要深度緩衝 (Depth)，因為只是 2D 圖片相加
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTexture[i], 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MainScene::SetupChidori()
    {
        lightningShader = std::make_unique<Shader>("res/shaders/Lightning.vp", "res/shaders/Lightning.fp");
        chidori = std::make_unique<CG::Lightning>();
    }

    void MainScene::SetupFirework()
    {
        // 無狀態粒子的精髓：我們只需要一個空殼 VAO 來欺騙 OpenGL 管線啟動 Vertex Shader
        glGenVertexArrays(1, &fireworkVAO);
        glBindVertexArray(fireworkVAO);
        glBindVertexArray(0);
        fireworkShader = std::make_unique<Shader>("res/shaders/Firework.vp", "res/shaders/Firework.fp");
    }
}
