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

int setTileSize()
{
	int tileSize;
	if(Iw2DGetSurfaceHeight() > 500)
		tileSize = 48;
	else
		tileSize = 24;

	return tileSize;
}

int main(int argc, char* argv[])
{
	Iw2DInit();
	IwResManagerInit();
	g_Input.Init(); //handle ret val, inform etc
	IwGetResManager()->LoadGroup("tiles.groUp");

	int tileSize			= setTileSize();
	Game *game				= new Game(tileSize);
	int updateLogicNext		= (int)s3eTimerGetMs();
	bool logicUpdated		= false;
	bool takeTouch			= true;
	Mode gameMode			= TitleMode;

	//debug vars
	int testCounter			= 0;
	int testDeltaSum		= 0;
	int testTimer			= (int)s3eTimerGetMs();
	int testFramesDropped  = 0;

	s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback, NULL);

	while (1)
	{
		s3eDeviceYield(0);

		if(g_screenSizeChanged)
		{
			game->reloadUI();
			g_screenSizeChanged = false;
		}

		g_Input.Update();

		if (s3eDeviceCheckQuitRequest())
			break;

		switch(gameMode)
		{ 
		case PlayMode:
			{
				if((int)s3eTimerGetMs() > updateLogicNext)
				{
					testCounter--;
					testDeltaSum	+= (int)s3eTimerGetMs() - testTimer;
					testTimer		= (int)s3eTimerGetMs();

					gameMode			= game->Update();
					updateLogicNext		+= GAME_SPEED;
					logicUpdated		= true;
				}

				//Render if game is updated and correct framerate is maintained
				if(logicUpdated)
				{
					if((int)s3eTimerGetMs() < updateLogicNext)
					{
						Iw2DSurfaceClear(0xffff9900);
						game->render();
						Iw2DSurfaceShow();
					}
					else
					{
						testFramesDropped++;
					}
					logicUpdated = false;
				}

				if(testCounter <= 0)
				{
//					std::cout << "Dropped: " << testFramesDropped << " frames last second.\n";
					testCounter = 1000/GAME_SPEED;
					testDeltaSum = 0;
					testFramesDropped = 0;
				}
			}
			break;

		case PausedMode:
			gameMode = game->manangePausedMode();
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
			gameMode = game->manageGameEnded();
			break;
		}
	}

	s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback);
	delete game;
	g_Input.Release();
	IwResManagerTerminate();
	Iw2DTerminate();
	return 0;
}
//==============================================================================
