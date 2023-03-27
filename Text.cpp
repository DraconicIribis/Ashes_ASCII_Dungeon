#include <iostream>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <future>
#include "Text.h"

using namespace std;

void textType(string output, bool reverse)
{
	int strPlace = 0;
	int time = 60;
	string replaceStr;
	string const OUTPUT = output;

	int asyncTimeIncr = time;
	int asyncTimeLim = output.length() * asyncTimeIncr;

	auto skip = async(launch::async, [asyncTimeLim, asyncTimeIncr]
		{
			bool type = true;

			for (int t = 0; t < asyncTimeLim - asyncTimeIncr; t += asyncTimeIncr)
			{
				if (_kbhit() != 0) //Detect if the keyboard is hit, and then end the async thread once detected
				{
					(void)_getch(); //Clear the _kbhit() so that future instances don't automatically call this if statement

					type = false;
					return type;
					break;
				}

				Sleep(asyncTimeIncr);
			}

			if (type)
			{
				return type;
			}
		});  //Launch the asynchronous function to get input during text typing. If input is found, typing will stop and the rest of the text will be output

	for (char a : output) //Type out each character in the output
	{
		if (skip.wait_for(0.06s) == future_status::ready) { //If the async function finishes, trunkate all the already typed chars from the output string and then print the rest of the output string
			for (int x = 0; x < strPlace; x++)
			{
				output.erase(output.begin());
			}

			cout << output;

			break;
		}

		strPlace++;

		cout << a;
	}

	if (reverse) //If the reverse setting is selected, type out the output then delete it in the same speed
	{
		asyncTimeLim += (OUTPUT.length() * 60);
		int sleepTimer = 60;

		auto skip = async(launch::async, [asyncTimeLim, asyncTimeIncr]
			{
				bool type = true;

				for (int t = 0; t < asyncTimeLim - asyncTimeIncr; t += asyncTimeIncr)
				{
					if (_kbhit() != 0) //Detect if the keyboard is hit, and then end the async thread once detected
					{
						(void)_getch(); //Clear the _kbhit() so that future instances don't automatically call this if statement

						type = false;
						return type;
						break;
					}

					Sleep(asyncTimeIncr);
				}

				if (type)
				{
					return type;
				}
			});  //Launch the asynchronous function to get input during text typing. If input is found, typing will stop and the rest of the text will be output

		Sleep(1000);

		for (int a = OUTPUT.size() - 1; a >= 0; a--)
		{
			if (skip.wait_for(sleepTimer * 1ms) == future_status::ready) {
				sleepTimer = 0;
			}
			replaceStr = OUTPUT[a];

			textReplace(replaceStr, "", false);
		}
	}
}

void textReplace(string prevLine, string output, bool typeOut) //Replace text with the designated new text
{
	bool multiLine = false;
	int newLines = 0;
	int inputY;

	for (char a : prevLine)
	{
		if (a == '\n')
		{
			multiLine = true;
			newLines += 1;
		}
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo); //Under the CONSOLE_SCREEN_BUFFER_INFO struct, find the location of the cursor on the console using a standard handle

	if (prevLine.length() > consoleInfo.dwMaximumWindowSize.X && !multiLine) //Check if the string parameter has a new line character at the end, then set the cursor position up onto the line which contains the string to prepare for it to be replaced
	{
		inputY = consoleInfo.dwCursorPosition.Y - (prevLine.length() / consoleInfo.dwMaximumWindowSize.X);

		COORD upLine = { 0, inputY}; //Transfer the y coordinate in the cursor position COORD into a new COORD object with the x coordinate being the height of the input string
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), upLine); //Move the cursor to the designated spot (beginning of the first line in the input string)
	}
	else if (multiLine) //Detect if there are newline chars in here
	{
		inputY = newLines;

		COORD upLine = { 0, consoleInfo.dwCursorPosition.Y - inputY }; //Set the COORDs to the beginning of the first line in the input string
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), upLine);
	}
	else
	{
		inputY = consoleInfo.dwCursorPosition.Y;
		cout << string(prevLine.length(), '\b'); //Backspace over to the front of the string, and then replace it with blanks. Then, fills the blanks with output.
	}

	if(!multiLine) //If there was no newline character detected, then simply erase the single line of text by printing space chars over the text
	{
		for (int x = 0; x < prevLine.length(); x++)
		{
			cout << " ";
		}
	}
	else //Otherwise, print spaces across the text, moving down a line when there is a newline char detected
	{
		for (char a : prevLine)
		{
			if (a != '\n')
			{
				cout << " ";
			}
			else
			{
				cout << "\n";
			}
		}
	} //Consolidate this erroneous if else statement when you get the chance, no point in having to detect a newline just use the code in the else statement
	
	COORD restart = { 0, inputY }; //Put the console cursor back at the beginning of where the input string used to be
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), restart);

	if (typeOut) //If the type setting is selected, type the output text out
	{
		textType(output, false);
	}
	else
	{
		cout << output;
	}
}

void clearScreen()
{
	string fill = "";

	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo); //Get the information about the console

	for (int i = 0; i < consoleInfo.dwMaximumWindowSize.X; i++) //For the maximum x value (or length) of the console screen, make the fill string full of spaces equal to that maximum value. This fill string will print over every line in the currently visible console to clear the screen.
	{
		fill += " ";
	}

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0, 0});

	for (int i = 0; i <= consoleInfo.dwMaximumWindowSize.Y; i++) //Print out the fill string on every line in the visible console
	{
		cout << fill << endl;
	}

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 }); //Return the cursor to the start for further functions
}

int setInput(int min, int max)
{
	bool error = false;
	string errorMsg = "That is not a valid option, please enter a valid option. Remember, numbered inputs only.";
	int choice;

	do //Get player input on a specified int range
	{
		choice = _getch() - 48;

		if ((choice >= min) && (choice <= max))
		{
			textReplace(errorMsg, "", false);
			error = false;

		}
		else
		{
			if (error)
			{
				textReplace(errorMsg, errorMsg, false);
			}
			else
			{
				error = true;
				cout << errorMsg;
			}
		}
	} while (error);

	return choice;
}
