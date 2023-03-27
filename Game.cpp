#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include "Game.h"
#include "Text.h"

using namespace std;

bool combat(Non_Player_Character enemy1, Character& player, Room& room)
{
	bool success = false;
	string itemDisplay;
	int pChoice, wDamage;

	clearScreen();
	textType("You start to fight " + enemy1.name + "!\n", false);

	Sleep(600);

	while (player.health > 0 || !success)
	{
		if (enemy1.health <= 0)
		{
			success = true;
			break;
		}
		
		if (player.health < 0)
		{
			success = false;
			break;
		}

		cout << "\n--------------------" << endl;
		textType("Player- HP: " + to_string(player.health), false);
		textType("\n\nvs.\n\n" + enemy1.name + "- HP: " + to_string(enemy1.health), false);
		cout << "\n--------------------" << endl;

		textType("\nWhat item would you like to use (answer only with the number of the inventory item, 1 - last item number)?\n", false);

		textType("\n\033[4mInventory:\033[0m ", false);

		for (Item iItem : player.inventory)
		{
			itemDisplay = "\033[03m" + iItem.name + "\033[00m - DMG: " + to_string(iItem.dmg) + " - Heal: " + to_string(iItem.heal) + " - Evasion: " + to_string(iItem.dex);

			textType(" | " + itemDisplay, false);
		}

		cout << "\n\n";

		while (true)
		{
			pChoice = setInput(1, player.inventory.size());

			if (player.inventory[pChoice - 1].heal > 0)
			{
				textType("You healed yourself with your potion.\n", false);
				player.health += player.inventory[pChoice - 1].heal;
				player.inventory.erase(next(player.inventory.begin(), pChoice - 1));
				wDamage = -1;
				break;
			}
			else if (player.inventory[pChoice - 1].dmg > 0)
			{
				wDamage = (rand() % 5) + player.inventory[pChoice - 1].dmg - 1;
				enemy1.health -= wDamage;

				textType("You hit " + enemy1.name + " for " + to_string(wDamage) + " damage!\n", false);
				break;
			}
			else
			{
				textType("That is not a valid item option for combat. Try again.\n", false);
			}
		}

		if(wDamage != -1) //If the player chose to heal, the enemy doesn't get a chance to strike
		{
			if (((rand() % 100) + player.inventory[pChoice - 1].dex) >= ((rand() % 100) + enemy1.accuracy) && (enemy1.health >= 0))
			{
				textType("You dodged the " + enemy1.name + "'s attack!\n", false);
			}
			else if (enemy1.health > 0)
			{
				wDamage = (rand() % 5) + enemy1.dmg - 2;
				player.health -= wDamage;

				textType("The " + enemy1.name + " hit you for " + to_string(wDamage) + "!\n", false);
			}
		}

		Sleep(1000);
		clearScreen();
	}

	if (success)
	{
		textType("\n\nYou defeated the " + enemy1.name + "!", false);
	}
	else
	{
		textType("\n\nYou died!", true);
	}

	if(room.pos[0] + room.pos[1] != 0)
	{
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);

		COORD holdCoords;
		short playerStatsBoxY;

		Sleep(1500);
		clearScreen();

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); //Set the dungeon to red color, and changes to white as you move around
		room.display(room.map);

		textType(room.description, false);

		cout << "\033[00m\n\n\033[04mKey:\033[00m\t\tUse WASD to move"; //Output the key, but broken up to the have the player data output before the rest of the key
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
		holdCoords = consoleInfo.dwCursorPosition;
		playerStatsBoxY = holdCoords.Y;

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { holdCoords });
		cout << "\n\nWall: " << (char)205
			<< "\nSpace: ."
			<< "\nFilled Space:"
			<< "\n - Key: " << (char)157 << "\t\t- Enemy: !"
			<< "\n - Item: i" << "\t\t- Player: P"
			<< "\n - Chest:" << "\t\t- Door:"
			<< "\n   - Locked: " << (char)37 << "\t\t - Locked: " << (char)216
			<< "\n   - Unlocked: " << (char)67 << "\t - Unlocked: " << (char)197 << endl;

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { player.pos[0], player.pos[1] });
		cout << 'P';
	}

	return success;
}

bool encounter(Room& room, Character& player, vector<Room*> gameMap, vector<Item> &lockItems, vector<Item> &unlockItems, vector<Non_Player_Character> &enemies, Item chestLoot)
{
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);

	COORD holdCoords;
	Room *nextRoomPtr;
	bool go = true;
	bool squareChange = true;
	bool hasKey = false;
	char move;
	int spaceId, enemyIndex;
	short errorPosX, errorPosY, playerStatsBoxY;

	srand(time(NULL));

	clearScreen();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); //Set the dungeon to red color, and changes to white as you move around
	room.display(room.map);

	textType(room.description, false);

	cout << "\033[00m\n\n\033[04mKey:\033[00m\t\tUse WASD to move"; //Output the key, but broken up to the have the player data output before the rest of the key
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
	holdCoords = consoleInfo.dwCursorPosition;
	playerStatsBoxY = holdCoords.Y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { holdCoords });
	cout << "\n\nWall: " << (char)205
		 << "\nSpace: ."
		 << "\nFilled Space:"
		 << "\n - Key: " << (char)157 << "\t\t- Enemy: !"
		 << "\n - Item: i" << "\t\t- Player: P"
		 << "\n - Chest:" << "\t\t- Door:"
		 << "\n   - Locked: " << (char)37 << "\t\t - Locked: " << (char)216
		 << "\n   - Unlocked: " << (char)67 << "\t - Unlocked: " << (char)197 << endl;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { player.pos[0], player.pos[1] });
	cout << 'P';

	while (go) //Loop the encounter
	{
		int iHold1 = -1;
		int iHold2 = -1;
		string invDisplay, invDisplay2;

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwMaximumWindowSize.X / 2), playerStatsBoxY }); //Out put player data in its correct position, making sure to not destroy the console unless someone were to put it too thin...
		cout << "\033[04mPlayer:\033[00m";
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwMaximumWindowSize.X / 2), (short)(playerStatsBoxY + 1) });
		cout << " - Health: " << player.health;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwMaximumWindowSize.X / 2), (short)(playerStatsBoxY + 2) });
		cout << " - Items: "; //To be replaced with a menu option
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwMaximumWindowSize.X / 2), (short)(playerStatsBoxY + 3) });
		for (Item item : player.inventory)
		{
			if (invDisplay.size() > (consoleInfo.dwMaximumWindowSize.X / 2) - 16)
			{
				invDisplay2 += " - " + item.name;
			}
			else
			{
				invDisplay += " - " + item.name;
			}
		}
		cout << invDisplay;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwMaximumWindowSize.X / 2), (short)(playerStatsBoxY + 4) });
		cout << invDisplay2;

		for (int i = 0; i < room.map.size(); i++) //Search the room's map for an exclamation mark (enemy), and move the enemy closer to the player
		{
			if (room.map[i] == 33 && i != iHold1 && i != iHold2) //Once an enemy has been found, move it towards the player
			{
				int direction;
				int wallSwitch = -1;

				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(i % room.getRowLength(room.map)), (short)(i / room.getRowLength(room.map)) }); //Set the console cursor to the found exclamation mark
				cout << ".";

				if (((i / room.getRowLength(room.map)) == player.pos[1]) && ((i % room.getRowLength(room.map)) == player.pos[0])) //If the enemy catches the player
				{
					if (room.pos[0] + room.pos[1] == 0) //If it is the boss room, initiate combat with the boss
					{
						go = combat(stoneBoss, player, room);

						if (go)
						{
							clearScreen();

							textType("You stand over the shattered stone giant as the ground begins to shake, you turn as if to leave, but the ceiling caves in suddenly at your exit. You start to freak out as the ground stops shaking. You're trapped! Turning around, where the stone golem had once been standing, there was now a giant crevice. You walk over and look in, and see a horror before your eyes. This was not the dungeon's heart, or the only heart at least. You look down and see a giant city of rock and stone, and similar golems to the one you just killed milling about the city. An underground cliff seemingly extended to very middle of the Earth. It appears this isn't over yet... Not until the ashes have all fallen.\n\n - [Press any key to end] - ", false);

							_getch();
						}
						else
						{
							clearScreen();

							textType("You fall over dead, in a pool of your own blood. As you feel yourself fading, you whisper as if to no one, \"There is still hope.\" The darkness takes you.\n\n - [Press any key to end] -", false);

							_getch();
						}

						go = false;
					}
					else //If it is not the boss room, choose a random enemy and initiate combat with it
					{
						enemyIndex = rand() % (enemies.size() - 1);

						go = combat(enemies[enemyIndex], player, room);

						enemies.erase(next(enemies.begin(), enemyIndex));
					}

					direction = 2;
				}
				else if ((i % room.getRowLength(room.map)) == player.pos[0])
				{
					direction = 0;
				}
				else if ((i / room.getRowLength(room.map)) == player.pos[1])
				{
					direction = 1;
				}
				else
				{
					direction = rand() % 1; //Pick x (0) or y (1) direction to move the enemy if the enemy is not already at the same x or y as the player
				}

				GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
				if (direction == 1) //Move the enemy closer upon the x line
				{
					if (player.pos[0] < (consoleInfo.dwCursorPosition.X - 1) && room.map[i - 1] != 46) //Switch the direction if there is an object in the left direction when the enemy intends to go left
					{
						wallSwitch = 0;
					}
					else if (player.pos[0] < (consoleInfo.dwCursorPosition.X - 1))
					{
						wallSwitch = 1;
					}
					
					if (player.pos[0] > (consoleInfo.dwCursorPosition.X - 1) && room.map[i + 1] != 46) //Switch the direction if there is an object in the right direction when the enemy intends to go right
					{
						wallSwitch = 1;
					}
					else if (player.pos[0] > (consoleInfo.dwCursorPosition.X - 1))
					{
						wallSwitch = 0;
					}

					if (wallSwitch != 0) //If the enemy is to the left of the player, move it left
					{
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwCursorPosition.X - 2), consoleInfo.dwCursorPosition.Y });
						cout << "!";
						room.map[i - 1] = 33;
					}
					else if (wallSwitch != 1) //If the enemy is to the right of the player, move it right
					{
						cout << "!";
						room.map[i + 1] = 33;

						if (iHold1 != -1)
						{
							iHold2 = i + 1;
						}
						else
						{
							iHold1 = i + 1;
						}
					}
				}
				else if (direction == 0) //Move the enemy closer upon the y line
				{
					if (player.pos[1] < consoleInfo.dwCursorPosition.Y && room.map[i - room.getRowLength(room.map)] != 46) //Switch the direction if there is an object in the up direction where the enemy intends to go up
					{
						wallSwitch = 1;
					}
					else if(player.pos[1] < consoleInfo.dwCursorPosition.Y)
					{
						wallSwitch = 0;
					}

					if (player.pos[1] > consoleInfo.dwCursorPosition.Y && room.map[i + room.getRowLength(room.map)] != 46) //Switch the direction if there is an object in the down direction where the enemy intends to go down
					{
						wallSwitch = 0;
					}
					else if (player.pos[1] > consoleInfo.dwCursorPosition.Y)
					{
						wallSwitch = 1;
					}

					if (wallSwitch != 1) //If the enemy is the below the player, move it up
					{
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwCursorPosition.X - 1), (short)(consoleInfo.dwCursorPosition.Y - 1) });
						cout << "!";
						room.map[i - room.getRowLength(room.map)] = 33;
					}
					else if (wallSwitch != 0) //If the enemy is above the player, move it down
					{
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(consoleInfo.dwCursorPosition.X - 1), (short)(consoleInfo.dwCursorPosition.Y + 1) });
						cout << "!";
						room.map[i + room.getRowLength(room.map)] = 33;

						if (iHold1 != -1)
						{
							iHold2 = i + room.getRowLength(room.map);
						}
						else
						{
							iHold1 = i + room.getRowLength(room.map);
						}
					}
				}

				room.map[i] = 46;
			}
		}

		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)(player.pos[0] + 1), player.pos[1] });

		move = _getch(); //Get player input, desired wasd

		switch (move)
		{
			case 119: //w
				player.pos[1]--;
				break;

			case 97: //a
				player.pos[0]--;
				break;

			case 115: //s
				player.pos[1]++;
				break;

			case 100: //d
				player.pos[0]++;
				break;
		}

		spaceId = room.map[(room.getRowLength(room.map) * player.pos[1]) + player.pos[0]];
		
		if (spaceId == 37 || spaceId == 216) //If the space is a locked door or chest
		{
			int index = 0;

			for (Item item : player.inventory) //Search the player's inventory for a key
			{
				if (item.name == "Key")
				{
					hasKey = true;

					player.inventory.erase(next(player.inventory.begin(), index));

					break;
				}

				index++;
			}

			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
			holdCoords = consoleInfo.dwCursorPosition; //Save the current cursor location to be set to once the statement has been completed

			if(!hasKey)
			{
				errorPosY = (room.map.size() / room.getRowLength(room.map)) + (room.description.size() / consoleInfo.dwMaximumWindowSize.X) + 1; //Set the error message y coord to be past the varying sizes of the Y coords of the map and description
				errorPosX = consoleInfo.dwMaximumWindowSize.X / 2; //Set the error message x coord to half of the window size

				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { errorPosX, errorPosY });

				switch (spaceId) //Output error message according to object type
				{
					case 216: //Door
						textType("The door is locked.", false);
						break;

					case 37: //Chest
						textType("The chest is locked.", false);
						break;
				}
			}
			else
			{
				switch (spaceId) //Dependent upon what's being unlocked, switch its character to its unlocked state
				{
					case 216: //Door
						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { player.pos[0], player.pos[1] }); //Put cursor over door
						cout << (char)197;

						GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
						room.map[(room.getRowLength(room.map) * consoleInfo.dwCursorPosition.Y) + consoleInfo.dwCursorPosition.X - 1] = 197; //Save the state of the door so it remains that way

						break;

					case 37: //Chest
						int randIndex;

						SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { player.pos[0], player.pos[1] }); //Put the cursor over the chest
						cout << (char)67;

						GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
						room.map[(room.getRowLength(room.map) * consoleInfo.dwCursorPosition.Y) + consoleInfo.dwCursorPosition.X - 1] = 67; //Save the state of the chest so it remains that way

						if(lockItems.size() > 2)
						{
							randIndex = rand() % (lockItems.size() - 1);
							chestLoot = lockItems[randIndex]; //Randomly choose an item from the locked chest loot table for the next opened chest to drop
							lockItems.erase(next(lockItems.begin(), randIndex)); //Erase the item from the loot table
						}
						else
						{
							chestLoot = lockItems[0];
						}

						hasKey = false;

						break;
				}
			}

			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), holdCoords); //Put the cursor back to its original location
		}
		else if (spaceId == 197) //If the player passes into an unlocked door
		{
			if (player.pos[1] == 0) //Entering a door going north
			{
				nextRoomPtr = gameMap[((room.pos[1] + 1) * 4) + room.pos[0]];

				player.pos[1] = (nextRoomPtr->map.size() / nextRoomPtr->getRowLength(nextRoomPtr->map)) - 2;

				if (hasKey) //If the door became unlocked, switch the door in the other room to the unlocked state as well
				{
					nextRoomPtr->map[nextRoomPtr->map.size() - (nextRoomPtr->getRowLength(nextRoomPtr->map) - player.pos[0])] = 197;
					hasKey = false;
				}

				go = encounter(*nextRoomPtr, player, gameMap, lockItems, unlockItems, enemies, chestLoot);
			}
			else if (player.pos[0] == 0) //Entering a door going west
			{
				nextRoomPtr = gameMap[(room.pos[0] - 1) + (room.pos[1] * 4)];

				player.pos[0] = nextRoomPtr->getRowLength(nextRoomPtr->map) - 3;

				if (hasKey) //If the door became unlocked, switch the door in the other room to the unlocked state as well
				{
					nextRoomPtr->map[(nextRoomPtr->getRowLength(nextRoomPtr->map) * player.pos[1]) + player.pos[0] + 1] = 197;
					hasKey = false;
				}

				go = encounter(*nextRoomPtr, player, gameMap, lockItems, unlockItems, enemies, chestLoot);
			}
			else if ((player.pos[1] != (room.map.size() / room.getRowLength(room.map))) && (player.pos[0] == (room.getRowLength(room.map) - 2))) //Entering a door going east
			{
				player.pos[0] = 1;

				nextRoomPtr = gameMap[(room.pos[0] + 1) + (room.pos[1] * 4)];

				if (hasKey) //If the door became unlocked, switch the door in the other room to the unlocked state as well
				{
					nextRoomPtr->map[(nextRoomPtr->getRowLength(nextRoomPtr->map) * player.pos[1]) + player.pos[0] - 1] = 197;
					hasKey = false;
				}

				go = encounter(*nextRoomPtr, player, gameMap, lockItems, unlockItems, enemies, chestLoot);
			}
			else if ((player.pos[1] != 0) && (player.pos[0] != (room.getRowLength(room.map) - 2))) //Entering a door going south
			{
				player.pos[1] = 1;

				nextRoomPtr = gameMap[((room.pos[1] - 1) * 4) + room.pos[0]];

				if (hasKey) //If the door became unlocked, switch the door in the other room to the unlocked state as well
				{
					nextRoomPtr->map[(nextRoomPtr->getRowLength(nextRoomPtr->map) * (player.pos[1] - 1)) + player.pos[0]] = 197;
					hasKey = false;
				}

				go = encounter(*nextRoomPtr, player, gameMap, lockItems, unlockItems, enemies, chestLoot);
			}
		}
		else if (spaceId == 67) //if the player passes into a chest
		{
			//set the chest to a period and give the player the contents

			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
			room.map[(room.getRowLength(room.map) * player.pos[1]) + player.pos[0]] = 46; //Set the room's map to correctly reflect the displayed map by using the arithmetic from the cursor position to find it's index in the map vector

			if (chestLoot.name == "null") //If no item has not been predetermined (aka not a locked chest), then take from the Unlocked Chest loot table
			{
				int randIndex;

				if(unlockItems.size() > 1)
				{
					randIndex = rand() % (unlockItems.size() - 1);
					chestLoot = unlockItems[randIndex];
					unlockItems.erase(next(unlockItems.begin(), randIndex));
				}
				else
				{
					player.inventory.push_back(unlockItems[0]);
				}

				player.inventory.push_back(chestLoot);
			}
			else //If an item has been predetermined, give it to the player and take another from the locked chest loot table
			{
				int randIndex;

				if(lockItems.size() > 2)
				{
					player.inventory.push_back(chestLoot);

					randIndex = rand() % (lockItems.size() - 1);
					chestLoot = lockItems[randIndex];
					lockItems.erase(next(lockItems.begin(), randIndex));

					player.inventory.push_back(chestLoot);
				}
				else
				{
					player.inventory.push_back(lockItems[0]);
					player.inventory.push_back(lockItems[1]);
				}
			}

			chestLoot = { "null" };

			squareChange = true;
		}
		else
		{
			squareChange = true;
		}

		if ((spaceId >= 185 && spaceId <= 188) || (spaceId >= 200 && spaceId <= 205) || (spaceId == 216) || (spaceId == 37)) //If the space is any of the objects the player can't move into
		{
			switch (move) //Reset Player positioning
			{
			case 119: //w
				player.pos[1]++;
				break;

			case 97: //a
				player.pos[0]++;
				break;

			case 115: //s
				player.pos[1]--;
				break;

			case 100: //d
				player.pos[0]--;
				break;
			}
		}

		if (squareChange)
		{
			cout << "\b.";
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { player.pos[0], player.pos[1] });
			cout << 'P';

			squareChange = false;
		}
	}

	clearScreen();

	return go;
}

Room entrance = { {1, 0}, {
201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187, 10,
186,  67,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
200, 205, 187,  46,  46,  46,  46,  46,  46,  46, 186, 10,
201, 205, 188,  46,  46,  46,  46,  46,  46,  46, 186, 10,
216,  46,  46,  46,  46,  46,  46,  46,  46,  46, 197, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188, 10},
"Lora stands outside, waiting for you to complete the job. In order for you and Lora to be finished here, you have to destroy the dungeon's heart, a stone the sorcerers made to keep their evil creations going. This room is simple, only containing a little storage area and a couple doors. The wooden floors are solid as you walk across it, admiring the clay walls. Somehow, they keep this whole dungeon from caving in on itself."};
Room zero0 = { {0, 0}, {
201, 205, 205, 187,  32,  32,  32,  32, 201, 205, 205, 205, 187, 10,
186,  46,  46, 186,  32,  32,  32,  32, 186,  46,  46,  46, 186, 10,
186,  46,  46, 186,  32,  32,  32,  32, 186,  46,  46,  46, 186, 10,
186,  46,  46, 186,  32,  32,  32,  32, 186,  46,  46,  46, 186, 10,
186,  46,  46, 200, 205, 205, 205, 205, 188,  46,  46,  46, 216, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  33,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46, 201, 205, 205, 205, 205, 187,  46,  46,  46, 186, 10,
186,  46,  46, 186,  32,  32,  32,  32, 186,  46,  46,  46, 186, 10,
186,  46,  46, 186,  32,  32,  32,  32, 186,  46,  46,  46, 186, 10,
186,  46,  46, 186,  32,  32,  32,  32, 186,  46,  46,  46, 186, 10,
200, 205, 205, 188,  32,  32,  32,  32, 200, 205, 205, 205, 188, 10},
"You enter an ornate room, with murals of flames lining the wall. Straight ahead, you see the stone you were looking for. Yet, as you look closer, it moves. You soon realize it is living!"};
Room two0 = { {2, 0}, {
201, 205, 205, 205, 205, 197, 205, 205, 205, 205, 187, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
197,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  37,  46,  46,  46,  46, 186, 10,
200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188, 10},
"This room shares very similar design with the entrance to this dungeon. It is fairly drab. There is a door at the far end of the room you can try."};
Room two1 = { {2, 1}, {
201, 205, 205, 187,  32,  32,  32,  32, 10,
197,  46,  46, 186,  32,  32,  32,  32, 10,
186,  46,  46, 186,  32,  32,  32,  32, 10,
186,  46,  46, 186,  32,  32,  32,  32, 10,
186,  46,  46, 200, 205, 205, 205, 187, 10,
186,  46,  46,  46,  46,  33,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 197, 10,
186,  46,  46, 201, 203, 205, 205, 185, 10,
186,  46,  46, 186, 186,  46,  67, 186, 10,
186,  46,  46, 200, 188,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
200, 205, 205, 205, 205, 197, 205, 188, 10},
"It is dark in here, as there are only dim blue candles to light this room. A couple upturned tables look stored away for later. It almost looks like it used to house a meeting place of sorts. Thought, this is an odd place to hold a meeting..." };
Room one1 = { {1, 1}, {
	201, 216, 205, 187,  32, 201, 205, 205, 187, 10,
	186,  46,  46, 186,  32, 186,  46,  46, 197, 10,
	186,  46,  46, 186,  32, 186,  46,  46, 186, 10,
	186,  33,  46, 200, 205, 188,  46,  46, 186, 10,
	186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
	186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
	186,  46,  46,  67,  46,  46,  46,  46, 186, 10,
	186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
	200, 205, 205, 205, 205, 205, 205, 205, 188, 10},
"It is very cold in here. The same blue candles that lighted the other rooms light this room, too. Paintings line the walls of high ranking sorcerers in their military. If this was their headquarters, you may be in over your head." };
Room one2 = { {1, 2}, {
201, 205, 205, 187, 201, 205, 205, 187, 10,
186,  46,  33, 186, 186,  46,  46, 197, 10,
186,  46,  46, 186, 186,  46,  46, 186, 10,
186,  46,  46, 186, 200, 187,  46, 186, 10,
186,  46,  46, 200, 205, 188,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  67, 186, 10,
186,  46,  46, 201, 205, 205, 205, 188, 10,
186,  46,  46, 186,  32,  32,  32,  32, 10,
186,  46,  46, 186,  32,  32,  32,  32, 10,
186,  46,  46, 186,  32,  32,  32,  32, 10,
200, 216, 205, 188,  32,  32,  32,  32, 10},
"Hammocks hang from the ceiling. This must've been the troop quarters. It is so small, though, are there other places they stayed? Maybe they left some supplies behind you could scavenge." };
Room three1 = { {3, 1}, {
 32, 201, 205, 205, 216, 205, 205, 205, 205, 205, 205, 205, 187, 10,
 32, 186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
 32, 186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
 32, 186,  46,  46, 201, 205, 203, 205, 205, 187,  46,  46, 186, 10,
 32, 186,  46,  46, 186,  32, 186,  67,  46, 186,  46,  46, 186, 10,
 32, 186,  46,  46, 186,  32, 186,  46,  46, 186,  46,  46, 186, 10,
 32, 186,  46,  46, 200, 205, 188,  46,  46, 186,  46,  46, 186, 10,
 32, 186,  46,  46,  46,  46,  46,  46,  46, 186,  46,  46, 186, 10,
201, 202, 187,  46,  46,  46,  46,  46,  33, 186,  46,  46, 186, 10,
197,  46, 200, 205, 205, 205, 205, 205, 205, 188,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188, 10},
"As soon as you enter this room, it starts to make you feel sick. The room seems pointless, the only thing decorating the place are oddly colored candles that line the walls. Somehow they seem to be the cause of your nausea..."};
Room two2 = { {2, 2}, {
201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187, 10,
197,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 197, 10,
186,  46,  46,  37,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188, 10},
"You enter a simple hallway that holds a very ornate chandelier. For such boring clay walls, they somehow enjoyed extravagant decor. Other ostentatious decorations line the walls. There are mirrors and plates of gold. Even the chest in this room glitters in the candle light." };
Room three2 = {{3, 2}, {
201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187, 10,
197,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  33,  46,  46, 186, 10,
186,  46,  46,  46,  33,  46,  37,  46, 201, 187,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46, 186, 186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46, 186, 186,  46,  67,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46, 186, 186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46, 201, 205, 205, 205, 188, 200, 205, 205, 205, 187,  46,  46,  46, 186, 10,
186,  46,  46,  46, 200, 205, 205, 205, 187, 201, 205, 205, 205, 188,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46, 186, 186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46, 186, 186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46, 186, 186,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46, 200, 188,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
186,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46,  46, 186, 10,
200, 205, 205, 205, 216, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188, 10},
"This used to be the barracks. The only well lit room so far, you can see scorches training dummies in the corners and against the walls. Broken and training weapons sit atop racks. This place has definitely seen trainers, as they are numerous imperfections in the walls and floors. Not to mention, an entire section of the wood is peeled back from some previous fire that got out of control."};

Item key = { "Key" };
Item shortSword = { "Short Sword", 8, 0, 55 };
Item longSword = { "Long Sword", 16, 0, 60 };
Item spear = { "Spear", 15, 0, 80 };
Item dagger = { "Dagger", 3, 0, 50 };
Item messer = { "Messer", 20, 0, 30 };
Item healingPotion = { "Healing Potion", 0, 20 };

Character player = { 100, {dagger} };

Non_Player_Character oldBat = { 12, 50, 4, "Old Bat" };
Non_Player_Character crazedLooter = { 38, 47, 8, "Crazed Looter" };
Non_Player_Character fireElemental = { 63, 77, 13, "Fire Elemental" };
Non_Player_Character rockElemental = { 125, 34, 4, "Rock Elemental" };
Non_Player_Character stoneBoss = { 180, 89, 18, "Dungeon's Heart Piece" };

/*
Ascii Key:
Space Char is 0
Newline Char is 10

Wall: 201╔ 205═ 200╚ 202╩ 203╦ 204╠ 206╬ 185╣ 186║ 187╗ 188╝
Space: 46.
Filled Space:
 - Item: 105i
 - Enemy: 33!
 - Player: 80P
 - Key: 157¥
 - Chest:
	- Locked: 37%
	- Unlocked: 67C
 - Door:
	- Unlocked: 197┼
	- Locked: 216
*/