#include <iostream>

#include "IwResManager.h"
#include "io.h"

const int BLACK				= 0xffffffff;
const int L_GREEN			= 0xff10be36;
const int D_GREEN			= 0xff046b0a;
const int GREY				= 0xff4e4949;
const int BLUE				= 0xffff9900;
const int PURPLE			= 0xFFd03D50;
const int NUM_TILE_TYPES	= 33;
const int NUM_OF_BUTTONS	= 6;
const int NUM_OF_SPACES		= 5;

CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DFont* font;

Io::Io(int tileSize)
{
	setUpGrapicRes(tileSize);
	reset();
}
Io::~Io()
{
	cleanUpImages();
}
void Io::reset()
{
	takeNextInputAt	= INT_MAX;
	holdingCounter	= 0;
	currTouch		= 0;
}
// Return the event that describes the touch.
// Returns DoNothingInputEvent when no touch is processed or when touching 
// between buttons in the sidebar menu
InputEvent Io::handleInput(bool showMenu)
{
	InputEvent event = DoNothingInputEvent;
	if(g_Input.getTouchCount() == 0)
	{
		if(holdingCounter > 0)
		{
			event = GridInputEvent;
		}
		takeNextInputAt		= 0;
		holdingCounter	= 0;
	}
	else if((uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		currTouch = g_Input.getTouch(0);
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

		if(withinBorders())
		{
			if(topBarTouch())
			{
				if(textAreaTouch())
				{
					event = ClearEvent;
				}
				else // Touching menu button
				{
					event = MenuEvent;
				}
			}
			else if(gridTouch(showMenu))
			{
				invokeGridTouch();
			}
			// At this point menu must be showing and the touch is within it
			else if(playTouch())
			{
				event = PlayInputEvent;
			}
			else if(buyDamageTouch())
			{
				event = Btn1Event;
			}
			else if(buySpeedTouch())
			{
				event = Btn2Event;
			}
			else if(buyRangeTouch())
			{
				event = Btn3Event;
			}
			else if(sellTouch())
			{
				event = SellInputEvent;
			}
			else if(pauseTouch())
			{
				event = PauseBtnInputEvent;
			}

			lastTouchX = currTouch->x;
			lastTouchY = currTouch->y;
		}
	}
	return event;
}
void Io::renderBg() const
{
	/*Iw2DSetColour(GREY);
	Iw2DFillRect(CIwSVec2(horBorder, 0), 
		CIwSVec2(buttonX - horBorder, verOffset));
	Iw2DSetColour(BLACK);*/
}
void Io::setUpUI(int gridColumns, int gridRows, int tileSize)
{	
	int wid = Iw2DGetSurfaceWidth();
	int hi = Iw2DGetSurfaceHeight();
	
	horBorder		= (wid - gridColumns*tileSize) / 2;
	verOffset		= hi - gridRows*tileSize;
	buttonWid		= 4 * tileSize;
	buttonHi		= (hi - verOffset) / 7;

	choiceBtnLeftX  = (gridColumns / 2 - 4) * tileSize;
	choiceBtnRightX = (gridColumns / 2 + 3) * tileSize;
	choiceBtnTopY	= (gridRows / 2 - 1) * tileSize;

	widthMinusBorder	= wid - horBorder;
	buttonX				= gridColumns*tileSize - buttonWid + horBorder; 
	
	setButtonSize();
	setTextAreas(tileSize);
}
void Io::renderAlphaButton(Image img, Button btn) const
{
	Iw2DSetColour(0x7740C020);
	renderNoAlphaButton(img, btn);
	Iw2DSetColour(BLACK);
}
void Io::renderPauseButton()
{
	renderNoAlphaButton(PauseImage, PauseButton);
}
void Io::renderFastSpeedButton()
{
	renderNoAlphaButton(FastSpeedImage, PlayButton);
}
void Io::renderNormalSpeedButton()
{
	renderNoAlphaButton(NormalSpeedImage, PlayButton);
}
void Io::renderPlayButton()
{
	renderNoAlphaButton(PlayImage, PlayButton);
}
void Io::renderButton(bool active, Image img, Button btn)
{
	if(active)
		renderNoAlphaButton(img, btn);
	else
		renderAlphaButton(img, btn);
}
void Io::renderPaused(int qx, int cx, int y) const
{
	Iw2DSetColour(D_GREEN);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(buttonWid, buttonHi));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(buttonWid, buttonHi));

	Iw2DSetColour(L_GREEN); 

	Iw2DDrawString("QUIT", CIwSVec2(qx, y), CIwSVec2(buttonWid, buttonHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("CONTINIUE", CIwSVec2(cx, y), CIwSVec2(buttonWid, buttonHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(BLACK); 
}
void Io::renderTitleScren(int newX, int newY) const
{
	Iw2DSurfaceClear(0);
	Iw2DSetColour(L_GREEN);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(buttonWid, buttonHi));

	Iw2DSetColour(D_GREEN); 

	Iw2DDrawString("NEW GAME", CIwSVec2(newX, newY), 
		CIwSVec2(buttonWid, buttonHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(BLACK);
}
void Io::renderGameEnded(int x, int y, int lives) const
{
	Iw2DSetColour(D_GREEN);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(buttonWid, buttonHi));

	Iw2DSetColour(L_GREEN); 

	if(lives == 0)
		Iw2DDrawString("GAME OVER", CIwSVec2(x, y), 
		CIwSVec2(buttonWid, buttonHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	else
		Iw2DDrawString("[Insert victory message]", CIwSVec2(x, y), 
		CIwSVec2(buttonWid, buttonHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(BLACK); 
}
void Io::renderText( const char* str, Text txt ) const
{
	Iw2DDrawString(str, CIwSVec2(textX[txt], textY[txt]), 
		CIwSVec2(textLength[txt], verOffset),
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);
}
void Io::setTextColor(bool textColorOn) const
{
	if(textColorOn)
		Iw2DSetColour(PURPLE); 
	else
		Iw2DSetColour(BLACK); 
}
void Io::setButtonSize()
{
	int buttonSpacing = (Iw2DGetSurfaceHeight() - verOffset - 
		NUM_OF_BUTTONS*buttonHi) / NUM_OF_SPACES;
	int verticalSpace = buttonHi + buttonSpacing;

	buttonY[PauseButton]	= Iw2DGetSurfaceHeight() - buttonHi;
	buttonY[SellButton]		= buttonY[PauseButton] - verticalSpace;
	buttonY[Btn3Button]		= buttonY[SellButton] - verticalSpace;
	buttonY[Btn2Button]		= buttonY[Btn3Button] - verticalSpace;
	buttonY[Btn1Button]		= buttonY[Btn2Button] - verticalSpace;
	//buttonY[SendButton]		= buttonY[Btn1Button] - verticalSpace;
	buttonY[PlayButton]		= verOffset;

	buttonY[SellBottomButton]	= buttonY[SellButton] + buttonHi;
	buttonY[PlayBottomButton]	= buttonY[PlayButton] + buttonHi;
	buttonY[PauseBottomButton]	= buttonY[PauseButton] + buttonHi;
	buttonY[Btn1BottomButton]	= buttonY[Btn1Button] + buttonHi;
	buttonY[Btn2BottomButton]	= buttonY[Btn2Button] + buttonHi;
	buttonY[Btn3BottomButton]	= buttonY[Btn3Button] + buttonHi;
}
bool Io::isTouchingLargeBtn(unsigned int x, unsigned int y) const
{
	return currTouch->x > x && currTouch->x <= x + buttonWid
		&& currTouch->y >= y && currTouch->y < y + buttonHi;
}
bool Io::buyRangeTouch() const
{
	return currTouch->y < buttonY[Btn3BottomButton]
	&& currTouch->y >= buttonY[Btn3Button];
}
bool Io::buyDamageTouch() const
{
	return currTouch->y < buttonY[Btn1BottomButton]
	&& currTouch->y >= buttonY[Btn1Button];
}
bool Io::buySpeedTouch() const
{
	return currTouch->y < buttonY[Btn2BottomButton]
	&& currTouch->y >= buttonY[Btn2Button];
}
bool Io::pauseTouch() const
{
	return currTouch->y < buttonY[PauseBottomButton]
	&& currTouch->y >= buttonY[PauseButton];
}
bool Io::sellTouch() const
{
	return currTouch->y < buttonY[SellBottomButton]
	&& currTouch->y >= buttonY[SellButton];
}
bool Io::gridTouch(bool showMenu) const
{
	if(showMenu)
		return currTouch->x < buttonX;
	return true;//currTouch->x < widthMinusBorder;
}
void Io::setTextAreas(int tileSize)
{
	textY[LivesText]	= 0;
	textY[WaveText]		= 0;
	textY[CreditsText]	= 0;
	textY[InfoText]		= 0;	
	textY[MenuText]		= 0;
	textY[WallText]		= 0;

	textLength[LivesText]	= 2*tileSize;
	textLength[WaveText]	= 2*tileSize;
	textLength[WallText]	= 2*tileSize;
	textLength[CreditsText] = 3*tileSize;
	textLength[MenuText]	= buttonWid;

	textX[LivesText]	= horBorder;
	textX[WaveText]		= textX[LivesText] + textLength[LivesText];
	textX[WallText]		= textX[WaveText] + textLength[WaveText];
	textX[CreditsText]	= textX[WallText] + textLength[WallText];
	textX[InfoText]		= textX[CreditsText] + textLength[CreditsText];
	textX[MenuText]		= widthMinusBorder - buttonWid;

	textLength[InfoText]	= buttonX - textX[CreditsText] - textLength[CreditsText];
}
bool Io::withinBorders() const
{
	return currTouch->x >= horBorder
		&& currTouch->x < widthMinusBorder;
		//&& currTouch->y >= horBorder
		//&& currTouch->y < heigthMinusBorder; //TODO add ver border
}
bool Io::playTouch() const
{
	return currTouch->y >= buttonY[PlayButton] &&
		currTouch->y < buttonY[PlayBottomButton];
}
bool Io::buttonTouchX() const
{
	return currTouch->x >= buttonX;
}
Mode Io::manangePausedMode()
{
	renderPaused(choiceBtnLeftX, choiceBtnRightX, choiceBtnTopY);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	else if((uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

		if(g_Input.getTouchCount() > 0)
		{
			currTouch = g_Input.getTouch(0);
			if(isTouchingLargeBtn(choiceBtnLeftX, choiceBtnTopY))
			{
				return TitleMode;
			}
			else if(isTouchingLargeBtn(choiceBtnRightX, choiceBtnTopY))
			{
				return PlayMode;
			}
		}
	}
	return PausedMode;
}
Mode Io::manageTitleMode()
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	int newGameX = (Iw2DGetSurfaceWidth() / 2) - buttonWid / 2;
	int newGameY = (Iw2DGetSurfaceHeight() / 2) - buttonHi / 2;

	renderTitleScren(newGameX, newGameY);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 &&
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		currTouch = g_Input.getTouch(0);
		if(isTouchingLargeBtn(newGameX, newGameY))
		{
			takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
			return PlayMode;
		}
	}
	return TitleMode;
}
Mode Io::manageGameEnded(int lives)
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	int x = (Iw2DGetSurfaceWidth() / 2) - buttonWid / 2,
		y = (Iw2DGetSurfaceHeight() / 2) - buttonHi / 2;

	renderGameEnded(x, y, lives);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 && 
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
		return TitleMode;
	}
	return EndedMode;
}
void Io::invokeGridTouch()
{
	holdingCounter++;
}
int Io::getBorder() const
{
	return horBorder;
}
int Io::getOffset() const
{
	return verOffset;
}
void Io::cleanUpImages()
{
	for(int i=0; i < NUM_TILE_TYPES; i++)
		delete tileImage[i];

	delete font;
}
void Io::drawTile(int colour, int x, int y) const
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x, y)
		);
}
void Io::drawTile(int colour, int x, int y, int wi, int hi) const
{
	Iw2DDrawImage(
		tileImage[colour],
		CIwSVec2(x, y),
		CIwSVec2(wi, hi)
		);
}
void Io::setUpGrapicRes(int tileSize)
{
	cleanUpImages();
	tileImage[BuyDamageImage]	= Iw2DCreateImageResource("buy_dmg_tile");
	tileImage[BuyRangeImage]	= Iw2DCreateImageResource("buy_range_tile");
	tileImage[BuySpeedImage]	= Iw2DCreateImageResource("buy_speed_tile");
	tileImage[CrossWallImage]	= Iw2DCreateImageResource("cross_wall_tile");
	tileImage[ExitImage]		= Iw2DCreateImageResource("exit_tile");
	tileImage[FastSpeedImage]	= Iw2DCreateImageResource("fast_speed_tile");
	tileImage[GrassImage]		= Iw2DCreateImageResource("grass_tile");
	tileImage[IceImage]			= Iw2DCreateImageResource("ice_tile");
	tileImage[LdWallImage]		= Iw2DCreateImageResource("ld_wall_tile");
	tileImage[LuWallImage]		= Iw2DCreateImageResource("lu_wall_tile");
	tileImage[MonsterImage]		= Iw2DCreateImageResource("purmon_tile");
	tileImage[MudImage]			= Iw2DCreateImageResource("mud_tile");
	tileImage[NormalSpeedImage]	= Iw2DCreateImageResource("normal_speed_tile");
	tileImage[PauseImage]		= Iw2DCreateImageResource("pause_tile");
	tileImage[PlayImage]		= Iw2DCreateImageResource("speed_tile");
	tileImage[RdWallImage]		= Iw2DCreateImageResource("rd_wall_tile");
	tileImage[RedShotImage]		= Iw2DCreateImageResource("red_shot_tile");
	tileImage[RedTowerImage]	= Iw2DCreateImageResource("red_tower_tile");
	tileImage[RlWallImage]		= Iw2DCreateImageResource("rl_wall_tile");
	tileImage[RldWallImage]		= Iw2DCreateImageResource("rld_wall_tile");
	tileImage[RluWallImage]		= Iw2DCreateImageResource("rlu_wall_tile");
	tileImage[RuWallImage]		= Iw2DCreateImageResource("ru_wall_tile");
	tileImage[SelectionImage]	= Iw2DCreateImageResource("tile_selected_tile");
	tileImage[SellImage]		= Iw2DCreateImageResource("sell_tile");
	tileImage[SpawnImage]		= Iw2DCreateImageResource("spawn_tile");
	tileImage[TealShotImage]	= Iw2DCreateImageResource("teal_shot_tile");
	tileImage[TealTowerImage]	= Iw2DCreateImageResource("teal_tower_tile");
	tileImage[UdWallImage]		= Iw2DCreateImageResource("ud_wall_tile");
	tileImage[UdlWallImage]		= Iw2DCreateImageResource("udl_wall_tile");
	tileImage[UdrWallImage]		= Iw2DCreateImageResource("udr_wall_tile");
	tileImage[WaterImage]		= 0;//Iw2DCreateImageResource("tilesWater");
	tileImage[YellowShotImage]	= Iw2DCreateImageResource("yellow_shot_tile");
	tileImage[YellowTowerImage]	= Iw2DCreateImageResource("yellow_tower_tile");

	if(tileSize < 40)
		font = Iw2DCreateFontResource("font9");
	else
		font = Iw2DCreateFontResource("font18");

	Iw2DSetFont(font);
}
int Io::getLastTouchX() const
{
	return lastTouchX;
}
int Io::getLastTouchY() const
{
	return lastTouchY;
}
void Io::initProgBars(ProgBar **dmgProgressBar,
					  ProgBar **asProgressBar, 
					  ProgBar **ranProgressBar)
{
	int progBarHi = buttonHi / 5;
	int topLeftY = buttonY[Btn1BottomButton] - progBarHi;
	*dmgProgressBar = new ProgBar(buttonX, topLeftY, buttonWid, progBarHi);

	topLeftY = buttonY[Btn2BottomButton] - progBarHi;
	*asProgressBar = new ProgBar(buttonX, topLeftY, buttonWid, progBarHi);

	topLeftY = buttonY[Btn3BottomButton] - progBarHi;
	*ranProgressBar = new ProgBar(buttonX, topLeftY, buttonWid, progBarHi);
}
void Io::renderProgressBar( ProgBar *pBar ) const
{
	Iw2DSetColour(L_GREEN);
	Iw2DFillRect(CIwSVec2(pBar->getTopLeftX(), pBar->getTopLeftY()),
		CIwSVec2(pBar->getProgress(), pBar->getHeight()));
		Iw2DSetColour(BLACK);
}
void Io::renderTileSelected(int x, int y, int tileSize) const
{
	drawTile(SelectionImage, x, y, tileSize, tileSize);
}
void Io::renderMenuBtn() const
{
	Iw2DSetColour(0x77d03D50);
	Iw2DFillRect(CIwSVec2(buttonX, 0), CIwSVec2(buttonWid, verOffset));
	setTextColor(true);
	Iw2DDrawString("MENU", CIwSVec2(textX[MenuText], textY[MenuText]), 
		CIwSVec2(buttonWid, verOffset),
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(BLACK);
}
bool Io::topBarTouch() const
{
	return currTouch->y < verOffset;
}
bool Io::textAreaTouch() const
{
	return currTouch->x < buttonX;
}
void Io::renderMenuBG() const
{
	Iw2DSetColour(0x77d03D50);
	Iw2DFillRect(CIwSVec2(buttonX, verOffset), 
		CIwSVec2(buttonWid, Iw2DGetSurfaceHeight()));
	Iw2DSetColour(BLACK);	
}
void Io::renderSellBtn(bool active) const
{
	if(active)
		renderNoAlphaButton(SellImage, SellButton);
	else
		renderAlphaButton(SellImage, SellButton);
}
void Io::renderNoAlphaButton( int color, int yIndex ) const
{
	drawTile(color, buttonX, buttonY[yIndex], buttonWid, buttonHi);
}
void Io::renderSpawn(int x, int y, int size) const
{
	drawTile(SpawnImage, x, y, size, size);
}
void Io::renderExit(int x, int y, int size) const
{
	drawTile(ExitImage, x, y, size, size);
}
