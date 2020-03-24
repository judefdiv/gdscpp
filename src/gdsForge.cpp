/**
 * Author:		  J.F. de Villiers & H.F. Herbst
 * Origin:  	  E&E Engineering - Stellenbosch University
 * For:			    Supertools, Coldflux Project - IARPA
 * Created: 	  2019-08-26
 * Modified:
 * license:     MIT License
 * Description: Creates GDS files using the gdsClass
 * File:		    gdsForge.cpp
 */

#include "gdsForge.hpp"
using namespace std;
/**
 * Constructor
 */

gdsForge::gdsForge() {}

/**
 * [gdsForge::gdsCreate - Generates/creates/exports the GDS file]
 * @param  FileName [The file name of the to be created GDS file]
 * @param  inVec    [The vector of GDS structure to be created into the GDS
 * file]
 * @param  double   [The scale the GDS file must use]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */
int gdsForge::gdsCreate(const string &FileName, vector<gdsSTR> &inVec,
                        double units[2])
{
  this->STR = inVec;

  // Initializing the writing
  this->fileName = FileName;
  gdsFile = fopen(FileName.c_str(), "wb");

  // if(!gdsFile.is_open()){
  // 	cout << "Error: GDS file \"" << FileName << "\" FAILED to be opened for
  // writing." << endl;

  this->gdsBegin();

  for (unsigned int i = 0; i < this->GDSfileNameToBeImport.size(); i++) {
    this->gdsCopyFile(this->GDSfileNameToBeImport[i]);
  }

  bool minimal = true;

  for (const auto &gds_str : this->STR) {
    // Start of the structure
    this->gdsStrStart(gds_str.name);
    // References
    for (const auto &gds_sref : gds_str.SREF) {
      this->gdsSRef(gds_sref, minimal);
    }
    // Array References
    for (const auto &gds_aref : gds_str.AREF) {
      this->gdsARef(gds_aref, minimal);
    }
    // Boundaries
    for (const auto &gds_boundary : gds_str.BOUNDARY) {
      this->gdsBoundary(gds_boundary, minimal);
    }
    // Paths
    for (const auto &gds_path : gds_str.PATH) {
      this->gdsPath(gds_path, minimal);
    }
    // Nodes
    for (const auto &gds_node : gds_str.NODE) {
      this->gdsNode(gds_node, minimal);
    }
    // Texts
    for (const auto &gds_text : gds_str.TEXT) {
      this->gdsText(gds_text, minimal);
    }
    this->gdsStrEnd();
  }

  this->gdsEnd();

  // Terminating the writing
  fclose(gdsFile);
  cout << "Creating \"" << this->fileName << "\" done." << endl;

  return 0;
}

/***********************************************************************************
 ********************* Functions to easily draw in GDSfiles
 *************************
 ***********************************************************************************/

/**
 * [drawBoundary - Easily create a GDS boundary]
 * @param  layer [The layer number]
 * @param  corX  [The X-coordinates]
 * @param  corY  [The Y-coordinates]
 * @return       [Class of GDS boundary which can be used in a GDS structure]
 */
gdsBOUNDARY drawBoundary(int layer, vector<int> &corX, vector<int> &corY)
{
  gdsBOUNDARY foo;

  foo.layer = layer;
  foo.xCor = corX;
  foo.yCor = corY;
  // foo.dataType = 1;

  return foo;
}

/**
 * [drawPath - Easily create a GDS path]
 * @param  layer 		[The layer number]
 * @param  width 		[The thickness of the track]
 * @param  corX  		[The X-coordinates]
 * @param  corY  		[The Y-coordinates]
 * @return       		[Class of GDS path which can be used in a GDS
 * structure]
 */
gdsPATH drawPath(int layer, unsigned int width, vector<int> &corX,
                 vector<int> &corY)
{
  gdsPATH foo;

  foo.layer = layer;
  foo.width = width;
  foo.xCor = corX;
  foo.yCor = corY;
  // foo.pathtype = path_type;

  return foo;
}

/**
 * [draw2ptBox - Draws a boundary box with 2 points]
 * @param  layer [The layer number]
 * @param  blX   [Bottom Left X coordinate]
 * @param  blY   [Bottom Left Y coordinate]
 * @param  trX   [Top Right X coordinate]
 * @param  trY   [Top Right Y coordinate]
 * @return       [Class of GDS boundary which can be used in a GDS structure]
 */
gdsBOUNDARY draw2ptBox(int layer, int blX, int blY, int trX, int trY)
{
  vector<int> ptsX;
  vector<int> ptsY;

  ptsX.resize(5);
  ptsY.resize(5);

  ptsX[0] = blX;
  ptsY[0] = blY;
  ptsX[1] = trX;
  ptsY[1] = blY;
  ptsX[2] = trX;
  ptsY[2] = trY;
  ptsX[3] = blX;
  ptsY[3] = trY;
  ptsX[4] = blX;
  ptsY[4] = blY;

  return drawBoundary(layer, ptsX, ptsY);
}

/**
 * [drawSREF - Draws a structure using a reference]
 * @param  STRname [The name of the structure that must be referenced]
 * @param  Xcor    [X coordinate]
 * @param  Ycor    [Y coordinate]
 * @return         [Class of GDS s-reference which can be used in a GDS
 * structure]
 */
gdsSREF drawSREF(const string &STRname, int Xcor, int Ycor)
{
  gdsSREF foo;

  foo.name = STRname;
  foo.xCor = Xcor;
  foo.yCor = Ycor;

  return foo;
}

/**
 * [drawSREF - A more complex functions that generates a structure reference]
 * @param  STRname [The name of the structure that must be referenced]
 * @param  Xcor    [X coordinate]
 * @param  Ycor    [Y coordinate]
 * @param  angle   [The angle at which the reference should be]
 * @param  mag     [The scale of what the reference should be]
 * @param  mirror  [Is the structure mirrored]
 * @return         [description]
 */
gdsSREF drawSREF(const string &STRname, int Xcor, int Ycor, double angle,
                 double mag, bool mirror)
{
  gdsSREF foo;

  foo.name = STRname;
  foo.xCor = Xcor;
  foo.yCor = Ycor;
  foo.scale = mag;
  foo.angle = angle;
  foo.reflection = mirror;

  return foo;
}

/***********************************************************************************
 **************************** UpperGround Level
 *************************************
 ***********************************************************************************/

/**
 * [gdsForge::gdsBegin - Starts of the GDS file with the correct stuffs]
 */
void gdsForge::gdsBegin()
{
  int tempArr[1];

  tempArr[0] = 600;
  this->GDSwriteInt(GDS_HEADER, tempArr, 1);

  this->GDSwriteInt(GDS_BGNLIB, gsdTime(), 12);

  this->GDSwriteStr(GDS_LIBNAME, "ColdFlux.db");

  this->GDSwriteUnits();
  // Precision error, ultra fine...
  // double dArr[2] = {0.001, 1e-9};
  // GDSwriteRea(GDS_UNITS, dArr, 2);
}

/**
 * [gdsForge::gdsEnd - End the GDS file]
 */
void gdsForge::gdsEnd()
{
  this->GDSwriteRec(GDS_ENDLIB);
}

/**
 * [gdsForge::gdsStrStart - Outputs the correct GDS records to start a
 * structure]
 * @param strName [The name of the structure]
 */
void gdsForge::gdsStrStart(const string &strName)
{
  this->GDSwriteInt(GDS_BGNSTR, gsdTime(), 12);
  this->GDSwriteStr(GDS_STRNAME, strName);
}

/**
 * [gdsForge::gdsStrEnd - Outputs the correct GDs records to end a structure]
 */
void gdsForge::gdsStrEnd()
{
  this->GDSwriteRec(GDS_ENDSTR);
}

/**
 * [gdsForge::gdsPath - Create the PATH structure]
 * @param in_PATH [The class containing the PATH structure]
 * @param minimal [If true only the minimal is PATH structure is created]
 */
void gdsForge::gdsPath(const gdsPATH &in_PATH, bool minimal)
{
  int data[1];
  int corXY[in_PATH.xCor.size() * 2];

  this->GDSwriteRec(GDS_PATH);

  // ELFLAGS and PLEX are optional

  // Layer number, 0 to 63
  data[0] = in_PATH.layer;
  this->GDSwriteInt(GDS_LAYER, data, 1);

  // unimportant, set to zero.
  data[0] = in_PATH.dataType;
  this->GDSwriteInt(GDS_DATATYPE, data, 1);

  if (minimal) { // true
    data[0] = 2;
    this->GDSwriteInt(GDS_PATHTYPE, data, 1);
  } else { // false
    data[0] = in_PATH.pathtype;
    this->GDSwriteInt(GDS_PATHTYPE, data, 1);
  }

  // Width of tracks
  data[0] = in_PATH.width;
  this->GDSwriteInt(GDS_WIDTH, data, 1);

  // XY coordinates
  // boundary must be closed, first and last coordinate must be the same
  // minimum of 4 points(triangle)
  for (int i = 0; i < in_PATH.xCor.size() * 2; i++) {
    if (i % 2) {
      corXY[i] = in_PATH.yCor[i / 2];
    } else {
      corXY[i] = in_PATH.xCor[i / 2];
    }
  }

  this->GDSwriteInt(GDS_XY, corXY, in_PATH.xCor.size() * 2);

  // Optional goodies
  if (!minimal) { // false
    data[0] = in_PATH.propattr;
    this->GDSwriteInt(GDS_PROPATTR, data, 1);

    this->GDSwriteStr(GDS_PROPVALUE, in_PATH.propvalue);
  }

  this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsForge::gdsBoundary - Create the BOUNDARY structure]
 * @param in_BOUNDARY [The class containing the BOUNDARY structure]
 * @param minimal [If true only the minimal is BOUNDARY structure is created]
 */
void gdsForge::gdsBoundary(const gdsBOUNDARY &in_BOUNDARY, bool minimal)
{
  int data[1];
  int corXY[in_BOUNDARY.xCor.size() * 2];

  this->GDSwriteRec(GDS_BOUNDARY);

  // ELFLAGS and PLEX are optional

  // Layer number, 0 to 63
  data[0] = in_BOUNDARY.layer;
  this->GDSwriteInt(GDS_LAYER, data, 1);

  // unimportant, set to zero.
  data[0] = in_BOUNDARY.dataType;
  this->GDSwriteInt(GDS_DATATYPE, data, 1);

  // XY coordinates
  // boundary must be closed, first and last coordinate must be the same
  // minimum of 4 points(triangle)

  for (int i = 0; i < in_BOUNDARY.xCor.size() * 2; i++) {
    if (i % 2) {
      // cout << "corY[" << i/2  << "]: " << corY[i/2] << endl;
      corXY[i] = in_BOUNDARY.yCor[i / 2];
    } else {
      // cout << "corX[" << i/2  << "]: " << corX[i/2] << endl;
      corXY[i] = in_BOUNDARY.xCor[i / 2];
    }
  }

  this->GDSwriteInt(GDS_XY, corXY, in_BOUNDARY.xCor.size() * 2);

  // Optional goodies
  if (!minimal) { // false
    data[0] = in_BOUNDARY.propattr;
    this->GDSwriteInt(GDS_PROPATTR, data, 1);

    this->GDSwriteStr(GDS_PROPVALUE, in_BOUNDARY.propvalue);
  }

  this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsForge::gdsSRef - Create the SREF structure]
 * @param in_SREF [The class containing the SREF structure]
 * @param minimal [If true only the minimal is SREF structure is created]
 */
void gdsForge::gdsSRef(const gdsSREF &in_SREF, bool minimal)
{
  int data[1];
  this->GDSwriteRec(GDS_SREF);

  // ELFLAGS and PLEX are optional

  // name of the structure that is referenced
  this->GDSwriteStr(GDS_SNAME, in_SREF.name);

  // two byte, bit array controlling reflection, magnification and rotation.
  bitset<16> bits;

  // reflect across x-axis
  bits.set(15, in_SREF.reflection);

  // magnification, default 1
  if (in_SREF.scale == 1)
    bits.set(2, 0);
  else
    bits.set(2, 1);

  // angle of rotation
  if (in_SREF.angle == 0)
    bits.set(1, 0);
  else
    bits.set(1, 1);

  this->GDSwriteBitArr(GDS_STRANS, bits);

  double arrDou[1];
  // Magnification, default 1
  if (in_SREF.scale != 1) {
    arrDou[0] = in_SREF.scale;
    this->GDSwriteRea(GDS_MAG, arrDou, 1);
  }

  // Angle measured counterclockwise
  if (in_SREF.angle != 0) {
    arrDou[0] = in_SREF.angle;
    this->GDSwriteRea(GDS_ANGLE, arrDou, 1);
  }

  int corXY[2];
  corXY[0] = in_SREF.xCor;
  corXY[1] = in_SREF.yCor;
  this->GDSwriteInt(GDS_XY, corXY, 2);

  // Optional goodies
  if (!minimal) {
    data[0] = in_SREF.propattr;
    this->GDSwriteInt(GDS_PROPATTR, data, 1);

    this->GDSwriteStr(GDS_PROPVALUE, in_SREF.propvalue);
  }

  this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsForge::gdsARef - Create the AREF structure]
 * @param in_SREF [The class containing the AREF structure]
 * @param minimal [If true only the minimal is AREF structure is created]
 */
void gdsForge::gdsARef(const gdsAREF &in_AREF, bool minimal)
{
  int data[1];
  this->GDSwriteRec(GDS_SREF);

  // ELFLAGS and PLEX are optional
  if (!minimal) {
    data[0] = in_AREF.plex;
    this->GDSwriteInt(GDS_PLEX, data, 1);
  }

  // name of the structure that is referenced
  this->GDSwriteStr(GDS_SNAME, in_AREF.name);

  // two byte, bit array controlling reflection, magnification and rotation.
  bitset<16> bits;

  // reflect across x-axis
  bits.set(15, in_AREF.reflection);

  // magnification, default 1
  if (in_AREF.scale == 1)
    bits.set(2, 0);
  else
    bits.set(2, 1);

  // angle of rotation
  if (in_AREF.angle == 0)
    bits.set(1, 0);
  else
    bits.set(1, 1);

  this->GDSwriteBitArr(GDS_STRANS, bits);

  double arrDou[1];
  // Magnification, default 1
  if (in_AREF.scale != 1) {
    arrDou[0] = in_AREF.scale;
    this->GDSwriteRea(GDS_MAG, arrDou, 1);
  }

  // Angle measured counterclockwise
  if (in_AREF.angle != 0) {
    arrDou[0] = in_AREF.angle;
    this->GDSwriteRea(GDS_ANGLE, arrDou, 1);
  }

  int colrow[2];
  colrow[0] = in_AREF.colCnt;
  colrow[1] = in_AREF.rowCnt;
  this->GDSwriteInt(GDS_COLROW, colrow, 2);

  int corXY[6];
  corXY[0] = in_AREF.xCor;
  corXY[1] = in_AREF.yCor;
  corXY[2] = in_AREF.xCorRow;
  corXY[3] = in_AREF.yCorRow;
  corXY[4] = in_AREF.xCorCol;
  corXY[5] = in_AREF.yCorCol;
  this->GDSwriteInt(GDS_XY, corXY, 6);

  // Optional goodies
  if (!minimal) {
    data[0] = in_AREF.propattr;
    this->GDSwriteInt(GDS_PROPATTR, data, 1);
    this->GDSwriteStr(GDS_PROPVALUE, in_AREF.propvalue);
  }

  this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsForge::gdsSRef - Create the NODE structure]
 * @param gdsNODE [The class containing the NODE structure]
 * @param minimal [If true only the minimal is NODE structure is created]
 */
void gdsForge::gdsNode(const gdsNODE &in_NODE, bool minimal)
{
  int data[1];
  int corXY[in_NODE.xCor.size() * 2];

  this->GDSwriteRec(GDS_NODE);

  // ELFLAGS and PLEX are optional
  if (!minimal) { // false
    data[0] = in_NODE.plex;
    this->GDSwriteInt(GDS_PLEX, data, 1);
  }

  // Layer number, 0 to 63
  data[0] = in_NODE.layer;
  this->GDSwriteInt(GDS_LAYER, data, 1);

  data[0] = in_NODE.nodetype;
  this->GDSwriteInt(GDS_NODETYPE, data, 1);

  // XY coordinates
  // boundary must be closed, first and last coordinate must be the same
  // minimum of 4 points(triangle)

  for (int i = 0; i < in_NODE.xCor.size() * 2; i++) {
    if (i % 2) {
      // cout << "corY[" << i/2  << "]: " << corY[i/2] << endl;
      corXY[i] = in_NODE.yCor[i / 2];
    } else {
      // cout << "corX[" << i/2  << "]: " << corX[i/2] << endl;
      corXY[i] = in_NODE.xCor[i / 2];
    }
  }

  this->GDSwriteInt(GDS_XY, corXY, in_NODE.xCor.size() * 2);

  // Optional goodies
  if (!minimal) { // false
    data[0] = in_NODE.propattr;
    this->GDSwriteInt(GDS_PROPATTR, data, 1);

    this->GDSwriteStr(GDS_PROPVALUE, in_NODE.propvalue);
  }

  this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsForge::gdsText - Create the TEXT structure]
 * @param gdsTEXT [The class containing the TEXT structure]
 * @param minimal [If true only the minimal is TEXT structure is created]
 */
void gdsForge::gdsText(const gdsTEXT &in_TEXT, bool minimal)
{
  int data[1];
  this->GDSwriteRec(GDS_TEXT);

  // ELFLAGS and PLEX are optional

  // PRESENTATION; PATHTYPE, WIDTH, STRANS, MAG, and ANGLE are optional

  // Layer number, 0 to 63
  data[0] = in_TEXT.layer;
  this->GDSwriteInt(GDS_LAYER, data, 1);

  data[0] = in_TEXT.text_type;
  this->GDSwriteInt(GDS_TEXTTYPE, data, 1);

  int corXY[2];
  corXY[0] = in_TEXT.xCor;
  corXY[1] = in_TEXT.yCor;
  this->GDSwriteInt(GDS_XY, corXY, 2);

  // text/string to be used of the structure that is referenced
  this->GDSwriteStr(GDS_STRING, in_TEXT.textbody);

  // Optional goodies
  if (!minimal) { // false
    data[0] = in_TEXT.propattr;
    this->GDSwriteInt(GDS_PROPATTR, data, 1);
    this->GDSwriteStr(GDS_PROPVALUE, in_TEXT.propvalue);
  }

  this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsForge::gdsCopyFile - Copies the structure from a GDS file to another]
 * @param  fileName [Name of file to be imported]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */
int gdsForge::gdsCopyFile(const string &fileName)
{
  cout << "Copying GDS binaries from \"" << fileName << "\"" << endl;

  ifstream gdsFileIn;

  gdsFileIn.open(fileName, ios::in | ios::binary);

  if (!gdsFileIn.is_open()) {
    cout << "FAILED to open GDS file \"" << fileName << "\"" << endl;
    return 1;
  }

  char *readBlk;
  uint32_t sizeBlk;
  uint32_t hexKey;
  bool cpEN = false;
  bool firstLine = true;
  string strName;
  static set<string> StoredStr;

  gdsFileIn.seekg(0, ios::beg);

  do {
    readBlk = new char[2];
    gdsFileIn.read(readBlk, 2); // Get the size of the record
    sizeBlk = (((unsigned char)readBlk[0] << 8) |
               (unsigned char)readBlk[1]); // Interpret the size of the record

    gdsFileIn.seekg(-2, ios::cur);
    readBlk = new char[sizeBlk]; // Reading in the full length of the record
                                 // excluding the size
    gdsFileIn.read(readBlk, sizeBlk);

    hexKey = (((unsigned char)readBlk[2] << 8) | (unsigned char)readBlk[3]);

    if (hexKey == GDS_BGNSTR) { // Beginning of structure
      cpEN = true;
      firstLine = true;
    }

    if (cpEN && hexKey == GDS_STRNAME) { // get the name of the structure
      strName = "";
      for (uint8_t i = 4; i < sizeBlk; i++) {
        strName = strName + readBlk[i];
      }

      if (StoredStr.find(strName) != StoredStr.end()) { // found
        cout << "Structure: \"" << strName << "\""
             << " already copied." << endl;
        cpEN = false;
      } else { // didnt find
        cout << "Structure: \"" << strName << "\""
             << " copying" << endl;
        StoredStr.insert(strName);
        this->GDSwriteInt(GDS_BGNSTR, gsdTime(), 12);
      }

      firstLine = false;
    }

    if (cpEN && !firstLine) { // Write(copy) the GSD record
      fwrite(readBlk, 1, sizeBlk, this->gdsFile);
    }

    if (hexKey == GDS_ENDSTR) { // end of structure
      cpEN = false;
    }

  } while (hexKey != GDS_ENDLIB);

  delete[] readBlk;

  gdsFileIn.close();
  cout << "Copying GDS binaries of \"" << fileName << "\" done." << endl;
  return 0;
}

/***********************************************************************************
 **************************** UnderGround Level
 *************************************
 ***********************************************************************************/

/**
 * [GDSwriteInt - Writes integer values to file]
 * @param record [GDS record type]
 * @param arrInt [Array of integers to be written]
 * @param cnt    [Amount of integers to be written]
 * @return 			 [0 - Exit Success; 1 - Exit Failure]
 */
int gdsForge::GDSwriteInt(int record, int arrInt[], int cnt)
{
  unsigned int dataSize = record & 0xff;

  if (dataSize == 0x02 && cnt > 0) {
    dataSize = 2; // should/could be omitted
  } else if (dataSize == 0x03 && cnt > 0) {
    dataSize = 4;
  } else if (dataSize == 0x00 && cnt == 0) {
    dataSize = 0; // should/could be omitted
  } else {
    cout << "Incorrect parameters for record: 0x" << hex << record << endl;
    cout << dec;
    return 1;
  }

  unsigned int sizeByte = cnt * dataSize + 4;
  unsigned char OHout[4];

  OHout[0] = sizeByte >> 8 & 0xff;
  OHout[1] = sizeByte & 0xff;
  OHout[2] = record >> 8 & 0xff;
  OHout[3] = record & 0xff;
  fwrite(OHout, 1, 4, this->gdsFile);

  unsigned char dataOut[dataSize];

  for (int i = 0; i < cnt; i++) {
    for (unsigned int j = 0; j < dataSize; j++) {
      dataOut[j] = arrInt[i] >> (((dataSize - 1) * 8) - (j * 8)) & 0xff;
    }
    fwrite(dataOut, 1, dataSize, this->gdsFile);
  }

  return 0;
}

/**
 * [gdsForge::GDSwriteStr - Writes a string value to file]
 * @param  record [GDS record type]
 * @param  inStr  [The string to be written]
 * @return 				[0 - Exit Success; 1 - Exit Failure]
 */
int gdsForge::GDSwriteStr(int record, string inStr)
{
  if ((record & 0xff) != 0x06) {
    cout << "Incorrect record: 0x" << hex << record << endl;
    cout << dec;
    return 1;
  }
  unsigned char OHout[4];

  if (inStr.length() % 2 == 1) {
    // therefore odd
    inStr.push_back('\0');
  }

  int lenStr = inStr.length();

  OHout[0] = ((lenStr + 4) >> 8) & 0xff;
  OHout[1] = (lenStr + 4) & 0xff;
  OHout[2] = record >> 8 & 0xff;
  OHout[3] = record & 0xff;
  fwrite(OHout, 1, 4, this->gdsFile);

  char dataOut[lenStr];
  strcpy(dataOut, inStr.c_str());
  fwrite(dataOut, 1, lenStr, this->gdsFile);

  return 0;
}

/**
 * [gdsForge::GDSwriteBitArr - Write 16 bit array in GDS format]
 * @param  record [GDS record type]
 * @param  inBits [Bits that must be written]
 * @return 				[0 - Exit Success; 1 - Exit Failure]
 */
int gdsForge::GDSwriteBitArr(int record, bitset<16> inBits)
{
  if ((record & 0xff) != 0x01) {
    cout << "Incorrect record: 0x" << hex << record << endl;
    cout << dec;
    return 1;
  }
  unsigned char OHout[4];

  OHout[0] = 0 & 0xff;
  OHout[1] = (2 + 4) & 0xff;
  OHout[2] = record >> 8 & 0xff;
  OHout[3] = record & 0xff;
  fwrite(OHout, 1, 4, this->gdsFile);

  unsigned char dataOut[2] = {0, 0};

  for (int i = 15; i >= 8; i--) {
    dataOut[0] = dataOut[0] | (inBits[i] << (i - 8));
  }
  for (int i = 8; i > 0; i--) {
    dataOut[1] = dataOut[1] | (inBits[i] << i);
  }

  fwrite(dataOut, 1, 2, this->gdsFile);

  return 0;
}

/**
 * [gdsForge::GDSwriteRea - Writes doubles values to file]
 * @param record [GDS record type]
 * @param arrInt [Array of doubles to be written]
 * @param cnt    [Amount of doubles to be written]
 * @return 		   [0 - Exit Success; 1 - Exit Failure]
 */
int gdsForge::GDSwriteRea(int record, double arrInt[], int cnt)
{
  unsigned int dataSize = record & 0xff;
  unsigned long long realVal;

  if (dataSize == 0x05 && cnt > 0) {
    dataSize = 8; // should/could be omitted
  } else {
    cout << "Incorrect parameters for record: 0x" << hex << record << endl;
    cout << dec;
    return 1;
  }

  unsigned int sizeByte = cnt * dataSize + 4;
  unsigned char OHout[4];

  OHout[0] = sizeByte >> 8 & 0xff;
  OHout[1] = sizeByte & 0xff;
  OHout[2] = record >> 8 & 0xff;
  OHout[3] = record & 0xff;
  fwrite(OHout, 1, 4, this->gdsFile);

  unsigned char dataOut[dataSize];

  for (int i = 0; i < cnt; i++) {
    realVal = GDSfloatCalc(arrInt[i]);
    for (unsigned int j = 0; j < dataSize; j++) {
      // dataOut[j] = realVal >> (((dataSize-1)*8) - (j*8)) & 0xff;
      dataOut[j] = realVal >> (56 - (j * 8)) & 0xff;
    }
    fwrite(dataOut, 1, dataSize, this->gdsFile);
  }

  return 0;
}

/**
 * [gdsForge::GDSwriteUnits - Hard coding the UNIT record for precision]
 */
void gdsForge::GDSwriteUnits()
{
  unsigned char data[20];

  data[0] = 0x00;
  data[1] = 0x14;
  data[2] = 0x03;
  data[3] = 0x05;
  data[4] = 0x3e;
  data[5] = 0x41;
  data[6] = 0x89;
  data[7] = 0x37;
  data[8] = 0x4b;
  data[9] = 0xc6;
  data[10] = 0xa7;
  data[11] = 0xf0;
  data[12] = 0x39;
  data[13] = 0x44;
  data[14] = 0xb8;
  data[15] = 0x2f;
  data[16] = 0xa0;
  data[17] = 0x9b;
  data[18] = 0x5a;
  data[19] = 0x50;

  fwrite(data, 1, 20, this->gdsFile);
}

/**
 * [gdsForge::GDSwriteRec - Writes record value to file]
 * @param record [GDS record type]
 * @return 			 [0 - Exit Success; 1 - Exit Failure]
 */
int gdsForge::GDSwriteRec(int record)
{
  unsigned char OHout[4];

  OHout[0] = 0;
  OHout[1] = 4;
  OHout[2] = record >> 8 & 0xff;
  OHout[3] = record & 0xff;

  if (OHout[3] != 0) {
    cout << "The smoke has escaped. The record must be dataless" << endl;
    return 1;
  }

  fwrite(OHout, 1, 4, this->gdsFile);

  return 0;
}