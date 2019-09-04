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

#include "gdsCpp.hpp"

/**
 * [gdscpp::read description]
 * @param  fileName [Teh file name of the GDS file that is going to be read in]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdscpp::import(string fileName){
  ifstream gdsFile;
  gdsFile.open(fileName, ios::in|ios::binary);

  if(!gdsFile.is_open()){
    cout << "GDS file \"" << fileName << "\" FAILED to be opened." << endl;
    return EXIT_FAILURE;
  }

  cout << "Importing \"" << fileName << "\" into GDSCpp." << endl;

  char            *current_readBlk;
  uint32_t        current_sizeBlk;
  uint32_t        current_GDSKey;
  bitset<16>      current_bitarr;
  vector<char>    current_integer;
  vector<double>  current_B8Real;
  string          current_words   ="\0";

  gdsFile.seekg(0, ios::beg);
  do{
    current_readBlk = new char [2];
    gdsFile.read(current_readBlk, 2);

    current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) | (unsigned char)current_readBlk[1]);

    gdsFile.seekg(-2, ios::cur);
    current_readBlk = new char [current_sizeBlk];
    gdsFile.read(current_readBlk, current_sizeBlk);

    if(GDSdistill(current_readBlk, current_GDSKey, current_bitarr, current_integer, current_B8Real, current_words)){
      cout << "GDS read error" << endl;
      break;
    }
    else    // Highest tier of data import: HEADER, BGNLIB, LIBNAME, GENERATIONS, UNITS, BGNSTR, ENDDLIB
    {
      if(GDSrecord2ASCII(current_readBlk)){         //
        cout << "GDS read error" << endl;           // \   Remove this once debugging finished
        break;                                      // /
      }                                             //
      switch (current_GDSKey)
      {
      case GDS_HEADER:
        //version_number = current_integer; // TODO: Continue from here
        break;
      case GDS_BGNLIB:

        break;
      case GDS_LIBNAME:

        break;
      case GDS_GENERATIONS:

        break;
      case GDS_UNITS:

        break;
      case GDS_BGNSTR:

        break;
      case GDS_ENDLIB:
        cout << "Reached end of library." << endl;
        break;

      default:
        cout << "Unrecognized record." << endl;
        break;
      }
    }
  } while(current_GDSKey != GDS_ENDLIB);
  delete[] current_readBlk;
  cout << "GDS file successfully imported." << endl;
   return 0;
}

/**
 * [gdscpp::write - Creating a GDS file from STR class]
 * @param  fileName [The file name of the GDS file that is going to generated]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdscpp::write(string fileName){
  gdsForge foo;
  return foo.gdsCreate(fileName, this->STR, this->units);
}


/**
 * [gdscpp::quick2ASCII - Quick converts the GDS file to ASCII without storing the data]
 * @param  fileName [File name of the to be converted gds file]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdscpp::quick2ASCII(string fileName){
  ifstream gdsFile;

  gdsFile.open(fileName, ios::in|ios::binary);

  if(!gdsFile.is_open()){
    cout << "GDS file \"" << fileName << "\" FAILED to be opened." << endl;
    return 1;
  }

  cout << "Converting \"" << fileName << "\" to ASCII." << endl;

  char *readBlk;
  uint32_t sizeBlk;
  uint32_t hexKey;

  gdsFile.seekg(0, ios::beg);

  do{
    readBlk = new char [2];
    gdsFile.read(readBlk, 2);

    sizeBlk = (((unsigned char)readBlk[0] << 8) | (unsigned char)readBlk[1]);

    gdsFile.seekg(-2, ios::cur);
    readBlk = new char [sizeBlk];
    gdsFile.read(readBlk, sizeBlk);

    hexKey = ((readBlk[2] << 8) | readBlk[3]);

    if(GDSrecord2ASCII(readBlk)){
      cout << "GDS read error" << endl;
      break;
    }
  } while(hexKey != GDS_ENDLIB);

  delete[] readBlk;

  gdsFile.close();
  cout << "Converting \"" << fileName << "\" to ASCII done." << endl;
  return 0;
}

/**
 * [gdscpp::GDSrecord2ASCII - Read a single record]
 * @param  recIn [Char pointer to the start of binary GDS record]
 * @return       [0 - Exit Success; 1 - Exit Failure]
 */

int gdscpp::GDSrecord2ASCII(char *recIn){
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

  string foo;

  foo = GDSkey2ASCII(hexKey);

  if(!foo.compare("\0")){      // remove for speed increase
    cout << "Key not found: 0x" << hex << hexKey << dec << endl;
    return 1;
  }

  cout << "[" << foo << "]";

  if(dataType == 0){
    // no data
    cout << endl;
  }
  else if(dataType == 1){
    //bit array
    bitset<8> bitsIn0(recIn[i++]);
    bitset<8> bitsIn1(recIn[i]);
    lineOut = "0b" + bitsIn0.to_string() + " 0b" + bitsIn1.to_string();
    cout << ":{" << lineOut << "}" << endl;
  }
  else if(dataType == 2){
    //2 byte signed int
    for(i = 4; i <= sizeBlk; i = i + 2){
      intOut = conBytes(recIn, i, 2);
      lineOut = lineOut + to_string(intOut) + ", ";
    }
    lineOut.pop_back();
    lineOut.pop_back();
    cout << ":{" << lineOut << "}" << endl;
  }
  else if(dataType == 3){
    //4 byte signed int
    for(i = 4; i <= sizeBlk; i = i + 4){
      intOut = conBytes(recIn, i, 4);
      lineOut = lineOut + to_string(intOut) + ", ";
    }
    lineOut.pop_back();
    lineOut.pop_back();
    cout << ":{" << lineOut << "}" << endl;
  }
  else if(dataType == 4){
    //4 byte real (NOT USED)
    cout << "Unsupported 4 byte real variable." << endl;
  }
  else if(dataType == 5){
    //8 byte real
    double val = 0;
    int sign = 0;
    double exp = 0;
    double mantissa = 0;

    cout << ":{";
    for(i = 4; i <= sizeBlk; i = i + 8){

      sign = (unsigned char)recIn[i] >> 7;
      exp = (unsigned char)recIn[i] & 0b01111111;
      mantissa = conBytesLL(recIn, i+1, 7);

      val = mantissa / pow(2, 56) * pow(16, exp -64);
      if(sign) val *= -1;

      cout.precision(12);
      cout << val;
      if(i != sizeBlk-6)
        cout << ", ";
    }
    cout << "}" << endl;
  }
  else if(dataType == 6){
    // ASCII string
    string foo = "";
    sizeBlk++;
    for(i = 4; i <= sizeBlk; i++){
      if(recIn[i] == '\0')  // if string record's size is odd, it must be padded with NULL
        continue;
      foo = foo + recIn[i];
    }
    cout << ":{\""<< foo << "\"}" << endl;
  }
  else{
    cout << "Smoke detected." << endl;
  }

  return 0;
}

/**
 * [gdscpp::to_str - Displays all the stored data in the class]
 */

void gdscpp::to_str(){
  cout << "GDScpp class:" << endl;

  for(unsigned int i = 0; i < this->STR.size(); i++){
    this->STR[i].to_str();
  }
}

/**
 * [gdsSTR::to_str - Displays all the stored data in the class]
 */

void gdsSTR::to_str(){
  cout << "GDS STR class:" << endl;

  for(unsigned int i = 0; i < this->SREF.size(); i++){
    this->SREF[i].to_str();
  }

  for(unsigned int i = 0; i < this->BOUNDARY.size(); i++){
    this->BOUNDARY[i].to_str();
  }

  for(unsigned int i = 0; i < this->PATH.size(); i++){
    this->PATH[i].to_str();
  }

  for(unsigned int i = 0; i < this->NODE.size(); i++){
    this->NODE[i].to_str();
  }

  for(unsigned int i = 0; i < this->TEXT.size(); i++){
    this->TEXT[i].to_str();
  }
}

/**
 * [gdsSREF::to_str - Displays all the stored data in the class]
 */

void gdsSREF::to_str(){
  cout << "GDS REF class:" << endl;

  cout << "  Name: " << this->name << endl;
  cout << "  reflection: " << this->reflection << endl;
  cout << "  angle: " << this->angle << endl;
  cout << "  scale: " << this->scale << endl;
  cout << "  xCor: " << this->xCor << endl;
  cout << "  yCor: " << this->yCor << endl;
}

/**
 * [gdsBOUNDARY::to_str - Displays all the stored data in the class]
 */

void gdsBOUNDARY::to_str(){
  cout << "GDS BOUNDARY class:" << endl;

  cout << "  layer: " << this->layer << endl;

  cout << "  xCor:\tyCor:" << endl;
  for(unsigned int i = 0; i < this->xCor.size(); i++){
    cout << this->xCor[i] << "\t" << this->xCor[i] << endl;
  }
}

/**
 * [gdsPATH::to_str - Displays all the stored data in the class]
 */

void gdsPATH::to_str(){
  cout << "GDS PATH class:" << endl;

  cout << "  layer: " << this->layer << endl;
  cout << "  width: " << this->width << endl;

  cout << "  xCor:\tyCor:" << endl;
  for(unsigned int i = 0; i < this->xCor.size(); i++){
    cout << this->xCor[i] << "\t" << this->xCor[i] << endl;
  }
}

/**
 * [gdsNODE::to_str - Displays all the stored data in the class]
 */

void gdsNODE::to_str(){
  cout << "GDS NODE class:" << endl;

  cout << "  layer: " << this->layer << endl;
  cout << "  nodetype: " << this->nodetype << endl;
  cout << "  propattr: " << this->propattr << endl;
  cout << "  propvalue: " << this->propvalue << endl;

  cout << "  xCor:\tyCor:" << endl;
  for(unsigned int i = 0; i < this->xCor.size(); i++){
    cout << this->xCor[i] << "\t" << this->xCor[i] << endl;
  }
}

/**
 * [gdsTEXT::to_str - Displays all the stored data in the class]
 */

void gdsTEXT::to_str(){
  cout << "GDS TEXT class:" << endl;

  cout << "  layer: " << this->layer << endl;
  cout << "  textbody: " << this->textbody << endl;
  cout << "  scale: " << this->scale << endl;
  cout << "  xCor: " << this->xCor << endl;
  cout << "  yCor: " << this->yCor << endl;
}