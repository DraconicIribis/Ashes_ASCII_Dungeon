#pragma once

#include <iostream>
#include <vector>

using namespace std;

struct Room
{
	vector<int> pos;
	vector<int> map;
	string description;

	void display(vector<int> map)
	{
		for (int tile : map)
		{
			cout << (char)tile;
		}
	};

	int getRowLength(vector<int> map)
	{
		int rowLength = 0;

		for (int tile : map)
		{
			if (tile != 10)
			{
				rowLength++;
			}
			else
			{
				rowLength++;
				break;
			}
		}

		return rowLength;
	}
};

struct Item
{
	string name;
	int dmg;
	int heal = 0;
	int dex = 0;
};

struct Character
{
	int health = 100;
	vector<Item> inventory;
	vector<short> room = { 0, 0 }; //pos[0] & [1] is the x & y coord of the room the character is in
	vector<short> pos = { 5, 1 };//pos[0] & [1] is the x & y coord in the room the character is located at, and pos[2] is the y coord in the room the character is located at
};

struct Non_Player_Character
{
	int health = 12;
	int accuracy = 60;
	int dmg = 5;
	string name = "null";
};

bool combat(Character enemy1, Character& player, Room& room);
bool encounter(Room& room, Character& player, vector<Room*> gameMap, vector<Item> &lockItems, vector<Item> &unlockItems, vector<Non_Player_Character> &enemies, Item chestLoot = {"null"});

extern Room entrance, zero0, two0, one1, two1, three1, one2, two2, three2;

extern Item key, shortSword, longSword, spear, dagger, messer, healingPotion;

extern Character player;

extern Non_Player_Character oldBat, crazedLooter, fireElemental, rockElemental, stoneBoss;

