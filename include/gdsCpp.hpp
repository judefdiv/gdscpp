/**
 * Author:
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license:
 * Description: .
 * File:				gdsCpp.hpp
 */

#ifndef gdsCpp
#define gdsCpp

using namespace std;

class gdsSTR;
class gdsBOUNDARY;
class gdsPATH;
class gdsNODE;
class gdsSREF;
class gdsTEXT;

#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <map>
#include <fstream>
#include <bitset>
#include "gdsParser.hpp"
#include "gdsForge.hpp"
#include "gdsImport.hpp"

class gdscpp{  // (GDS file)
  private:
    vector<gdsSTR> STR;
    int GDSrecord2ASCII(char *recIn); // Does it belong here???

  public:
    gdscpp();
    ~gdscpp(){};

    void setSTR(vector<gdsSTR>& exVec){STR = exVec;};
    void getSTR(vector<gdsSTR>& exVec){exVec = STR;};

    int read(string fileName);
    int write(string fileName);

    int quick2ASCII(string fileName);   // does not store data, legacy code.

    void to_str();
};

class gdsSTR{
  private:

  public:
    gdsSTR();
    ~gdsSTR(){};

    void to_str();

    string name = "\0";
    vector<gdsSREF> SREF;
    vector<gdsBOUNDARY> BOUNDARY;
    vector<gdsPATH> PATH;
    vector<gdsNODE> NODE;
    vector<gdsTEXT> TEXT;

};

class gdsSREF{
  private:

  public:
    gdsSREF();
    ~gdsSREF(){};

    void to_str();

    string name = "\0";
    bool reflection = false;
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

    void to_str();

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

    void to_str();

    unsigned int layer = 0;
    unsigned int dataType = 0;
    unsigned int width = 0;
    vector<int> xCor;
    vector<int> yCor;
};

class gdsNODE{
  private:

  public:
    gdsNODE();
    ~gdsNODE(){};

    void to_str();

    unsigned int plex = 0;  // optional
    unsigned int layer = 0;
    unsigned int nodetype = 0;
    // A maximum of 50 coordinates
    vector<int> xCor;
    vector<int> yCor;
    unsigned int propattr = 0;
    string propvalue = "\0";
};

class gdsTEXT{
  private:

  public:
    gdsTEXT();
    ~gdsTEXT(){};

    void to_str();

    string textbody = "\0";
    unsigned int layer = 0;
    double scale = 1;
    int xCor;
    int yCor;
};

#endif