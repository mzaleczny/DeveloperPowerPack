#pragma once
#include "Tilc/Game.h"


class TGame : public Tilc::TGame
{
public:
	TGame(Tilc::EGameType GameType);
	virtual ~TGame();

	virtual void OnInitialize() override;

	// Returns true if game is not quiting and rest of game loop may be processed. False otherwise.
//	virtual bool HandleInput() override;
	// DeltaTime - Time of LastFrame in seconds
//	virtual void Update(float DeltaTime) override;
};
