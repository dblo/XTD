#ifndef _IO_H
#define _IO_H

#include "Iw2D.h"

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
	//SpeedBtnInputEvent,
	PauseBtnInputEvent,
	UndoInputEvent,
	DmgBtnInputEvent,
	AsBtnInputEvent,
	RangeBtnInputEvent
};

enum Text 
{
	CreditsText,
	IncomeText,
	WaveText,
	LivesText
};

enum SpeedMode
{
	ImmobileSpeedMode,
	NormalSpeedMode,
	FastSpeedMode
};

enum Button {
	BuyButton,
	SpeedButton,
	PauseButton,
	BuyDamageButton,
	UndoButton,
	UndoBottomButton,
	BuyBottomButton,
	SpeedBottomButton,
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

class Io
{
public:
	Io(int _tileSize);
	~Io();
	InputEvent handleInput(CTouch **touch);

	void drawTile(int colour, int x, int y) const;
	void drawTile(int colour, int x, int y, int wi, int hi) const;
	//bool contWavesActive() const;
	void reset();
	void setUpUI(int &_gridColumns, int &_gridRows);
	void renderBg() const;
	void renderLivesText(int lives) const;
	void renderCreditsText(int credits) const;
	void renderWaveText(int wave) const;
	void renderIncomeText(int income) const;
	void renderButtons(int mobsAlive, bool newTowerBuilt, 
		ButtonState asState, ButtonState dmgState, 
		ButtonState rangeState, SpeedMode speedMode) const;
	Mode manangePausedMode();
	Mode manageTitleMode();
	Mode manageGameEnded(int lives);
	int getHorizontalBorder() const;
	int gethorizontalOffset() const;
	void setTextColor();
	int getLastTouchX() const;
	int getLastTouchY() const;
private:
	//bool contWaves;
	unsigned int takeNextInputAt;
	unsigned int buttonY[NUM_BUTTON_YPOS];
	unsigned int textY[4];	
	unsigned int horizontalOffset;
	unsigned int horizontalBorder;
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
