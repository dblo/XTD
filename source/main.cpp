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
	IwGetResManager()->LoadGroup("tiles.groUp");

	g_Input.Init(); //handle ret val, inform etc

	int tileSize = UpdateScreenSize();	
	Game *game = new Game(tileSize);

	s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback, NULL);

	uint32 timer = (uint32)s3eTimerGetMs();
	uint32 UpdateLogicAgain = timer;
	bool logicUpdated = false;
	//int counter = 0;
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
				tileSize = UpdateScreenSize();	
				game->setTileSize(tileSize);
				game->setUpUI();
			}
			g_screenSizeChanged = false;
		}

		g_Input.Update();

		if (s3eDeviceCheckQuitRequest())
			break;

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

		switch(game->getGameMode())
		{ 
		case PlayMode:
			{
				if(GAME_SPEED < (uint32)s3eTimerGetMs() - UpdateLogicAgain)
				{
					game->Update();

					UpdateLogicAgain = (uint32)s3eTimerGetMs();
					logicUpdated = true;
					//counter--;

					//deltaSum += (uint32)s3eTimerGetMs() - timeCheck;
					//timeCheck = (uint32)s3eTimerGetMs();
				}

				//Render if game is Updated and correct framerate is maintained
				if(logicUpdated && GAME_SPEED > (uint32)s3eTimerGetMs() - UpdateLogicAgain)
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
			break;
		case PausedMode:
			{
				game->manangePausedMode();
				if(game->getGameMode() == TitleMode)
				{
					delete game; 
					game = new Game(tileSize);
				}
			}
			break;
		case TitleMode:
			{
				game->manageTitleMode();
				if(game->getGameMode())
				{
					if(game->getGameMode() == PlayMode)
						game->newGame();
				}
			}
			break;
		case EndedMode:
			{
				game->manageGameEnded();
				if(game->getGameMode() == TitleMode)
				{
					delete game; 
					game = new Game(tileSize);
				}
			}
		}
	}

	s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback);

	delete game;
	cleanUpImages();
	g_Input.Release();
	IwResManagerTerminate();
	Iw2DTerminate();
	return 0;
}
//==============================================================================
