#pragma once

#include <array>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <Shader/Shader.h>
#include <Skybox/Skybox.h>
#include <Camera/Camera.h>
#include <Lightning/Lightning.h>

constexpr auto PARTSNUM = 15;
constexpr double FIREWORK_DEFAULT  = 3.0f;


namespace CG
{
	class MainScene
	{
	public:
		struct eulerAnglesT
		{
		    float pitch = 0.0f;
			float roll = 0.0f;
			float yaw = 0.0f;
		};

		struct JointSet
		{
		    eulerAnglesT core;
		    eulerAnglesT head;
			eulerAnglesT upRightArm;
			eulerAnglesT upLeftArm;
			eulerAnglesT downBody;
			eulerAnglesT upLeftLeg;
			eulerAnglesT upRightLeg;
			eulerAnglesT handRight;
			eulerAnglesT handLeft;
			eulerAnglesT footLeft;
			eulerAnglesT footRight;
			float downLeftArm = 0.0f;
			float downRightArm = 0.0f;
			float downLeftLeg = 0.0f;
			float downRightLeg = 0.0f;
		};

		struct ChidoriSet
		{
            float offset = 0.0f;
			float intensity = 1.0f;
			glm::vec3 color = {0.3f, 0.8f, 1.0f};
			bool show = false;
			bool rightHand = false;
		};

		struct fireWorkSet
		{
		    glm::vec3 position = {0,0,0};
			glm::vec3 direction = {0,1,0};
			float intensity = 1.0f;
			glm::vec3 color = {0.3f, 0.8f, 1.0f};
			float speed = 1.0f;
		};

        struct FireworkTrigger {
            double triggerTime = 0.0f;
            fireWorkSet attributes;
        };

        struct ActiveFirework {
            double age = 0.0f;       // 這個煙火專屬的 u_time
            fireWorkSet attributes;
        };

        struct fireBallSet
		{
		    float offset = 0.0f;
			float intensity = 1.0f;
			float speed = 1.0f;
			bool show = false;
		};

		struct keyFrame
		{
		    glm::vec3 offset = {0,16,0};
		    JointSet pose;
			ChidoriSet chidori;
			fireBallSet fireball;
			double time = 0;
			int nextFrameIndex = 0;
		};

		struct keyFrameSet
		{
		    std::vector<keyFrame> frames;
			std::vector<FireworkTrigger> fireworkTrack;
			double totalTimeSpan = 0;
		};


		struct waterAttribute
		{
		    float reflectionRate = 0.02;
			float distortionCo = 0.03;
			glm::vec3 waterColor = {0.05f, 0.2f, 0.35f};
		};

		struct envMapAttribute
		{
		    float reflectionIntensity = 0.4;
			float exposure = 1.2;
		};

		MainScene();
		~MainScene();

		auto Initialize() -> bool;
		void Update(double dt);
		void Render();

		void OnResize(int width, int height);
		void OnKeyboard(int key, int action);
		void OnMouseButton(int button, int action);
		void OnMouseMove(double x, double y);
		void OnMouseScroll(double xoffset, double yoffset);


		//for GUI
		void SetJoint(float pitch, float roll, float yaw, int part); //use this to set joint, directly
		void SetOffset(float x, float y, float z);
		JointSet GetJoint(); //get a JointSet for all value
		glm::vec3 GetOffset();
		bool GetInstanced();
		int GetMemoryFrameNum();

		void InsertKeyFrame(double timeSpan,int index); //automatically append the desire frame to back of memory
		void DeleteKeyFrame(int index);
		void AdjustKeyFrame(int index);
		void WriteIntoJson();

		void AdjustTimeSpan(double timeSpan, int index);
		void ResetMemory(); //flush all memory

		void ShowKeyFrame(int index);


		void SetAnimationSpeed(double multiplier);

		void ResetAction();
		void SetAction(int action);
		void SetMode(int mode);
		void SetEdit();


		void SetWaterMode();
		bool GetWaterMode();
		void AdjustWaterAttribute(float R, float D, glm::vec3 color);
		waterAttribute GetWaterAttribute();

		envMapAttribute GetEnvMapAttribute();
		void AdjustEnvMapAttribute(float R, float E);

		ChidoriSet GetChidoriAttribute();
		void SetChidoriAttribute(ChidoriSet attribute);

		fireBallSet GetFireBallAttribute();
		void SetFireBallAttribute(fireBallSet attribute);

		bool GetMosaic();
		void SetMosaic();

		void SetMotionBlur();
		bool GetMotionBlur();

		void SetToon();
		bool GetToon();

		void AdjustPointLight(glm::vec3 pos);
		glm::vec3 GetPointLightPos();
		void AdjustPointLightIntensity(float intensity);
		float GetPointLightIntensity();

		void SetAmbientMode();
        bool GetAmbientMode();

        void SetLightMode();
        bool GetLightMode();

        int GetFireWorkAmount();
	    std::vector<FireworkTrigger> GetFireWorkAttribute(bool tempMode);
        void SetFireWorkAttribute(bool tempMode, int index, fireWorkSet attribute);
        void SetFireWorkTime(bool tempMode, int index, double time);
        void InsertNewFireWork();
        void DeleteFireWork(int index);

        void SetEditFireWork(bool state);
        bool GetEditFireWork();
        void SetEditCreateFireWork(bool state);
        bool GetEditCreateFireWork();
        void SetEditTimeLine(double time);
        double GetEditTimeLine();
        double GetMemoryTotalTimeSpan();

	private:
	    glm::mat4 translate(float x, float y, float z);
        glm::mat4 scale(float x, float y, float z);
        glm::mat4 rotate(float angle, float x, float y, float z);
        JointSet calcutatingJointSet(double time,keyFrameSet frameSet);
		glm::vec3 calcutatingPosition(double time,keyFrameSet frameSet);
		ChidoriSet calcutatingChidori(double time,keyFrameSet frameSet);
		fireBallSet calcutatingFireball(double time,keyFrameSet frameSet);
		std::vector<ActiveFirework> EvaluateFireworksAtTime(double currentTime, const keyFrameSet& frameSet);
		std::vector<ActiveFirework> EvaluateTempFireworksAtTime(double currentTime);

		auto LoadScene() -> bool;
		void SetupFloor();
		void SetupMosaic();
		void LoadModel();
		void Load2Buffer(const char* obj, int i);

        void    SetupVertexObject();
        void    SetupMovementUniform();
        void    SetupUBO();
        void    SetupSkybox();
        void    SetupWater();
        void    SetupMotionBlur();
        void    SetupShadowFBO();
        void    SetupChidori();
        void    SetupFirework();

		void UpdateAction(double dt);
		void UpdateModel();
		void UpdateInstanceModel();


		void ResetJointSet();
		void SetInstanced(bool isInstanced);

		void UpdateOffset(float x,float y,float z);
		void UpdateJointSet(JointSet frame);
		void UpdateJointInstance(JointSet frame, int index);
		void UpdateOffsetInstance(float x, float y, float z, int index);
		void UpdateChidori(ChidoriSet attribute);
		void UpdateFireball(fireBallSet attribute);
		void UpdateFireWork(const std::vector<ActiveFirework>& newFireworks);


		void InitializeStand();
		void InitializeWalk();
		void InitializeSitUp();
		void InitializePushUp();
		void InitializeHopakDance();
		void InitializeAPTDance();
		void InitializeSCJ();
		void SaveKeyFrameSet(const keyFrameSet* sourceSet, const std::string& filename);
        void LoadKeyFrameSet(keyFrameSet* targetSet, const std::string& filename);




		void RenderSkybox();
		void RenderRobot();
		void RenderChidori(int frame);
		void RenderWater(double cTime);
		void RenderRobotDepthOnly();
		void RenderShadow();
		void updateSSBO();
		void RenderFloor();
		void RenderFireball(double cTime);
		void RenderFirework();


	private:
	    //Camera
		Camera camera;

		//Floor
		GLuint floorVAO;
		GLuint floorVBO;
		int waterVertexCount = 0;


		GLuint VAO;
		GLuint VBO;
		GLuint uVBO;
		GLuint nVBO;
		GLuint mVBO;
		GLuint UBO;
		std::array<GLuint, PARTSNUM> VBOs;
		std::array<GLuint, PARTSNUM> uVBOs;
		std::array<GLuint, PARTSNUM> nVBOs;

		GLuint waterFBO = 0;          // 隱形畫布的總管
        GLuint waterColorTexture = 0; // 存反射畫面的貼圖
        GLuint waterDepthRBO = 0;
        int fboWidth = 800;
        int fboHeight = 600;

        GLuint quadVAO;
        GLuint quadVBO;
        GLuint mainFBO = 0;
        GLuint mainColorTexture = 0;
        GLuint mainDepthRBO = 0;


        GLuint pingpongFBO[2];
        GLuint pingpongTexture[2];
        int pingpongIndex = 0;

        GLuint shadowFBO;
        GLuint depthMap;
        const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
        glm::mat4 lightSpaceMatrix;

        GLuint fireballVAO;

        GLuint fireworkVAO;
        std::vector<ActiveFirework> fireworkRendering;
        double editTimeLine = 0.0f;
        FireworkTrigger tempFirework;


		std::unique_ptr<Shader> defaultShader;
		std::unique_ptr<Shader> skyboxShader;
		std::unique_ptr<Shader> waterShader;
		std::unique_ptr<Shader> robotShader;
		std::unique_ptr<Shader> lightningShader;
		std::unique_ptr<Shader> mosaicShader;
		std::unique_ptr<Shader> motionBlurShader;
		std::unique_ptr<Shader> shadowDepthShader;
		std::unique_ptr<Shader> floorShader;
		std::unique_ptr<Shader> fireballShader;
		std::unique_ptr<Shader> fireworkShader;
		std::unique_ptr<CG::Skybox> mySkybox;

		std::unique_ptr<CG::Lightning> chidori;
		ChidoriSet chidoriAttribute;

		fireBallSet fireBallAttibute;

		int action = 0;
		bool edit = false;
		bool editFireWork = false;
		bool editCreateFireWork = false;
		bool isInstanced = false;
		GLenum mode = 0; // fill

		glm::vec3 rootOffset={0,0,0} ;
		glm::vec3 instanceRootOffset[5]= {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
		JointSet rendering;
		JointSet instanceRendering[5];

		glm::vec3 pointLightPos = glm::vec3(0.0f, 35.0f, 20.0f); // 預設位置
		float lightIntensity = 1.0f;

		double playSpeed = 1;
		keyFrameSet walk;
		keyFrameSet pushUp;
		keyFrameSet sitUp;
		keyFrameSet AptDance;
		keyFrameSet hopakDance;
		keyFrameSet SCJ;
		keyFrameSet Memory; //can only remember a set
		keyFrame stand;

		waterAttribute waterState;
		envMapAttribute envMapState;

		float eyeAngley = 0.0;
		float eyedistance = 20.0;
		float size = 1;
		GLfloat movex, movey;
		GLuint instanceSSBO;

		int vertices_size[PARTSNUM];
		int uvs_size[PARTSNUM];
		int normals_size[PARTSNUM];
		int materialCount[PARTSNUM];

		std::vector<std::string> mtls[PARTSNUM];//use material
		std::vector<unsigned int> faces[PARTSNUM];//face count for material

		std::map<std::string, glm::vec3> KDs;//mtl-name&Kd
		std::map<std::string, glm::vec3> KSs;//mtl-name&Ks
		std::map<std::string, glm::vec3> KAs;//mtl-name&Ka
		std::map<std::string, glm::vec3> KEs; //mtl-name&Ke
		std::map<std::string, float> NSs; //mtl-name&Ns
		std::map<std::string, float> NIs; //mtl-name&Ni
		std::map<std::string, float> Ds; //mtl-name&d
		std::map<std::string, int> ILLUMs; //mtl-name&illum

		glm::mat4 Models[PARTSNUM];
		glm::mat4 InstanceModels[5][PARTSNUM];

		bool movingView = false;
		bool rotatingView = false;
		bool waterShowcase = false;

		bool mosaicShowcase = false;
		bool motionBlurShowcase = false;
		bool toonShowcase = false;
		bool ambientShowcase = false;
		bool dotLightShowcase = true;

		int MouseButton = 0; // left:0, right:1, middle:2
		int MouseAction = 0; // press:1, release:0
		float lastX = 0;
		float lastY = 0;
		float dx = 0;
		float dy = 0;
	};
}
