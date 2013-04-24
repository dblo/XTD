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

	int tileSize = updateScreenSize();	
	Game *game = new Game(tileSize);
	GameMode gameMode = MODE_TITLE;

	s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback, NULL);

	uint32 timer = (uint32)s3eTimerGetMs();
	uint32 updateLogicAgain = timer;
	bool logicUpdated = false;
	int counter = 0;
	int deltaSum = 0;
	bool takeTouch = true;

	uint32 timeCheck = (uint32)s3eTimerGetMs();

	while (1)
	{
		s3eDeviceYield(0);

		if(g_screenSizeChanged)
		{
			if(game)
			{
				game->setTileSize(updateScreenSize());
				game->setUpUI();
			}
			g_screenSizeChanged = false;
		}

		g_Input.Update();

		if (s3eDeviceCheckQuitRequest())
			break;

		// Check for screen resizing/rotation
		//UpdateScreenSize();

		// Calculate the amount of time that's passed since last frame
		//int delta = uint32(s3eTimerGetMs()) - timer;
		//timer = (uint32)s3eTimerGetMs();
		//std::cout << "Frametime: " << delta << "\n";
		//timer += delta;

		//// Make sure the delta-time value is safe
		//if (delta < 0)
		//	delta = 0;
		//if (delta > 100)
		//	delta = 100;

		if(gameMode == MODE_GAMEPLAY)
		{
			if(GAMESPEED < (uint32)s3eTimerGetMs() - updateLogicAgain)
			{
				game->update();
				gameMode = game->getGameMode();

				updateLogicAgain = (uint32)s3eTimerGetMs();
				logicUpdated = true;
				counter--;

				//deltaSum += (uint32)s3eTimerGetMs() - timeCheck;
				//timeCheck = (uint32)s3eTimerGetMs();
			}

			//Render if game is updated and correct framerate is maintained
			if(logicUpdated && GAMESPEED > (uint32)s3eTimerGetMs() - updateLogicAgain)
			{
				Iw2DSurfaceClear(0xffff9900);
				game->render();
				Iw2DSurfaceShow();
				logicUpdated = false;
			}

			//check framrate only. deltaSum > 1000 =>losing frames
			/*if(counter == 0)
			{
			std::cout << "Delta: " << deltaSum << "\n";
			counter = 1000/GAMESPEED;
			deltaSum = 0;
			}*/
		}
		else if(gameMode == MODE_PAUSED)
		{
			game->manangePausedMode();
			gameMode = game->getGameMode();

			if(gameMode == MODE_TITLE)
			{
				delete game; 
				int tileSize = updateScreenSize();	
				game = new Game(tileSize);
			}
		}
		else if(gameMode == MODE_TITLE)
		{
			if(game->manageTitleMode())
			{
				gameMode = MODE_GAMEPLAY;
				game->newGame();
			}
		}
		else if(gameMode == MODE_GAME_ENDED)
		{
			game->update();
			gameMode = game->getGameMode();

			if(gameMode == MODE_TITLE)
			{
				delete game; 
				int tileSize = updateScreenSize();	
				game = new Game(tileSize);
			}
		}
	}

	s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback);

	delete game;
	cleanupImages();
	g_Input.Release();
	IwResManagerTerminate();
	Iw2DTerminate();
	return 0;
}
//==============================================================================
