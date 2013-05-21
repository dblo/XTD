#ifndef _IO_H
#define _IO_H

#include "progBar.h"
#include "resources.h"
#include "input.h"

enum InputEvent
{
	DoNothingInputEvent,
	PlayInputEvent,
	GridInputEvent,
	PauseBtnInputEvent,
	SellInputEvent,
	Btn1Event,
	Btn2Event,
	Btn3Event,
	MenuEvent,
	ClearEvent,
	HoldingGridEvent,
	ReleasedGridEvent
};

enum Text 
{
	CreditsText,
	WaveText,
	WallText,
	LivesText,
};

enum Button {
	PlayButton,
	PauseButton,
	SendButton,
	SellButton,
	SellBottomButton,
	PlayBottomButton,
	PauseBottomButton,
	UpgradeButton,
	QuitButton,
	Btn1Button,
	Btn2Button,
	Btn3Button,
	Btn1BottomButton,
	Btn2BottomButton,
	Btn3BottomButton,
};

const int NUM_BUTTON_POS = 17;
const int NUM_TEXT_POS = 6;

class Io
{
public:
	Io(int tileSize);
	~Io();
	InputEvent handleInput(bool showMenu);

	InputEvent DetermineEvent( bool showMenu );

	void renderSpawn(int x, int y, int size) const;
	void renderExit(int x, int y, int size) const;
	void renderText(const char* str, Text txt) const;
	void drawTile(Image img, int x, int y) const;
	void drawTile(Image img, int x, int y, int wi, int hi) const;
	void reset();
	void setUpUI(int gridColumns, int gridRows, int tileSize);
	void renderBg() const;
	void renderPauseButton();
	void renderFastSpeedButton();
	void renderNormalSpeedButton();
	void renderPlayButton();
	void renderButton(bool active, Image img, Button btn);
	void renderProgressBar(ProgBar *pBar) const;
	void renderShowRange(int x, int y, int tileSize) const;
	void renderSendButton() const;
	void renderUpgradeButton() const;
	void renderSellBtn(bool active) const;
	void renderMenuBG() const;
	Mode manangePausedMode();
	Mode manageTitleMode();
	Mode manageGameEnded(int lives);
	void initProgBars(ProgBar **dmgProgressBar,
		ProgBar **asProgressBar, ProgBar **ranProgressBar,
		int tileSize);
	int getBorder() const;
	int getOffset() const;
	void setTextColor(bool textColorOn) const;
	int getLastTouchX() const;
	int getLastTouchY() const;
private:
	CTouch *currTouch;
	unsigned int takeNextInputAt;
	unsigned int buttonX[NUM_BUTTON_POS];
	unsigned int buttonY[NUM_BUTTON_POS];
	unsigned int textY;
	unsigned int textX[NUM_TEXT_POS];
	unsigned int verOffset;
	unsigned int horBorder;
	unsigned int widthMinusBorder;
	int textLength[NUM_TEXT_POS];
	int menuBtnWid;
	int menuBtnHi;
	int holdingCounter;
	int buttonWid[NUM_BUTTON_POS];
	int buttonHi[NUM_BUTTON_POS];
	unsigned int minSwipeLen;
	unsigned int lastTouchX;
	unsigned int lastTouchY;
	unsigned int swipeBeginX;
	unsigned int swipeBeginY;
	
	int getGridColumns();
	void cleanUpImages();
	void setUpGrapicRes(int tileSize);
	void invokeSpeedBtn();
	void invokeGridTouch();
	void setTextAreas(int tileSize);
	void setButtonSize(int tileSize);
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void renderGameEnded(int x, int y, int topScore) const;
	void renderAlphaButton(Image img, Button btn) const;
	void renderNoAlphaButton(Image img, Button btn) const;
	bool menuBtnTouch(unsigned int x, 
		unsigned int y) const;
	bool upgradeTouchX() const;
	bool withinBorders() const;
	bool sellTouch() const;
	bool pauseTouch() const;
	bool playTouch() const;
	bool gridTouch(bool showMenu) const;
	bool gridTouchCheckY(bool showMenu) const;
	bool upgrade1Touch() const;
	bool upgrade2Touch() const;
	bool upgrade3Touch() const;
	bool menuToch() const;	
	bool topPanelTouch() const;
	bool verticalSwipe() const;
	bool upgradeTouch() const;
};

#endif // _IO_H
