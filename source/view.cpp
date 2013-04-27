#include <iostream>

//==============================================================================
void View::renderShots() const
{
	Iw2DSetColour(0xffffffff);
	for(std::list<TrackingShot*>::const_iterator it = game.shots.begin(); 
		it != game.shots.end(); it++)
	{
		drawTile(ShotImage, (*it)->getTopLeftX(), (*it)->getTopLeftY(), 
			(*it)->getRadius()*2, (*it)->getRadius()*2);
	}
}
//==============================================================================
void View::renderWalls() const
{
	Iw2DSetColour(0xffffffff);
	for(std::vector<Wall*>::const_iterator it = game.walls.begin(); it != game.walls.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY());
}
//==============================================================================
void View::renderNewTowers() const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF0C5907);

	for(std::deque<Tower*>::const_iterator it = game.newTowers.begin(); it != game.newTowers.end(); it++)
		drawTile((*it)->getColor(), (*it)->getTopLeftX(), (*it)->getTopLeftY(),
		game.tileSize, game.tileSize);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void View::renderMonsters() const
{
	Iw2DSetColour(0xffffffff);
	for(std::vector<Monster*>::const_iterator it = game.monsters.begin();
		it != game.monsters.end(); it++)
		if((*it)->monsterIsAlive())
			drawTile(MonsterImage, (*it)->getTopLeftX(), 
				(*it)->getTopLeftY(), game.tileSize, game.tileSize);
}
//=============================================================================
void View::renderAlphaButton(int color, int yIndex) const
{
	Iw2DSetColour(0xFFfff020);
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	drawTile(color, game.buttonX, game.buttonY[yIndex], game.buttonWid, game.buttonHi);
	Iw2DSetColour(0xffffffff);
	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//=============================================================================
void View::renderButtons() const
{
	Iw2DSetColour(0xffffffff);

	if(game.contWaves)
		drawTile(ContWavesImage, game.buttonX, game.buttonY[game.SpeedButton], game.buttonWid, game.buttonHi);
	else if(game.speedMode == game.ImmobileSpeedMode && game.mobsAlive == 0)
	{
		renderAlphaButton(SpeedImage, game.SpeedButton);
	}
	else if(game.speedMode == game.NormalSpeedMode)
		drawTile(NormalSpeedImage, game.buttonX, game.buttonY[game.SpeedButton], game.buttonWid, game.buttonHi); 
	else
		drawTile(FastSpeedImage, game.buttonX, game.buttonY[game.SpeedButton], game.buttonWid, game.buttonHi); 

	/*if(showBuildMenu)
	renderAlphaButton(BuyImage, BuyButton);
	else*/
	drawTile(BuyImage, game.buttonX, game.buttonY[game.BuyButton], game.buttonWid, game.buttonHi);

	if(!game.newTowers.empty())
	{
		renderAlphaButton(UndoImage, game.UndoButton);
	}
	else
		drawTile(UndoImage, game.buttonX, game.buttonY[game.UndoButton], game.buttonWid, game.buttonHi);

	drawTile(PauseImage, game.buttonX, game.buttonY[game.PauseButton], game.buttonWid, game.buttonHi);

	if(game.showBuildMenu)
	{
		if(game.credits >= 100)
		{
			if(Tower::dmgUncapped())
				renderAlphaButton(BuyDamageImage, game.BuyDamageButton);

			if(Tower::asUncapped())
				renderAlphaButton(BuySpeedImage, game.BuySpeedButton);

			if(Tower::rangeUncapped())
				renderAlphaButton(BuyRangeImage, game.BuyRangeButton);
		}
		else
		{
			if(Tower::dmgUncapped())
				drawTile(BuyDamageImage, game.buttonX, game.buttonY[game.BuyDamageButton], game.buttonWid, game.buttonHi);

			if(Tower::asUncapped())
				drawTile(BuySpeedImage, game.buttonX, game.buttonY[game.BuySpeedButton], game.buttonWid, game.buttonHi);

			if(Tower::rangeUncapped())
				drawTile(BuyRangeImage, game.buttonX, game.buttonY[game.BuyRangeButton], game.buttonWid, game.buttonHi);
		}
	}
	else
	{
		if(game.credits >= INCOME_PRICE)
		{
			renderAlphaButton(IncomeImage, game.IncomeButton);
		}
		else
			drawTile(IncomeImage, game.buttonX, game.buttonY[game.IncomeButton], game.buttonWid, game.buttonHi);

	}
}
//==============================================================================
void View::renderPaused(int qx, int cx, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(qx, y), 
		CIwSVec2(game.largeButtonWid, game.tileSize*2));
	Iw2DFillRect(CIwSVec2(cx, y), 
		CIwSVec2(game.largeButtonWid, game.tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	Iw2DDrawString("QUIT", CIwSVec2(qx, y), CIwSVec2(game.largeButtonWid, game.tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DDrawString("CONTINIUE", CIwSVec2(cx, y), CIwSVec2(game.largeButtonWid, game.tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void View::renderTitleScren(int newX, int newY) const
{
	Iw2DSetColour(0xFF40C020);
	Iw2DFillRect(CIwSVec2(newX, newY), 
		CIwSVec2(game.largeButtonWid, 2*game.tileSize));

	Iw2DSetColour(0xFF12AB09); 

	Iw2DDrawString("NEW GAME", CIwSVec2(newX, newY), CIwSVec2(game.largeButtonWid, game.tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
}
//==============================================================================
void View::renderGameEnded(int x, int y) const
{
	Iw2DSetColour(0xFF0C5907);
	Iw2DFillRect(CIwSVec2(x, y), 
		CIwSVec2(game.largeButtonWid*3, game.tileSize*2));

	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF12AB09); 

	char str[30];
	sprintf(str, "Your highest score was %d", game.topScore);
	Iw2DDrawString(str, CIwSVec2(x, y), CIwSVec2(game.largeButtonWid*3, game.tileSize*2), 
		IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void View::renderText() const
{
	Iw2DSetAlphaMode(IW_2D_ALPHA_ADD);
	Iw2DSetColour(0xFF40C020);//0xFF12AB09); //0xFF18860D);

	drawText(game.CreditsText, 'C', game.credits);
	drawText(game.IncomeText, 'I', game.income);
	drawText(game.WaveText, 'W', game.currWave);

	char str[9]; //TODO build string
	if(game.score > 99999)
		sprintf(str, "S %d", game.score);
	else if(game.score > 9999)
		sprintf(str, "S 0%d", game.score);
	else if(game.score > 999)
		sprintf(str, "S 00%d", game.score);
	else if(game.score > 99)
		sprintf(str, "S 000%d", game.score);
	else if(game.score > 9)
		sprintf(str, "S 0000%d", game.score);
	else
		sprintf(str, "S 00000%d", game.score);

	Iw2DDrawString(str, CIwSVec2(game.textX[game.ScoreText], game.textY), 
		CIwSVec2(game.textWid, game.textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);

	Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
}
//==============================================================================
void View::drawText(Game::Text x, char c, int text) const
{
	char str[6];
	if(text > 99)
		sprintf(str, "%c %d", c, text);
	else if(text > 9)
		sprintf(str, "%c 0%d", c, text);
	else
		sprintf(str, "%c 00%d", c, text);

	Iw2DDrawString(str, CIwSVec2(game.textX[x], game.textY), CIwSVec2(game.textWid, game.textHi), 
		IW_2D_FONT_ALIGN_LEFT, IW_2D_FONT_ALIGN_TOP);
}
//==============================================================================
