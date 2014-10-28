#ifndef KEEPRUNNING_H
#define KEEPRUNNING_H

/*
A small tool to keep the console window from disappearing the moment a program ends/breaks.

Instantiate a KeepRunning object in the first line of main() to have it work, the
destructor will be called when the program attempts to leave the scope of main().

Based on:
http://www.cplusplus.com/forum/beginner/1988/
*/

#include <iostream>
#include <numeric>

class KeepRunning
{
public:
	~KeepRunning()
	{
		// Whatever code you prefer to keep the program running, e.g.
		std::cout << "Press ENTER to continue..." << std::endl;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
};

#endif