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
	ChangeSpeedInputEvent,
	GridInputEvent,
	PauseBtnInputEvent,
	UndoInputEvent,
	Btn2Event,
	Btn3Event,
	Btn4Event,
	MenuEvent,
	ClearEvent
};

enum Text 
{
	CreditsText,
	WaveText,
	LivesText,
	InfoText,
	MenuText
};

enum Button {
	PlayButton,
	PauseButton,
	BuyDamageButton,
	SendButton,
	SellButton,
	PlayBottomButton,
	PauseBottomButton,
	BuyDamageBottomButton,
	QuitButton,
	ContiniueButton,
	BuySpeedButton,
	BuyRangeButton,
	BuySpeedBottomButton,
	BuyRangeBottomButton,
};

enum ButtonState
{
	ActiveButtonState,
	InactiveButtonState,
	InvisButtonState
};

const int NUM_BUTTON_POS = 20;
const int NUM_TEXT_POS = 5;

class Io
{
public:
	Io(int _tileSize);
	~Io();
	InputEvent handleInput(bool structureSelected);

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
	void renderUpgDmgButton(bool active);
	void renderUpgSpdButton(bool active);
	void renderUpgRangeButton(bool active);
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
	bool menuShowing() const;
private:
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
	unsigned int heigthMinusBorder;
	int gridHeigth;
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
	void invokeGridTouch(CTouch *touch);
	void setBorders();
	void setTextAreas();
	void setButtonSize();
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void renderGameEnded(int x, int y, int topScore) const;
	void renderAlphaButton(int color, int yIndex) const;
	bool isTouchingLargeBtn(CTouch *touch, unsigned int x, 
		unsigned int y) const;
	bool buttonTouchX(CTouch *touch) const;
	bool withinBorders(CTouch *touch) const;
	bool textAreaTouch(CTouch *touch) const;
	bool sellTouch(CTouch *touch) const;
	bool pauseTouch(CTouch *touch) const;
	bool playTouch(CTouch *touch) const;
	bool gridTouch(CTouch *touch, bool structureSelected) const;
	bool buyDamageTouch(CTouch *touch) const;
	bool buySpeedTouch(CTouch *touch) const;
	bool buyRangeTouch(CTouch *touch) const;
	bool menuToch(CTouch *touch) const;	
	bool topBarTouch(CTouch *touch) const;
};

#endif // _IO_H
