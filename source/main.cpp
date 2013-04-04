/*
Made by Olle Olsson
*/
#include "s3eKeyboard.h"
#include "s3ePointer.h"
#include "s3eDevice.h"
#include <time.h>

#include "Iw2D.h"
#include "IwResManager.h"

#include "game.h"
#include "rendering.h"

// Entry point
// The main() function initialises application data and required modules; it then
// enters a while() loop that exits only when the application throws a "quit" event. Each
// iteration of this loop performs a general "update" and "render" of the application. If the
// loop exits, application data is deleted and modules are terminated before exiting.
int main(int argc, char* argv[])
{
    // Initialisation of Studio modules
    Iw2DInit();         // Initialise support for rendering with the standard SW renderer
    IwResManagerInit();

    // Load all application data
    IwGetResManager()->LoadGroup("tiles.group");

    // Setup materials based on screen size
    UpdateScreenSize();
	
    Game * game = new Game;
    //TitleScreen * title = new TitleScreen;

    uint32 timer = (uint32)s3eTimerGetMs();

    while (1)
    {
        s3eDeviceYield(0);

        // Check for user quit
        if (s3eDeviceCheckQuitRequest())
            break;

        // Check for screen resizing/rotation
        //UpdateScreenSize();

        // Calculate the amount of time that's passed since last frame
        int delta = uint32(s3eTimerGetMs()) - timer;
        timer += delta;

        // Make sure the delta-time value is safe
        if (delta < 0)
            delta = 0;
        if (delta > 100)
            delta = 100;

        //UpdateInput(delta);

        // Clear screen
        Iw2DSurfaceClear(0);

            game->Update(delta);
            game->Render();
        

        //Present the rendered surface to the screen
        Iw2DSurfaceShow();
    }

    // Delete objects and terminate systems
    s3eSurfaceUnRegister(S3E_SURFACE_SCREENSIZE, 0); //replace 0 with callback func

    delete game;

    CleanupImages();

    // Terminate system modules
    IwResManagerTerminate();
    Iw2DTerminate();

    return 0;
}
