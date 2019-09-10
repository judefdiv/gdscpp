/**
 * Author:
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license:
 * Description: Creates GDS files using the gdsClass
 * File:				gdsForge.cpp
 */

#include "gdsForge.hpp"

/**
 * Constructor
 */

gdsForge::gdsForge(){

}

/**
 * [gdsForge::gdsCreate - Generates/creates/exports the GDS file]
 * @param  FileName [The file name of the to be created GDS file]
 * @param  inVec    [The vector of GDS structure to be created into the GDS file]
 * @param  double   [The scale the GDS file must use]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdsForge::gdsCreate(string FileName, vector<gdsSTR>& inVec, double units[2]){
	this->STR = inVec;

	// Initializing the writing
	this->fileName = FileName;
	gdsFile = fopen(FileName.c_str(), "wb");

	// if(!gdsFile.is_open()){
	// 	cout << "Error: GDS file \"" << FileName << "\" FAILED to be opened for writing." << endl;

	this->gdsBegin();

	// gdsCopyStr("ImportFileStrs.gds2");

	for(unsigned int i = 0; i < this->STR.size(); i++){
		// Start of the structure
		gdsStrStart(this->STR[i].name);
			// References
			for(unsigned int j = 0; j < this->STR[i].SREF.size(); j++){
				gdsSRef(this->STR[i].SREF[j], true);
			}
			// Boundaries
			for(unsigned int j = 0; j < this->STR[i].BOUNDARY.size(); j++){
				gdsBoundary(this->STR[i].BOUNDARY[j], true);
			}
			// Paths
			for(unsigned int j = 0; j < this->STR[i].PATH.size(); j++){
				gdsPath(this->STR[i].PATH[j], true);
			}
			// Nodes
			for(unsigned int j = 0; j < this->STR[i].NODE.size(); j++){
				gdsNode(this->STR[i].NODE[j], true);
			}
			// Texts
			for(unsigned int j = 0; j < this->STR[i].TEXT.size(); j++){
				gdsText(this->STR[i].TEXT[j], true);
			}
		gdsStrEnd();

	}

	this->gdsEnd();

	// Terminating the writing
	fclose(gdsFile);
	cout << "Creating \"" << this->fileName << "\" done." << endl;

	return 0;
}

/***********************************************************************************
 ********************* Functions to easily draw in GDSfiles ************************
 ***********************************************************************************/

/**
 * [drawBoundary - Easily create a GDS boundary]
 * @param  layer [The layer number]
 * @param  corX  [The X-coordinates]
 * @param  corY  [The Y-coordinates]
 * @return       [Class of GDS boundary which can be used in a GDS structure]
 */

gdsBOUNDARY drawBoundary(int layer, vector<int> corX, vector<int> corY){
	gdsBOUNDARY foo;

	foo.layer = layer;
	foo.xCor = corX;
	foo.yCor = corY;

	return foo;
}

/**
 * [drawPath - Easily create a GDS path]
 * @param  layer [The layer number]
 * @param  width [The thickness of the track]
 * @param  corX  [The X-coordinates]
 * @param  corY  [The Y-coordinates]
 * @return       [Class of GDS path which can be used in a GDS structure]
 */

gdsPATH drawPath(int layer, unsigned int width, vector<int> corX, vector<int> corY){
	gdsPATH foo;

	foo.layer = layer;
	foo.width = width;
	foo.xCor = corX;
	foo.yCor = corY;

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

gdsBOUNDARY draw2ptBox(int layer, int blX, int blY, int trX, int trY){
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
 * @return         [Class of GDS s-reference which can be used in a GDS structure]
 */

gdsSREF drawSREF(string STRname, int Xcor, int Ycor){
	gdsSREF foo;

	foo.name = STRname;
	foo.xCor = Xcor;
	foo.yCor = Ycor;

	return foo;
}

/***********************************************************************************
 **************************** UpperGround Level ************************************
 ***********************************************************************************/

/**
 * [gdsForge::gdsBegin - Starts of the GDS file with the correct stuffs]
 */

void gdsForge::gdsBegin(){
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

void gdsForge::gdsEnd(){
	this->GDSwriteRec(GDS_ENDLIB);
}


/**
 * [gdsForge::gdsCopyStr - Copies structure from a GDS file. This function is just a passer]
 * @param fileName [Name of the file]
 */

void gdsForge::gdsCopyStr(string fileName){
	// this->copyGDSstrs(fileName);
}

/**
 * [gdsForge::gdsStrStart - Outputs the correct GDS records to start a structure]
 * @param strName [The name of the structure]
 */

void gdsForge::gdsStrStart(string strName){
	this->GDSwriteInt(GDS_BGNSTR, gsdTime(), 12);
	this->GDSwriteStr(GDS_STRNAME, strName);
}

/**
 * [gdsForge::gdsStrEnd - Outputs the correct GDs records to end a structure]
 */

void gdsForge::gdsStrEnd(){
	this->GDSwriteRec(GDS_ENDSTR);
}

/**
 * [gdsForge::gdsPath - Create the PATH structure]
 * @param in_PATH [The class containing the PATH structure]
 * @param minimal [If true only the minimal is PATH structure is created]
 */

void gdsForge::gdsPath(gdsPATH& in_PATH, bool minimal){
	int data[1];
	int corXY[in_PATH.xCor.size()*2];

	this->GDSwriteRec(GDS_PATH);

	// ELFLAGS and PLEX are optional

	// Layer number, 0 to 63
	data[0] = in_PATH.layer;
	this->GDSwriteInt(GDS_LAYER, data, 1);

	// unimportant, set to zero.
	data[0] = in_PATH.dataType;
	this->GDSwriteInt(GDS_DATATYPE, data, 1);

	data[0] = in_PATH.pathtype;
	this->GDSwriteInt(GDS_PATHTYPE, data, 1);

	if(minimal){	// true
		data[0] = 1;
		this->GDSwriteInt(GDS_PATHTYPE, data, 1);
	}
	else{					// false
		data[0] = in_PATH.pathtype;
		this->GDSwriteInt(GDS_PATHTYPE, data, 1);
	}

	// Width of tracks
	data[0] = in_PATH.width;
	this->GDSwriteInt(GDS_WIDTH, data, 1);

	// XY coordinates
	// boundary must be closed, first and last coordinate must be the same
	// minimum of 4 points(triangle)
	for(int i = 0; i < in_PATH.xCor.size()*2; i++){
		if(i % 2){
			corXY[i] = in_PATH.yCor[i/2];
		}
		else{
			corXY[i] = in_PATH.xCor[i/2];
		}
	}

	this->GDSwriteInt(GDS_XY, corXY, in_PATH.xCor.size()*2);

	// Optional goodies
	if(!minimal){ 				//false
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

void gdsForge::gdsBoundary(gdsBOUNDARY& in_BOUNDARY, bool minimal){
	int data[1];
	int corXY[in_BOUNDARY.xCor.size()*2];

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

	for(int i = 0; i < in_BOUNDARY.xCor.size()*2; i++){
		if(i % 2){
			// cout << "corY[" << i/2  << "]: " << corY[i/2] << endl;
			corXY[i] = in_BOUNDARY.yCor[i/2];
		}
		else{
			// cout << "corX[" << i/2  << "]: " << corX[i/2] << endl;
			corXY[i] = in_BOUNDARY.xCor[i/2];
		}
	}

	this->GDSwriteInt(GDS_XY, corXY, in_BOUNDARY.xCor.size() * 2);

	// Optional goodies
	if(!minimal){ 				//false
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

void gdsForge::gdsSRef(gdsSREF& in_SREF, bool minimal){
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
	if(in_SREF.scale == 1)
		bits.set(2, 0);
	else
		bits.set(2, 1);

	// angle of rotation
	if(in_SREF.angle == 0)
		bits.set(1, 0);
	else
		bits.set(1, 1);

	this->GDSwriteBitArr(GDS_STRANS, bits);

	double arrDou[1];
	// Magnification, default 1
	if(in_SREF.scale != 1){
		arrDou[0] = in_SREF.scale;
		this->GDSwriteRea(GDS_MAG, arrDou, 1);
	}

	// Angle measured counterclockwise
	if(in_SREF.angle != 0){
		arrDou[0] = in_SREF.angle;
		this->GDSwriteRea(GDS_ANGLE, arrDou, 1);
	}

	int corXY[2];
	corXY[0] = in_SREF.xCor;
	corXY[1] = in_SREF.yCor;
	this->GDSwriteInt(GDS_XY, corXY, 2);

	// Optional goodies
	if(!minimal){ 				//false
		data[0] = in_SREF.propattr;
		this->GDSwriteInt(GDS_PROPATTR, data, 1);

		this->GDSwriteStr(GDS_PROPVALUE, in_SREF.propvalue);
	}

	this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsForge::gdsSRef - Create the NODE structure]
 * @param gdsNODE [The class containing the NODE structure]
 * @param minimal [If true only the minimal is NODE structure is created]
 */

void gdsForge::gdsNode(gdsNODE& in_NODE, bool minimal){
	int data[1];
	int corXY[in_NODE.xCor.size()*2];

	this->GDSwriteRec(GDS_NODE);

	// ELFLAGS and PLEX are optional
	if(!minimal){ 				//false
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

	for(int i = 0; i < in_NODE.xCor.size()*2; i++){
		if(i % 2){
			// cout << "corY[" << i/2  << "]: " << corY[i/2] << endl;
			corXY[i] = in_NODE.yCor[i/2];
		}
		else{
			// cout << "corX[" << i/2  << "]: " << corX[i/2] << endl;
			corXY[i] = in_NODE.xCor[i/2];
		}
	}

	this->GDSwriteInt(GDS_XY, corXY, in_NODE.xCor.size() * 2);

	// Optional goodies
	if(!minimal){ 				//false
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

void gdsForge::gdsText(gdsTEXT in_TEXT, bool minimal){
	int data[1];
	this->GDSwriteRec(GDS_TEXT);

	// ELFLAGS and PLEX are optional

	// PRESENTATION; PATHTYPE, WIDTH, STRANS, MAG, and ANGLE are optional

	// Layer number, 0 to 63
	data[0] = in_TEXT.layer;
	this->GDSwriteInt(GDS_LAYER, data, 1);

	// data[0] = in_TEXT.;
	// this->GDSwriteInt(GDS_TEXTTYPE, data, 1);


	int corXY[2];
	corXY[0] = in_TEXT.xCor;
	corXY[1] = in_TEXT.yCor;
	this->GDSwriteInt(GDS_XY, corXY, 2);

	// text/string to be used of the structure that is referenced
	this->GDSwriteStr(GDS_STRING, in_TEXT.textbody);

	// Optional goodies
	if(!minimal){ 				//false
		data[0] = in_TEXT.propattr;
		this->GDSwriteInt(GDS_PROPATTR, data, 1);

		this->GDSwriteStr(GDS_PROPVALUE, in_TEXT.propvalue);
	}

	this->GDSwriteRec(GDS_ENDEL);
}

/***********************************************************************************
 **************************** UnderGround Level ************************************
 ***********************************************************************************/

/**
 * [GDSwriteInt - Writes integer values to file]
 * @param record [GDS record type]
 * @param arrInt [Array of integers to be written]
 * @param cnt    [Amount of integers to be written]
 * @return 			 [0 - Exit Success; 1 - Exit Failure]
 */

int gdsForge::GDSwriteInt(int record, int arrInt[], int cnt){
	unsigned int dataSize = record & 0xff;

	if(dataSize == 0x02 && cnt > 0){
		dataSize = 2;					// should/could be omitted
	}
	else if(dataSize == 0x03 && cnt > 0){
		dataSize = 4;
	}
	else if(dataSize == 0x00 && cnt == 0){
		dataSize = 0;					// should/could be omitted
	}
	else{
		cout << "Incorrect parameters for record: 0x" << hex << record << endl;
		cout << dec;
		return 1;
	}

	unsigned int sizeByte = cnt * dataSize + 4;
	unsigned char OHout[4];

	OHout[0] = sizeByte  >> 8 & 0xff;
	OHout[1] = sizeByte & 0xff;
	OHout[2] = record >> 8 & 0xff;
	OHout[3] = record & 0xff;
	fwrite(OHout, 1, 4, this->gdsFile);

	unsigned char dataOut[dataSize];

	for(int i = 0; i < cnt; i++){
		for(unsigned int j = 0; j < dataSize; j++){
			dataOut[j] = arrInt[i] >> (((dataSize-1)*8) - (j*8)) & 0xff;
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

int gdsForge::GDSwriteStr(int record, string inStr){
	if((record & 0xff) != 0x06){
		cout << "Incorrect record: 0x" << hex << record << endl;
		cout << dec;
		return 1;
	}
	unsigned char OHout[4];

	if(inStr.length() % 2 != 0){
		// therefore odd
		inStr.push_back('\0');
	}

	int lenStr = inStr.length();

	OHout[0] = ((lenStr + 4) >> 8 ) & 0xff;
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

int gdsForge::GDSwriteBitArr(int record, bitset<16> inBits){
	if((record & 0xff) != 0x01){
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

	for(int i = 15; i >= 8; i--){
		dataOut[0] = dataOut[0] | (inBits[i] << (i-8));
	}
	for(int i = 8; i > 0; i--){
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

int gdsForge::GDSwriteRea(int record, double arrInt[], int cnt){
	unsigned int dataSize = record & 0xff;
	unsigned long long realVal;

	if(dataSize == 0x05 && cnt > 0){
		dataSize = 8;					// should/could be omitted
	}
	else{
		cout << "Incorrect parameters for record: 0x" << hex << record << endl;
		cout << dec;
		return 1;
	}

	unsigned int sizeByte = cnt * dataSize + 4;
	unsigned char OHout[4];

	OHout[0] = sizeByte  >> 8 & 0xff;
	OHout[1] = sizeByte & 0xff;
	OHout[2] = record >> 8 & 0xff;
	OHout[3] = record & 0xff;
	fwrite(OHout, 1, 4, this->gdsFile);

	unsigned char dataOut[dataSize];

	for(int i = 0; i < cnt; i++){
		realVal = GDSfloatCalc(arrInt[i]);
		for(unsigned int j = 0; j < dataSize; j++){
			// dataOut[j] = realVal >> (((dataSize-1)*8) - (j*8)) & 0xff;
			dataOut[j] = realVal >> (56 - (j*8)) & 0xff;
		}
		fwrite(dataOut, 1, dataSize, this->gdsFile);
	}

	return 0;
}

/**
 * [gdsForge::GDSwriteUnits - Hard coding the UNIT record for precision]
 */

void gdsForge::GDSwriteUnits(){
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

int gdsForge::GDSwriteRec(int record){
	unsigned char OHout[4];

	OHout[0] = 0;
	OHout[1] = 4;
	OHout[2] = record >> 8 & 0xff;
	OHout[3] = record & 0xff;

	if(OHout[3] != 0){
		cout << "The smoke has escaped. The record must be dataless" << endl;
		return 1;
	}

	fwrite(OHout, 1, 4, this->gdsFile);

	return 0;
}