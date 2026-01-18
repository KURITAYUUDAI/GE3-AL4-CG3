#include "Game.h"

//bool IsPress(uint8_t key)
//{
//	
//}
//
//// 押した瞬間を検出する
//bool IsTrigger(uint8_t key)
//{
//
//}
//
//// 離した瞬間を検出する
//bool IsRelease(uint8_t key)
//{
//	
//}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Dict_Framework* game = new Game;

	game->Run();

	delete game;

	return 0;
}