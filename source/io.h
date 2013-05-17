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
	InfoText,
	MenuText
};

enum Button {
	PlayButton,
	PauseButton,
	SendButton,
	SellButton,
	SellBottomButton,
	PlayBottomButton,
	PauseBottomButton,
	QuitButton,
	Btn1Button,
	Btn2Button,
	Btn3Button,
	Btn1BottomButton,
	Btn2BottomButton,
	Btn3BottomButton,
};

const int NUM_BUTTON_POS = 20;
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
	void drawTile(int colour, int x, int y) const;
	void drawTile(int colour, int x, int y, int wi, int hi) const;
	void reset();
	void setUpUI(int gridColumns, int gridRows, int tileSize);
	void renderBg() const;
	void renderPauseButton();
	void renderFastSpeedButton();
	void renderNormalSpeedButton();
	void renderPlayButton();
	void renderButton(bool active, Image img, Button btn);
	void renderProgressBar(ProgBar *pBar) const;
	void renderTileSelected(int x, int y, int tileSize) const;
	void renderMenuBtn() const;
	void renderSellBtn(bool active) const;
	void renderMenuBG() const;
	Mode manangePausedMode();
	Mode manageTitleMode();
	Mode manageGameEnded(int lives);
	void initProgBars(ProgBar **dmgProgressBar,
		ProgBar **asProgressBar, ProgBar **ranProgressBar);
	int getBorder() const;
	int getOffset() const;
	void setTextColor(bool textColorOn) const;
	int getLastTouchX() const;
	int getLastTouchY() const;
private:
	CTouch *currTouch;
	unsigned int takeNextInputAt;
	unsigned int buttonX;
	unsigned int buttonY[NUM_BUTTON_POS];
	unsigned int textY;
	unsigned int textX[NUM_TEXT_POS];
	unsigned int verOffset;
	unsigned int horBorder;
	unsigned int widthMinusBorder;
	int textLength[NUM_TEXT_POS];
	int holdingCounter;
	int buttonWid;
	int buttonHi;
	int lastTouchX;
	int lastTouchY;
	int swipeBeginX;
	int swipeBeginY;

	int getGridColumns();
	void cleanUpImages();
	void setUpGrapicRes(int tileSize);
	void invokeSpeedBtn();
	void invokeGridTouch();
	void setTextAreas(int tileSize);
	void setButtonSize();
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void renderGameEnded(int x, int y, int topScore) const;
	void renderAlphaButton(Image img, Button  btn) const;
	void renderNoAlphaButton(int color, int yIndex) const;
	bool isTouchingLargeBtn(unsigned int x, 
		unsigned int y) const;
	bool buttonTouchX() const;
	bool withinBorders() const;
	bool textAreaTouch() const;
	bool sellTouch() const;
	bool pauseTouch() const;
	bool playTouch() const;
	bool gridTouch(bool showMenu) const;
	bool gridTouchCheckY(bool showMenu) const;
	bool buyDamageTouch() const;
	bool buySpeedTouch() const;
	bool buyRangeTouch() const;
	bool menuToch() const;	
	bool topPanelTouch() const;
	bool horizontalSwipe() const;
};

#endif // _IO_H
