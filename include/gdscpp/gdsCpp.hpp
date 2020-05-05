/**
 * Authors:     J.F. de Villiers & H.F. Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license:     MIT License
 * Description: Contains all the class definitions for the GdsCpp Library
 * File:				gdsCpp.hpp
 */

#ifndef gdsCpp
#define gdsCpp

// ==================== Class declarations =====================
class gdscpp;      // All encompassing master class
class gdsSTR;      // Subclass containing all structures
class gdsBOUNDARY; // x2subclass belonging to gdsSTR
class gdsPATH;     // ''
class gdsSREF;     // ''
class gdsAREF;     // ''
class gdsTEXT;     // ''
class gdsNODE;     // ''
class gdsBOX;      // ''

// ========================== Includes ========================
#include "gdsForge.hpp"
#include "gdsParser.hpp"
#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

// ================== Function Declarations ===================

int gdsToText(const std::string &fileName);
int gdsRecordToText(char *recIn);

// ================= Structure Declarations ===================

// ===================== Class Definitions ====================

/*
 * [gdscpp - This is the highest class, holding all the information of a single
 * .gds file.]
 */
class gdscpp
{ // (GDS file)
private:
  int version_number = 7;             // GDS version number. Default to 7
  int generations = 3;                // Default generations. Don't really use
  int highest_heirarchical_level = 0; // Zero_indexed
  double units[2] = {0.001, 1e-9};    // micron default
  // std::vector<gdsSTR> STR; // Holds all the structures of the gds file
  std::unordered_map<std::string, int>
      STR_Lookup;                 // Allows for searching by name
  std::vector<int> last_modified; // TODO: Default to current datetime
  std::string library_name = "Untitled_library"; // Default libname

  std::vector<std::string> GDSfileName;

public:
  gdscpp(){};
  ~gdscpp(){};

  std::vector<gdsSTR> STR; // Holds all the structures of the gds file

  void setSTR(gdsSTR target_structure); // <-- to be replaced with push_back_STR
  void setSTR(std::vector<gdsSTR> target_structure); // to be replaced
  void push_back_STR(
      gdsSTR target_structure); // <-- to be replaced with push_back_STR
  void push_back_STR(std::vector<gdsSTR> target_structure); // to be replaced

  void getSTR(std::vector<gdsSTR> &exVec,
              std::unordered_map<std::string, int> &exMap)
  {
    exVec = STR;
    exMap = STR_Lookup;
  };
  int get_highest_heirarchical_level()
  {
    return highest_heirarchical_level;
  };
  int import(std::string fileName);
  bool check_name(std::string name, std::vector<std::string> ref_vector);

  int resolve_heirarchy_and_bounding_boxes();
  int calculate_STR_bounding_box(int structure_index, int *destination);
  int fetch_boundary_bounding_box(gdsBOUNDARY target_boundary,
                                  int *destination);
  int fetch_box_bounding_box(gdsBOX target_box, int *destination);

  int write(const std::string &fileName);
  double get_database_units();
  double get_database_units_in_m();
  void to_str();
  void bb_to_str();
  void reset();
  void importGDSfile(const std::string &fileName)
  {
    this->GDSfileName.push_back(fileName);
  }

  std::vector<unsigned int> findRootSTR();
  int genDot(const std::string &fileName);
};

/*
 * [gdsSTR - Subclass of gdscpp which holds a structure's information.]
 */
class gdsSTR
{
private:
public:
  gdsSTR(){};
  ~gdsSTR(){};

  void to_str();
  void reset();

  std::string name = "\0";

  unsigned int heirarchical_level = 0;
  int bounding_box[4] = {
      0, 0, 0, 0}; // x1,y1,x2,y2 (minimum number of points to represent box)

  void bb_cout()
  {
    std::cout << name << " BBox: " << bounding_box[0] << ", ";
    std::cout << bounding_box[1] << "; ";
    std::cout << bounding_box[2] << ", ";
    std::cout << bounding_box[3] << std::endl;
  };

  std::vector<int> last_modified;
  std::vector<gdsBOUNDARY> BOUNDARY;
  std::vector<gdsPATH> PATH;
  std::vector<gdsSREF> SREF;
  std::vector<gdsAREF> AREF;
  std::vector<gdsTEXT> TEXT;
  std::vector<gdsNODE> NODE;
  std::vector<gdsBOX> BOX;
};

/*
 * [gdsBOUNDARY - Subclass of a gdsSTR class which stores boundary structures.]
 */
class gdsBOUNDARY
{
private:
public:
  gdsBOUNDARY(){};
  ~gdsBOUNDARY(){};

  void to_str();
  void reset();

  int plex = 0;
  unsigned int layer = 0;
  unsigned int dataType = 0;
  std::vector<int> xCor;
  std::vector<int> yCor;
  // Property attribute seldom used. Valid range specified from 1 to  127
  // If value is 0, no attribute was set
  unsigned int propattr = 0;
  std::string propvalue = "\0";
};

/*
 * [gdsPATH - Subclass of a gdsSTR class which stores path structures.]
 * Pathtype: This record contains a value that describes the type of path
 * endpoints. The value is 0 for square-ended paths that endflush with their
 * endpoints 1 for round-ended paths 2 for square-ended paths that extend a
 * half-width beyond their endpoints
 */
class gdsPATH
{
private:
public:
  gdsPATH(){};
  ~gdsPATH(){};

  void to_str();
  void reset();

  int plex = 0;
  unsigned int layer = 0;
  unsigned int dataType = 0;
  unsigned int pathtype = 0;
  unsigned int width = 0;
  std::vector<int> xCor;
  std::vector<int> yCor;
  unsigned int propattr = 0;
  std::string propvalue = "\0";
};

/*
 * [gdsSREF - Subclass of a gdsSTR class which stores structure reference
 * structures.]
 */
class gdsSREF
{
private:
public:
  gdsSREF(){};
  ~gdsSREF(){};

  void to_str();
  void reset();

  int plex = 0;
  std::string name = "\0";
  std::bitset<16> sref_flags;
  bool reflection = false; // boolean,    flag at bit 0
  double scale = 1;        // multiplier, flag at bit 13
  double angle = 0;        // degrees,    flag at bit 14
  int xCor = 0;
  int yCor = 0;
  unsigned int propattr = 0;
  std::string propvalue = "\0";
};

/*
 * [gdsAREF - Subclass of a gdsSTR class which stores array reference
 * structures.]
 */
class gdsAREF
{
private:
public:
  gdsAREF(){};
  ~gdsAREF(){};

  void to_str();
  void reset();

  int plex = 0;
  std::string name = "\0"; // SNAME
  std::bitset<16> aref_transformation_flags;
  bool reflection = false; // STRANS
  double angle = 0;        // subSTRANS
  double scale = 1;        // subSTRANS
  int colCnt = 0;
  int rowCnt = 0;
  int xCor = 0;    // X_1
  int yCor = 0;    // Y_1
  int xCorRow = 0; // X_2
  int yCorRow = 0; // Y_2
  int xCorCol = 0; // X_3
  int yCorCol = 0; // Y_3
  unsigned int propattr = 0;
  std::string propvalue = "\0";
};

/*
 * [gdsTEXT - Subclass of a gdsSTR class which stores text structures.]
 */
class gdsTEXT
{
private:
public:
  gdsTEXT(){};
  ~gdsTEXT(){};

  void to_str();
  void reset();

  /* Bits 10 and 11, taken together as a binary number,
   * specify the font (00 means font 0, 01 rneans font 1,
   * 10 means font 2, and 11 means font 3). Bits 12 and 13 specify the vertical
   * justification (00 means top, 01 means middle, and 10 means bottom). Bits 14
   * and 15 specify the horizontal justification (00 means left, 01 means
   * center, and 10 means right). Bits 0 through 9 are reserved for future use
   * and must be cleared. If this record is omitted, then top-left justification
   * and font 0 are assumed.
   */
  int plex = 0;
  std::bitset<16> presentation_flags;
  std::string textbody = "\0";
  unsigned int layer = 0;
  int text_type = 0;
  int path_type = 0;
  int width = 0;
  std::bitset<16> text_transformation_flags;
  double scale = 1;
  double angle = 0;
  int xCor = 0;
  int yCor = 0;
  unsigned int propattr = 0;
  std::string propvalue = "\0";
};

/*
 * [gdsNODE - Subclass of a gdsSTR class which stores node structures.]
 */
class gdsNODE
{
private:
public:
  gdsNODE(){};
  ~gdsNODE(){};

  void to_str();
  void reset();

  unsigned int plex = 0; // optional
  unsigned int layer = 0;
  unsigned int nodetype = 0;
  // A maximum of 50 coordinates
  std::vector<int> xCor;
  std::vector<int> yCor;
  unsigned int propattr = 0;
  std::string propvalue = "\0";
};

/*
 * [gdsBOX - Subclass of a gdsSTR class which stores box structures.]
 */
class gdsBOX
{
private:
public:
  gdsBOX(){};
  ~gdsBOX(){};

  void to_str();
  void reset();

  unsigned int plex = 0; // optional
  unsigned int layer = 0;
  unsigned int boxtype = 0;
  // A maximum of 50 coordinates
  std::vector<int> xCor;
  std::vector<int> yCor;
  unsigned int propattr = 0;
  std::string propvalue = "\0";
};

#endif
