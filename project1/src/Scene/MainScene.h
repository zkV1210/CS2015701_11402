#pragma once

#include <array>
#include <string>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Camera.h"

constexpr auto PARTSNUM = 15;

namespace CG
{
	class MainScene
	{
	public:
		MainScene();
		~MainScene();
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

		struct keyFrame
		{
		    glm::vec3 offset = {0,16,0};
		    JointSet pose;
			double time = 0;
			int nextFrameIndex = 0;
		};

		struct keyFrameSet
		{
		    std::vector<keyFrame> frames;
			double totalTimeSpan = 0;
		};

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

		void AdjustTimeSpan(double timeSpan, int index);
		void ResetMemory(); //flush all memory

		void ShowKeyFrame(int index);


		void SetAnimationSpeed(double multiplier);

		void ResetAction();
		void SetAction(int action);
		void SetMode(int mode);
		void SetEdit();


	private:
		auto LoadScene() -> bool;
		void SetupFloor();
		void LoadModel();
		void Load2Buffer(const char* obj, int i);

		void UpdateAction(double dt);
		void UpdateModel();
		void UpdateInstanceModel();

		void ResetJointSet();
		void SetInstanced(bool isInstanced);
		void UpdateOffset(float x,float y,float z);
		void UpdateJointSet(JointSet frame);
		void UpdateJointInstance(JointSet frame, int index);
		void UpdateOffsetInstance(float x, float y, float z, int index);
		JointSet calcutatingJointSet(double time,keyFrameSet frameSet);
		glm::vec3 calcutatingPosition(double time,keyFrameSet frameSet);
		void InitializeStand();
		void InitializeWalk();
		void InitializeSitUp();
		void InitializePushUp();
		void InitializeHopakDance();
		void InitializeAPTDance();
		void InitializeSCJ();



	private:
		Camera camera;

		GLuint floorVAO;
		GLuint floorVBO;


		GLuint VAO;
		GLuint VBO;
		GLuint uVBO;
		GLuint nVBO;
		GLuint mVBO;
		GLuint UBO;
		std::array<GLuint, PARTSNUM> VBOs;
		std::array<GLuint, PARTSNUM> uVBOs;
		std::array<GLuint, PARTSNUM> nVBOs;
		GLuint program;

		int action = 0; // idle
		bool edit = false;
		bool isInstanced = false;
		GLenum mode = 0; // fill

		glm::vec3 rootOffset={0,0,0} ;
		glm::vec3 instanceRootOffset[5]= {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
		JointSet rendering;
		JointSet instanceRendering[5];

		double playSpeed = 1;
		keyFrameSet walk;
		keyFrameSet pushUp;
		keyFrameSet sitUp;
		keyFrameSet AptDance;
		keyFrameSet hopakDance;
		keyFrameSet SCJ;
		keyFrameSet Memory; //can only remember a set
		keyFrame stand;


		float eyeAngley = 0.0;
		float eyedistance = 20.0;
		float size = 1;
		GLfloat movex, movey;
		GLint MatricesIdx;
		GLuint ModelID;
		GLuint instanceSSBO;
		GLuint PartIndexID;

		int vertices_size[PARTSNUM];
		int uvs_size[PARTSNUM];
		int normals_size[PARTSNUM];
		int materialCount[PARTSNUM];

		GLuint M_KaID;
		GLuint M_KdID;
		GLuint M_KsID;
		GLuint M_KeID;
		GLuint M_NsID;
		GLuint M_dID;

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

		/*
		bool clicked = false;
		glm::vec3 pressFront;
		double pressPosX = 0;
		double pressPosY = 0;
		*/

		bool movingView = false;
		bool rotatingView = false;

		int MouseButton = 0; // left:0, right:1, middle:2
		int MouseAction = 0; // press:1, release:0
		float lastX = 0;
		float lastY = 0;
		float dx = 0;
		float dy = 0;
	};
}
