/**
 * Author:  		
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license: 
 * Description: .
 * File:				gdsExport.hpp
 */

#ifndef gdsExporter
#define gdsExporter

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
#include "gdsClass.hpp"

using namespace std;

class gdsExport{
	private:
		string fileName;
		FILE *gdsFile;

		vector<gdsSTR> STR;

		// Surface level

		// Lowish level
		void gdsBegin();
		void gdsEnd();
		void gdsStrStart(string strName);
		void gdsStrEnd();
		void gdsPath(int Layer, int width, vector<int>& corX, vector<int>& corY);
		void gdsBoundary(int Layer, vector<int>& corX, vector<int>& corY);
		void gdsSREF(string refName, bool ReflX, double mag, double ang, int corX, int corY);
		void gdsCopyStr(string fileName);


		// Lower level
		int GDSwriteRec(int record);
		int GDSwriteInt(int record, int arrInt[], int cnt);
		int GDSwriteStr(int record, string inStr);
		int GDSwriteBitArr(int record, bitset<16> inBits);
		int GDSwriteRea(int record, double arrInt[], int cnt);
		void GDSwriteUnits();																		// <--- must be removed...
	
	public:
		gdsExport(vector<gdsSTR>& inVec);
		~gdsExport(){};

		int gdsCreate(string FileName);

		void to_str();
};


void copyGDSstrs(string fileName);					// unsure, important function

// void GDSwriteUnits();

// shortcuts for key that are used
#define GDS_HEADER 		0x0002
#define GDS_BGNLIB 		0x0102
#define GDS_LIBNAME 	0x0206
#define GDS_UNITS 		0x0305
#define GDS_ENDLIB 		0x0400
#define GDS_BGNSTR		0x0502
#define GDS_STRNAME		0x0606
#define GDS_ENDSTR		0x0700
#define GDS_BOUNDARY 	0x0800
#define GDS_PATH 			0x0900
#define GDS_SREF			0x0a00
#define GDS_LAYER 		0x0d02
#define GDS_DATATYPE 	0x0e02
#define GDS_WIDTH			0x0f03
#define GDS_XY				0x1003
#define GDS_ENDEL			0x1100
#define GDS_SNAME  		0x1206
#define GDS_STRANS		0x1a01
#define GDS_MAG				0x1b05
#define GDS_ANGLE			0x1c05
#define GDS_PATHTYPE  0x2102

// #define GDS_AREF 			0x0b00
// #define GDS_TEXT			0x0c00
// #define GDS_COLROW		0x1302
// #define GDS_NODE			0x1500
// #define GDS_TEXTTYPE	0x1602
// #define GDS_PRESENTATION	0x1701
// #define GDS_STRING		0x1906
// #define GDS_REFLIBS		0x1f06
// #define GDS_FONTS			0x2006
// #define GDS_GENERATIONS		0x2202
// #define GDS_ATTRTABLE	0x2306
// #define GDS_EFLAGS		0x2601
// #define GDS_NODETYPE	0x2a02
// #define GDS_PROPATTR	0x2b02
// #define GDS_PROPVALUE	0x2c06

#endif