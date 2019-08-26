/**
 * Author:  		
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license: 
 * Description: Creates GDS files using the gdsClass
 * File:				gdsExport.cpp
 */

#include "gdsExport.hpp"

/**
 * Constructor
 */

gdsExport::gdsExport(vector<gdsSTR>& inVec){
	this->STR = inVec;	
}

/**
 * [gdsExport::gdsCreate - Generates/creates/exports the GDS file]
 * @param  FileName [The file name of the to be created GDS file]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdsExport::gdsCreate(string FileName){
	// // Initializing the writing
	// this->fileName = FileName;
	// gdsFile = fopen(FileName.c_str(), "wb");
	
	// // if(!gdsFile.is_open()){
	// // 	cout << "Error: GDS file \"" << FileName << "\" FAILED to be opened for writing." << endl;

	// this->gdsBegin();

	// // gdsCopyStr("ImportFileStrs.gds2");


	// for(unsigned int i = 0; i < this->STR.size(); i++){
	// 	gdsStrStart("StrName");
	// 		gdsSREF("RefName", false, 1, 0, 0, 0);
	// 		gdsBoundary(1, 0, 0);
	// 		gdsPath(1, 30, 0, 0);
	// 	gdsStrEnd();

	// }

	// this->gdsEnd();

	// // Terminating the writing
	// fclose(gdsFile);
	// cout << "Creating \"" << this->fileName << "\" done." << endl;

	return 0;
}


/***********************************************************************************
 **************************** UpperGround Level ************************************
 ***********************************************************************************/

/**
 * [gdsExport::gdsBegin - Starts of the GDS file with the correct stuffs]
 */

void gdsExport::gdsBegin(){
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
 * [gdsExport::gdsEnd - End the GDS file]
 */

void gdsExport::gdsEnd(){
	this->GDSwriteRec(GDS_ENDLIB);
}


/**
 * [gdsExport::gdsCopyStr - Copies structure from a GDS file. This function is just a passer]
 * @param fileName [Name of the file]
 */

void gdsExport::gdsCopyStr(string fileName){
	// this->copyGDSstrs(fileName);
}

/**
 * [gdsExport::gdsStrStart - Outputs the correct GDS records to start a structure]
 * @param strName [The name of the structure]
 */

void gdsExport::gdsStrStart(string strName){
	this->GDSwriteInt(GDS_BGNSTR, gsdTime(), 12);
	this->GDSwriteStr(GDS_STRNAME, strName);
}

/**
 * [gdsExport::gdsStrEnd - Outputs the correct GDs records to end a structure]
 */

void gdsExport::gdsStrEnd(){
	this->GDSwriteRec(GDS_ENDSTR);
}

/**
 * [gdsExport::gdsPath - Draws a track/path/route in the correct GDS format]
 * @param Layer [number of the layer that the track must be on]
 * @param width [The thickness of the tracks]
 * @param corXY [A vector of X coordinates of the tracks]
 * @param corXY [A vector of Y coordinates of the tracks]
 */

void gdsExport::gdsPath(int Layer, int width, vector<int>& corX, vector<int>& corY){
	int data[1];
	int corXY[corX.size()*2];

	this->GDSwriteRec(GDS_PATH);

	// ELFLAGS and PLEX are optional

	// Layer number, 0 to 63
	data[0] = Layer;
	this->GDSwriteInt(GDS_LAYER, data, 1);

	// unimportant, set to zero. 
	data[0] = 0;
	this->GDSwriteInt(GDS_DATATYPE, data, 1);

	// Shape of the end cap. 
	// 0=Flush
	// 1=Half Round Extension
	// 2=Half Width Extension
	data[0] = 2;
	this->GDSwriteInt(GDS_PATHTYPE, data, 1);

	// Width of tracks 
	data[0] = width;
	this->GDSwriteInt(GDS_WIDTH, data, 1);

	// XY coordinates
	// boundary must be closed, first and last coordinate must be the same
	// minimum of 4 points(triangle)
	for(int i = 0; i < corX.size()*2; i++){
		if(i % 2){
			corXY[i] = corY[i/2];
		}
		else{
			corXY[i] = corX[i/2];
		}
	}

	this->GDSwriteInt(GDS_XY, corXY, corX.size()*2);

	this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsExport::gdsBoundary - Draws a shape in the correct GDS format]
 * @param Layer [number of the layer that the shape must be on]
 * @param corX [A vector of X coordinates of the corners of shape]
 * @param corY [A vector of Y coordinates of the corners of shape]
 */

void gdsExport::gdsBoundary(int Layer, vector<int>& corX, vector<int>& corY){
	int data[1];
	int corXY[corX.size()*2];

	this->GDSwriteRec(GDS_BOUNDARY);

	// ELFLAGS and PLEX are optional

	// Layer number, 0 to 63
	data[0] = Layer;
	this->GDSwriteInt(GDS_LAYER, data, 1);

	// unimportant, set to zero. 
	data[0] = 0;
	this->GDSwriteInt(GDS_DATATYPE, data, 1);

	// XY coordinates
	// boundary must be closed, first and last coordinate must be the same
	// minimum of 4 points(triangle)
	
	for(int i = 0; i < corX.size()*2; i++){
		if(i % 2){
			// cout << "corY[" << i/2  << "]: " << corY[i/2] << endl;
			corXY[i] = corY[i/2];
		}
		else{
			// cout << "corX[" << i/2  << "]: " << corX[i/2] << endl;
			corXY[i] = corX[i/2];
		}
	}

	this->GDSwriteInt(GDS_XY, corXY, corX.size() * 2);

	this->GDSwriteRec(GDS_ENDEL);
}

/**
 * [gdsExport::gdsSREF - Creates a reference to another cell in the correct GDS format]
 * @param refName [Name of the structure reference]
 * @param ReflX   [Reflection across x-axis]
 * @param mag     [Magnification of the cell, default 1]
 * @param ang     [Angle of rotation, measured anticlockwise]
 * @param corX    [X-coordinate]
 * @param corY    [Y-coordinate]
 */

void gdsExport::gdsSREF(string refName, bool ReflX, double mag, double ang, int corX, int corY){
	this->GDSwriteRec(GDS_SREF);

	// ELFLAGS and PLEX are optional

	// name of the structure that is referenced 
	this->GDSwriteStr(GDS_SNAME, refName);

	// two byte, bit array controlling reflection, magnification and rotation.
	bitset<16> bits;

	// reflect across x-axis
	bits.set(15, ReflX);

	// magnification, default 1
	if(mag == 1)
		bits.set(2, 0);
	else
		bits.set(2, 1);

	// angle of rotation
	if(ang == 0)
		bits.set(1, 0);
	else
		bits.set(1, 1);

	this->GDSwriteBitArr(GDS_STRANS, bits);

	double arrDou[1];
	// Magnification, default 1
	if(mag != 1){
		arrDou[0] = mag;
		this->GDSwriteRea(GDS_MAG, arrDou, 1);
	}

	// Angle measured counterclockwise
	if(ang != 0){
		arrDou[0] = ang;
		this->GDSwriteRea(GDS_ANGLE, arrDou, 1);
	}

	int corXY[2];
	corXY[0] = corX;
	corXY[1] = corY;
	this->GDSwriteInt(GDS_XY, corXY, 2);

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

int gdsExport::GDSwriteInt(int record, int arrInt[], int cnt){
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
 * [gdsExport::GDSwriteStr - Writes a string value to file]
 * @param  record [GDS record type]
 * @param  inStr  [The string to be written]
 * @return 				[0 - Exit Success; 1 - Exit Failure]
 */

int gdsExport::GDSwriteStr(int record, string inStr){
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
 * [gdsExport::GDSwriteBitArr - Write 16 bit array in GDS format]
 * @param  record [GDS record type]
 * @param  inBits [Bits that must be written]
 * @return 				[0 - Exit Success; 1 - Exit Failure]
 */

int gdsExport::GDSwriteBitArr(int record, bitset<16> inBits){
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
 * [gdsExport::GDSwriteRea - Writes doubles values to file]
 * @param record [GDS record type]
 * @param arrInt [Array of doubles to be written]
 * @param cnt    [Amount of doubles to be written]
 * @return 		   [0 - Exit Success; 1 - Exit Failure]
 */

int gdsExport::GDSwriteRea(int record, double arrInt[], int cnt){
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
 * [gdsExport::GDSwriteUnits - Hard coding the UNIT record for precision]
 */

void gdsExport::GDSwriteUnits(){
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
 * [gdsExport::GDSwriteRec - Writes record value to file]
 * @param record [GDS record type]
 * @return 			 [0 - Exit Success; 1 - Exit Failure]
 */

int gdsExport::GDSwriteRec(int record){
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