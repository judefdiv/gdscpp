/**
 * Author:      J.F. de Villiers & H.F. Herbst
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-10-29
 * Modified:
 * license:     MIT License
 * Description: Example of GDScpp.
 * File:        example.cpp
 */

// ============================ Includes ============================
#include "gdsCpp.hpp"
#include <iostream>     //stream
#include <string> //string goodies

// ========================== Miscellaneous ==========================
using namespace std;

// ================= Local Function Declarations =====================
void example1(const string &fileName);
void example2(const string &fileNameIn, const string &fileNameOut);

// =========================== Main Loop =============================
int main(int argc, char *argv[])
{
  example1("example1.gds");
  example2("example1.gds", "example2.gds");

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

