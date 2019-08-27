
/**
 * Authors:  		Jude de Villiers & Heinrich Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-22
 * Modified:
 * license: 
 * Description: .
 * File:			gdscpp.cpp
 */

#include "gdsClass.hpp"

/**
 * Constructor
 */

gdscpp::gdscpp()
{
	// mapping the keys
	GDSkeys[GDS_HEADER] = "HEADER";
	GDSkeys[GDS_BGNLIB] = "BGNLIB";
	GDSkeys[GDS_LIBNAME] = "LIBNAME";
	GDSkeys[GDS_UNITS] = "UNITS";
	GDSkeys[GDS_ENDLIB] = "ENDLIB";
	GDSkeys[GDS_BGNSTR] = "BGNSTR";
	GDSkeys[GDS_STRNAME] = "STRNAME";
	GDSkeys[GDS_ENDSTR] = "ENDSTR";
	GDSkeys[GDS_BOUNDARY] = "BOUNDARY";
	GDSkeys[GDS_PATH] = "PATH";
	GDSkeys[GDS_SREF] = "SREF";
	GDSkeys[0x0b00] = "AREF";
	GDSkeys[0x0c00] = "TEXT";
	GDSkeys[GDS_LAYER] = "LAYER";
	GDSkeys[GDS_DATATYPE] = "DATATYPE";
	GDSkeys[GDS_WIDTH] = "WIDTH";
	GDSkeys[GDS_XY] = "XY";
	GDSkeys[GDS_ENDEL] = "ENDEL";
	GDSkeys[GDS_SNAME] = "SNAME";
	GDSkeys[0x1302] = "COLROW";
	GDSkeys[0x1500] = "NODE";
	GDSkeys[0x1602] = "TEXTTYPE";
	GDSkeys[0x1701] = "PRESENTATION";
	GDSkeys[0x1906] = "STRING";
	GDSkeys[GDS_STRANS] = "STRANS";
	GDSkeys[GDS_MAG] = "MAG";
	GDSkeys[GDS_ANGLE] = "ANGLE";
	GDSkeys[0x1f06] = "REFLIBS";
	GDSkeys[0x2006] = "FONTS";
	GDSkeys[GDS_PATHTYPE] = "PATHTYPE";
	GDSkeys[0x2202] = "GENERATIONS";
	GDSkeys[0x2306] = "ATTRTABLE";
	GDSkeys[0x2601] = "EFLAGS";
	GDSkeys[0x2a02] = "NODETYPE";
	GDSkeys[0x2b02] = "PROPATTR";
	GDSkeys[0x2c06] = "PROPVALUE";
	GDSkeys[0x2f03] = "PLEX"; // new addition, XIC, no idea...
}

/**
 * [gdscpp::quick2ASCII - Quick converts the GDS file to ASCII without storing the data]
 * @param  fileName [File name of the to be converted gds file]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdscpp::quick2ASCII(string fileName)
{
	ifstream gdsFile;

	gdsFile.open(fileName, ios::in | ios::binary);

	if (!gdsFile.is_open())
	{
		cout << "GDS file \"" << fileName << "\" FAILED to be opened." << endl;
		return EXIT_FAILURE;
	}

	cout << "Converting \"" << fileName << "\" to ASCII." << endl;

	char *readBlk;
	uint32_t sizeBlk;
	uint32_t hexKey;

	gdsFile.seekg(0, ios::beg);

	do
	{
		readBlk = new char[2];													  // The first two bytes of the header contain a count (in eight-bit bytes) of the total record length.
		gdsFile.read(readBlk, 2);												  // Determine record length
		sizeBlk = (((unsigned char)readBlk[0] << 8) | (unsigned char)readBlk[1]); // Convert length from binary to int
		gdsFile.seekg(-2, ios::cur);											  // Return to start of record as count includes first 2 bytes.
		readBlk = new char[sizeBlk];											  // Create the record of specified size.
		gdsFile.read(readBlk, sizeBlk);											  // Read the record.
		hexKey = ((readBlk[2] << 8) | readBlk[3]);								  // Store the hex key while check
		if (GDSrecord2ASCII(readBlk))
		{
			cout << "GDS read error" << endl;
			break;
		}
	} while (hexKey != GDS_ENDLIB);

	delete[] readBlk;

	gdsFile.close();
	cout << "Converting \"" << fileName << "\" to ASCII done." << endl;
	return EXIT_SUCCESS;
}

/**
 * [gdscpp::GDSrecord2ASCII - Read a single record]
 * @param recIn [Char pointer to the start of binary GDS record]
 */

int gdscpp::GDSrecord2ASCII(char *recIn)
{
	uint32_t sizeBlk;
	uint32_t hexKey;
	uint32_t i = 0;
	int intOut = 0;
	uint8_t dataType;
	string lineOut = "";

	sizeBlk = (((unsigned char)recIn[0] << 8) | (unsigned char)recIn[1]) - 2;
	hexKey = ((recIn[2] << 8) | recIn[3]);
	dataType = hexKey;
	i = 4;

	if (this->GDSkeys.find(hexKey) == this->GDSkeys.end())
	{ // remove for speed increase
		cout << "Key not found: 0x" << hex << hexKey << dec << endl;
		return EXIT_FAILURE;
	}
	cout << "[" << this->GDSkeys.find(hexKey)->second << "]";				// Place brackets and repeat record
	if (dataType == 0)														// no data
	{
		cout << endl;
	}
	else if (dataType == 1)													//bit array
	{
		bitset<8> bitsIn0(recIn[i++]);
		bitset<8> bitsIn1(recIn[i]);
		lineOut = "0b" + bitsIn0.to_string() + " 0b" + bitsIn1.to_string();
		cout << ":{" << lineOut << "}" << endl;
	}
	else if (dataType == 2)													// Two byte signed int
	{
		for (i = 4; i <= sizeBlk; i = i + 2)
		{
			intOut = conBytes(recIn, i, 2);
			lineOut = lineOut + to_string(intOut) + ", ";
		}
		lineOut.pop_back();
		lineOut.pop_back();
		cout << ":{" << lineOut << "}" << endl;
	}
	else if (dataType == 3)													// Four byte signed int
	{
		for (i = 4; i <= sizeBlk; i = i + 4)
		{
			intOut = conBytes(recIn, i, 4);
			lineOut = lineOut + to_string(intOut) + ", ";
		}
		lineOut.pop_back();
		lineOut.pop_back();
		cout << ":{" << lineOut << "}" << endl;
	}
	else if (dataType == 4)													// Four byte real (NOT USED)
	{
		cout << "Unsupported 4 byte real variable." << endl;
	}
	else if (dataType == 5)													// Eight byte real
	{
		double val = 0;
		int sign = 0;
		double exp = 0;
		double mantissa = 0;

		cout << ":{";
		for (i = 4; i <= sizeBlk; i = i + 8)
		{

			sign = (unsigned char)recIn[i] >> 7;
			exp = (unsigned char)recIn[i] & 0b01111111;
			mantissa = conBytesLL(recIn, i + 1, 7);

			val = mantissa / pow(2, 56) * pow(16, exp - 64);
			if (sign)
				val *= -1;

			cout.precision(12);
			cout << val;
			if (i != sizeBlk - 6)
				cout << ", ";
		}
		cout << "}" << endl;
	}
	else if (dataType == 6)													// ASCII string
	{
		string foo = "";
		sizeBlk++;
		for(i = 4; i <= sizeBlk; i++){
			if(recIn[i] == '\0')	// if string record's size is odd, it must be padded with NULL
				continue;
  		foo = foo + recIn[i];
  	}
  	cout << ":{\""<< foo << "\"}" << endl;
	}
	else
	{
		cout << "Unhandled state." << endl;
	}

	return EXIT_SUCCESS;
}
