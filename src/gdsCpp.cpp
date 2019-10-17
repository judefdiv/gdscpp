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
 * [gdsToText - Quick converts the GDS file to ASCII without storing the data]
 * @param  fileName [File name of the to be converted gds file]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdsToText(string fileName){
  ifstream gdsFile;

  gdsFile.open(fileName, ios::in | ios::binary);

  if(!gdsFile.is_open()){
    cout << "Error: GDS file \"" << fileName << "\" FAILED to be opened." << endl;
    return 1;
  }

  cout << "Converting \"" << fileName << "\" to ASCII." << endl;

  char *readBlk;
  uint32_t sizeBlk;
  uint32_t hexKey;

  gdsFile.seekg(0, ios::beg);

  do{
    readBlk = new char[2];
    gdsFile.read(readBlk, 2);

    sizeBlk = (((unsigned char)readBlk[0] << 8) | (unsigned char)readBlk[1]);

    gdsFile.seekg(-2, ios::cur);
    readBlk = new char[sizeBlk];
    gdsFile.read(readBlk, sizeBlk);

    hexKey = ((readBlk[2] << 8) | readBlk[3]);

    if(gdsRecordToText(readBlk)){
      cout << "GDS read error" << endl;
      break;
    }
  } while (hexKey != GDS_ENDLIB);

  delete[] readBlk;

  gdsFile.close();
  cout << "Converting \"" << fileName << "\" to ASCII done." << endl;
  return 0;
}


/**
 * [gdsRecordToText - Converts a GDS record to ASCII]
 * @param  recIn [The pointer in memory to the GDS record to be converted]
 * @return       [0 - Exit Success; 1 - Exit Failure]
 */

int gdsRecordToText(char *recIn){
  uint32_t GDSKey;
  bitset<16> bitarr;
  vector<int> integer;
  vector<double> B8Real;
  string words;
  string keyName;
  string lineOut;

  uint8_t dataType;

  if(GDSdistill(recIn, GDSKey, bitarr, integer, B8Real, words) == 1){
    return 1;
  }
  dataType = GDSKey;

  keyName = GDSkey2ASCII(GDSKey);

  if (!keyName.compare("\0")){
    cout << "Key not found: 0x" << hex << GDSKey << dec << endl;
    return 1;
  }

  cout << "[" << keyName << "]";

  if (dataType == 0){
    // no data
    cout << endl;
  }
  else if (dataType == 1){
    //bit array
    int i = 4;
    bitset<8> bitsIn0(recIn[i++]);
    bitset<8> bitsIn1(recIn[i]);

    cout << ":{" << "0b" << bitsIn0.to_string() << " 0b" + bitsIn1.to_string() << "}" << endl;
  }
  else if (dataType == 2 || dataType == 3){
    //signed integers

    cout << ":{";
    for(unsigned int j = 0; j < integer.size(); j++){
      cout << integer[j];
      if(j < integer.size() -1){
        cout << ", ";
      }
    }
    cout << "}" << endl;
  }
  else if (dataType == 4){
    //4 byte real (NOT USED)
    cout << "Unsupported 4 byte real variable." << endl;
    return 1;
  }
  else if (dataType == 5){
    //8 byte real

    cout << ":{";
    cout.precision(12);
    for(unsigned int j = 0; j < B8Real.size(); j++){
      cout << B8Real[j];
      if(j < B8Real.size() -1){
        cout << ", ";
      }
    }
    cout << "}" << endl;
  }
  else if (dataType == 6)
  {
    // ASCII string

    cout << ":{" << words << "}" << endl;
  }
  else
  {
    cout << "Smoke detected." << endl;
    return 1;
  }

  return 0;
}

/**
 * [gdscpp::write - Creating a GDS file from STR class]
 * @param  fileName [The file name of the GDS file that is going to generated]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdscpp::write(string fileName){
  gdsForge foo;
  foo.importGDSfile(this->GDSfileName);
  return foo.gdsCreate(fileName, this->STR, this->units);
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
    cout << "Error: GDS file \"" << fileName << "\" FAILED to be opened." << endl;
    return 1;
  }

  cout << "Converting \"" << fileName << "\" to ASCII." << endl;

  char *readBlk;
  uint32_t sizeBlk;
  uint32_t hexKey;

  gdsFile.seekg(0, ios::beg);

  do
  {
    readBlk = new char[2];
    gdsFile.read(readBlk, 2);

    sizeBlk = (((unsigned char)readBlk[0] << 8) | (unsigned char)readBlk[1]);

    gdsFile.seekg(-2, ios::cur);
    readBlk = new char[sizeBlk];
    gdsFile.read(readBlk, sizeBlk);

    hexKey = ((readBlk[2] << 8) | readBlk[3]);

    if (GDSrecord2ASCII(readBlk))
    {
      cout << "GDS read error" << endl;
      break;
    }
  } while (hexKey != GDS_ENDLIB);

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

  string foo;

  foo = GDSkey2ASCII(hexKey);

  if (!foo.compare("\0"))
  { // remove for speed increase
    cout << "Key not found: 0x" << hex << hexKey << dec << endl;
    return 1;
  }

  cout << "[" << foo << "]";

  if (dataType == 0)
  {
    // no data
    cout << endl;
  }
  else if (dataType == 1)
  {
    //bit array
    bitset<8> bitsIn0(recIn[i++]);
    bitset<8> bitsIn1(recIn[i]);
    lineOut = "0b" + bitsIn0.to_string() + " 0b" + bitsIn1.to_string();
    cout << ":{" << lineOut << "}" << endl;
  }
  else if (dataType == 2)
  {
    //2 byte signed int
    for (i = 4; i <= sizeBlk; i = i + 2)
    {
      intOut = conBytes(recIn, i, 2);
      lineOut = lineOut + to_string(intOut) + ", ";
    }
    lineOut.pop_back();
    lineOut.pop_back();
    cout << ":{" << lineOut << "}" << endl;
  }
  else if (dataType == 3)
  {
    //4 byte signed int
    for (i = 4; i <= sizeBlk; i = i + 4)
    {
      intOut = conBytes(recIn, i, 4);
      lineOut = lineOut + to_string(intOut) + ", ";
    }
    lineOut.pop_back();
    lineOut.pop_back();
    cout << ":{" << lineOut << "}" << endl;
  }
  else if (dataType == 4)
  {
    //4 byte real (NOT USED)
    cout << "Unsupported 4 byte real variable." << endl;
  }
  else if (dataType == 5)
  {
    //8 byte real
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
  else if (dataType == 6)
  {
    // ASCII string
    string foo = "";
    sizeBlk++;
    for (i = 4; i <= sizeBlk; i++)
    {
      if (recIn[i] == '\0') // if string record's size is odd, it must be padded with NULL
        continue;
      foo = foo + recIn[i];
    }
    cout << ":{\"" << foo << "\"}" << endl;
  }
  else
  {
    cout << "Smoke detected." << endl;
  }

  return 0;
}

/**
 * [gdscpp::to_str - Displays all the stored data in the class]
 */

void gdscpp::to_str()
{
  cout << "GDScpp class:" << endl;

  for (unsigned int i = 0; i < this->STR.size(); i++)
  {
    this->STR[i].to_str();
  }
}

/**
 * [gdsSTR::to_str - Displays all the stored data in the class]
 */

void gdsSTR::to_str()
{
  cout << "GDS STR class:" << endl;

  for (unsigned int i = 0; i < this->SREF.size(); i++)
  {
    this->SREF[i].to_str();
  }

  for (unsigned int i = 0; i < this->BOUNDARY.size(); i++)
  {
    this->BOUNDARY[i].to_str();
  }

  for (unsigned int i = 0; i < this->PATH.size(); i++)
  {
    this->PATH[i].to_str();
  }

  for (unsigned int i = 0; i < this->NODE.size(); i++)
  {
    this->NODE[i].to_str();
  }

  for (unsigned int i = 0; i < this->TEXT.size(); i++)
  {
    this->TEXT[i].to_str();
  }
}

/**
 * [gdsSREF::to_str - Displays all the stored data in the class]
 */

void gdsSREF::to_str()
{
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

void gdsBOUNDARY::to_str()
{
  cout << "GDS BOUNDARY class:" << endl;

  cout << "  layer: " << this->layer << endl;

  cout << "  xCor:\tyCor:" << endl;
  for (unsigned int i = 0; i < this->xCor.size(); i++)
  {
    cout << this->xCor[i] << "\t" << this->xCor[i] << endl;
  }
}

/**
 * [gdsPATH::to_str - Displays all the stored data in the class]
 */

void gdsPATH::to_str()
{
  cout << "GDS PATH class:" << endl;

  cout << "  layer: " << this->layer << endl;
  cout << "  width: " << this->width << endl;

  cout << "  xCor:\tyCor:" << endl;
  for (unsigned int i = 0; i < this->xCor.size(); i++)
  {
    cout << this->xCor[i] << "\t" << this->xCor[i] << endl;
  }
}

/**
 * [gdsNODE::to_str - Displays all the stored data in the class]
 */

void gdsNODE::to_str()
{
  cout << "GDS NODE class:" << endl;

  cout << "  layer: " << this->layer << endl;
  cout << "  nodetype: " << this->nodetype << endl;
  cout << "  propattr: " << this->propattr << endl;
  cout << "  propvalue: " << this->propvalue << endl;

  cout << "  xCor:\tyCor:" << endl;
  for (unsigned int i = 0; i < this->xCor.size(); i++)
  {
    cout << this->xCor[i] << "\t" << this->xCor[i] << endl;
  }
}

/**
 * [gdsTEXT::to_str - Displays all the stored data in the class]
 */

void gdsTEXT::to_str()
{
  cout << "GDS TEXT class:" << endl;

  cout << "  layer: " << this->layer << endl;
  cout << "  textbody: " << this->textbody << endl;
  cout << "  scale: " << this->scale << endl;
  cout << "  xCor: " << this->xCor << endl;
  cout << "  yCor: " << this->yCor << endl;
}

// Re-sets the specified cpp object to its default values
void gdscpp::reset()
{
  version_number = 7;
  generations = 3;
  units[0] = 0.001;
  units[1] = 1e-9;
  STR.clear();
  last_modified.clear();
  library_name = "Untitled_library";
}

// Re-sets the specified STR object to its default values
void gdsSTR::reset()
{
  string name = "\0";
  heirarchical_level = 0;
  bounding_box[0] = {0};
  bounding_box[1] = {0};
  BOUNDARY.clear();
  PATH.clear();
  SREF.clear();
  AREF.clear();
  TEXT.clear();
  NODE.clear();
  BOX.clear();
}
// Re-sets the specified BOUNDARY object to its default values
void gdsBOUNDARY::reset()
{
  layer = 0;
  dataType = 0;
  xCor.clear();
  yCor.clear();
  propattr = 0;
  propvalue = "\0";
}

// Re-sets the specified PATH object to its default values
void gdsPATH::reset()
{
  layer = 0;
  dataType = 0;
  pathtype = 0;
  width = 0;
  xCor.clear();
  yCor.clear();
  propattr = 0;
  propvalue = "\0";
}

// Re-sets the specified SREF object to its default values
void gdsSREF::reset()
{
  name = "\0";
  sref_flags.reset();
  reflection = false;
  angle = 0;
  scale = 1;
  xCor = 0;
  yCor = 0;
  propattr = 0;
  propvalue = "\0";
}

void gdsAREF::reset()
{
  name = "\0";
  aref_transformation_flags.reset();
  reflection = false;
  angle = 0;
  scale = 1;
  colCnt = 0;
  rowCnt = 0;
  xCor = 0;
  yCor = 0;
  xCorRow = 0;                 // X_2
  yCorRow = 0;                 // Y_2
  xCorCol = 0;                 // X_3
  yCorCol = 0;                 // Y_3
  propattr = 0;
  propvalue = "\0";
}

void gdsTEXT::reset()
{
  presentation_flags.reset();
  text_type = 0;
  path_type = 0;
  width = 0;
  text_transformation_flags.reset();
  textbody = "\0";
  layer = 0;
  scale = 1;
  angle = 0;
  xCor = 0;
  yCor = 0;
  propattr = 0;
  propvalue = "\0";
}

void gdsNODE::reset()
{
  plex = 0; // optional
  layer = 0;
  nodetype = 0;
  // A maximum of 50 coordinates
  xCor.clear();
  yCor.clear();
  propattr = 0;
  propvalue = "\0";
}

void gdsBOX::reset()
{
  plex = 0; // optional
  layer = 0;
  boxtype = 0;
  // A maximum of 50 coordinates
  xCor.clear();
  yCor.clear();
  propattr = 0;
  propvalue = "\0";
}

// Standard function for adding one structure onto the stack.
void gdscpp::setSTR(gdsSTR target_structure)
{
  if (!STR_Lookup.count(target_structure.name))//if doesn't already exist
  {
    STR.push_back(target_structure);
    STR_Lookup.insert({target_structure.name, (STR.size()-1)});
  }
}

// Overloaded function for appending multiple structures
void gdscpp::setSTR(vector<gdsSTR> target_structure)
{
  STR.insert(STR.end(), target_structure.begin(), target_structure.end());
}

/**
 * [gdscpp::createHierarchy - Creates a tree structure of all the GDS structure dependencies]
 * @return [0 - Exit Success; 1 - Exit Failure]
 */

double gdscpp::get_database_units()
{
  return units[1];
}

int gdscpp::createHierarchy(){
  vector<unsigned int> rootSTRi;
  rootSTRi = this->findRootSTR();

  return 0;
}


/**
 * [gdscpp::findRootSTR finds the root structures]
 * @return [vector of the STR indexes of the root GDS STR]
 */

vector<unsigned int> gdscpp::findRootSTR(){
  cout << "Finding the root structures." << endl;
  vector<string> mainSTR;
  vector<string> refSTR;

  vector<unsigned int> rootSTRindexes;

  bool vecFound;

  for(unsigned int i = 0; i < this->STR.size(); i++){
    vecFound = false;
    for(unsigned int j = 0; j < this->STR.size(); j++){
      for(unsigned int k = 0; k < this->STR[j].SREF.size(); k++){
        if(!this->STR[i].name.compare(this->STR[j].SREF[k].name)){
          vecFound = true;
          break;
        }
      }
      if(vecFound){
        break;
      }
    }
    if(vecFound == false){
      rootSTRindexes.push_back(i);
    }
  }

  // display the root GDS STR
  cout << "Root GDS structures: ";

  vector<unsigned int>::iterator fooVec;
  for(fooVec = rootSTRindexes.begin(); fooVec != rootSTRindexes.end(); fooVec++){
    cout << "  " << this->STR[*fooVec].name;
  }
  cout << endl;

  return rootSTRindexes;
}

/**
 * [gdscpp::genDot - Creates the tree diagram of all the GDS structure dependencies]
 * @param  fileName [The file name of the to be created dot file]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */

int gdscpp::genDot(string fileName){
  cout << "Generating Dot file:\"" << fileName << "\" file" << endl;

  vector<string> fromSTR;
  vector<string> toSTR;

  bool foundSTR;
  for(int i = 0; i < this->STR.size(); i++){
    for(int j = 0; j < this->STR[i].SREF.size(); j++){
      foundSTR = true;
      for(unsigned int k = 0; k < fromSTR.size(); k++){
        if(!fromSTR[k].compare(this->STR[i].name) && !toSTR[k].compare(this->STR[i].SREF[j].name)){
          foundSTR = false;
          break;
        }
      }
      if(foundSTR){
        fromSTR.push_back(this->STR[i].name);
        toSTR.push_back(this->STR[i].SREF[j].name);
      }
    }
  }

  // ------------------------ creating dot file ------------------------

  FILE *dotFile;
  dotFile = fopen("foo.dot", "w");

  string lineStr;

  lineStr = "digraph GDStree {\n";
  fputs(lineStr.c_str(), dotFile);

  for(unsigned int i = 0; i < fromSTR.size(); i++){
    lineStr = "\t" + fromSTR[i] + " -> " + toSTR[i] + ";\n";
    fputs(lineStr.c_str(), dotFile);
    }

  lineStr = "}";
  fputs(lineStr.c_str(), dotFile);

  fclose(dotFile);
  cout << "Dot file done." << endl;

  // ------------------------ executing dot script ------------------------
  string bashCmd;
  bashCmd = "dot -Tjpg foo.dot -o " + fileName;

  if(system(bashCmd.c_str()) == -1){
    cout << "Bash command :\"" << bashCmd << "\" error." << endl;
    return 1;
  }

  bashCmd = "rm foo.dot";
  if(system(bashCmd.c_str()) == -1){
    cout << "Bash command :\"" << bashCmd << "\" error." << endl;
    return 1;
  }

  return 0;
}