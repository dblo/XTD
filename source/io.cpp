#include <iostream>

#include "IwResManager.h"
#include "io.h"
#include "Iw2D.h"

const int WHITE				= 0xffffffff;
const int L_GREEN			= 0xff10be36;
const int D_GREEN			= 0xff046b0a;
const int GREY				= 0xff4e4949;
const int BLUE				= 0xffff9900;
const int PURPLE			= 0xFFd03D50;
const int NUM_TILE_TYPES	= 35;
const int NUM_OF_BUTTONS	= 8;
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
		if(holdingCounter == 1)
		{
			event = DetermineEvent(showMenu);
		} 
		else if (holdingCounter > 1)
		{
			if(verticalSwipe())
			{
				if(swipeBeginX >= buttonX[UpgradeButton])
					event = MenuEvent;
				else if(swipeBeginX >= buttonX[PauseButton])
					event = PauseBtnInputEvent;
				else if(swipeBeginX <= buttonX[PlayButton] + 
					buttonWid[PlayButton])
					event = PlayInputEvent;
			}
			else if(gridTouchCheckY(showMenu))
				event = ReleasedGridEvent;
		}
		takeNextInputAt	= 0;
		holdingCounter	= 0;
	}
	else if((uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		currTouch = g_Input.getTouch(0);
		takeNextInputAt = (uint32)s3eTimerGetMs() + TOUCH_INTERVAL;

		if(withinBorders())
		{
			if(holdingCounter == 0)
			{
				swipeBeginX = currTouch->x;
				swipeBeginY = currTouch->y;
			}
			if(holdingCounter > 0 && gridTouchCheckY(showMenu))
			{
				event = HoldingGridEvent;
			}
			holdingCounter++;
			lastTouchX = currTouch->x;
			lastTouchY = currTouch->y;
		}
	}
	return event;
}
void Io::renderBg() const
{
	Iw2DSetColour(GREY);
	Iw2DFillRect(CIwSVec2(horBorder, 0), 
	CIwSVec2(Iw2DGetSurfaceWidth() - 2*horBorder, 
	Iw2DGetSurfaceHeight()));
	Iw2DSetColour(WHITE);
}
void Io::setUpUI(int gridColumns, int gridRows, int tileSize)
{	
	int wid = Iw2DGetSurfaceWidth();
	int hi = Iw2DGetSurfaceHeight();

	minSwipeLen	= tileSize/2;
	horBorder	= (wid - gridColumns*tileSize) / 2;
	verOffset	= hi - gridRows*tileSize;

	widthMinusBorder = wid - horBorder;
	setButtonSize(tileSize);
	setTextAreas(tileSize);
}
void Io::renderAlphaButton(Image img, Button btn) const
{
	Iw2DSetColour(0x7740C020);
	renderNoAlphaButton(img, btn);
	Iw2DSetColour(WHITE);
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
	Iw2DSetColour(PURPLE);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(menuBtnWid, menuBtnHi));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(menuBtnWid, menuBtnHi));
	Iw2DSetColour(WHITE); 

	Iw2DDrawString("Quit", CIwSVec2(qx, y), 
		CIwSVec2(menuBtnWid, menuBtnHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("Continiue", CIwSVec2(cx, y), 
		CIwSVec2(menuBtnWid, menuBtnHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(WHITE); 
}
void Io::renderTitleScren(int newX, int newY) const
{
	Iw2DSurfaceClear(0);
	Iw2DSetColour(PURPLE);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(menuBtnWid, menuBtnHi));

	Iw2DSetColour(WHITE);

	Iw2DDrawString("New game", 
		CIwSVec2(newX, newY), 
		CIwSVec2(menuBtnWid, menuBtnHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(WHITE);
}
void Io::renderGameEnded(int x, int y, int lives) const
{
	Iw2DSetColour(PURPLE);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(menuBtnWid, menuBtnHi));

	Iw2DSetColour(WHITE); 

	if(lives == 0)
		Iw2DDrawString("Game over", 
		CIwSVec2(x, y), 
		CIwSVec2(menuBtnWid, menuBtnHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	else
		Iw2DDrawString("[Insert victory message]", 
		CIwSVec2(x, y), 
		CIwSVec2(menuBtnWid, menuBtnHi), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(WHITE); 
}
void Io::renderText( const char* str, Text txt ) const
{
	Iw2DDrawString(str, CIwSVec2(textX[txt], textY), 
		CIwSVec2(textLength[txt], verOffset),
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
}
void Io::setTextColor(bool textColorOn) const
{
	Iw2DSetColour(WHITE); // TODO
}
void Io::setButtonSize(int tileSize)
{
	//	if(tileSize == 54)
	{
		buttonHi[UpgradeButton] = verOffset;
		buttonHi[SendButton] = verOffset;
		buttonHi[PauseButton] = verOffset;
		buttonHi[PlayButton] = verOffset;

		buttonHi[SellButton] = (tileSize*3) / 2;
		buttonHi[Btn1Button] = 2*verOffset;
		buttonHi[Btn2Button] = 2*verOffset;
		buttonHi[Btn3Button] = 2*verOffset;

		buttonWid[UpgradeButton] = tileSize*3;
		buttonWid[SendButton]	 = tileSize*3;
		buttonWid[PauseButton]	 = tileSize*3;
		buttonWid[PlayButton]	 = tileSize*3;

		buttonWid[SellButton] = tileSize*3;
		buttonWid[Btn1Button] = tileSize*3;
		buttonWid[Btn2Button] = tileSize*3;
		buttonWid[Btn3Button] = tileSize*3;

		menuBtnHi = tileSize*2;
		menuBtnWid = tileSize*3;
	}
	if(tileSize < 30)
	{
		buttonWid[SendButton]	 = tileSize*1;
	}
	int buttonSpacing = (Iw2DGetSurfaceHeight() - verOffset - 
		3*buttonHi[Btn1Button] - buttonHi[SellButton]) / 3;
	int verticalSpace = buttonHi[Btn1Button] + buttonSpacing;

	buttonY[PauseButton]	= 0;
	buttonY[SellButton]		= 0;
	buttonY[Btn1Button]		= verOffset;
	buttonY[Btn2Button]		= buttonY[Btn1Button] + verticalSpace;
	buttonY[Btn3Button]		= buttonY[Btn2Button] + verticalSpace;
	buttonY[PlayButton]		= 0;
	buttonY[SendButton]		= 0;
	buttonY[UpgradeButton] = 0;
	buttonY[SellButton] = Iw2DGetSurfaceHeight() - buttonHi[SellButton];

	buttonY[SellBottomButton]	= buttonY[SellButton] + buttonHi[SellButton];
	buttonY[PlayBottomButton]	= buttonY[PlayButton] + buttonHi[PlayButton];
	buttonY[PauseBottomButton]	= buttonY[PauseButton] + buttonHi[PauseButton];
	buttonY[Btn1BottomButton]	= buttonY[Btn1Button] + buttonHi[Btn1Button];
	buttonY[Btn2BottomButton]	= buttonY[Btn2Button] + buttonHi[Btn2Button];
	buttonY[Btn3BottomButton]	= buttonY[Btn3Button] + buttonHi[Btn3Button];

	buttonX[UpgradeButton] = Iw2DGetSurfaceWidth() - 
		horBorder - buttonWid[UpgradeButton];
	buttonX[SendButton] = buttonX[UpgradeButton] - buttonWid[SendButton];
	buttonX[PauseButton] = buttonX[SendButton] - buttonWid[PauseButton];
	buttonX[PlayButton] = horBorder;

	buttonX[Btn3Button] = buttonX[UpgradeButton];
	buttonX[Btn2Button] = buttonX[UpgradeButton];
	buttonX[Btn1Button] = buttonX[UpgradeButton];
	buttonX[SellButton] = buttonX[UpgradeButton];
}
bool Io::menuBtnTouch(unsigned int x, unsigned int y) const
{
	return currTouch->x > x && currTouch->x <= x + menuBtnWid
		&& currTouch->y >= y && currTouch->y < y + menuBtnHi;
}
bool Io::upgrade3Touch() const
{
	return currTouch->y < buttonY[Btn3BottomButton]
	&& currTouch->y >= buttonY[Btn3Button];
}
bool Io::upgrade1Touch() const
{
	return currTouch->y < buttonY[Btn1BottomButton]
	&& currTouch->y >= buttonY[Btn1Button];
}
bool Io::upgrade2Touch() const
{
	return currTouch->y < buttonY[Btn2BottomButton]
	&& currTouch->y >= buttonY[Btn2Button];
}
bool Io::pauseTouch() const
{
	return currTouch->x >= buttonX[PauseButton];
}
bool Io::sellTouch() const
{
	return currTouch->y >= buttonY[SellButton];
}
bool Io::gridTouch(bool showMenu) const
{
	if(showMenu)
		return currTouch->x < buttonX[UpgradeButton];
	return true;//currTouch->x < widthMinusBorder;
}
void Io::setTextAreas(int tileSize)
{
	textY = 0;

	textLength[LivesText]	= 2*tileSize;
	textLength[WaveText]	= tileSize*3;
	textLength[WallText]	= (tileSize*3)/2;
	textLength[CreditsText] = (tileSize*5) / 2;

	textX[WaveText]		= horBorder;
	textX[WallText]		= textX[WaveText] + textLength[WaveText];
	textX[LivesText]	= textX[WallText] + textLength[WallText];
	textX[CreditsText]	= textX[LivesText] + textLength[LivesText];
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
	return currTouch->x <= buttonX[PlayButton] + buttonWid[PlayButton];
}
bool Io::upgradeTouchX() const
{
	return currTouch->x >= buttonX[UpgradeButton];
}
Mode Io::manangePausedMode()
{
	int leftBtnX	= (Iw2DGetSurfaceWidth() / 2) - menuBtnWid / 2;
	int rightBtnX	= (Iw2DGetSurfaceWidth() / 2) + menuBtnWid / 2;
	int btnY		= (Iw2DGetSurfaceHeight() / 2) - menuBtnHi / 2;

	renderPaused(leftBtnX, rightBtnX, btnY);
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
			if(menuBtnTouch(leftBtnX, btnY))
			{
				return TitleMode;
			}
			else if(menuBtnTouch(rightBtnX, btnY))
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
	int newGameX = (Iw2DGetSurfaceWidth() / 2) - menuBtnWid / 2;
	int newGameY = (Iw2DGetSurfaceHeight() / 2) - menuBtnHi / 2;

	renderTitleScren(newGameX, newGameY);
	Iw2DSurfaceShow();

	if(g_Input.getTouchCount() > 0 &&
		(uint32)s3eTimerGetMs() > takeNextInputAt)
	{
		currTouch = g_Input.getTouch(0);
		if(menuBtnTouch(newGameX, newGameY))
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
	int x = (Iw2DGetSurfaceWidth() / 2) - menuBtnWid / 2,
		y = (Iw2DGetSurfaceHeight() / 2) - menuBtnHi / 2;

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
void Io::drawTile(Image img, int x, int y) const
{
	Iw2DDrawImage(
		tileImage[img],
		CIwSVec2(x, y)
		);
}
void Io::drawTile(Image img, int x, int y, int wi, int hi) const
{
	Iw2DDrawImage(
		tileImage[img],
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
	tileImage[ShowRangeImage]	= Iw2DCreateImageResource("show_range_tile");
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
	tileImage[ShowPathIamge]	= Iw2DCreateImageResource("show_path_tile");
	tileImage[SendImage]		= Iw2DCreateImageResource("send_tile");

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
void Io::initProgBars(ProgBar **damageProgBar,
					  ProgBar **speedProgBar, 
					  ProgBar **rangeProgBar,
					  int tileSize)
{
	int x = buttonX[PauseButton]- tileSize/2;
	int wid = tileSize/2;
	*damageProgBar = new ProgBar(x, 0, wid, verOffset);

	*speedProgBar = new ProgBar(x - wid, 0, wid, verOffset);

	*rangeProgBar = new ProgBar(x - 2*wid, 0, wid, verOffset);
}
void Io::renderProgressBar( ProgBar *pBar ) const
{
	Iw2DSetColour(PURPLE);
	Iw2DFillRect(CIwSVec2(pBar->getTopLeftX(), pBar->getTopLeftY()),
		CIwSVec2(pBar->getWidth(), pBar->getProgress()));
	Iw2DSetColour(WHITE);
}
void Io::renderShowRange(int x, int y, int tileSize) const
{
	Iw2DSetColour(0x664e4949);
	drawTile(ShowRangeImage, x, y, tileSize, tileSize);
	setTextColor(true);
}
void Io::renderUpgradeButton() const
{
	Iw2DSetColour(PURPLE);
	Iw2DFillRect(CIwSVec2(buttonX[UpgradeButton], buttonY[UpgradeButton]), 
		CIwSVec2(buttonWid[UpgradeButton], buttonHi[UpgradeButton]));
	Iw2DSetColour(WHITE);
	Iw2DDrawString("Upgrade", CIwSVec2(buttonX[UpgradeButton], textY), 
		CIwSVec2(buttonWid[UpgradeButton], buttonHi[UpgradeButton]),
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DSetColour(WHITE);
}
bool Io::topPanelTouch() const
{
	return currTouch->y < verOffset;
}
void Io::renderMenuBG() const
{
	Iw2DSetColour(0x77d03D50);
	Iw2DFillRect(CIwSVec2(buttonX[UpgradeButton], verOffset), 
		CIwSVec2(buttonWid[UpgradeButton], Iw2DGetSurfaceHeight() - verOffset));
	Iw2DSetColour(WHITE);	
}
void Io::renderSellBtn(bool active) const
{
	if(active)
		renderNoAlphaButton(SellImage, SellButton);
	else
		renderAlphaButton(SellImage, SellButton);
}
void Io::renderNoAlphaButton(Image img, Button btn) const
{
	drawTile(img, buttonX[btn], buttonY[btn], 
		buttonWid[btn], buttonHi[btn]);
}
void Io::renderSpawn(int x, int y, int size) const
{
	drawTile(SpawnImage, x, y, size, size);
}
void Io::renderExit(int x, int y, int size) const
{
	drawTile(ExitImage, x, y, size, size);
}

InputEvent Io::DetermineEvent( bool showMenu )
{
	InputEvent event = DoNothingInputEvent;
	if(topPanelTouch())
	{
		if(upgradeTouch())
		{
			event = MenuEvent;
		}
		else if(pauseTouch())
		{
			event = PauseBtnInputEvent;
		}
		else if(playTouch())
		{
			event = PlayInputEvent;
		}
		else
			event = ClearEvent;
	}
	else if(gridTouch(showMenu))
	{
		event = GridInputEvent;
	}
	// At this point menu must be showing and the touch is within it
	else if(upgrade1Touch())
	{
		event = Btn1Event;
	}
	else if(upgrade2Touch())
	{
		event = Btn2Event;
	}
	else if(upgrade3Touch())
	{
		event = Btn3Event;
	}
	else if(sellTouch())
	{
		event = SellInputEvent;
	}
	return event;
}

bool Io::gridTouchCheckY( bool showMenu ) const
{
	return !topPanelTouch() && gridTouch(showMenu);
}

// Returns true if swipelength in vertical direction is more than 3 times
// swipelength in horizontal direction and swipelength is atleast 10 pixels
bool Io::verticalSwipe() const
{
	return (swipeBeginX == lastTouchX ||
		((lastTouchY - swipeBeginY) / abs(lastTouchX - swipeBeginX)) >= 2) && 
		lastTouchY - swipeBeginY >= minSwipeLen;
}

bool Io::upgradeTouch() const
{
	return currTouch->x >= buttonX[UpgradeButton];
}

void Io::renderSendButton() const
{
	renderNoAlphaButton(SendImage, SendButton);
}
