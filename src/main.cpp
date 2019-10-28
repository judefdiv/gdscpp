/**
 * Author:		J.F. de Villiers & H.F. Herbst
 * Origin:  	E&E Engineering - Stellenbosch University
 * For:			Supertools, Coldflux Project - IARPA
 * Created: 	2019-08-21
 * Modified:
 * license:
 * Description: Primary file for the program.
 * File:		main.cpp
 */

// ============================ Includes ============================
#include <iostream>							//stream
#include <string>							//string goodies
#include <set>
#include <iomanip> 							// setprecision()
#include "gdsCpp.hpp"
#include "gdsForge.hpp"						// used to testing function

// ========================== Miscellanious ==========================
#define versionNo 0.1
#define outfileName "data/results/gds/"		// Default file output name
using namespace std;

// ================= Local Function Declarations =====================
void welcomeScreen();
void helpScreen();
int RunTool(int argCount, char** argValues);
void testGDS(string fileName);

// =========================== Main Loop =============================
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

	set<string> validCommands = {"-g", "-i", "-r", "-rw","-v", "-h"};

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
			gdsToText(gdsFName);
			return 1;
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("-rw")){
		if(gdsFName.compare("\0")){
			gdscpp gdsfile;
			gdsfile.import(gdsFName);
			gdsfile.write("outGDS.gds");
			return 1;
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
		}
	else if(!command.compare("-r")){
		if(gdsFName.compare("\0")){
			gdscpp gdsfile;
			gdsfile.import(gdsFName);
			// gdsfile.createHierarchy();
			gdsfile.genDot("data/tree.jpg");
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

/**
 * [testGDS - Simple testing function for gdsForge]
 * @param  fileName  [File name of string]
 */
void testGDS(string fileName){
	gdscpp fooGDS;

	vector<gdsSTR> arrSTR;
	arrSTR.resize(arrSTR.size()+1);

	arrSTR.back().name = "Hein_structure";
	// arrSTR.back().BOUNDARY.push_back(draw2ptBox(1, -10000, -10000, 10000, 10000));
	// arrSTR.back().SREF.push_back(drawSREF("LSmitll_AND2T", 100000, 100000));
	// arrSTR.back().SREF.push_back(drawSREF("LSmitll_AND2T", -100000, -100000));
	// arrSTR.back().PATH.push_back(drawPath(int layer,  unsigned int width,  vector<int> corX,  vector<int> corY));
	vector<int> x_co_ordinates = {0,100,100,200};
	vector<int> y_co_ordinates = {0,100,0,0};
	arrSTR.back().PATH.push_back( drawPath(60, 5, 2, x_co_ordinates, y_co_ordinates) );

	// fooGDS.importGDSfile("data/LSmitll_AND2T.gds");

	fooGDS.setSTR(arrSTR);

	fooGDS.write(fileName);
}

void helpScreen(){
	cout << "===============================================================================" << endl;
	cout << "Usage: GDScpp [ OPTION ] [ filenames ]" << endl;
	cout << "-g(DS)          Does stuffs." << endl;
	cout << "                  [.gds/.gds2/ file] -o [.gds2 file]" << endl;
	cout << "-i(nterpret)    Translates a GDS file directly to ASCII." << endl;
	cout << "                  [.gds/.gds2/ file]" << endl;
	cout << "-r(ead)  		   Reads in specified file." << endl;
	cout << "                  [.gds/.gds2/ file]" << endl;
	cout << "-rw(read/write) Reads in a GDS file and writes out what is stored." << endl;
	cout << "                  [.gds/.gds2/ file]" << endl;
	cout << "-v(ersion)    Displays the version number." << endl;
	cout << "-h(elp)       Help screen." << endl;
	cout << "===============================================================================" << endl;
}

/**
 * [welcomeScreen - Displays ascii art of GDS-cpp logo]
 * @param  fileName  [File name of string]
 */
void welcomeScreen(){
	cout << "              ___     ___" << endl;
	cout << "             |   |   |   |" << endl;
	cout << "             |___|   |___|" << endl;
	cout << " __            |       |            __" << endl;
	cout << "|  |___        |       |        ___|  |" << endl;
	cout << "|__|   |     __|_______|__     |   |__|" << endl;
	cout << "       |____|             |____|" << endl;
	cout << " __         |             |         __" << endl;
	cout << "|  |________|   GDS-cpp   |________|  |" << endl;
	cout << "|__|        |             |        |__|" << endl;
	cout << "        ____|             |____" << endl;
	cout << " __    |    |_____________|    |    __" << endl;
	cout << "|  |___|       |       |       |___|  |" << endl;
	cout << "|__|           |       |           |__|" << endl;
	cout << "              _|_     _|_" << endl;
	cout << "             |   |   |   |" << endl;
	cout << "             |___|   |___|" << endl;
}