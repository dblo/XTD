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
	DmgBtnInputEvent,
	AsBtnInputEvent,
	RangeBtnInputEvent
};

enum Text 
{
	CreditsText,
	WaveText,
	LivesText,
	WallsText,
	PriceText
};

enum Button {
	PlayButton,
	PauseButton,
	BuyDamageButton,
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

const int NUM_BUTTON_YPOS = 18;
const int NUM_TEXT_YPOS = 5;

class Io
{
public:
	Io(int _tileSize);
	~Io();
	InputEvent handleInput();

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
	Mode manangePausedMode();
	Mode manageTitleMode();
	Mode manageGameEnded(int lives);
	void initProgBars(ProgBar **roundProgressBar, ProgBar **dmgProgressBar,
		ProgBar **asProgressBar, ProgBar **ranProgressBar);
	int getBorder() const;
	int getOffset() const;
	void setTextColor(bool textColorOn);
	int getLastTouchX() const;
	int getLastTouchY() const;
private:
	unsigned int takeNextInputAt;
	unsigned int buttonY[NUM_BUTTON_YPOS];
	unsigned int textY[NUM_TEXT_YPOS];
	unsigned int gridOffset;
	unsigned int border;
	unsigned int largeButtonWid;
	unsigned int largeButtonHi;
	unsigned int tileSize;
	int holdingCounter;
	int buttonX;
	int buttonWid;
	int buttonHi;
	int gridColumns;
	int gridRows;
	int textAreaWid;
	int textAreaHi;
	int textHi;
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
	bool validTouch(CTouch *touch) const;
	bool damageTouch(CTouch *touch) const;
	bool undoTouch(CTouch *touch) const;
	bool pauseTouch(CTouch *touch) const;
	bool speedTouch(CTouch *touch) const;
	bool buyTouch(CTouch *touch) const;
	bool gridTouch(CTouch *touch) const;
	bool buySpeedTouch(CTouch *touch) const;
	bool buyRangeTouch(CTouch *touch) const;
	
};

#endif // _IO_H
