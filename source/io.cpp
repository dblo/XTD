#include <iostream>

#include "IwResManager.h"
#include "io.h"

const int BLACK				= 0xffffffff;
const int L_GREEN			= 0xff10be36;
const int D_GREEN			= 0xff046b0a;
const int GREY				= 0xff4e4949;
const int BLUE				= 0xffff9900;
const int NUM_TILE_TYPES	= 29;

CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DFont* font;

Io::Io(int _tileSize) : tileSize(_tileSize) 
{
	menuOn = false;
	setUpGrapicRes(tileSize);
	reset();
}
Io::~Io()
{
	cleanUpImages();
}
void Io::reset()
{
	takeNextInputAt		= INT_MAX;
	holdingCounter	= 0;
}
InputEvent Io::handleInput(bool structureSelected)
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
		CTouch *touch = g_Input.getTouch(0);
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

		if(withinBorders(touch))
		{
			if(topBarTouch(touch))
			{
				if(textAreaTouch(touch))
				{
					menuOn = false;
					event = ClearEvent;
				}
				else // Touching menu button
				{
					// Don't toggle menu if it's showing due to selected structure
					if(!structureSelected)
					{
						menuOn = !menuOn;
					}
					event = MenuEvent;
				}
			}
			else if(gridTouch(touch, structureSelected))
			{
				invokeGridTouch(touch);
			}
			// At this point menu must be showing and the touch is within it
			else if(pauseTouch(touch))
			{
				event = PauseBtnInputEvent;
			}
			else if(playTouch(touch))
			{
				event = ChangeSpeedInputEvent;
			}
			else if(buyDamageTouch(touch))
			{
				event = Btn2Event;
			}
			else if(buySpeedTouch(touch))
			{
				event = Btn3Event;
			}
			else if(buyRangeTouch(touch))
			{
				event = Btn4Event;
			}
		}
		lastTouchX = touch->x;
		lastTouchY = touch->y;

		if(menuOn && event != MenuEvent)
			menuOn = false;
	}
	return event;
}
void Io::renderBg() const
{
	Iw2DSetColour(GREY);
	Iw2DFillRect(CIwSVec2(horBorder, horBorder), 
		CIwSVec2(buttonX - horBorder, verOffset));
	Iw2DSetColour(BLACK);
}
void Io::setUpUI(int &_gridColumns, int &_gridRows)
{	
	if(tileSize > 40) //iphone 5
	{
		//buttonWid	= 150;
		//textHi		= 26;
	}
	else
	{
		//buttonWid	= 60;
		//textHi		= 13;
	}

	buttonWid = 2 * tileSize;

	int wid = Iw2DGetSurfaceWidth();
	int hi = Iw2DGetSurfaceHeight();

	gridColumns	= _gridColumns	= Iw2DGetSurfaceWidth() / tileSize;
	gridRows = _gridRows = 11;

	if(wid < hi)
	{
		int temp	= wid;
		wid			= hi;
		hi			= temp;
	}

	horBorder = (wid - gridColumns*tileSize) / 2;
	//	setBorders();
	verOffset		= Iw2DGetSurfaceHeight() - gridRows*tileSize;
	largeButtonWid	= tileSize * 4;
	largeButtonHi	= (tileSize * 2) /3;
	widthMinusBorder = wid - horBorder;
	heigthMinusBorder = hi - horBorder;
	gridHeigth = gridRows*tileSize;
	buttonX	 = Iw2DGetSurfaceWidth() - buttonWid - horBorder; 
	buttonHi = tileSize;//tileSize*2;

	setButtonSize();
	setTextAreas();
}
void Io::setBorders()
{

	//verOffset = (hi - - statusBar - gridRows*tileSize) / 2;
}
void Io::renderAlphaButton(int color, int yIndex) const
{
	Iw2DSetColour(0xbb40C020);
	drawTile(color, buttonX, buttonY[yIndex], buttonWid, buttonHi);
	Iw2DSetColour(0xffffffff);
}
void Io::renderPauseButton()
{
	drawTile(PauseImage, buttonX, buttonY[PauseButton], buttonWid, buttonHi);
}
void Io::renderFastSpeedButton()
{
	drawTile(FastSpeedImage, buttonX, buttonY[PlayButton], buttonWid, buttonHi); 
}
void Io::renderNormalSpeedButton()
{
	drawTile(NormalSpeedImage, buttonX, buttonY[PlayButton], buttonWid, buttonHi); 
}
void Io::renderPlayButton()
{
	drawTile(PlayImage, buttonX, buttonY[PlayButton], buttonWid, buttonHi);
}
void Io::renderUpgDmgButton(bool active)
{
	if(active)
		drawTile(BuyDamageImage, buttonX, buttonY[BuyDamageButton], buttonWid, buttonHi);
	else
		renderAlphaButton(BuyDamageImage, BuyDamageButton);
}
void Io::renderUpgSpdButton(bool active)
{
	if(active)
		drawTile(BuySpeedImage, buttonX, buttonY[BuySpeedButton], buttonWid, buttonHi);
	else
		renderAlphaButton(BuySpeedImage, BuySpeedButton);
}
void Io::renderUpgRangeButton(bool active)
{
	if(active)
		drawTile(BuyRangeImage, buttonX, buttonY[BuyRangeButton], buttonWid, buttonHi);
	else
		renderAlphaButton(BuyRangeImage, BuyRangeButton);
}
void Io::renderPaused(int qx, int cx, int y) const
{
	Iw2DSetColour(0xFF046b0a);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));

	Iw2DSetColour(0xff10be36); 

	Iw2DDrawString("QUIT", CIwSVec2(qx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("CONTINIUE", CIwSVec2(cx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(0xffffffff); 
}
void Io::renderTitleScren(int newX, int newY) const
{
	Iw2DSetColour(L_GREEN);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(largeButtonWid, 2*tileSize));

	Iw2DSetColour(D_GREEN); 

	Iw2DDrawString("NEW GAME", CIwSVec2(newX, newY), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(BLACK);
}
void Io::renderGameEnded(int x, int y, int lives) const
{
	Iw2DSetColour(0xFF046b0a);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2));

	Iw2DSetColour(0xff10be36); 

	if(lives == 0)
		Iw2DDrawString("GAME OVER", CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	else
		Iw2DDrawString("[Insert victory message]", CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(0xffffffff); 
}
void Io::renderText( const char* str, Text txt ) const
{
	Iw2DDrawString(str, CIwSVec2(textX[txt], textY[txt]), 
		CIwSVec2(100, verOffset),
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);
}
void Io::setTextColor(bool textColorOn)
{
	if(textColorOn)
		Iw2DSetColour(0xff10be36); 
	else
		Iw2DSetColour(0xffffffff); 
}
void Io::setButtonSize()
{
	int buttonSpacing = (Iw2DGetSurfaceHeight() - verOffset - 7*buttonHi)/7;
	int verticalSpace = buttonHi + buttonSpacing;

	buttonY[PlayButton]			= verOffset +  buttonSpacing;
	buttonY[SendButton]			= buttonY[PlayButton] + verticalSpace;
	buttonY[SellButton]			= buttonY[SendButton] + verticalSpace;
	buttonY[BuyDamageButton]	= buttonY[SellButton] + verticalSpace;
	buttonY[BuySpeedButton]		= buttonY[BuyDamageButton] + verticalSpace;
	buttonY[BuyRangeButton]		= buttonY[BuySpeedButton] + verticalSpace;
	buttonY[PauseButton]		= buttonY[BuyRangeButton] + verticalSpace;

	buttonY[PlayBottomButton]		= buttonY[PlayButton] + buttonHi;
	buttonY[PauseBottomButton]		= buttonY[PauseButton] + buttonHi;
	buttonY[BuyDamageBottomButton]	= buttonY[BuyDamageButton] + buttonHi;
	buttonY[BuySpeedBottomButton]	= buttonY[BuySpeedButton] + buttonHi;
	buttonY[BuyRangeBottomButton]	= buttonY[BuyRangeButton] + buttonHi;
}
bool Io::isTouchingLargeBtn(CTouch *touch, unsigned int x, unsigned int y) const
{
	return touch->x > x && touch->x <= x + largeButtonWid
		&& touch->y >= y && touch->y < y + tileSize*2;
}
bool Io::buyRangeTouch(CTouch *touch) const
{
	return touch->y < buttonY[BuyRangeBottomButton]
	&& touch->y >= buttonY[BuyRangeButton];
}
bool Io::buyDamageTouch(CTouch *touch) const
{
	return touch->y < buttonY[BuyDamageBottomButton]
	&& touch->y >= buttonY[BuyDamageButton];
}
bool Io::buySpeedTouch(CTouch *touch) const
{
	return touch->y < buttonY[BuySpeedBottomButton]
	&& touch->y >= buttonY[BuySpeedButton];
}
bool Io::pauseTouch(CTouch *touch) const
{
	return touch->y < buttonY[PauseBottomButton]
	&& touch->y >= buttonY[PauseButton];
}
bool Io::sellTouch(CTouch *touch) const
{
	//return touch->y < buttonY[UndoBottomButton]
	//&& touch->y >= buttonY[UndoButton];
	return 1;//dummy
}
bool Io::gridTouch(CTouch *touch, bool structureSelected) const
{
	if(menuOn || structureSelected)
		return touch->x < buttonX;
	return touch->x < widthMinusBorder;
}
void Io::setTextAreas()
{
	textY[LivesText]	= 0;
	textY[WaveText]		= 0;
	//textY[WallsText]	= textY[WaveText] + textHi;
	textY[CreditsText]	= 0;
	textY[InfoText]	= 0;
	textY[MenuText]		= 0;

	textLength[LivesText] = textLength[WaveText] = (2*buttonWid) / 3;;
	textLength[CreditsText] = textLength[InfoText] = buttonWid;

	textX[LivesText]	= horBorder;
	textX[WaveText]		= textX[LivesText] + textLength[LivesText];
	textX[CreditsText]	= textX[WaveText] + textLength[WaveText];
	textX[InfoText]		= textX[CreditsText] + textLength[CreditsText];
	textX[MenuText]		= widthMinusBorder - buttonWid;
}
bool Io::withinBorders(CTouch *touch) const
{
	return touch->x >= horBorder
		&& touch->x < widthMinusBorder
		&& touch->y >= horBorder
		&& touch->y < heigthMinusBorder; //TODO add ver border
}
bool Io::playTouch(CTouch *touch) const
{
	return touch->y >= buttonY[PlayButton] &&
		touch->y < buttonY[PlayBottomButton];
}
bool Io::buttonTouchX(CTouch *touch) const
{
	return touch->x >= buttonX;
}
Mode Io::manangePausedMode()
{
	int quitLeftX		= (gridColumns / 2 - 4) * tileSize,
		continiueLeftX	= (gridColumns / 2 + 3) * tileSize,
		y				= (gridRows / 2 - 1) * tileSize;

	renderPaused(quitLeftX, continiueLeftX, y);
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
			CTouch *touch = g_Input.getTouch(0);
			if(isTouchingLargeBtn(touch, quitLeftX, y))
			{
				return TitleMode;
			}
			else if(isTouchingLargeBtn(touch, continiueLeftX, y))
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
	int newGameX = (Iw2DGetSurfaceWidth() - largeButtonWid) / 2;
	int newGameY = 6*tileSize;

	renderTitleScren(newGameX, newGameY);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 &&
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		CTouch *touch = g_Input.getTouch(0);
		if(isTouchingLargeBtn(touch, newGameX, newGameY))
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
	int x = (Iw2DGetSurfaceWidth() - 3*largeButtonWid) / 2,
		y = (gridRows / 2 - 1) * tileSize;

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
void Io::invokeGridTouch(CTouch *touch)
{
	holdingCounter++;
	menuOn = false;
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
void Io::setUpGrapicRes(int _tileSize)
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
	tileImage[RlWallImage]		= Iw2DCreateImageResource("rl_wall_tile");
	tileImage[RldWallImage]		= Iw2DCreateImageResource("rld_wall_tile");
	tileImage[RluWallImage]		= Iw2DCreateImageResource("rlu_wall_tile");
	tileImage[RuWallImage]		= Iw2DCreateImageResource("ru_wall_tile");
	tileImage[SelectionImage]	= Iw2DCreateImageResource("tile_selected_tile");
	tileImage[SellImage]		= Iw2DCreateImageResource("sell_tile");
	tileImage[ShotImage]		= Iw2DCreateImageResource("shot_tile");
	tileImage[SpawnImage]		= Iw2DCreateImageResource("spawn_tile");
	tileImage[TowerImage]		= Iw2DCreateImageResource("tower_tile");
	tileImage[UdWallImage]		= Iw2DCreateImageResource("ud_wall_tile");
	tileImage[UdlWallImage]		= Iw2DCreateImageResource("udl_wall_tile");
	tileImage[UdrWallImage]		= Iw2DCreateImageResource("udr_wall_tile");
	tileImage[WaterImage]		= 0;//Iw2DCreateImageResource("tilesWater");

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
void Io::initProgBars(ProgBar **roundProgressBar, ProgBar **dmgProgressBar,
					  ProgBar **asProgressBar, ProgBar **ranProgressBar)
{
	int progBarHi = buttonHi / 5;

	int topLeftY = buttonY[PlayButton] - 2*horBorder - progBarHi;
	*roundProgressBar = new ProgBar(buttonX, topLeftY, buttonWid, progBarHi);

	topLeftY = buttonY[BuyDamageBottomButton] - progBarHi;
	*dmgProgressBar = new ProgBar(buttonX, topLeftY, buttonWid, progBarHi);

	topLeftY = buttonY[BuySpeedBottomButton] - progBarHi;
	*asProgressBar = new ProgBar(buttonX, topLeftY, buttonWid, progBarHi);

	topLeftY = buttonY[BuyRangeBottomButton] - progBarHi;
	*ranProgressBar = new ProgBar(buttonX, topLeftY, buttonWid, progBarHi);
}
void Io::renderProgressBar( ProgBar *pBar ) const
{
	Iw2DSetColour(L_GREEN);
	Iw2DFillRect(CIwSVec2(pBar->getTopLeftX(), pBar->getTopLeftY()),
		CIwSVec2(pBar->getProgress(), pBar->getHeight()));

	//Iw2DSetColour(0xFF046b0a);
	//Iw2DDrawRect(CIwSVec2(pBar->getTopLeftX(), pBar->getTopLeftY()),
	//	CIwSVec2(pBar->getWidth(), pBar->getHeight()));

	Iw2DSetColour(BLACK);
}
void Io::renderTileSelected(int x, int y) const
{
	drawTile(SelectionImage, x, y, tileSize, tileSize)	;
}
void Io::renderButtonSelected( Button btn ) const
{
	switch (btn)
	{
	case BuyDamageButton:
		drawTile(SelectionImage, buttonX, buttonY[BuyDamageButton],
			buttonWid, buttonHi);
		break;
	case BuyRangeButton:
		drawTile(SelectionImage, buttonX, buttonY[BuyRangeButton],
			buttonWid, buttonHi);
		break;
	case BuySpeedButton:
		drawTile(SelectionImage, buttonX, buttonY[BuySpeedButton],
			buttonWid, buttonHi);
		break;
	default:
		break;
	}
}

void Io::renderMenuBtn() const
{
	Iw2DSetColour(GREY);
	Iw2DFillRect(CIwSVec2(buttonX, 0), CIwSVec2(buttonWid, verOffset));
	Iw2DSetColour(L_GREEN);
	Iw2DDrawString("MENU", CIwSVec2(textX[MenuText], textY[MenuText]), 
		CIwSVec2(100, verOffset),
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(BLACK);
	//drawTile(PauseImage, buttonX, 0, buttonWid, verOffset);
}

bool Io::menuShowing() const
{
	return menuOn;
}

bool Io::topBarTouch(CTouch *touch) const
{
	return touch->y < verOffset;
}

bool Io::textAreaTouch( CTouch *touch ) const
{
	return touch->x < buttonX;
}

void Io::renderMenuBG() const
{
	Iw2DSetColour(GREY);
	Iw2DFillRect(CIwSVec2(buttonX, verOffset), 
		CIwSVec2(buttonWid, gridHeigth));
	Iw2DSetColour(BLACK);	
}
