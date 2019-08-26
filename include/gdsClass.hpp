/**
 * Author:  		
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-22
 * Modified:
 * license: 
 * Description: .
 * File:				gdsClass.hpp
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
	gdscpp();
	~gdscpp(){};

	int gdsRead(string fileName);
	int gdsWrite(string fileName);

	int quick2ASCII(string fileName); // does not store data, legacy code

	void to_str();
};

class gdsSTR
{
private:
	string name = "\0";
	vector<gdsSREF> SREF;
	vector<gdsBOUNDARY> BOUNDARY;
	vector<gdsPATH> PATH;
	vector<gdsNODE> NODE;

public:
	gdsSTR();
	~gdsSTR(){};
};

class gdsSREF
{
private:
	unsigned int sname = 0;
	// bitset STRANS; 					// 2 bytes
	float angle = 0; // double?
	long long xCor;
	long long yCor;

public:
	gdsSREF();
	~gdsSREF(){};
};

class gdsBOUNDARY
{
private:
	unsigned int layer = 0;
	unsigned int dataType = 0;
	vector<long long> xCor;
	vector<long long> yCor;

public:
	gdsBOUNDARY();
	~gdsBOUNDARY(){};
};

class gdsPATH
{
private:
	unsigned int layer = 0;
	unsigned int dataType = 0;
	vector<long long> xCor;
	vector<long long> yCor;

public:
	gdsPATH();
	~gdsPATH(){};
};

class gdsNODE
{
private:
	unsigned int plex = 0;
	unsigned int layer = 0;
	unsigned int nodetype = 0;
	vector<long long> xCor;
	vector<long long> yCor;
	unsigned int propattr = 0;
	string propvalue = "\0";

public:
	gdsNODE();
	~gdsNODE(){};
};

#endif