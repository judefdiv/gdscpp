/**
 * Author:      J.F. de Villiers & H.F. Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				  Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license:     MIT License
 * Description: Library of all the low level GDSII functions.
 * File:			  gdsParser.hpp
 */

#ifndef gdsParser
#define gdsParser

#include <bitset>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <set>
#include <stdio.h>
#include <string>
#include <vector>

int GDSdistill(char *recIn, uint32_t &GDSKey, std::bitset<16> &bitarr,
               std::vector<int> &integer, std::vector<double> &B8Real,
               std::string &words);

uint64_t GDSfloatCalc(double inVar); // low level
uint64_t bitShiftR(uint64_t inVar,
                   int cnt); // ultra low level - should be removed
uint64_t bitShiftL(uint64_t inVar,
                   int cnt); // ultra low level - should be removed
int conBytes(char inArry[], int start, int cnt); // ultra low level
uint64_t conBytesLL(char inArry[], int start,
                    int cnt); // ultra low level

int *gsdTime();
std::string GDSkey2ASCII(unsigned int inHex);

// shortcuts for key that are used
constexpr auto GDS_HEADER = 0x0002;
constexpr auto GDS_BGNLIB = 0x0102;
constexpr auto GDS_LIBNAME = 0x0206;
constexpr auto GDS_UNITS = 0x0305;
constexpr auto GDS_ENDLIB = 0x0400;
constexpr auto GDS_BGNSTR = 0x0502;
constexpr auto GDS_STRNAME = 0x0606;
constexpr auto GDS_ENDSTR = 0x0700;
constexpr auto GDS_BOUNDARY = 0x0800;
constexpr auto GDS_PATH = 0x0900;
constexpr auto GDS_SREF = 0x0a00;
constexpr auto GDS_AREF = 0x0b00;
constexpr auto GDS_TEXT = 0x0c00;
constexpr auto GDS_LAYER = 0x0d02;
constexpr auto GDS_DATATYPE = 0x0e02;
constexpr auto GDS_WIDTH = 0x0f03;
constexpr auto GDS_XY = 0x1003;
constexpr auto GDS_ENDEL = 0x1100;
constexpr auto GDS_SNAME = 0x1206;
constexpr auto GDS_COLROW = 0x1302;
constexpr auto GDS_NODE = 0x1500;
constexpr auto GDS_TEXTTYPE = 0x1602;
constexpr auto GDS_PRESENTATION = 0x1701;
constexpr auto GDS_STRING = 0x1906;
constexpr auto GDS_STRANS = 0x1a01;
constexpr auto GDS_MAG = 0x1b05;
constexpr auto GDS_ANGLE = 0x1c05;
constexpr auto GDS_REFLIBS = 0x1f06;
constexpr auto GDS_FONTS = 0x2006;
constexpr auto GDS_PATHTYPE = 0x2102;
constexpr auto GDS_GENERATIONS = 0x2202;
constexpr auto GDS_ATTRTABLE = 0x2306;
constexpr auto GDS_EFLAGS = 0x2601;
constexpr auto GDS_NODETYPE = 0x2a02;
constexpr auto GDS_PROPATTR = 0x2b02;
constexpr auto GDS_PROPVALUE = 0x2c06;
constexpr auto GDS_BOX = 0x2d00;
constexpr auto GDS_BOXTYPE = 0x2e02;
constexpr auto GDS_PLEX = 0x2f03;

#endif