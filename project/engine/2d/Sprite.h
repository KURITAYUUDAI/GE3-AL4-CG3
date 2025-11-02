#pragma once
#include "myMath.h"

class SpriteBase;

class Sprite
{
public:

	struct VertexData
	{
		
	};


public:

	void Initialize(SpriteBase* spriteBase);

	void Update();

	void Draw();

	void Finalize();

private:

	SpriteBase* spriteBase_ = nullptr;



};