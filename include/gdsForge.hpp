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

class gdsForge{
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
		void gdsNode(int Layer, vector<int>& corX, vector<int>& corY);
		void gdsText(int Layer, string words, int corX, int corY);
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
		// gdsForge(vector<gdsSTR>& inVec);
		gdsForge();
		~gdsForge(){};

		// int gdsCreate(string FileName);
		int gdsCreate(string FileName, vector<gdsSTR>& inVec);

		void to_str();
};

void copyGDSstrs(string fileName);					// unsure, important function

#endif