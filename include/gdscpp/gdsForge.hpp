/**
 * Author:			J.F. de Villiers & H.F. Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				  Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license:
 * Description: Header for GDS drawing functions.
 * File:			  gdsForge.hpp
 */

#ifndef GDSForge
#define GDSForge

// ============================ Includes ============================
#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>
#include "gdsCpp.hpp"
#include "gdsParser.hpp"

// ===================== Function Declarations ======================
// Functions to easily draw in GDSfiles

gdsBOUNDARY draw2ptBox(int layer, int blX, int blY, int trX, int trY);
gdsBOUNDARY drawBoundary(int layer, std::vector<int> corX,
                         std::vector<int> corY);
gdsPATH drawPath(int layer, unsigned int width, unsigned int path_type,
                 std::vector<int> corX, std::vector<int> corY);
gdsSREF drawSREF(std::string STRname, int Xcor, int Ycor);

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
  void gdsStrStart(std::string strName);
  void gdsStrEnd();

  int gdsCopyFile(std::string fileName);

  void gdsPath(gdsPATH &in_PATH, bool minimal);
  void gdsBoundary(gdsBOUNDARY &in_BOUNDARY, bool minimal);
  void gdsSRef(gdsSREF &in_SREF, bool minimal);
  void gdsARef(gdsAREF &in_AREF, bool minimal);
  void gdsNode(gdsNODE &in_NODE, bool minimal);
  void gdsText(gdsTEXT in_TEXT, bool minimal);
  // void gdsARefV2(gdsAREF in_AREF, bool minimal);
  // void gdsBOXV2(gdsBOX in_BOX, bool minimal);

  // Lower level
  int GDSwriteRec(int record);
  int GDSwriteInt(int record, int arrInt[], int cnt);
  int GDSwriteStr(int record, std::string inStr);
  int GDSwriteBitArr(int record, std::bitset<16> inBits);
  int GDSwriteRea(int record, double arrInt[], int cnt);
  void GDSwriteUnits(); // <--- must be removed...

public:
  gdsForge();
  ~gdsForge(){};

  int gdsCreate(std::string FileName, std::vector<gdsSTR> &inVec,
                double units[2]);

  void importGDSfile(std::vector<std::string> &fileNames)
  {
    this->GDSfileNameToBeImport = fileNames;
  };

  void to_str();
};

#endif