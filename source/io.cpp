#include <iostream>

#include "IwResManager.h"
#include "io.h"

CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DFont* font;
//=============================================================================
Io::Io(int _tileSize) : tileSize(_tileSize) 
{
	setUpGrapicRes(tileSize);
	reset();
}
//=============================================================================
Io::~Io()
{
	cleanUpImages();
}
//=============================================================================
void Io::reset()
{
	//contWaves			= false;
	takeNextInputAt		= INT_MAX;
	holdingCounter	= 0;
}
//=============================================================================
InputEvent Io::handleInput(CTouch **touch) //TODO opti
{
	InputEvent event = DoNothingInputEvent;
	if(g_Input.getTouchCount() == 0)
	{
		if(holdingCounter > 0)
			event = GridInputEvent;
		takeNextInputAt		= 0;
		holdingCounter	= 0;
	}
	else if((uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		*touch = g_Input.getTouch(0);
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

		if(validTouch(*touch))
		{
			if(gridTouch(*touch))
			{
				/*if((*touch)->x == lastTouchX && (*touch)->y == lastTouchY)
				{
				event = UndoInputEvent;
				holdingCounter	= 0;

				}
				else*/
				invokeGridTouch(*touch);
			}
			else
			{
				if(buttonTouchX(*touch))
				{
					if(speedTouch(*touch))
					{
						/*	if(holdingCounter > 1)
						event = DoNothingInputEvent;*/
						//invokeSpeedBtn();
						event = ChangeSpeedInputEvent;
					}
					else if(pauseTouch(*touch))
					{
						event = PauseBtnInputEvent;
					}
					/*	else if(undoTouch(*touch))
					{
					return UndoBtnInputEvent;
					}*/
					if(damageTouch(*touch))
					{
						event = DmgBtnInputEvent;
					}
					else if(buySpeedTouch(*touch))
					{
						event = AsBtnInputEvent;
					}
					else if(buyRangeTouch(*touch))
					{
						event = RangeBtnInputEvent;
					}
				}
			}
		}
		lastTouchX = (*touch)->x;
		lastTouchY = (*touch)->y;
	}
	return event;
}
//==============================================================================
void Io::renderBg() const
{
	//Iw2DSetColour(0xFF4E4949);//0xed10be36);
	//Iw2DFillRect(CIwSVec2(0,0), 
	//	CIwSVec2(Iw2DGetSurfaceWidth(), Iw2DGetSurfaceHeight()));
	//Iw2DSetColour(0xffffffff);
}
//==============================================================================
void Io::setUpUI(int &_gridColumns, int &_gridRows)
{	
	if(tileSize < 40)
	{
		buttonWid	= 50;
		textHi		= 13;
	}
	else
	{
		buttonWid	= 130;
		textHi		= 26;
	}

	_gridColumns	= (Iw2DGetSurfaceWidth() - buttonWid) / tileSize;
	gridColumns		= ((Iw2DGetSurfaceWidth() - buttonWid) % tileSize >= 15) ? //TODO
_gridColumns : _gridColumns-1;
	_gridColumns	= gridColumns;

	setBorders();

	textAreaHi		= textHi*6;
	textAreaWid		= buttonWid;
	largeButtonWid	= tileSize * 5;
	largeButtonHi	= tileSize * 2;

	// Since vert border = hor border in game atm use hor here too. Later
	// use a single border for all sides and add left over spacing between meny and tilegrid?
	buttonX	 = horizontalBorder; 
	buttonHi = (Iw2DGetSurfaceHeight() - textAreaHi - 12*horizontalBorder) /5;
	gridRows = _gridRows = 13;

	setButtonSize();
	setTextAreas();
}
//==============================================================================
void Io::setBorders()
{
	int wid = Iw2DGetSurfaceWidth();
	int hi = Iw2DGetSurfaceHeight();

	if(wid < hi)
	{
		int temp	= wid;
		wid			= hi;
		hi			= temp;
	}
	horizontalBorder = (hi - gridRows*tileSize) / 2;
	horizontalOffset = 2*horizontalBorder + textAreaWid;
}
//=============================================================================
void Io::renderAlphaButton(int color, int yIndex) const
{
	Iw2DSetColour(0xbb40C020);
	drawTile(color, buttonX, buttonY[yIndex], buttonWid, buttonHi);
	Iw2DSetColour(0xffffffff);
}
//=============================================================================
void Io::renderButtons(int mobsAlive, bool newTowerBuilt, 
					   ButtonState asState, ButtonState dmgState, 
					   ButtonState rangeState, SpeedMode speedMode) const
{
	Iw2DSetColour(0xffffffff);
	/*
	if(contWaves)
	drawTile(ContWavesImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi);
	else*/ 
	if(speedMode == ImmobileSpeedMode && mobsAlive == 0)
	{
		drawTile(SpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi);
	}
	else if(speedMode == NormalSpeedMode)
		drawTile(NormalSpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi); 
	else
		drawTile(FastSpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi); 

	//if(newTowersIsEmpty)
	//{
	//	renderAlphaButton(UndoImage, UndoButton);
	//}
	//else
	//	drawTile(UndoImage, buttonX, buttonY[UndoButton], buttonWid, buttonHi);

	drawTile(PauseImage, buttonX, buttonY[PauseButton], buttonWid, buttonHi);
	if(dmgState == InactiveButtonState)
		renderAlphaButton(BuyDamageImage, BuyDamageButton);
	else
		drawTile(BuyDamageImage, buttonX, buttonY[BuyDamageButton], buttonWid, buttonHi);

	if(asState != InvisButtonState)
		if(asState == InactiveButtonState)
			renderAlphaButton(BuySpeedImage, BuySpeedButton);
		else
			drawTile(BuySpeedImage, buttonX, buttonY[BuySpeedButton], buttonWid, buttonHi);

	if(rangeState != InvisButtonState)
		if(rangeState == InactiveButtonState)
			renderAlphaButton(BuyRangeImage, BuyRangeButton);
		else
			drawTile(BuyRangeImage, buttonX, buttonY[BuyRangeButton], buttonWid, buttonHi);
}
//============================================================================
void Io::renderPaused(int qx, int cx, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));

	Iw2DSetColour(0xff10be36); 

	Iw2DDrawString("QUIT", CIwSVec2(qx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("CONTINIUE", CIwSVec2(cx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void Io::renderTitleScren(int newX, int newY) const
{
	Iw2DSetColour(0xFF12AB09);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(largeButtonWid, 2*tileSize));

	Iw2DDrawString("NEW GAME", CIwSVec2(newX, newY), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void Io::renderGameEnded(int x, int y, int lives) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2));

	if(lives == 0)
		Iw2DDrawString("GAME OVER", CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	else
		Iw2DDrawString("[Insert victory message]", CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void Io::renderWaveText(int wave) const
{
	char str[6];
	sprintf(str,  "%d W", wave);

	Iw2DDrawString(str, CIwSVec2(buttonX, textY[WaveText]), CIwSVec2(textAreaWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
void Io::renderCreditsText(int credits) const
{
	char str[8];
	sprintf(str, "%d C", credits);

	Iw2DDrawString(str, CIwSVec2(buttonX, textY[CreditsText]), 
		CIwSVec2(textAreaWid, textHi),
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void Io::renderLivesText(int lives) const
{
	char str[6];
	sprintf(str, "%d L", lives);

	Iw2DDrawString(str, CIwSVec2(buttonX, textY[LivesText]), 
		CIwSVec2(textAreaWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
void Io::setTextColor()
{
	Iw2DSetColour(0xffffffff);//0xFF40C020);
}
//==============================================================================
void Io::setButtonSize()
{
	int verticalSpace = buttonHi + 2*horizontalBorder;

	buttonY[PauseButton]		= Iw2DGetSurfaceHeight() - horizontalBorder - buttonHi;
	buttonY[BuyRangeButton]		= buttonY[PauseButton] - verticalSpace;
	buttonY[BuySpeedButton]		= buttonY[BuyRangeButton] - verticalSpace;
	buttonY[BuyDamageButton]	= buttonY[BuySpeedButton] - verticalSpace;
	buttonY[SpeedButton]		= buttonY[BuyDamageButton] - verticalSpace;

	buttonY[SpeedBottomButton]		= buttonY[SpeedButton] + buttonHi;
	buttonY[PauseBottomButton]		= buttonY[PauseButton] + buttonHi;
	buttonY[BuyDamageBottomButton]	= buttonY[BuyDamageButton] + buttonHi;
	buttonY[BuySpeedBottomButton]	= buttonY[BuySpeedButton] + buttonHi;
	buttonY[BuyRangeBottomButton]	= buttonY[BuyRangeButton] + buttonHi;
}
//=============================================================================

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

bool Io::damageTouch(CTouch *touch) const
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

bool Io::undoTouch(CTouch *touch) const
{
	return touch->y < buttonY[UndoBottomButton]
	&& touch->y >= buttonY[UndoButton];
}
bool Io::gridTouch(CTouch *touch) const
{
	return touch->x >= horizontalOffset;
}
void Io::setTextAreas()
{
	textY[LivesText] = horizontalBorder;
	textY[WaveText] = textY[LivesText] + textHi;
	textY[CreditsText] = textY[WaveText] + textHi;
}
bool Io::validTouch(CTouch *touch) const
{
	return touch->x >= horizontalBorder
		&& touch->x < Iw2DGetSurfaceWidth() - horizontalBorder
		&& touch->y >= horizontalBorder
		&& touch->y < Iw2DGetSurfaceHeight() - horizontalBorder;
}
bool Io::buyTouch(CTouch *touch) const
{
	return touch->y > buttonY[BuyButton] &&
		touch->y < buttonY[BuyBottomButton];
}

bool Io::speedTouch(CTouch *touch) const
{
	return touch->y < buttonY[SpeedBottomButton];
}

bool Io::buttonTouchX(CTouch *touch) const
{
	return touch->x < horizontalOffset - horizontalBorder;
}

Mode Io::manangePausedMode()
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	int quitLeftX		= (gridColumns / 2 - 4) * tileSize,
		continiueLeftX	= (gridColumns / 2 + 3) * tileSize,
		y				= (gridRows / 2 - 1) * tileSize;

	renderPaused(quitLeftX, continiueLeftX, y);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 &&
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		CTouch *touch = g_Input.getTouch(0);

		if(isTouchingLargeBtn(touch, quitLeftX, y))
		{
			takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
			return TitleMode;
		}
		else if(isTouchingLargeBtn(touch, continiueLeftX, y))
		{
			takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
			return PlayMode;
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

	Iw2DSurfaceClear(0xFF4E4949);
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
}

void Io::invokeSpeedBtn()
{
	//if(holdingPlayCounter > 2)
	//{
	//	contWaves = true;
	//}
	//else if(contWaves)
	//{
	//	contWaves = false;

	//	//Prevent speed change in next input-handling cycle
	//	holdingPlayCounter = 0;
	//}
}
//bool Io::contWavesActive() const
//{
//	return contWaves;
//}
int Io::getHorizontalBorder() const
{
	return horizontalBorder;
}
int Io::gethorizontalOffset() const
{
	return horizontalOffset;
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

	tileImage[GrassImage]		= Iw2DCreateImageResource("grass_tile");
	//tileImage[WaterImage]		= Iw2DCreateImageResource("tilesWater");
	tileImage[SpawnImage]		= Iw2DCreateImageResource("spawn_tile");
	tileImage[ExitImage]		= Iw2DCreateImageResource("exit_tile");
	tileImage[MonsterImage]		= Iw2DCreateImageResource("purmon_tile");
	tileImage[SpeedImage]		= Iw2DCreateImageResource("speed_tile");
	tileImage[PauseImage]		= Iw2DCreateImageResource("pause_tile");
	tileImage[SellImage]		= Iw2DCreateImageResource("sell_tile");
	tileImage[BuyDamageImage]	= Iw2DCreateImageResource("buy_dmg_tile");
	tileImage[BuySpeedImage]	= Iw2DCreateImageResource("buy_speed_tile");
	tileImage[BuyRangeImage]	= Iw2DCreateImageResource("buy_range_tile");
	tileImage[NormalSpeedImage]	= Iw2DCreateImageResource("normal_speed_tile");
	tileImage[FastSpeedImage]	= Iw2DCreateImageResource("fast_speed_tile");
	tileImage[IceImage]			= Iw2DCreateImageResource("ice_tile");
	tileImage[MudImage]			= Iw2DCreateImageResource("mud_tile");
	tileImage[RlWallImage]		= Iw2DCreateImageResource("rl_wall_tile");
	tileImage[UdWallImage]		= Iw2DCreateImageResource("ud_wall_tile");
	tileImage[TowerImage]		= Iw2DCreateImageResource("tower_tile");
	tileImage[ShotImage]		= Iw2DCreateImageResource("shot_tile");
	tileImage[CrossWallImage]	= Iw2DCreateImageResource("cross_wall_tile");
	tileImage[RdWallImage]		= Iw2DCreateImageResource("rd_wall_tile");
	tileImage[RuWallImage]		= Iw2DCreateImageResource("ru_wall_tile");
	tileImage[LdWallImage]		= Iw2DCreateImageResource("ld_wall_tile");
	tileImage[LuWallImage]		= Iw2DCreateImageResource("lu_wall_tile");
	tileImage[RluWallImage]		= Iw2DCreateImageResource("rlu_wall_tile");
	tileImage[RldWallImage]		= Iw2DCreateImageResource("rld_wall_tile");
	tileImage[UdrWallImage]		= Iw2DCreateImageResource("udr_wall_tile");
	tileImage[UdlWallImage]		= Iw2DCreateImageResource("udl_wall_tile");

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
