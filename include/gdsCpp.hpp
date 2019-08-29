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
// ========================== Includes ========================
#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <map>

#include <fstream>
#include <bitset>

using namespace std;

class gdscpp;                                  // All encompassing master class
class gdsInfo;                                 // Subclass of gds file
class gdsSTR;                                  // Subclass containing all structures
class gdsBOUNDARY;                             // x2subclass belonging to gdsSTR
class gdsPATH;                                 // ''
class gdsNODE;                                 // ''
class gdsSREF;                                 // ''
class gdsTEXT;                                 // ''
class gdsNODE;                                 // ''  TODO implement class
class gdsBOX;                                  // ''  TODO implement class

#include "gdsParser.hpp"
#include "gdsForge.hpp"
#include "gdsImport.hpp"

class gdscpp{  // (GDS file)
  private:
    gdsInfo INFO;                             // Holds gds file into information
    vector<gdsSTR> STR;                       // Holds all the structures of the gds file
    int GDSrecord2ASCII(char *recIn);         // Does it belong here???   

  public:
    gdscpp();
    ~gdscpp(){};

    void setSTR(vector<gdsSTR>& exVec){STR = exVec;};
    void getSTR(vector<gdsSTR>& exVec){exVec = STR;};

    int read(string fileName);
    int write(string fileName);

    int quick2ASCII(string fileName);       // does not store data, legacy code.

    void to_str();
};

class gdsInfo{
  private:
    int version_number=7;                     // GDS version number. Default to 7
    int last_modified[6] = {0,0,0,0,0,0};     // Default to current datetime if unread
    int last_accessed[6] = {0,0,0,0,0,0};     // Default to current datetime if unread
    string library_name = "Untitled_library"; // Default libname
    int generations = 3;                      // Default generations. Don't really use
    double units[2] = {0.001, 1e-09};         // micron default 
  public:
    gdsInfo();
    ~gdsInfo(){};

    void to_str();
};

class gdsSTR{
  private:

  public:
    gdsSTR();
    ~gdsSTR(){};

    void to_str();

    string name = "\0";
    
    vector<gdsBOUNDARY> BOUNDARY;
    vector<gdsPATH> PATH;
    vector<gdsSREF> SREF;
    vector<gdsAREF> AREF;
    vector<gdsTEXT> TEXT;
    vector<gdsNODE> NODE;
    vector<gdsBOX> BOX;
    
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

class gdsAREF{
  private:

  public:
    gdsAREF();
    ~gdsAREF(){};

    void to_str();

                              // PLEX
    string name = "\0";       // SNAME
    bool reflection = false;  // STRANS
    double angle = 0;         //  subSTRANS
    double scale = 1;         //  subSTRANS
    int xCor;                 // XY
    int yCor;                 // XY
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