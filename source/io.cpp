#include <iostream>

#include "IwResManager.h"
#include "io.h"

const int BLACK				= 0xffffffff;
const int L_GREEN			= 0xff10be36;
const int D_GREEN			= 0xff046b0a;
const int GREY				= 0xff4e4949;
const int BLUE				= 0xffff9900;
const int NUM_TILE_TYPES	= 36;

CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DFont* font;

Io::Io(int _tileSize) : tileSize(_tileSize) 
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
	//contWaves			= false;
	takeNextInputAt		= INT_MAX;
	holdingCounter	= 0;
}
InputEvent Io::handleInput()
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
		CTouch *touch = g_Input.getTouch(0);
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

		if(validTouch(touch))
		{
			if(gridTouch(touch))
			{
				invokeGridTouch(touch);
			}
			else
			{
				if(buttonTouchX(touch))
				{
					if(speedTouch(touch))
					{
						event = ChangeSpeedInputEvent;
					}
					else if(pauseTouch(touch))
					{
						event = PauseBtnInputEvent;
					}
					if(damageTouch(touch))
					{
						event = DmgBtnInputEvent;
					}
					else if(buySpeedTouch(touch))
					{
						event = AsBtnInputEvent;
					}
					else if(buyRangeTouch(touch))
					{
						event = RangeBtnInputEvent;
					}
				}
			}
		}
		lastTouchX = touch->x;
		lastTouchY = touch->y;
	}
	return event;
}
void Io::renderBg() const
{
	//Iw2DSetColour(0xFF4E4949);//0xed10be36);
	//Iw2DFillRect(CIwSVec2(0,0), 
	//	CIwSVec2(Iw2DGetSurfaceWidth(), Iw2DGetSurfaceHeight()));
	//Iw2DSetColour(0xffffffff);
}
void Io::setUpUI(int &_gridColumns, int &_gridRows)
{	
	if(tileSize < 40)
	{
		buttonWid	= 60;
		textHi		= 13;
	}
	else
	{
		buttonWid	= 150;
		textHi		= 26;
	}

	_gridColumns	= (Iw2DGetSurfaceWidth() - buttonWid) / tileSize;
	gridColumns		= ((Iw2DGetSurfaceWidth() - buttonWid) % tileSize >= 12) ? //TODO
_gridColumns : _gridColumns-1;
	_gridColumns	= gridColumns;
	gridRows = _gridRows = 13;

	setBorders();

	textAreaHi		= textHi*6;
	textAreaWid		= buttonWid;
	largeButtonWid	= tileSize * 4;
	largeButtonHi	= (tileSize * 2) /3;

	// Since vert border = hor border in game atm use hor here too. Later
	// use a single border for all sides and add left over spacing between meny and tilegrid?
	buttonX	 = border; 
	buttonHi = (Iw2DGetSurfaceHeight() - textAreaHi - 12*border) /5;

	setButtonSize();
	setTextAreas();
}
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
	border = (hi - gridRows*tileSize) / 2;
	gridOffset = wid - border - gridColumns*tileSize;
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
	Iw2DDrawString(str, CIwSVec2(buttonX, textY[txt]), 
		CIwSVec2(textAreaWid, textHi),
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
	int verticalSpace = buttonHi + 2*border;

	buttonY[PauseButton]		= Iw2DGetSurfaceHeight() - border - buttonHi;
	buttonY[BuyRangeButton]		= buttonY[PauseButton] - verticalSpace;
	buttonY[BuySpeedButton]		= buttonY[BuyRangeButton] - verticalSpace;
	buttonY[BuyDamageButton]	= buttonY[BuySpeedButton] - verticalSpace;
	buttonY[PlayButton]		= buttonY[BuyDamageButton] - verticalSpace;

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
	//return touch->y < buttonY[UndoBottomButton]
	//&& touch->y >= buttonY[UndoButton];
	return 1;//dummy
}
bool Io::gridTouch(CTouch *touch) const
{
	return touch->x >= gridOffset;
}
void Io::setTextAreas()
{
	textY[LivesText] = border;
	textY[WaveText] = textY[LivesText] + textHi;
	textY[WallsText] = textY[WaveText] + textHi;
	textY[CreditsText] = textY[WallsText] + textHi;
	textY[PriceText] = buttonY[PlayButton] - textHi - 
		2*border;
}
bool Io::validTouch(CTouch *touch) const
{
	return touch->x >= border
		&& touch->x < Iw2DGetSurfaceWidth() - border
		&& touch->y >= border
		&& touch->y < Iw2DGetSurfaceHeight() - border;
}
bool Io::buyTouch(CTouch *touch) const
{
	//return touch->y > buttonY[BuyButton] &&
	//	touch->y < buttonY[BuyBottomButton];
	return 1;//dummy
}
bool Io::speedTouch(CTouch *touch) const
{
	return touch->y >= buttonY[PlayButton] &&
		 touch->y < buttonY[PlayBottomButton];
}
bool Io::buttonTouchX(CTouch *touch) const
{
	return touch->x < gridOffset - border;
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
}
int Io::getBorder() const
{
	return border;
}
int Io::getOffset() const
{
	return gridOffset;
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
	tileImage[PlayImage]		= Iw2DCreateImageResource("speed_tile");
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
	tileImage[SelectionImage]	= Iw2DCreateImageResource("tile_selected_tile");

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

	int topLeftY = buttonY[PlayButton] - 2*border - progBarHi;
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
