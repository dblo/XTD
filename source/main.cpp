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
	g_Input.Init(); //handle ret val, inform etc
	IwGetResManager()->LoadGroup("tiles.groUp");

	int tileSize			= UpdateScreenSize();
	Game *game				= new Game(tileSize);
	uint32 updateLogicNext	= (uint32)s3eTimerGetMs();
	bool logicUpdated		= false;
	bool takeTouch			= true;
	Mode gameMode			= TitleMode;

	int testCounter			= 0;
	int testDeltaSum		= 0;
	uint32 testTimer		= (uint32)s3eTimerGetMs();

	s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback, NULL);

	while (1)
	{
		s3eDeviceYield(0);

		if(g_screenSizeChanged)
		{
			game->setUpUI();
			g_screenSizeChanged = false;
		}

		g_Input.Update();

		if (s3eDeviceCheckQuitRequest())
			break;

		switch(gameMode)
		{ 
		case PlayMode:
			{
				if((uint32)s3eTimerGetMs() > updateLogicNext)
				{
					testCounter--;
					testDeltaSum	+= (uint32)s3eTimerGetMs() - testTimer;
					testTimer		= (uint32)s3eTimerGetMs();

					gameMode			= game->Update();
					updateLogicNext		+= GAME_SPEED;
					logicUpdated		= true;
				}

				//Render if game is updated and correct framerate is maintained
				if(logicUpdated)
				{
					if((uint32)s3eTimerGetMs() < updateLogicNext)
					{
						Iw2DSurfaceClear(0xFF0C5907);//ffff9900);
						game->render();
						Iw2DSurfaceShow();
					}
					else
					{
						//std::cout << "Dropping a frame!\n";*/
					}
					logicUpdated = false;
				}

				if(testCounter <= 0)
				{
					//	std::cout << "Delta: " << testDeltaSum << "\n";
					testCounter = 1000/GAME_SPEED;
					testDeltaSum = 0;
				}
			}
			break;
		case PausedMode:
			{
				gameMode = game->manangePausedMode();
				if(gameMode == TitleMode)
				{
				}
			}
			break;
		case TitleMode:
			{
				gameMode = game->manageTitleMode();

				if(gameMode == PlayMode)
				{
					game->cleanUp();
					game->reset();
				}
			}
			break;
		case EndedMode:
			{
				gameMode = game->manageGameEnded();
				if(gameMode == TitleMode)
				{
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
