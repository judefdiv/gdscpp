/**
 * Author:  		Jude de Villiers & Heinrich Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-22
 * Modified:
 * license:
 * Description: .
 * File:			gdsClass.hpp
 */

#ifndef gdscppheader
#define gdscppheader
// ========================== Includes ========================
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <map>
#include <bitset>
#include "gdsParser.hpp"

using namespace std;

class gdsSTR;
class gdsBOUNDARY;
class gdsPATH;
class gdsNODE;
class gdsSREF;

class gdscpp
{
private:
	map<int, string> GDSkeys;
	vector<gdsSTR> STR;
	int GDSrecord2ASCII(char *recIn);

public:
	gdscpp();	// Function to map keys
	~gdscpp(){};

		void setSTR(vector<gdsSTR>& exVec){STR = exVec;};
		void getSTR(vector<gdsSTR>& exVec){exVec = STR;};
		// int gdsRead(string fileName);
		// int gdsWrite(string fileName);

	int quick2ASCII(string fileName); // does not store data, legacy code

	void to_str();
};

class gdsSTR{
	private:

	public:
		gdsSTR();
		~gdsSTR(){};

		string name = "\0";
		vector<gdsSREF> SREF;
		vector<gdsBOUNDARY> BOUNDARY;
		vector<gdsPATH> PATH;
		vector<gdsNODE> NODE;

};

class gdsSREF{
	private:

	public:
		gdsSREF();
		~gdsSREF(){};

		unsigned int sname = 0;
		bitset<16> STRANS;
		double angle = 0;
		double scale = 1;
		int xCor;
		int yCor;
};

class gdsBOUNDARY{
	private:

	public:
		gdsBOUNDARY();
		~gdsBOUNDARY(){};

		unsigned int layer = 0;
		unsigned int dataType = 0;
		vector<int> xCor;
		vector<int> yCor;
};

class gdsPATH{
	private:

	public:
		gdsPATH();
		~gdsPATH(){};

		unsigned int layer = 0;
		unsigned int dataType = 0;
		vector<int> xCor;
		vector<int> yCor;
};

class gdsNODE{
	private:

	public:
		gdsNODE();
		~gdsNODE(){};

		unsigned int plex = 0;
		unsigned int layer = 0;
		unsigned int nodetype = 0;
		vector<int> xCor;
		vector<int> yCor;
		// int xCor = 0;
		// int yCor = 0;
		unsigned int propattr = 0;
		string propvalue = "\0";
};

#endif