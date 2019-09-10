/**
 * Author:
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license:
 * Description: .
 * File:				gdsForge.hpp
 */

#ifndef GDSForge
#define GDSForge

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <map>
#include <set>
#include <math.h>
#include <cstring>
#include <bitset>

#include "gdsCpp.hpp"
#include "gdsParser.hpp"

using namespace std;

// Functions to easily draw in GDSfiles
gdsBOUNDARY draw2ptBox(int layer, int blX, int blY, int trX, int trY);
gdsBOUNDARY drawBoundary(int layer, vector<int> corX, vector<int> corY);
gdsPATH drawPath(int layer, unsigned int width, vector<int> corX, vector<int> corY);
gdsSREF drawSREF(string STRname, int Xcor, int Ycor);

class gdsForge{
	private:
		string fileName;
		FILE *gdsFile;

		vector<gdsSTR> STR;

		// Surface level

		// UpperGround level
		void gdsBegin();
		void gdsEnd();
		void gdsStrStart(string strName);
		void gdsStrEnd();

		void gdsCopyStr(string fileName);

		void gdsPath(gdsPATH& in_PATH, bool minimal);
		void gdsBoundary(gdsBOUNDARY& in_BOUNDARY, bool minimal);
		void gdsSRef(gdsSREF& in_SREF, bool minimal);
		void gdsNode(gdsNODE& in_NODE, bool minimal);
		void gdsText(gdsTEXT in_TEXT, bool minimal);
		// void gdsARefV2(gdsAREF in_AREF, bool minimal);
		// void gdsBOXV2(gdsBOX in_BOX, bool minimal);

		// Lower level
		int GDSwriteRec(int record);
		int GDSwriteInt(int record, int arrInt[], int cnt);
		int GDSwriteStr(int record, string inStr);
		int GDSwriteBitArr(int record, bitset<16> inBits);
		int GDSwriteRea(int record, double arrInt[], int cnt);
		void GDSwriteUnits();																		// <--- must be removed...

	public:
		gdsForge();
		~gdsForge(){};

		int gdsCreate(string FileName, vector<gdsSTR>& inVec, double units[2]);

		void to_str();
};

// void copyGDSstrs(string fileName);					// unsure, important function

#endif