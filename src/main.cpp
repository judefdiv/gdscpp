/**
 * Author:  		Jude de Villiers & Heinrich Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-21
 * Modified:
 * license:
 * Description: Primary file for the program.
 * File:				main.cpp
 */

#include <iostream> //stream
#include <string>   //string goodies
#include <set>
// #include <map>
#include <iomanip> // setprecision()

#include "gdsCpp.hpp"

#define versionNo 0.1
#define InfilePath "data/" // Default file output name
using namespace std;

/**
 * Declaring functions
 */

void welcomeScreen();
void helpScreen();
int RunTool(int argCount, char **argValues);

/**
 * Main loop
 */

int main(int argc, char *argv[])
{
	welcomeScreen();
	if (RunTool(argc, argv)==EXIT_FAILURE)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}

/**
 * [RunTool - Runs the appropriate tool]
 * @param  argCount  [Size of the input argument array]
 * @param  argValues [The input argument array]
 * @return           [1 - all good; 0 - error]
 */

int RunTool(int argCount, char **argValues)
{
	welcomeScreen();

	if (argCount <= 1)
	{
		return EXIT_FAILURE;
	}
	//interpret, version
	set<string> validCommands = {"-load", "-version", "-help"};
	string gdsFName = "\0"; // The GDS/GDS2 file
	string command = "\0";  // The command to be executed

	// search for command
	string argument;
	for (int i = 0; i < argCount; i++)
	{
		argument = string(argValues[i]);
		if (validCommands.find(argument) != validCommands.end())
		{
			command = argument;
		}
	}
	// No commands found
	if (!command.compare("\0"))
	{
		cout << "No commands found. Try -help" << endl;
		return EXIT_FAILURE;
	}
	// search for gds/gsd2
	for (int i = 0; i < argCount; i++)
	{
		argument = string(argValues[i]);
		if (argument.find(".gds") != string::npos || argument.find(".gds2") != string::npos)
		{
			gdsFName = InfilePath;
			gdsFName = gdsFName + argument;
		}
	}

	if (command.compare("-load")==0)
	{ //Interpret
		if (gdsFName.compare("\0"))
		{
			gdscpp gdsfile;
			gdsfile.quick2ASCII(gdsFName);
			return EXIT_SUCCESS;
		}
		else
		{
			cout << "Input argument error." << endl;
			return EXIT_FAILURE;
		}
	}
	else if (!command.compare("-version"))
	{
		if (argCount == 1 + 1)
		{
			cout << setprecision(2);
			cout << "Version: " << versionNo << endl;
			return EXIT_SUCCESS;
		}
		cout << "Input argument error." << endl;
		return EXIT_FAILURE;
	}
	else if (!command.compare("-help"))
	{
		helpScreen();
		return EXIT_SUCCESS;
	}
	else
	{
		cout << "Error: Unhandled state. Function should have returned by now." << endl;
		return EXIT_FAILURE;
	}

	cout << "Error: Unhandled state. Function should have returned by now." << endl;
	return EXIT_FAILURE;
}

void helpScreen()
{
	cout << "===============================================================================" << endl;
	cout << "Usage: GDScpp [ OPTION ] [ filenames ]" << endl;
	cout << "-load  Reads in specified file and displays content." << endl;
	cout << "                [.gds/.gds2/ file]" << endl;
	cout << " GDS file should be placed inside data folder." << endl;
	cout << "-version    Displays the version number." << endl;
	cout << "-help       Help screen." << endl;
	cout << "===============================================================================" << endl;
}

/**
 * Welcoming screen
 */

void welcomeScreen()
{
	cout << "=====================================" << endl;
	cout << "               GDScpp" << endl;
	cout << "=====================================" << endl;
}