#include <iostream>
#include<Windows.h>
#include "Text.h"
#include "Game.h"

using namespace std;

/*
Controls/Commands:
WASD to move
M for the menu with all the other data inside
*/

int main()
{
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;

	srand(time(NULL));

	textType(" - [Press any key to skip typing text] - \n\n\tLora: \"The Kingdom of Lothor is in shambles, Rykor, our mad and power hungry patrol leader was murdered by our own hands, and they sent a task force to take us out! How can I not be in ruin right now, Perellia!\"\n\n\tPerellia stayed silent as they moved forward, towards the hideaway dungeon the twisted sorcerers had made. Even in their demise they did all they could to spite Lothor's divine right. As they continued walking, Perellia turned to Lora.\n\n\tPerellia: \"Listen, Lora, I know I dragged you into this mess. But why do you still drag your feet when you know you're fighting for the right cause?\"\n\n\tLora: \"I can't see what's right and wrong through all this mess. I can't see any of it right now. It just doesn't make sense anymore.\"\n\n\tPerellia: \"The path ahead is not always in view. Sometimes, ashes cover the trail. But there is always a way to push on the right path. You make your way, and you do all you can to strain your neck -- \"\n\tThe forest they had been walking through cleared and they stood at a small hill embankment. There was a large hole inlaid in the side of the hill. What truly caused Perellia to stop, however, was the sun breaking over the top of the hill. It was a brilliant sight.\n\t\" -- just to see the sun, even when all you can see is...\n\n", false);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	textType("-- |", false);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
	textType(" ASHES ", false);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	textType("| --\033[00m\n\n", false);

	Sleep(500);

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
	for (int i = 0; i < 10; i++) //Randomly generated ash falling effect
	{
		for (int j = 0; j < consoleInfo.dwMaximumWindowSize.X; j++) //In 10 iterations, randomly create a line of the visual effect from 3 selected ascii characters and then print out that randomly generated line
		{
			for (int l = 0; l < (rand() % 10) + 1; l++)
			{
				cout << " ";
			}

			if (j % 2 == 0)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			}

			cout << (char)((rand() % 2) + 248);
		}

		Sleep((14.8184 * i * i) - (133.333 * i) + 300); //Make the Sleep timer within the interval [0,9] a parabola that has a max of 300 and a zero at x=4.5
		//The equation for the parabola is f(x)=14.8148x^2-133.333x+300
	}

	vector<Room*> gameMap = {
	&zero0, &entrance, &two0, &entrance,
	&entrance, &one1, &two1, &three1,
	&entrance, &one2, &two2, &three2};
	
	vector<Item> lockItems = {key, key, longSword, spear, messer, key, shortSword}; //Loot table for locked chests
	vector<Item> unlockItems = {key, key, key, shortSword, healingPotion, healingPotion, key}; //Loot table for unlocked chests
	vector<Non_Player_Character> enemies = { oldBat, oldBat, crazedLooter, fireElemental, rockElemental, rockElemental }; //Enemy table for randomized spawns

	bool go = encounter(entrance, player, gameMap, lockItems, unlockItems, enemies);



	return 0; //On structs, remember bool operator functions that allow for == and other operands on structs
}