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

// ==================== Class declarations =====================
class gdscpp;                                  // All encompassing master class
class gdsSTR;                                  // Subclass containing all structures
class gdsBOUNDARY;                             // x2subclass belonging to gdsSTR
class gdsPATH;                                 // ''
class gdsSREF;                                 // ''
class gdsAREF;                                 // ''
class gdsTEXT;                                 // ''
class gdsNODE;                                 // ''
class gdsBOX;                                  // ''

// ========================== Includes ========================
#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <map>
#include <fstream>
#include <bitset>
#include "gdsParser.hpp"
#include "gdsForge.hpp"

// ===================== Class Definitions ====================
/*
This is the highest class, holding all the information of a single .gds file.
*/
class gdscpp{  // (GDS file)
  private:
    vector<gdsSTR> STR;                       // Holds all the structures of the gds file

  int version_number=7;                     // GDS version number. Default to 7
    int last_modified[6] = {0,0,0,0,0,0};     // Default to current datetime if unread
    int last_accessed[6] = {0,0,0,0,0,0};     // Default to current datetime if unread
    string library_name = "Untitled_library"; // Default libname
    int generations = 3;                      // Default generations. Don't really use
    double units[2] = {0.001, 1e-09};         // micron default

    int GDSrecord2ASCII(char *recIn);         // Does it belong here???

  public:
    gdscpp(){};
    ~gdscpp(){};

    void setSTR(vector<gdsSTR>& exVec){STR = exVec;};
    void getSTR(vector<gdsSTR>& exVec){exVec = STR;};

    int import(string fileName);
    int write(string fileName);

    int quick2ASCII(string fileName);       // does not store data, legacy code.

    void to_str();
};

/*
Subclass of gdscpp which holds a structure's information.
*/

class gdsSTR{
  private:

  public:
    gdsSTR(){};
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

/*
Subclass of a gdsSTR class which stores boundary structures.
*/
class gdsBOUNDARY{
  private:

  public:
    gdsBOUNDARY(){};
    ~gdsBOUNDARY(){};

    void to_str();

    unsigned int layer = 0;
    unsigned int dataType = 0;
    vector<int> xCor;
    vector<int> yCor;
    unsigned int propattr = 0;
    string propvalue = "\0";
};

/*
Subclass of a gdsSTR class which stores path structures.

Pathtype: This record contains a value that describes the type of path endpoints. The value is
0 for square-ended paths that endflush with their endpoints
1 for round-ended paths
2 for square-ended paths that extend a half-width beyond their endpoints
*/
class gdsPATH{
  private:

  public:
    gdsPATH(){};
    ~gdsPATH(){};

    void to_str();

    unsigned int layer = 0;
    unsigned int dataType = 0;
    unsigned int pathtype = 0;  // Shape: 0=Flush; 1=Round end; 2=square end, Half-Width
    unsigned int width = 0;
    vector<int> xCor;
    vector<int> yCor;
    unsigned int propattr = 0;
    string propvalue = "\0";
};

/*
Subclass of a gdsSTR class which stores structure reference structures.
*/

class gdsSREF{
  private:

  public:
    gdsSREF(){};
    ~gdsSREF(){};

    void to_str();

    string name = "\0";
    bool reflection = false;
    double angle = 0;
    double scale = 1;
    int xCor;
    int yCor;
    unsigned int propattr = 0;
    string propvalue = "\0";
};

/*
Subclass of a gdsSTR class which stores array reference structures.
*/

class gdsAREF{
  private:

  public:
    gdsAREF(){};
    ~gdsAREF(){};

    void to_str();

                              // PLEX
    string name = "\0";       // SNAME
    bool reflection = false;  // STRANS
    double angle = 0;         //  subSTRANS
    double scale = 1;         //  subSTRANS
    int xCor;                 // XY
    int yCor;                 // XY
    unsigned int propattr = 0;
    string propvalue = "\0";
};

/*
Subclass of a gdsSTR class which stores text structures.
*/

class gdsTEXT{
  private:

  public:
    gdsTEXT(){};
    ~gdsTEXT(){};

    void to_str();

    string textbody = "\0";
    unsigned int layer = 0;
    double scale = 1;
    int xCor;
    int yCor;
    unsigned int propattr = 0;
    string propvalue = "\0";
};

/*
Subclass of a gdsSTR class which stores node structures.
*/
class gdsNODE{
  private:

  public:
    gdsNODE(){};
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

/*
Subclass of a gdsSTR class which stores box structures.
*/
class gdsBOX{
  private:

  public:
    gdsBOX(){};
    ~gdsBOX(){};

    void to_str();

    unsigned int plex = 0;  // optional
    unsigned int layer = 0;
    unsigned int boxtype = 0;
    // A maximum of 50 coordinates
    vector<int> xCor;
    vector<int> yCor;
    unsigned int propattr = 0;
    string propvalue = "\0";
};

#endif