/*
Made by Olle Olsson
*/

#include <time.h>
#include "Iw2D.h"
#include "IwResManager.h"
#include "s3eKeyboard.h"
#include "s3ePointer.h"
#include "s3eDevice.h"

#include "game.h"
#include "rendering.h"

int main(int argc, char* argv[])
{
	Iw2DInit();
	IwResManagerInit();

	IwGetResManager()->LoadGroup("tiles.group");
	updateScreenSize();

	g_Input.Init(); //handle ret val, inform etc

	Game * game = new Game;
	//TitleScreen * title = new TitleScreen;

	uint32 timer = (uint32)s3eTimerGetMs();
	uint32 updateLogicAgain = timer;

	while (1)
	{
		s3eDeviceYield(0);

		uint32 timeCheck = (uint32)s3eTimerGetMs();

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


		if(g_gameSpeed < (uint32)s3eTimerGetMs() - updateLogicAgain)
		{
			game->Update();

			updateLogicAgain = (uint32)s3eTimerGetMs();

			Iw2DSurfaceClear(0xFFFF9900);
			game->Render();

			Iw2DSurfaceShow();
		//	std::cout << "Delta: " << (uint32)s3eTimerGetMs() - timeCheck << "\n";

		}
	}
	s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, 0); //replace 0 with callback func

	delete game;
	cleanupImages();

	g_Input.Release();
	IwResManagerTerminate();
	Iw2DTerminate();
	return 0;
}
