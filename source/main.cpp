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
bool g_ScreenSizeChanged = true;
Mode g_GameMode	= TitleMode;

int32 ScreenSizeChangeCallback(void* systemData, void* userData)
{
	g_ScreenSizeChanged = true;
	return 0;
}

int pauseCallback(void* systemData, void* userData)
{
	if(g_GameMode == PlayMode)
		g_GameMode = PausedMode;
	return 0;
}

int setTileSize()
{
	int tileSize;
	if(Iw2DGetSurfaceHeight() > 500)
		tileSize = 54;
	else
		tileSize = 27;

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
	int updateLogicNext		= (int)s3eTimerGetMs() + 2;
	bool logicUpdated		= false;
	bool takeTouch			= true;

	//debug vars
	int testCounter			= 0;
	int testDeltaSum		= 0;
	int testTimer			= (int)s3eTimerGetMs();
	int testFramesDropped   = 0;
	int testSaveDropped		= 0;

	s3eSurfaceRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback, NULL);
	s3eDeviceRegister(S3E_DEVICE_PAUSE, pauseCallback, NULL);

	while (1)
	{
		s3eDeviceYield(0);

		if(g_ScreenSizeChanged)
		{
			game->setUI();
			g_ScreenSizeChanged = false;
		}

		g_Input.Update();

		if (s3eDeviceCheckQuitRequest())
			break;
		
		switch(g_GameMode)
		{ 
		case PlayMode:
			{
				if((int)s3eTimerGetMs() > updateLogicNext)
				{
					testCounter--;
					testDeltaSum	+= (int)s3eTimerGetMs() - testTimer;
					testTimer		= (int)s3eTimerGetMs();

					g_GameMode			= game->update();
					updateLogicNext		+= GAME_SPEED;
					logicUpdated		= true;
				}

				if(logicUpdated)
				{
					//Render if correct framerate is maintained
					if((int)s3eTimerGetMs() < updateLogicNext)
					{
						Iw2DSurfaceClear(0);//0xFF4E4949);
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
					//std::cout << "Dropped: " << testFramesDropped << " frames last second.\n";
					testDeltaSum = 0;
					testSaveDropped = testFramesDropped;
					testFramesDropped = 0;
					testCounter = 1000/GAME_SPEED;
				}
				// If back button is pressed, pause game		
			}
			break;

		case PausedMode:
			g_GameMode = game->manangePausedMode();

			if(g_GameMode == PlayMode)
				// Prevent game from "rushing" when unpaused
				updateLogicNext = (int)s3eTimerGetMs();
			break;

		case TitleMode:
			{
				g_GameMode = game->manageTitleMode();
				if(g_GameMode == PlayMode)
				{
					game->cleanUp();
					game->reset();
				}
			}
			break;

		case EndedMode:
			g_GameMode = game->manageGameEnded();
			break;
		}
	}

	s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, ScreenSizeChangeCallback);
	s3eDeviceUnRegister(S3E_DEVICE_PAUSE, pauseCallback);

	delete game;
	g_Input.Release();
	IwResManagerTerminate();
	Iw2DTerminate();
	return 0;
}
