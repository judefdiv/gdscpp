/**
 * Author:			J.F. de Villiers & H.F. Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				  Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license:     MIT License
 * Description: Header for GDS drawing functions.
 * File:			  gdsForge.hpp
 */

#ifndef GDSForge
#define GDSForge

// ============================ Includes ============================
#include "gdsCpp.hpp"
#include "gdsParser.hpp"
#include <bitset>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>

// ===================== Function Declarations ======================
// Functions to easily draw in GDSfiles

gdsBOUNDARY draw2ptBox(int layer, int blX, int blY, int trX, int trY);
gdsBOUNDARY drawBoundary(int layer, std::vector<int> &corX,
                         std::vector<int> &corY);
gdsPATH drawPath(int layer, unsigned int width, std::vector<int> &corX,
                 std::vector<int> &corY);
gdsSREF drawSREF(const std::string &STRname, int Xcor, int Ycor);
gdsSREF drawSREF(const std::string &STRname, int Xcor, int Ycor, double angle,
                 double mag, bool mirror);

// ============================= Classes ============================

/*
 * [gdsForge - Class which allows for creation of GDS data]
 */
class gdsForge
{
private:
  std::string fileName;
  FILE *gdsFile;

  std::vector<gdsSTR> STR;

  std::vector<std::string> GDSfileNameToBeImport;

  // Surface level

  // UpperGround level
  void gdsBegin();
  void gdsEnd();
  void gdsStrStart(const std::string &strName);
  void gdsStrEnd();

  int gdsCopyFile(const std::string &fileName);

  void gdsPath(const gdsPATH &in_PATH, bool minimal);
  void gdsBoundary(const gdsBOUNDARY &in_BOUNDARY, bool minimal);
  void gdsSRef(const gdsSREF &in_SREF, bool minimal);
  void gdsARef(const gdsAREF &in_AREF, bool minimal);
  void gdsNode(const gdsNODE &in_NODE, bool minimal);
  void gdsText(const gdsTEXT &in_TEXT, bool minimal);

  // Lower level
  int GDSwriteRec(int record);
  int GDSwriteInt(int record, int arrInt[], int cnt);
  int GDSwriteStr(int record, std::string inStr);
  int GDSwriteBitArr(int record, std::bitset<16> inBits);
  int GDSwriteRea(int record, double arrInt[], int cnt);
  void GDSwriteUnits(); // <--- must be removed...

public:
  gdsForge();
  ~gdsForge() {}

  int gdsCreate(const std::string &FileName, std::vector<gdsSTR> &inVec,
                double units[2]);

  void importGDSfile(std::vector<std::string> &fileNames)
  {
    this->GDSfileNameToBeImport = fileNames;
  }

  void to_str();
};

#endif
