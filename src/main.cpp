/**
 * Author:		  J.F. de Villiers & H.F. Herbst
 * Origin:  	  E&E Engineering - Stellenbosch University
 * For:			    Supertools, Coldflux Project - IARPA
 * Created: 	  2019-08-21
 * Modified:
 * license:     MIT License
 * Description: Primary file for the program.
 * File:		    main.cpp
 */

// ============================ Includes ============================
#include "gdsCpp.hpp"
#include <iomanip>  // setprecision()
#include <iostream> //stream
#include <set>
#include <string> //string goodies

// ========================== Miscellaneous ==========================
#define versionNo 0.9
#define outfileName "data/results/gds/" // Default file output name
using namespace std;

// ================= Local Function Declarations =====================
void welcomeScreen();
void helpScreen();
int RunTool(int argCount, char **argValues);
void example1(const string &fileName);
void example2(const string &fileNameIn, const string &fileNameOut);

// =========================== Main Loop =============================
int main(int argc, char *argv[])
{
  // welcomeScreen();
  if (!RunTool(argc, argv))
    return 0;

  return 0;
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

  if (argCount <= 1) {
    return 0;
  }

  set<string> validCommands = {"-g", "-i", "-r", "-rw", "-rs", "-v", "-h"};

  string outFName =
      "\0"; // The output file, which is follow by the -o parameter
  string gdsFName = "\0"; // The GDS/GDS2 file
  string command = "\0";  // The command to be executed

  string foo;

  // search for command
  for (int i = 0; i < argCount; i++) {
    foo = string(argValues[i]);
    if (validCommands.find(foo) != validCommands.end()) {
      command = foo;
    }
  }
  if (!command.compare("\0")) {
    cout << "Invalid." << endl;
    return 0;
  }

  // search for gds/gsd2
  for (int i = 0; i < argCount; i++) {
    foo = string(argValues[i]);
    if (foo.find(".gds") != string::npos || foo.find(".gds2") != string::npos) {
      gdsFName = foo;
      i=argCount;
    }
  }

  // search for output filename
  for (int i = 0; i < argCount - 1; i++) {
    if (!string(argValues[i]).compare("-o")) {
      outFName = string(argValues[i + 1]);
    }
  }
  // auto assign output filename if non has been set
  if (!outFName.compare("\0")) {
    if (!command.compare("-g")) {
      outFName = outfileName;
    }
  }

  // Run the commands
  if (!command.compare("-g")) {
    // if (outFName.compare("\0")) {
    example1("example1.gds");
    example2("example1.gds", "example2.gds");
    return 1;
    // } else {
    //   cout << "Input argument error." << endl;
    //   return 0;
    // }
  } else if (!command.compare("-i")) {
    if (gdsFName.compare("\0")) {
      gdsToText(gdsFName);
      return 1;
    } else {
      cout << "Input argument error." << endl;
      return 0;
    }
  } else if (!command.compare("-rw")) {
    if (gdsFName.compare("\0") && outFName.compare("\0")) {
      gdscpp gdsfile;
      gdsfile.import(gdsFName);
      gdsfile.write(outFName);
      return 1;
    } else {
      cout << "Input argument error." << endl;
      return 0;
    }
  } else if (!command.compare("-r")) {
    if (gdsFName.compare("\0")) {
      gdscpp gdsfile;
      gdsfile.import(gdsFName);
      // gdsfile.createHierarchy();
      gdsfile.genDot("diagram.jpg");
      return 1;
    } else {
      cout << "Input argument error." << endl;
      return 0;
    }
  } else if (!command.compare("-rs")) {
    if (gdsFName.compare("\0")) {
      gdscpp gdsfile;
      gdsfile.import(gdsFName);
      gdsfile.findRootSTR();
      return 1;
    } else {
      cout << "Input argument error." << endl;
      return 0;
    }
  } else if (!command.compare("-v")) {
    if (argCount == 1 + 1) {
      cout << setprecision(2);
      cout << "Version: " << versionNo << endl;
      return 1;
    }
    cout << "Input argument error." << endl;
    return 0;
  } else if (!command.compare("-h")) {
    helpScreen();
    return 1;
  } else {
    cout << "Quickly catch the smoke before it escapes." << endl;
    return 0;
  }

  cout << "I am smelling smoke." << endl;
  return 0;
}

/**
 * [example1 - Simple unction to demonstrate GDScpp and gdsForge]
 * @param  fileName  [File name of string]
 */

void example1(const string &fileName)
{
  gdscpp fooGDS;
  gdsSTR fooSTR;

  vector<int> corX;
  vector<int> corY;

  fooSTR.name = "arrow_box";

  corX = {000, 200, 400};
  corY = {700, 900, 700};
  fooSTR.PATH.push_back(drawPath(1, 5, corX, corY));

  corX = {200, 200};
  corY = {900, 000};
  fooSTR.PATH.push_back(drawPath(1, 5, corX, corY));

  corX = {0, 150, 150, 250, 250, 150, 150, 400, 400, 000, 0};
  corY = {0, 000, 250, 250, 150, 150, 000, 000, 400, 400, 0};
  fooSTR.BOUNDARY.push_back(drawBoundary(2, corX, corY));

  fooGDS.setSTR(fooSTR);
  fooGDS.write(fileName);
}

/**
 * [example2 - A more complex example to show how a GDS file can be imported
 * directly and referenced]
 * @param fileNameIn  [Input file name]
 * @param fileNameOut [Output file name]
 */
void example2(const string &fileNameIn, const string &fileNameOut)
{
  gdscpp fooGDS;
  vector<gdsSTR> arrSTR;
  arrSTR.resize(arrSTR.size() + 1);

  vector<int> corX;
  vector<int> corY;

  arrSTR.back().name = "example2";
  arrSTR.back().SREF.push_back(drawSREF("arrow_box", 0, 0));
  arrSTR.back().SREF.push_back(drawSREF("arrow_box", 500, 500, 0, 1, false));

  fooGDS.importGDSfile(fileNameIn);
  fooGDS.setSTR(arrSTR);
  fooGDS.write(fileNameOut);
}

void helpScreen()
{
  cout << "===================================================================="
          "==========="
       << endl;
  cout << "Usage: GDScpp [ OPTION ] [ filenames ]" << endl;
  cout << "-g(DS)          Does stuffs." << endl;
  cout << "                  [.gds/.gds2/ file] -o [.gds2 file]" << endl;
  cout << "-i(nterpret)    Translates a GDS file directly to ASCII." << endl;
  cout << "                  [.gds/.gds2/ file]" << endl;
  cout << "-r(ead)  		   Reads in specified file." << endl;
  cout << "                  [.gds/.gds2/ file]" << endl;
  cout << "-rw(read/write) Reads in a GDS file and writes out what is stored."
       << endl;
  cout << "                  [.gds/.gds2/ file] -o [.gds2 file]" << endl;
  cout << "-v(ersion)    Displays the version number." << endl;
  cout << "-h(elp)       Help screen." << endl;
  cout << "===================================================================="
          "==========="
       << endl;
}

/**
 * [welcomeScreen - Displays ascii art of GDS-cpp logo]
 */
void welcomeScreen()
{
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