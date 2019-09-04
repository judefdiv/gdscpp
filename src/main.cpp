/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-21
 * Modified:
 * license:
 * Description: Primary file for the program.
 * File:				main.cpp
 */

#include <iostream>				//stream
#include <string>					//string goodies
#include <set>
// #include <map>
#include <iomanip> 				// setprecision()

#include "gdsCpp.hpp"
#include "gdsForge.hpp"	// used to testing function

#define versionNo 0.1
#define outfileName "data/results/gds/"				// Default file output name
using namespace std;

/**
 * Declaring functions
 */

void welcomeScreen();
void helpScreen();
int RunTool(int argCount, char** argValues);
void testGDS(string fileName);

/**
 * Main loop
 */

int main(int argc, char* argv[]){
	// welcomeScreen();
	if(!RunTool(argc, argv)) return 0;

	return 0;
}

/**
 * [RunTool - Runs the appropriate tool]
 * @param  argCount  [Size of the input argument array]
 * @param  argValues [The input argument array]
 * @return           [1 - all good; 0 - error]
 */

int RunTool(int argCount, char** argValues){
	welcomeScreen();

	if(argCount <= 1){
		return 0;
	}

	set<string> validCommands = {"-g", "-i", "-v", "-h"};

	string outFName = "\0";			// The output file, which is follow by the -o parameter
	string gdsFName = "\0";			// The GDS/GDS2 file
	string command  = "\0";			// The command to be executed

	string foo;

	// search for command
	for(int i = 0; i < argCount; i++){
		foo = string(argValues[i]);
		if(validCommands.find(foo) != validCommands.end()){
			command = foo;
		}
	}
	if(!command.compare("\0")){
		cout << "Invalid." << endl;
		return 0;
	}

	// search for gds/gsd2
	for(int i = 0; i < argCount; i++){
		foo = string(argValues[i]);
	  if(foo.find(".gds")!=string::npos || foo.find(".gds2")!=string::npos){
	  	gdsFName = foo;
	  }
	}

	// search for output filename
	for(int i = 0; i < argCount-1; i++){
	  if(!string(argValues[i]).compare("-o")){
	  	outFName = string(argValues[i+1]);
	  }
	}
	// auto assign output filename if non has been set
	if(!outFName.compare("\0")){
		if(!command.compare("-g")){
			outFName = outfileName;
		}
	}

	// Run the commands
	if(!command.compare("-g")){
		if(outFName.compare("\0")){
			testGDS(outFName);
			return 1;
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("-i")){
		if(gdsFName.compare("\0")){
			gdscpp gdsfile;
			// gdsfile.import(gdsFName);				//Todo - comment in import
			gdsfile.quick2ASCII(gdsFName);
			return 1;
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("-t")){
		if(gdsFName.compare("\0")){

			return 1;
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("-v")){
		if(argCount == 1 + 1){
			cout << setprecision(2);
			cout << "Version: " << versionNo << endl;
			return 1;
		}
		cout << "Input argument error." << endl;
		return 0;
	}
	else if(!command.compare("-h")){
		helpScreen();
		return 1;
	}
	else{
		cout << "Quickly catch the smoke before it escapes." << endl;
		return 0;
	}

	cout << "I am smelling smoke." << endl;
	return 0;
}

void testGDS(string fileName){
	gdscpp fooGDS;

	vector<gdsSTR> arrSTR;
	arrSTR.resize(1);

	// vector<int> arrX;
	// vector<int> arrY;

	// gdsBOUNDARY draw2ptBox(int layer, int blX, int blY, int trX, int trY);
	// gdsBOUNDARY drawBoundary(int layer, vector<int> corX, vector<int> corY);
	// gdsPATH drawPath(int layer, unsigned int width, vector<int> corX, vector<int> corY);

	arrSTR[0].name = "MyStr";
	arrSTR[0].BOUNDARY.push_back(draw2ptBox(1, -100, -100, 100, 100));

	fooGDS.setSTR(arrSTR);

	fooGDS.write(fileName);
}

void helpScreen(){
	cout << "===============================================================================" << endl;
	cout << "Usage: GDScpp [ OPTION ] [ filenames ]" << endl;
	cout << "-g(DS)        Does stuffs." << endl;
	cout << "                -o [.gds2 file]" << endl;
	cout << "-i(nterpret)  Reads in specified file and displays content." << endl;
	cout << "                [.gds/.gds2/ file]" << endl;
	cout << "-v(ersion)    Displays the version number." << endl;
	cout << "-h(elp)       Help screen." << endl;
	cout << "===============================================================================" << endl;
}

/**
 * Welcoming screen
 */

void welcomeScreen(){
	cout << "=====================================" << endl;
	cout << "               GDScpp" << endl;
	cout << "=====================================" << endl;
}