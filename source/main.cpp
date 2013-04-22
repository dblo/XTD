/*
Made by Olle Olsson
*/
//==============================================================================

#include <time.h>
#include "Iw2D.h"
#include "IwResManager.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"
#include "s3eDevice.h"

#include "game.h"
//==============================================================================

void releaseGlobals();
//==============================================================================
bool g_screenSizeChanged = true;

//bool g_screenTooSmall = false;

int32 ScreenSizeChangeCallback(void* systemData, void* userData)
{
    g_screenSizeChanged = true;
    return 0;
}

int main(int argc, char* argv[])
{
	Iw2DInit();
	IwResManagerInit();
	IwGetResManager()->LoadGroup("tiles.group");

	g_Input.Init(); //handle ret val, inform etc

	updateScreenSize(); //run once here to allow Game to initialize properly
	Game * game = new Game;

	//TitleScreen * title = new TitleScreen;

    s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback, NULL);

	uint32 timer = (uint32)s3eTimerGetMs();
	uint32 updateLogicAgain = timer;
	bool logicUpdated = false;
	int counter = 0;
	int deltaSum = 0;

	uint32 timeCheck = (uint32)s3eTimerGetMs();

	while (1)
	{
		s3eDeviceYield(0);

		if(g_screenSizeChanged)
		{
			updateScreenSize();
			game->setBorders();
			game->setButtonSize();
			game->setTextAreas();
			g_screenSizeChanged = false;
		}

		g_Input.Update();

		if (s3eDeviceCheckQuitRequest())
			break;

		// Check for screen resizing/rotation
		//UpdateScreenSize();

		// Calculate the amount of time that's passed since last frame
		//int delta = uint32(s3eTimerGetMs()) - timer;
		//timer += delta;

		//// Make sure the delta-time value is safe
		//if (delta < 0)
		//	delta = 0;
		//if (delta > 100)
		//	delta = 100;

		if(GAMESPEED < (uint32)s3eTimerGetMs() - updateLogicAgain)
		{
			game->Update();

			updateLogicAgain = (uint32)s3eTimerGetMs();
			logicUpdated = true;
			counter--;

			deltaSum += (uint32)s3eTimerGetMs() - timeCheck;
			timeCheck = (uint32)s3eTimerGetMs();
		}

		if(logicUpdated) //add && no need to drop a aframe - delta
		{
			Iw2DSurfaceClear(0xffff9900);
			game->Render();
			Iw2DSurfaceShow();
			logicUpdated = false;
		}

		//check framrate only. deltaSum > 1000 =>losing frames
		if(counter == 0)
		{
			//std::cout << "Delta: " << deltaSum << "\n";
			counter = GAMESPEED;
			deltaSum = 0;
		}
	}
	s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback);

	delete game;
	cleanupImages();
	releaseGlobals();
	g_Input.Release();
	IwResManagerTerminate();
	Iw2DTerminate();
	return 0;
}
//==============================================================================

void releaseGlobals()
{

}
//==============================================================================
