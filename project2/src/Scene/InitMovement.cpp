#include "MainScene.h"

namespace CG
{
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
}
