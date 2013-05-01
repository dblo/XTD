#include <iostream>

#include "IwResManager.h"

#include "io.h"

CIw2DImage* tileImage[NUM_TILE_TYPES];
CIw2DImage* purpleMonster;
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
	contWaves			= false;
	showBuildMenu		= false;
	takeNextInputAt		= INT_MAX;
	holdingPlayCounter	= 0;
}
//=============================================================================
InputEvent Io::handleInput(CTouch **touch) //TODO opti
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
		if(holdingPlayCounter == 1)
		{
			holdingPlayCounter = 0;
			return ChangeSpeedInputEvent;
		}
		holdingPlayCounter = 0;
	}
	else if((uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		*touch = g_Input.getTouch(0);
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

		if(validTouch(*touch))
		{
			if(gridTouch(*touch))
			{
				return PlaceTowerInputEvent;
			}
			else
			{
				if(buttonTouchX(*touch))
				{
					if(speedTouch(*touch))
					{
						invokeSpeedBtn();
						return DoNothingInputEvent;
					}
					else if(buyTouch(*touch))
					{
						showBuildMenu = !showBuildMenu;
						return DoNothingInputEvent;
					}
					else if(pauseTouch(*touch))
					{
						return PauseBtnInputEvent;
					}
					else if(undoTouch(*touch))
					{
						return UndoBtnInputEvent;
					}
					else if(showBuildMenu)
					{
						if(damageTouch(*touch))
						{
							return DmgBtnInputEvent;
						}
						else if(buySpeedTouch(*touch))
						{
							return AsBtnInputEvent;
						}
						else if(buyRangeTouch(*touch))
						{
							return RangeBtnInputEvent;
						}
					}
					else if(incomeTouch(*touch))
					{
						return IncomeBtnInputEvent;
					}
				}
			}
		}
	}
	return DoNothingInputEvent;
}
//==============================================================================
void Io::renderBg() const
{
	Iw2DSetColour(0);
	Iw2DFillRect(CIwSVec2(0, verticalOffset - horizontalBorder), 
		CIwSVec2(buttonX-verticalBorder, Iw2DGetSurfaceHeight()));
	Iw2DSetColour(0xffffffff);
}
//==============================================================================
void Io::setUpUI(int &_gridColumns, int &_gridRows)
{	
	if(tileSize < 40)
	{
		buttonWid = 50;
	}
	else
		buttonWid = 130;

	textHi			= tileSize;
	textWid			= tileSize * 5;
	largeButtonWid	= tileSize * 5;
	largeButtonHi	= tileSize * 2;


	_gridColumns = (Iw2DGetSurfaceWidth() - buttonWid) / tileSize;
	gridColumns = ((Iw2DGetSurfaceWidth() - buttonWid) % tileSize >= 30) ? 
_gridColumns : _gridColumns-1;
	_gridColumns = gridColumns;

	_gridRows = (Iw2DGetSurfaceHeight() - textHi) / tileSize;
	gridRows = (Iw2DGetSurfaceHeight() % tileSize >= 10) ? 
_gridRows : _gridRows-1;
	_gridRows = gridRows;

	setBorders();

	verticalOffset  = horizontalBorder + tileSize;
	buttonX			= gridColumns * tileSize + 2*verticalBorder + verticalBorder/2;
	buttonHi		= (Iw2DGetSurfaceHeight() - 8*horizontalBorder) / 7;

	if(horizontalBorder > 8)
		textY			= horizontalBorder/2;
	else
		textY			= horizontalBorder;

	setButtonSize();
	setTextAreas();
}
//==============================================================================
void Io::setBorders()
{
	int cols = Iw2DGetSurfaceWidth();
	int hi = Iw2DGetSurfaceHeight();

	if(cols < hi)
	{
		int temp = cols;
		cols = hi;
		hi = temp;
	}

	horizontalBorder = (hi - gridRows*tileSize - textHi) / 2;
	verticalBorder = (cols - gridColumns*tileSize - buttonWid) / 3;	
}
//=============================================================================
void Io::renderAlphaButton(int color, int yIndex) const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_HALF); //TODO half or add best visually?
	Iw2DSetColour(0xFF40C020);
	drawTile(color, buttonX, buttonY[yIndex], buttonWid, buttonHi);

	Iw2DSetColour(0xffffffff);
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//=============================================================================
void Io::renderButtons(int mobsAlive, bool newTowersIsEmpty, 
					   ButtonState asState, ButtonState dmgState, 
					   ButtonState rangeState, ButtonState incomeState, 
					   SpeedMode speedMode) const
{
	Iw2DSetColour(0xffffffff);

	if(contWaves)
		drawTile(ContWavesImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi);
	else if(speedMode == ImmobileSpeedMode && mobsAlive == 0)
	{
		drawTile(SpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi);
	}
	else if(speedMode == NormalSpeedMode)
		drawTile(NormalSpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi); 
	else
		drawTile(FastSpeedImage, buttonX, buttonY[SpeedButton], buttonWid, buttonHi); 

	drawTile(BuyImage, buttonX, buttonY[BuyButton], buttonWid, buttonHi);

	if(newTowersIsEmpty)
	{
		renderAlphaButton(UndoImage, UndoButton);
	}
	else
		drawTile(UndoImage, buttonX, buttonY[UndoButton], buttonWid, buttonHi);

	drawTile(PauseImage, buttonX, buttonY[PauseButton], buttonWid, buttonHi);

	if(showBuildMenu)
	{
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
	else
	{
		if(incomeState != InvisButtonState)
			if(incomeState == InactiveButtonState)
				renderAlphaButton(IncomeImage, IncomeButton);
			else
				drawTile(IncomeImage, buttonX, buttonY[IncomeButton], buttonWid, buttonHi);
	}
}
//==============================================================================
void Io::renderPaused(int qx, int cx, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(largeButtonWid, tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	Iw2DDrawString("QUIT", CIwSVec2(qx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("CONTINIUE", CIwSVec2(cx, y), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Io::renderTitleScren(int newX, int newY) const
{
	Iw2DSetColour(0xFF12AB09);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(largeButtonWid, 2*tileSize));

	Iw2DSetColour(0xFF36BE10); 

	Iw2DDrawString("NEW GAME", CIwSVec2(newX, newY), CIwSVec2(largeButtonWid, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void Io::renderGameEnded(int x, int y, int topScore) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(largeButtonWid*3, tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	char str[30];
	sprintf(str, "Your highest score was %d", topScore);
	Iw2DDrawString(str, CIwSVec2(x, y), CIwSVec2(largeButtonWid*3, tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
void Io::renderIncomeText(int wave) const
{
	drawText(IncomeText, 'I', wave);
}
//==============================================================================
void Io::renderWaveText(int income) const
{
	drawText(WaveText, 'W', income);
}
//==============================================================================
void Io::renderCreditsText(int credits) const
{
	char str[8];
	if(credits > 999)
		sprintf(str, "C  %d", credits);
	else if(credits > 99)
		sprintf(str, "C  0%d", credits);
	else if(credits > 9)
		sprintf(str, "C  00%d", credits);
	else
		sprintf(str, "C  000%d", credits);

	Iw2DDrawString(str, CIwSVec2(textX[CreditsText], textY), CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void Io::renderScoreText(int score) const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF40C020);

	char str[10]; //TODO build string
	if(score > 99999)
		sprintf(str, "S  %d", score);
	else if(score> 9999)
		sprintf(str, "S  0%d", score);
	else if(score > 999)
		sprintf(str, "S  00%d", score);
	else if(score> 99)
		sprintf(str, "S  000%d", score);
	else if(score> 9)
		sprintf(str, "S  0000%d", score);
	else
		sprintf(str, "S  00000%d", score);

	Iw2DDrawString(str, CIwSVec2(textX[ScoreText], textY), 
		CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
void Io::drawText(Text x, char c, int text) const
{
	char str[7];
	if(text > 99)
		sprintf(str, "%c  %d", c, text);
	else if(text > 9)
		sprintf(str, "%c  0%d", c, text);
	else
		sprintf(str, "%c  00%d", c, text);

	Iw2DDrawString(str, CIwSVec2(textX[x], textY), CIwSVec2(textWid, textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
void Io::setButtonSize()
{
	int verticalSpace = buttonHi + horizontalBorder;

	buttonY[SpeedButton]		= horizontalBorder;
	buttonY[BuyButton]			= buttonY[SpeedButton] + verticalSpace;
	buttonY[IncomeButton]		= buttonY[BuyButton] + verticalSpace;
	buttonY[BuyDamageButton]	= buttonY[BuyButton] + verticalSpace;
	buttonY[BuySpeedButton]		= buttonY[BuyDamageButton] + verticalSpace;
	buttonY[BuyRangeButton]		= buttonY[BuySpeedButton] + verticalSpace;
	buttonY[UndoButton]			= buttonY[BuyRangeButton] + verticalSpace;
	buttonY[PauseButton]		= buttonY[UndoButton] + verticalSpace;

	buttonY[BuyBottomButton]		= buttonY[BuyButton] + buttonHi;
	buttonY[SpeedBottomButton]		= buttonY[SpeedButton] + buttonHi;
	buttonY[IncomeBottomButton]		= buttonY[IncomeButton] + buttonHi;
	buttonY[PauseBottomButton]		= buttonY[PauseButton] + buttonHi;
	buttonY[BuyDamageBottomButton]	= buttonY[BuyDamageButton] + buttonHi;
	buttonY[UndoBottomButton]		= buttonY[UndoButton] + buttonHi;
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
	return touch->x < tileSize * gridColumns + verticalBorder
		&& touch->y > verticalOffset;
}
void Io::setTextAreas()
{
	textX[ScoreText] = horizontalBorder;
	textX[IncomeText] = textX[ScoreText] + textWid;
	textX[WaveText] = textX[IncomeText] + textWid; 
	textX[CreditsText] = textX[WaveText] + textWid; 
}
bool Io::validTouch(CTouch *touch) const
{
	return touch->x >= verticalBorder			
		&& touch->x < Iw2DGetSurfaceWidth() - verticalBorder
		&& touch->y >= buttonY[SpeedButton]
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
	return touch->x >= gridColumns * tileSize + 2*verticalBorder;
}

bool Io::incomeTouch(CTouch *touch) const
{
	return touch->y < buttonY[IncomeBottomButton]
	&& touch->y >= buttonY[IncomeButton];
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

	Iw2DSurfaceClear(0xFF0C5907);
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

Mode Io::manageGameEnded(int topScore)
{
	if(g_Input.getTouchCount() == 0)
	{
		takeNextInputAt = 0;
	}
	int x = (Iw2DGetSurfaceWidth() - 3*largeButtonWid) / 2,
		y = (gridRows / 2 - 1) * tileSize;

	renderGameEnded(x, y, topScore);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 && 
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;
		return TitleMode;
	}
	return EndedMode;
}

void Io::invokeSpeedBtn()
{
	holdingPlayCounter++;
	if(holdingPlayCounter > 2)
	{
		contWaves = true;
	}
	else if(contWaves)
	{
		contWaves = false;

		//Prevent speed change in next input-handling cycle
		holdingPlayCounter = 0;
	}
}
bool Io::contWavesActive() const
{
	return contWaves;
}
int Io::getVerticalOffset() const
{
	return verticalOffset;
}
int Io::getVerticalBorder() const
{
	return verticalBorder;
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

	tileImage[GrassImage]		= Iw2DCreateImageResource("tilesGrass");
	tileImage[WaterImage]		= Iw2DCreateImageResource("tilesWater");
	tileImage[SpawnImage]		= Iw2DCreateImageResource("tilesspawn");
	tileImage[ExitImage]		= Iw2DCreateImageResource("tilesexit");
	tileImage[MonsterImage]		= Iw2DCreateImageResource("tilespurmon");
	tileImage[BuyImage]			= Iw2DCreateImageResource("tilesbuy");
	tileImage[SpeedImage]		= Iw2DCreateImageResource("tilesspeed");
	tileImage[ContWavesImage]	= Iw2DCreateImageResource("tilescontwaves");
	tileImage[IncomeImage]		= Iw2DCreateImageResource("tilesincome");
	tileImage[PauseImage]		= Iw2DCreateImageResource("tilespause");
	tileImage[UndoImage]		= Iw2DCreateImageResource("tilesundo");
	tileImage[BuyDamageImage]	= Iw2DCreateImageResource("tilesDamage");
	tileImage[BuySpeedImage]	= Iw2DCreateImageResource("tilesBuySpeed");
	tileImage[BuyRangeImage]	= Iw2DCreateImageResource("tilesBuyRange");
	tileImage[NormalSpeedImage]	= Iw2DCreateImageResource("tilesNormalSpeed");
	tileImage[FastSpeedImage]	= Iw2DCreateImageResource("tilesFastSpeed");

	/*const char* imgType[] = 
	{
	"tower",
	"shot",
	"horwall",
	"vertwall"
	"diag14wall",
	"diag23wall"
	};*/

	//int len = sizeof(imgType) / sizeof(char);
	char temp[32];
	//for(int i=0; i < len; i++)
	//	{
	//		sprintf(temp, "tiles%d%s", tileSize, imgType[i]);	//change filename with # for the faster pack?
	//		//tileImage[i]
	//}
	int name;
	if(_tileSize > 30) //TODO makeshift
		name = 40;
	else
		name = 20;

	sprintf(temp, "tiles%dtower", name);	
	tileImage[TowerImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dshot", name);	
	tileImage[ShotImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dhorwall", name);	
	tileImage[HorWallImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%dvertwall", name);	
	tileImage[VerWallImage] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%ddiag14wall", name);	
	tileImage[Wall14Image] = Iw2DCreateImageResource(temp);

	sprintf(temp, "tiles%ddiag23wall", name);	
	tileImage[Wall23Image] = Iw2DCreateImageResource(temp);

	if(tileSize < 40)
		font = Iw2DCreateFontResource("font10");
	else
		font = Iw2DCreateFontResource("font22");

	Iw2DSetFont(font);
}
