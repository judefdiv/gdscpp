/**
 * Authors:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				Supertools, Coldflux Project - IARPA
 * Created: 		2019-03-20
 * Modified:
 * license: 
 * Description: Library file containing GDSII reading and writing functions.
 * File:				gdsParser.cpp
 */

#include "gdsParser.hpp"

ifstream gdsFileRead;
FILE *gdsFileWrite;
string gdsFileNameRead = "\0";
string gdsFileNameWrite = "\0";

// map<int,string> GDSkeys;

/**
 * [openGDSread - Creates/opens the binary file]
 * @param  fileName [The file name of the GDSII file]
 * @return          [Returns 1 if all good else 0 if error]
 */

int openGDSread(string fileName){
	gdsFileNameRead = fileName;
	gdsFileRead.open(gdsFileNameRead, ios::in|ios::binary);

	if(!gdsFileRead.is_open()){
		cout << "GDS file ->" << gdsFileNameRead << "<- FAILED to be opened." << endl;
		return 0;
	}

	cout << "GDS file ->" << gdsFileNameRead << "<- opened." << endl;
	return 1;
}

/**
 * [closeGDSread - closes the opened GDS file]
 * @return [Returns 1 if all good else 0 if error]
 */

int closeGDSread(){ 
	if(gdsFileNameRead == "\0"){
		cout << "GDS file was not opened." << endl;
		return 0;
	}

	gdsFileRead.close();
	cout << "GDS file ->" << gdsFileNameRead << "<- closed." << endl;
	return 1;
}

/**
 * [openGDSwrite - Creates/opens the binary file]
 * @param  fileName [The file name of the GDSII file]
 * @return          [Returns 1 if all good else 0 if error]
 */

int openGDSwrite(string fileName){
	gdsFileNameWrite = fileName;
	// const char *charStr = gdsFileNameWrite.c_str();
	// gdsFileWrite.open(gdsFileNameWrite, ios::out|ios::binary|ios::trunc);
	gdsFileWrite = fopen(gdsFileNameWrite.c_str(), "wb");

	// if(!gdsFileWrite.is_open()){
	// 	cout << "GDS file ->" << gdsFileNameWrite << "<- FAILED to be opened for writing." << endl;
	// 	return 0;
	// }

	cout << "Creating GDS file -> "  << fileName << endl; 
	return 1;
}

/**
 * [closeGDSwrite - closes the opened GDS file]
 * @return [Returns 1 if all good else 0 if error]
 */

int closeGDSwrite(){ 
	if(gdsFileNameWrite == "\0"){
		cout << "GDS file was not opened." << endl;
		return 0;
	}

	fclose(gdsFileWrite);
	// cout << "GDS file ->" << gdsFileNameWrite << "<- closed." << endl;
	cout << "Creating GDS file done." << endl;
	return 1;
}

/**
 * [GDSwriteI - Writes integer values to file]
 * @param record [GDS record type]
 * @param arrInt [Array of integers to be written]
 * @param cnt    [Amount of integers to be written]
 * @return 			 [Returns 1 if all good else 0 if error]
 */

int GDSwriteInt(int record, int arrInt[], int cnt){
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
		return 0;
	}

	unsigned int sizeByte = cnt * dataSize + 4;
	unsigned char OHout[4];

	OHout[0] = sizeByte  >> 8 & 0xff;
	OHout[1] = sizeByte & 0xff;
	OHout[2] = record >> 8 & 0xff;
	OHout[3] = record & 0xff;
	fwrite(OHout, 1, 4, gdsFileWrite);

	unsigned char dataOut[dataSize];

	for(int i = 0; i < cnt; i++){
		for(unsigned int j = 0; j < dataSize; j++){
			dataOut[j] = arrInt[i] >> (((dataSize-1)*8) - (j*8)) & 0xff;
		}
		fwrite(dataOut, 1, dataSize, gdsFileWrite);
	}

	return 1;
}

/**
 * [GDSwriteStr description]
 * @param  record [GDS record type]
 * @param  inStr  [The string to be written]
 * @return 				[Returns 1 if all good else 0 if error]
 */

int GDSwriteStr(int record, string inStr){
	if((record & 0xff) != 0x06){
		cout << "Incorrect record: 0x" << hex << record << endl;
		cout << dec;
		return 0;
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
	fwrite(OHout, 1, 4, gdsFileWrite);

	char dataOut[lenStr];
	strcpy(dataOut, inStr.c_str()); 
	fwrite(dataOut, 1, lenStr, gdsFileWrite);

	return 1;
}

/**
 * [GDSwriteBitArr - Write 16 bit array in GDS format]
 * @param  record [GDS record type]
 * @param  inBits [Bits that must be written]
 * @return 				[Returns 1 if all good else 0 if error]
 */

int GDSwriteBitArr(int record, bitset<16> inBits){
	if((record & 0xff) != 0x01){
		cout << "Incorrect record: 0x" << hex << record << endl;
		cout << dec;
		return 0;
	}
	unsigned char OHout[4];

	OHout[0] = 0 & 0xff;
	OHout[1] = (2 + 4) & 0xff;
	OHout[2] = record >> 8 & 0xff;
	OHout[3] = record & 0xff;
	fwrite(OHout, 1, 4, gdsFileWrite);

	unsigned char dataOut[2] = {0, 0};

	for(int i = 15; i >= 8; i--){
		dataOut[0] = dataOut[0] | (inBits[i] << (i-8));
	}
	for(int i = 8; i > 0; i--){
		dataOut[1] = dataOut[1] | (inBits[i] << i);
	}

	fwrite(dataOut, 1, 2, gdsFileWrite);
	
	return 1;
}

/**
 * [GDSwriteRea - Writes doubles values to file]
 * @param record [GDS record type]
 * @param arrInt [Array of doubles to be written]
 * @param cnt    [Amount of doubles to be written]
 */

int GDSwriteRea(int record, double arrInt[], int cnt){
	unsigned int dataSize = record & 0xff;
	unsigned long long realVal;

	if(dataSize == 0x05 && cnt > 0){
		dataSize = 8;					// should/could be omitted
	}
	else{
		cout << "Incorrect parameters for record: 0x" << hex << record << endl;
		cout << dec;
		return 0;
	}

	unsigned int sizeByte = cnt * dataSize + 4;
	unsigned char OHout[4];

	OHout[0] = sizeByte  >> 8 & 0xff;
	OHout[1] = sizeByte & 0xff;
	OHout[2] = record >> 8 & 0xff;
	OHout[3] = record & 0xff;
	fwrite(OHout, 1, 4, gdsFileWrite);

	unsigned char dataOut[dataSize];

	for(int i = 0; i < cnt; i++){
		realVal = GDSfloatCalc(arrInt[i]);
		for(unsigned int j = 0; j < dataSize; j++){
			// dataOut[j] = realVal >> (((dataSize-1)*8) - (j*8)) & 0xff;
			dataOut[j] = realVal >> (56 - (j*8)) & 0xff;
		}
		fwrite(dataOut, 1, dataSize, gdsFileWrite);
	}

	return 1;
}

/**
 * [GDSwriteRec - Writes record value to file]
 * @param record [GDS record type]
 */

int GDSwriteRec(int record){
	unsigned char OHout[4];

	OHout[0] = 0;
	OHout[1] = 4;
	OHout[2] = record >> 8 & 0xff;
	OHout[3] = record & 0xff;

	if(OHout[3] != 0){
		cout << "The smoke has escaped. The record must be dataless" << endl;
		return 0;
	}

	fwrite(OHout, 1, 4, gdsFileWrite);

	return 1;
}

/**
 * [writeGDS - Test script to check if the writing works]
 * @param fileName [Name of file to be created]
 */

void writeGDS(string fileName){
	// double temparray[10] = {1, 0.5, 1.7, -0.7, 10000, 0, 123.678, 0.0016786, 1e-9, 3e10};

	cout << "Writing GDS file" << endl;
	openGDSwrite(fileName);
	// GDSwriteRea(GDS_UNITS, temparray, 10);

	bitset<16> bits(0x9342);

	GDSwriteBitArr(GDS_STRANS, bits);

	closeGDSwrite();
}

/**
 * [GDSfile2ASCII - Reads in a GDS file and converts it to ASCII]
 * @param fileName [The name of the GDS file to be read]
 */

// void GDSfile2ASCII(string fileName){
// 	openGDSread(fileName);
// 	MapGDSkeys();

//   char *readBlk;
//   uint32_t sizeBlk;
//   uint32_t hexKey;

// 	gdsFileRead.seekg(0, ios::beg);

// 	do{
// 		readBlk = new char [2];
// 		gdsFileRead.read(readBlk, 2);

// 		sizeBlk = (((unsigned char)readBlk[0] << 8) | (unsigned char)readBlk[1]);

// 		gdsFileRead.seekg(-2, ios::cur);
// 		readBlk = new char [sizeBlk];
// 		gdsFileRead.read(readBlk, sizeBlk);

// 		hexKey = ((readBlk[2] << 8) | readBlk[3]);

// 		// GDSrecord2ASCII(readBlk);
// 		if(!GDSrecord2ASCII(readBlk)){
// 			cout << "GDS read error" << endl;
// 			break;
// 		}
// 	} while(hexKey != GDS_ENDLIB);

// 	delete[] readBlk;
// 	closeGDSread();
// }

/**
 * [GDS2ASCII - Reads a single record and outputs it to the appropriate variable]
 * @param  recIn   [Char pointer to the start of binary GDS record]
 * @param  GDSKey  [The GDS key value of the record]
 * @param  bitarr  [Bitset of 16 bits]
 * @param  integer [Vector of 2 & 4 Byte signed integers]
 * @param  B8Real  [Vector of 8 Byte real value]
 * @param  words   [A string value]
 * @return         [0 - Exit Success; 1 - Exit Failure]
 */

int GDS2ASCII(char *recIn, uint32_t& GDSKey, bitset<16>& bitarr, vector<char>& integer, vector<double>& B8Real, string& words){
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

	if(dataType == 0){
		// no data
	}
	else if(dataType == 1){
		// 16 bitset
		bitarr = (recIn[i] << 8) | (recIn[i+1]);
	}
	else if(dataType == 2){
		//2 byte signed int
		for(i = 4; i <= sizeBlk; i = i + 2){
			integer.push_back(conBytes(recIn, i, 2));
		}
	}
	else if(dataType == 3){
		//4 byte signed int
		for(i = 4; i <= sizeBlk; i = i + 4){
			integer.push_back(conBytes(recIn, i, 4));
		}
	}
	else if(dataType == 4){
		//4 byte real (NOT USED)
		return 1;
	}
	else if(dataType == 5){
		//8 byte real
		double val = 0;
		int sign = 0;
		double exp = 0;
		double mantissa = 0;

		for(i = 4; i <= sizeBlk; i = i + 8){
			
			sign = (unsigned char)recIn[i] >> 7;
			exp = (unsigned char)recIn[i] & 0b01111111;
			mantissa = conBytesLL(recIn, i+1, 7);

			val = mantissa / pow(2, 56) * pow(16, exp -64);
			if(sign) val *= -1;

			B8Real.push_back(val);
		}
	}
	else if(dataType == 6){
		// ASCII string
		string foo = "";
		sizeBlk++;
		for(i = 4; i <= sizeBlk; i++){
			if(recIn[i] == '\0')	// if string record's size is odd, it must be padded with NULL
				continue;
  		foo = foo + recIn[i];
  	}
  	words = foo;
	}
	else{
		cout << "Unknown data type." << endl;
		return 1;
	}

	return 0;
}

/**
 * BUG loss of extreme precession, use UNIT function for unit header;
 */

unsigned long long GDSfloatCalc(double inVar){
	uint64_t GDSdec = 0;
	unsigned long long integral;
	double deci;
	unsigned long long sign = 0;
	unsigned long long exponent = 80;
	unsigned long long mantissa = 0;

	if(inVar == 0)
		return 0;

	if(inVar < 0){
		inVar *= -1;
		sign = 1;
	}

	integral = inVar;
	deci = inVar - integral;

	// Calculating the decimal
	for(int i = 63; i >= 0; i--){
		deci *= 2;
		if(deci >= 1){
			GDSdec = GDSdec | bitShiftL(1, i);
			deci--;
		}
		if(deci == 0)
			break;
	}

	mantissa = integral;
	for(int i = 15; i >= 0; i--){
		if((bitShiftR(mantissa, 59) & 0xffff) > 0) break;
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
 * [GDSwriteUnits - Hard coding the UNIT record for precision]
 */

void GDSwriteUnits(){
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

	fwrite(data, 1, 20, gdsFileWrite);
}

/**
 * [bitShiftR - bit shift to the right, > 30]
 * @param  inVar [variable to be shifted to the right]
 * @return       [shifted variable]
 */

unsigned long long bitShiftR(unsigned long long inVar, int cnt){

	if(cnt > 32){		// tempULL = (1 << i);
	inVar = inVar >> 30;
	inVar = inVar >> 2;
	inVar = inVar >> (cnt - 32);
	}
	else if(cnt > 30){
		inVar = inVar >> 30;
		inVar = inVar >> (cnt - 30);
	}
	else{
		inVar = inVar >> cnt;
	}

	return inVar;
}

/**
 * [bitShiftL - bit shift to the left, > 30]
 * @param  inVar [variable to be shifted to the left]
 * @return       [shifted variable]
 */

unsigned long long bitShiftL(unsigned long long inVar, int cnt){

	if(cnt > 32){		// tempULL = (1 << i);
	inVar = inVar << 30;
	inVar = inVar << 2;
	inVar = inVar <<( cnt - 32);
	}
	else if(cnt > 30){
		inVar = inVar << 30;
		inVar = inVar << (cnt - 30);
	}
	else{
		inVar = inVar << cnt;
	}

	return inVar;
}

/**
 * [conBytes - Concatenates the array of bytes]
 * @param  inArry [Input array of bytes]
 * @param  cnt    [Amount of bytes that must be concatenated]
 * @return        [The concatenated value]
 */

int conBytes(char inArry[], int start, int cnt){
	int outVal = 0;

	for(int i = 0; i < cnt; i++){
		outVal = (unsigned char)inArry[start + i] << ((cnt - i - 1) * 8) | outVal;
	}

	return outVal;
}

unsigned long long conBytesLL(char inArry[], int start, int cnt){
	unsigned long long outVal = 0;

	for(int i = 0; i < cnt; i++){
		outVal = ((unsigned long long)(unsigned char)inArry[start + i] << ((cnt - i - 1) * 8)) | outVal;
	}

	return outVal;
}

/**
 * [gsdTime - Sets the time once and fetches the original time]
 * @return [Gets the time in GDS format.]
 *           [year, month, day, hour, minute, seconds][last modification, last access]
 */

int * gsdTime(){
	static int timeIO[12];
	static bool timeSet = false;
	tm lctn;

	if(!timeSet){
		time_t now = time(0);
		lctn = *localtime(&now);
		timeSet = true;

		timeIO[0]  = lctn.tm_year + 1900;
		timeIO[1]  = lctn.tm_mon + 1;
		timeIO[2]  = lctn.tm_mday;
		timeIO[3]  = lctn.tm_hour;
		timeIO[4]  = lctn.tm_min;
		timeIO[5]  = lctn.tm_sec;
		timeIO[6]  = timeIO[0];
		timeIO[7]  = timeIO[1];
		timeIO[8]  = timeIO[2];
		timeIO[9]  = timeIO[3];
		timeIO[10] = timeIO[4];
		timeIO[11] = timeIO[5];
	}

	return timeIO;
}