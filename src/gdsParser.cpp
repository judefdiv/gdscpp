/**
 * Author:		  J.F. de Villiers & H.F. Herbst
 * Origin:  	  E&E Engineering - Stellenbosch University
 * For:			    Supertools, Coldflux Project - IARPA
 * Created: 	  2019-03-20
 * Modified:
 * license:     MIT License
 * Description: Library containing GDSII reading and writing functions.
 * File:		    gdsParser.cpp
 */

// ========================= Includes =========================
#include "gdsParser.hpp"

// ====================== Miscellanious =======================
using namespace std;
/**
 * [GDSdistill - Reads a single record and outputs it to the appropriate
 * variable]
 * @param  recIn   [Char pointer to the start of binary GDS record]
 * @param  GDSKey  [The GDS key value of the record]
 * @param  bitarr  [Bitset of 16 bits]
 * @param  integer [Vector of 2 & 4 Byte signed integers]
 * @param  B8Real  [Vector of 8 Byte real value]
 * @param  words   [A string value]
 * @return         [0 - Exit Success; 1 - Exit Failure]
 */

// ====================== Function Code =======================

/**
 * [GDSdistill]
 * @param  			[To be filled in]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */
int GDSdistill(char *recIn, uint32_t &GDSKey, bitset<16> &bitarr,
               vector<int> &integer, vector<double> &B8Real, string &words)
{
  uint32_t sizeBlk;
  uint32_t i = 0;
  uint8_t dataType;

  sizeBlk = (((unsigned char)recIn[0] << 8) | (unsigned char)recIn[1]) - 2;
  GDSKey = ((recIn[2] << 8) | recIn[3]);
  dataType = GDSKey;
  i = 4;

  bitarr.reset();
  integer.clear();
  B8Real.clear();
  words = "";

  if (dataType == 0) {
    // no data
  } else if (dataType == 1) {
    // 16 bitset
    bitarr = (recIn[i] << 8) | (recIn[i + 1]);
  } else if (dataType == 2) {
    // 2 byte signed int
    for (i = 4; i <= sizeBlk; i = i + 2) {
      integer.push_back(conBytes(recIn, i, 2));
    }
  } else if (dataType == 3) {
    // 4 byte signed int
    for (i = 4; i <= sizeBlk; i = i + 4) {
      integer.push_back(conBytes(recIn, i, 4));
    }
  } else if (dataType == 4) {
    // 4 byte real (NOT USED)
    return 1;
  } else if (dataType == 5) {
    // 8 byte real
    double val = 0;
    int sign = 0;
    double exp = 0;
    double mantissa = 0;

    for (i = 4; i <= sizeBlk; i = i + 8) {

      sign = (unsigned char)recIn[i] >> 7;
      exp = (unsigned char)recIn[i] & 0b01111111;
      mantissa = conBytesLL(recIn, i + 1, 7);
      val = mantissa / pow(2, 56) * pow(16, exp - 64);
      if (sign)
        val *= -1;

      B8Real.push_back(val);
    }
  } else if (dataType == 6) {
    // ASCII string
    string foo = "";
    sizeBlk++;
    for (i = 4; i <= sizeBlk; i++) {
      if (recIn[i] ==
          '\0') // if string record's size is odd, it must be padded with NULL
        continue;
      foo = foo + recIn[i];
    }
    words = foo;
  } else {
    cout << "Unknown data type." << endl;
    return 1;
  }

  return 0;
}

/**
 * BUG loss of extreme precession, use UNIT function for unit header;
 */
uint64_t GDSfloatCalc(double inVar)
{
  uint64_t GDSdec = 0;
  unsigned long long integral;
  double deci;
  unsigned long long sign = 0;
  unsigned long long exponent = 80;
  unsigned long long mantissa = 0;

  if (inVar == 0)
    return 0;

  if (inVar < 0) {
    inVar *= -1;
    sign = 1;
  }

  integral = inVar;
  deci = inVar - integral;

  // Calculating the decimal
  for (int i = 63; i >= 0; i--) {
    deci *= 2;
    if (deci >= 1) {
      GDSdec = GDSdec | bitShiftL(1, i);
      deci--;
    }
    if (deci == 0)
      break;
  }

  mantissa = integral;
  for (int i = 15; i >= 0; i--) {
    if ((bitShiftR(mantissa, 59) & 0xffff) > 0)
      break;
    mantissa = mantissa << 4;
    exponent--;
    mantissa = mantissa | (bitShiftR(GDSdec, 4 * i) & 0xf);
  }

  sign = bitShiftL(sign, 63);
  exponent = bitShiftL(exponent, 56);
  mantissa = bitShiftR(mantissa, 8);

  return sign | exponent | mantissa;
}

/**
 * [bitShiftR - bit shift to the right, > 30]
 * @param  inVar [variable to be shifted to the right]
 * @return       [shifted variable]
 */
uint64_t bitShiftR(uint64_t inVar, int cnt)
{

  // if (cnt > 32) { // tempULL = (1 << i);
  //   inVar = inVar >> 30;
  //   inVar = inVar >> 2;
  //   inVar = inVar >> (cnt - 32);
  // } else if (cnt > 30) {
  //   inVar = inVar >> 30;
  //   inVar = inVar >> (cnt - 30);
  // } else {
  //   inVar = inVar >> cnt;
  // }

  // return inVar;
  return inVar >> cnt;
}

/**
 * [bitShiftL - bit shift to the left, > 30]
 * @param  inVar [variable to be shifted to the left]
 * @return       [shifted variable]
 */
uint64_t bitShiftL(uint64_t inVar, int cnt)
{

  // if (cnt > 32) { // tempULL = (1 << i);
  //   inVar = inVar << 30;
  //   inVar = inVar << 2;
  //   inVar = inVar << (cnt - 32);
  // } else if (cnt > 30) {
  //   inVar = inVar << 30;
  //   inVar = inVar << (cnt - 30);
  // } else {
  //   inVar = inVar << cnt;
  // }

  // return inVar;
  return inVar << cnt;
}

/**
 * [conBytes - Concatenates the array of bytes]
 * @param  inArry [Input array of bytes]
 * @param  cnt    [Amount of bytes that must be concatenated]
 * @return        [The concatenated value]
 */
int conBytes(char inArry[], int start, int cnt)
{
  int outVal = 0;

  for (int i = 0; i < cnt; i++) {
    outVal = (unsigned char)inArry[start + i] << ((cnt - i - 1) * 8) | outVal;
  }

  return outVal;
}

uint64_t conBytesLL(char inArry[], int start, int cnt)
{
  unsigned long long outVal = 0;

  for (int i = 0; i < cnt; i++) {
    outVal = ((unsigned long long)(unsigned char)inArry[start + i]
              << ((cnt - i - 1) * 8)) |
             outVal;
  }

  return outVal;
}

/**
 * [gsdTime - Sets the time once and fetches the original time]
 * @return [Gets the time in GDS format.]
 *           [year, month, day, hour, minute, seconds][last modification, last
 * access]
 */
int *gsdTime()
{
  static int timeIO[12];
  static bool timeSet = false;
  tm lctn;

  if (!timeSet) {
    time_t now = time(0);
    lctn = *localtime(&now);
    timeSet = true;

    timeIO[0] = lctn.tm_year + 1900;
    timeIO[1] = lctn.tm_mon + 1;
    timeIO[2] = lctn.tm_mday;
    timeIO[3] = lctn.tm_hour;
    timeIO[4] = lctn.tm_min;
    timeIO[5] = lctn.tm_sec;
    timeIO[6] = timeIO[0];
    timeIO[7] = timeIO[1];
    timeIO[8] = timeIO[2];
    timeIO[9] = timeIO[3];
    timeIO[10] = timeIO[4];
    timeIO[11] = timeIO[5];
  }

  return timeIO;
}

/**
 * [GDSkey2ASCII - Converters a GDS header to text]
 * @param  inHex [Teh GDS header value]
 * @return       [The equivalent string for the hex value if found else NULL]
 */
string GDSkey2ASCII(unsigned int inHex)
{
  static map<int, string> GDSkeys;
  static map<int, string>::iterator it;

  static bool readInYet = false;

  if (!readInYet) {
    readInYet = true;
    GDSkeys[0x0002] = "HEADER";
    GDSkeys[0x0102] = "BGNLIB";
    GDSkeys[0x0206] = "LIBNAME";
    GDSkeys[0x0305] = "UNITS";
    GDSkeys[0x0400] = "ENDLIB";
    GDSkeys[0x0502] = "BGNSTR";
    GDSkeys[0x0606] = "STRNAME";
    GDSkeys[0x0700] = "ENDSTR";
    GDSkeys[0x0800] = "BOUNDARY";
    GDSkeys[0x0900] = "PATH";
    GDSkeys[0x0a00] = "SREF";
    GDSkeys[0x0b00] = "AREF";
    GDSkeys[0x0c00] = "TEXT";
    GDSkeys[0x0d02] = "LAYER";
    GDSkeys[0x0e02] = "DATATYPE";
    GDSkeys[0x0f03] = "WIDTH";
    GDSkeys[0x1003] = "XY";
    GDSkeys[0x1100] = "ENDEL";
    GDSkeys[0x1206] = "SNAME";
    GDSkeys[0x1302] = "COLROW";
    GDSkeys[0x1500] = "NODE";
    GDSkeys[0x1602] = "TEXTTYPE";
    GDSkeys[0x1701] = "PRESENTATION";
    GDSkeys[0x1906] = "STRING";
    GDSkeys[0x1a01] = "STRANS";
    GDSkeys[0x1b05] = "MAG";
    GDSkeys[0x1c05] = "ANGLE";
    GDSkeys[0x1f06] = "REFLIBS";
    GDSkeys[0x2006] = "FONTS";
    GDSkeys[0x2102] = "PATHTYPE";
    GDSkeys[0x2202] = "GENERATIONS";
    GDSkeys[0x2306] = "ATTRTABLE";
    GDSkeys[0x2601] = "EFLAGS";
    GDSkeys[0x2a02] = "NODETYPE";
    GDSkeys[0x2b02] = "PROPATTR";
    GDSkeys[0x2c06] = "PROPVALUE";
    GDSkeys[0x2f03] = "PLEX";
  }

  it = GDSkeys.find(inHex);

  if (it == GDSkeys.end())
    return "\0";

  return it->second;
}