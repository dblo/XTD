#ifndef _IO_H
#define _IO_H

#include "Iw2D.h"

#include "progBar.h"
#include "resources.h"
#include "input.h"

enum Mode
{
	TitleMode,
	PlayMode,
	PausedMode,
	EndedMode
};

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
	ClearEvent
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
	PlayBottomButton,
	PauseBottomButton,
	QuitButton,
	ContiniueButton,
	Btn1Button,
	Btn2Button,
	Btn3Button,
	Btn1BottomButton,
	Btn2BottomButton,
	Btn3BottomButton,
};

enum ButtonState
{
	ActiveButtonState,
	InactiveButtonState,
	InvisButtonState
};

const int NUM_BUTTON_POS = 20;
const int NUM_TEXT_POS = 6;

class Io
{
public:
	Io(int _tileSize);
	~Io();
	InputEvent handleInput(bool showMenu);

	void renderText(const char* str, Text txt) const;
	void drawTile(int colour, int x, int y) const;
	void drawTile(int colour, int x, int y, int wi, int hi) const;
	void reset();
	void setUpUI(int &_gridColumns, int &_gridRows);
	void renderBg() const;
	void renderPauseButton();
	void renderFastSpeedButton();
	void renderNormalSpeedButton();
	void renderPlayButton();
	void renderButton(bool active, Image img, Button btn);
	void renderProgressBar(ProgBar *pBar) const;
	void renderButtonSelected(Button btn) const;
	void renderTileSelected(int x, int y) const;
	void renderMenuBtn() const;
	Mode manangePausedMode();
	Mode manageTitleMode();
	void renderMenuBG() const;
	Mode manageGameEnded(int lives);
	void initProgBars(ProgBar **roundProgressBar, ProgBar **dmgProgressBar,
		ProgBar **asProgressBar, ProgBar **ranProgressBar);
	int getBorder() const;
	int getOffset() const;
	void setTextColor(bool textColorOn);
	int getLastTouchX() const;
	int getLastTouchY() const;
private:
	CTouch *currTouch;
	bool menuOn;
	unsigned int takeNextInputAt;
	unsigned int buttonY[NUM_BUTTON_POS];
	unsigned int textY[NUM_TEXT_POS];
	unsigned int textX[NUM_TEXT_POS];
	int textLength[NUM_TEXT_POS];
	unsigned int verOffset;
	unsigned int horBorder;
	unsigned int largeButtonWid;
	unsigned int largeButtonHi;
	unsigned int buttonX;
	unsigned int tileSize;
	unsigned int widthMinusBorder;
	//unsigned int heigthMinusBorder;
	int statusBarHeigth;
	int holdingCounter;
	int buttonWid;
	int buttonHi;
	int gridColumns;
	int gridRows;
	//int textHi;
	int lastTouchX;
	int lastTouchY;

	void cleanUpImages();
	void setUpGrapicRes(int tileSize);
	void invokeSpeedBtn();
	void invokeGridTouch();
	void setTextAreas();
	void setButtonSize();
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void renderGameEnded(int x, int y, int topScore) const;
	void renderAlphaButton(int color, int yIndex) const;
	bool isTouchingLargeBtn(unsigned int x, 
		unsigned int y) const;
	bool buttonTouchX() const;
	bool withinBorders() const;
	bool textAreaTouch() const;
	bool sellTouch() const;
	bool pauseTouch() const;
	bool playTouch() const;
	bool gridTouch(bool showMenu) const;
	bool buyDamageTouch() const;
	bool buySpeedTouch() const;
	bool buyRangeTouch() const;
	bool menuToch() const;	
	bool topBarTouch() const;
};

#endif // _IO_H
