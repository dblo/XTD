#ifndef _IO_H
#define _IO_H

#include "Iw2D.h"

#include "resources.h"
#include "input.h"
#include "rendering.h"

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
	PlaceTowerInputEvent,
	SpeedBtnInputEvent,
	PauseBtnInputEvent,
	UndoBtnInputEvent,
	DmgBtnInputEvent,
	AsBtnInputEvent,
	RangeBtnInputEvent,
	IncomeBtnInputEvent
};

enum Text 
{
	CreditsText,
	IncomeText,
	WaveText,
	ScoreText
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
	IncomeButton,
	BuyDamageButton,
	UndoButton,
	UndoBottomButton,
	BuyBottomButton,
	SpeedBottomButton,
	PauseBottomButton,
	IncomeBottomButton,
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
	bool contWavesActive() const;
	void reset();
	void setUpUI(int &_gridColumns, int &_gridRows);
	void renderBg() const;
	void renderScoreText(int score) const;
	void renderCreditsText(int credits) const;
	void renderWaveText(int wave) const;
	void renderIncomeText(int income) const;
	void renderButtons(int mobsAlive, bool newTowersIsEmpty, 
		ButtonState asState, ButtonState dmgState, 
		ButtonState rangeState, bool incomeActive, 
		SpeedMode speedMode) const;
	Mode manangePausedMode();
	Mode manageTitleMode();
	Mode manageGameEnded(int topScore);
	int getVerticalOffset() const;
	int getVerticalBorder() const;
private:
	bool contWaves;
	bool showBuildMenu;
	unsigned int takeNextInputAt;
	unsigned int buttonY[NUM_BUTTON_YPOS];
	unsigned int textX[4];	
	unsigned int verticalBorder;
	unsigned int horizontalBorder;
	unsigned int verticalOffset;
	unsigned int largeButtonWid;
	unsigned int largeButtonHi;
	unsigned int tileSize;
	int holdingPlayCounter;
	int buttonX;
	int buttonWid;
	int buttonHi;
	int gridColumns;
	int gridRows;
	int textY;
	int textWid;
	int textHi;
	void cleanUpImages();
	void setUpGrapicRes(int tileSize);
	void invokeSpeedBtn();
	void setBorders();
	void setTextAreas();
	void setButtonSize();
	void renderPaused(int qx, int cx, int y) const;
	void renderTitleScren(int newX, int newY) const;
	void renderGameEnded(int x, int y, int topScore) const;
	void renderAlphaButton(int color, int yIndex) const;
	void drawText(Text x, char c, int text) const;

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
	bool incomeTouch(CTouch *touch) const;
};

#endif // _IO_H
