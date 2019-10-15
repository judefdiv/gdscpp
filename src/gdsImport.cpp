/**
 * Author:
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-10-14
 * Modified:
 * license:
 * Description: Import function for storing gds in memory
 * File:				gdsCpp.hpp
 */

#include "gdscpp/gdsCpp.hpp"

/**
 * [gdscpp::read description]
 * @param  fileName [The file name of the GDS file that is going to be read in]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */
int gdscpp::import(const string &fileName)
{
  // Variable declarations
  ifstream gdsFile;
  char *current_readBlk;
  uint32_t current_sizeBlk;
  uint32_t current_GDSKey;
  bitset<16> current_bitarr;
  vector<int> current_integer;
  auto current_int_iter = current_integer.begin();
  vector<double> current_B8Real;
  string current_words = "\0";
  bool odd_state = false;

  // Memory where element objects are held until stored into library.
  gdsSTR plchold_str;
  gdsBOUNDARY plchold_bnd;
  gdsPATH plchold_path;
  gdsSREF plchold_sref;
  gdsAREF plchold_aref;
  gdsTEXT plchold_text;
  gdsNODE plchold_node;
  gdsBOX plchold_box;

  // Method
  gdsFile.open(fileName, ios::in | ios::binary);

  if (!gdsFile.is_open()) {
    cout << "Error: GDS file \"" << fileName << "\" FAILED to be opened."
         << endl;
    return EXIT_FAILURE;
  }
  cout << "Importing \"" << fileName << "\" into GDSCpp." << endl;
  gdsFile.seekg(0, ios::beg);
  do {
    current_readBlk = new char[2];
    gdsFile.read(current_readBlk, 2);
    current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                       (unsigned char)current_readBlk[1]);
    gdsFile.seekg(-2, ios::cur);
    current_readBlk = new char[current_sizeBlk];
    gdsFile.read(current_readBlk, current_sizeBlk);
    if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                   current_integer, current_B8Real, current_words)) {
      cout << "Error: Unable to read GDS file." << endl;
      break;
    } else {
      switch (current_GDSKey)
      // Highest tier of data: HEADER, BGNLIB, LIBNAME, GENERATIONS, UNITS,
      // BGNSTR, ENDDLIB
      {
      case GDS_HEADER:
        version_number = (int)current_integer[0];
        break;
      case GDS_BGNLIB:
        std::transform(current_integer.begin(), current_integer.end(),
                       std::back_inserter(last_modified),
                       [](char a) { return (int)a; });
        break;
      case GDS_LIBNAME:
        library_name = current_words;
        break;
      case GDS_GENERATIONS:
        generations = (int)current_integer[0];
        break;
      case GDS_UNITS:
        units[0] = current_B8Real[0];
        units[1] = current_B8Real[1];
        break;
      case GDS_BGNSTR:
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ FIRST NEST
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        //                           BOUNDARY, PATH SREF AREF TEXT NODE BOX
        plchold_str.reset();
        std::transform(current_integer.begin(), current_integer.end(),
                       std::back_inserter(plchold_str.last_modified),
                       [](char a) { return (int)a; });
        do {
          current_readBlk = new char[2];
          gdsFile.read(current_readBlk, 2);

          current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                             (unsigned char)current_readBlk[1]);

          gdsFile.seekg(-2, ios::cur);
          current_readBlk = new char[current_sizeBlk];
          gdsFile.read(current_readBlk, current_sizeBlk);

          if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                         current_integer, current_B8Real, current_words)) {
            cout << "Error: Unable to read GDS file." << endl;
            break;
          } else {
            switch (current_GDSKey) {
            case GDS_STRNAME:
              plchold_str.name = current_words;
              break;
            case GDS_BOUNDARY:
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ secondary nest [BOUNDARY]
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              // PLEX LAYER DATATYPE XY PROPATTR PROPVALUE
              plchold_bnd.reset();
              do {
                current_readBlk = new char[2];
                gdsFile.read(current_readBlk, 2);

                current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                                   (unsigned char)current_readBlk[1]);

                gdsFile.seekg(-2, ios::cur);
                current_readBlk = new char[current_sizeBlk];
                gdsFile.read(current_readBlk, current_sizeBlk);

                if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                               current_integer, current_B8Real,
                               current_words)) {
                  cout << "Error: Unable to read GDS file." << endl;
                  break;
                } else {
                  switch (current_GDSKey) {
                  case GDS_PLEX:
                    plchold_bnd.plex = current_integer[0];
                    break;
                  case GDS_LAYER:
                    plchold_bnd.layer = current_integer[0];
                    break;
                  case GDS_DATATYPE:
                    plchold_bnd.dataType = current_integer[0];
                    break;
                  case GDS_XY:
                    // confirm xy as pairs
                    if (current_integer.size() % 2 == 0) {
                      current_int_iter = current_integer.begin();
                      odd_state = false;
                      while (current_int_iter != current_integer.end()) {
                        if (odd_state == false) {
                          // x append
                          plchold_bnd.xCor.push_back(*current_int_iter);
                        } else {
                          // y append
                          plchold_bnd.yCor.push_back(*current_int_iter);
                        }
                        odd_state = !odd_state;
                        current_int_iter++;
                      }
                    } else {
                      cout << "Error: XY co_ordinates uneven" << endl;
                    }
                    break;
                  case GDS_PROPATTR:
                    plchold_bnd.propattr = current_integer[0];
                    break;
                  case GDS_PROPVALUE:
                    plchold_bnd.propvalue = current_words;
                    break;
                  case GDS_ENDEL:

                    break;
                  default:
                    cout << "Error: Unrecognized record." << endl;
                    break;
                  }
                }
              } while (current_GDSKey != GDS_ENDEL);
              plchold_str.BOUNDARY.push_back(plchold_bnd);
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              break;
            case GDS_PATH:
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ secondary nest [PATH]
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              // PLEX LAYER DATATYPE PATHTYPE WIDTH XY PROPATTR PROPVALUE
              plchold_path.reset();
              do {
                current_readBlk = new char[2];
                gdsFile.read(current_readBlk, 2);

                current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                                   (unsigned char)current_readBlk[1]);

                gdsFile.seekg(-2, ios::cur);
                current_readBlk = new char[current_sizeBlk];
                gdsFile.read(current_readBlk, current_sizeBlk);

                if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                               current_integer, current_B8Real,
                               current_words)) {
                  cout << "Error: Unable to read GDS file." << endl;
                  break;
                } else {
                  switch (current_GDSKey) {
                  case GDS_PLEX:
                    plchold_path.plex = current_integer[0];
                    break;
                  case GDS_LAYER:
                    plchold_path.layer = current_integer[0];
                    break;
                  case GDS_DATATYPE:
                    plchold_path.dataType = current_integer[0];
                    break;
                  case GDS_PATHTYPE:
                    plchold_path.pathtype = current_integer[0];
                    break;
                  case GDS_WIDTH:
                    plchold_path.width = current_integer[0];
                    break;
                  case GDS_XY:
                    // confirm xy as pairs
                    if (current_integer.size() % 2 == 0) {
                      current_int_iter = current_integer.begin();
                      odd_state = false;
                      while (current_int_iter != current_integer.end()) {
                        if (odd_state == false) {
                          // x append
                          plchold_path.xCor.push_back(*current_int_iter);
                        } else {
                          // y append
                          plchold_path.yCor.push_back(*current_int_iter);
                        }
                        odd_state = !odd_state;
                        current_int_iter++;
                      }
                    } else {
                      cout << "Error: XY co_ordinates uneven" << endl;
                    }
                    break;
                  case GDS_PROPATTR:
                    plchold_path.propattr = current_integer[0];
                    break;
                  case GDS_PROPVALUE:
                    plchold_path.propvalue = current_words;
                    break;
                  case GDS_ENDEL:

                    break;
                  default:
                    cout << "Error: Unrecognized record." << endl;
                    break;
                  }
                }
              } while (current_GDSKey != GDS_ENDEL);
              plchold_str.PATH.push_back(plchold_path);
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              break;
            case GDS_SREF:
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ secondary nest [SREF]
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              // PLEX SNAME STRANS MAG ANGLE XY PROPATTR PROPVALUE
              plchold_sref.reset();
              do {
                current_readBlk = new char[2];
                gdsFile.read(current_readBlk, 2);

                current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                                   (unsigned char)current_readBlk[1]);

                gdsFile.seekg(-2, ios::cur);
                current_readBlk = new char[current_sizeBlk];
                gdsFile.read(current_readBlk, current_sizeBlk);

                if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                               current_integer, current_B8Real,
                               current_words)) {
                  cout << "Error: Unable to read GDS file." << endl;
                  break;
                } else {
                  switch (current_GDSKey) {
                  case GDS_PLEX:
                    plchold_sref.plex = current_integer[0];
                    break;
                  case GDS_SNAME:
                    plchold_sref.name = current_words;
                    break;
                  case GDS_STRANS:
                    plchold_sref.sref_flags = current_bitarr;
                    plchold_sref.reflection = current_bitarr[15];
                    break;
                  case GDS_MAG:
                    plchold_sref.sref_flags.set(
                        13, 1); // Precaution incase other software forgot to
                                // set bit
                    plchold_sref.scale =
                        current_B8Real[0]; // TODO: Check that distill actually
                                           // kicks outb8real here.
                    break;
                  case GDS_ANGLE:
                    plchold_sref.angle = current_B8Real[0];
                    break;
                  case GDS_XY:
                    // confirm xy as pairs
                    if (current_integer.size() % 2 == 0) {
                      // x append
                      plchold_sref.xCor = current_integer[0];
                      // y append
                      plchold_sref.yCor = current_integer[1];
                    } else {
                      cout << "Error: Missing X or Y co-ordinate" << endl;
                    }
                    break;
                  case GDS_PROPATTR:
                    plchold_sref.propattr = current_integer[0];
                    break;
                  case GDS_PROPVALUE:
                    plchold_sref.propvalue = current_words;
                    break;
                  case GDS_ENDEL:

                    break;
                  default:
                    cout << "Error: Unrecognized record." << endl;
                    break;
                  }
                }
              } while (current_GDSKey != GDS_ENDEL);
              plchold_str.SREF.push_back(plchold_sref);
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              break;
            case GDS_AREF:
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ secondary nest [AREF]
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              // PLEX SNAME STRANS MAG ANGLE COLROW XY PROPATTR PROPVALUE
              plchold_aref.reset();
              do {
                current_readBlk = new char[2];
                gdsFile.read(current_readBlk, 2);

                current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                                   (unsigned char)current_readBlk[1]);

                gdsFile.seekg(-2, ios::cur);
                current_readBlk = new char[current_sizeBlk];
                gdsFile.read(current_readBlk, current_sizeBlk);

                if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                               current_integer, current_B8Real,
                               current_words)) {
                  cout << "Error: Unable to read GDS file." << endl;
                  break;
                } else {
                  switch (current_GDSKey) {
                  case GDS_PLEX:
                    plchold_aref.plex = current_integer[0];
                    break;
                  case GDS_SNAME:
                    plchold_aref.name = current_words;
                    break;
                  case GDS_STRANS:
                    plchold_aref.aref_transformation_flags = current_bitarr;
                    plchold_aref.reflection = current_bitarr[15];
                    break;
                  case GDS_MAG:
                    plchold_aref.aref_transformation_flags.set(
                        13, 1); // Precaution incase other software forgot to
                                // set bit
                    plchold_aref.scale = current_B8Real[0];
                    break;
                  case GDS_ANGLE:
                    plchold_aref.aref_transformation_flags.set(
                        14, 1); // Precaution incase other software forgot to
                                // set bit
                    plchold_aref.angle = current_B8Real[0];
                    break;
                  case GDS_COLROW:
                    plchold_aref.colCnt = current_integer[0];
                    plchold_aref.rowCnt = current_integer[1];
                    break;
                  case GDS_XY:
                    // confirm xy as pairs
                    if (current_integer.size() % 2 == 0) {
                      plchold_aref.xCor = current_integer[0];
                      plchold_aref.yCor = current_integer[1];
                      plchold_aref.xCorRow = current_integer[2];
                      plchold_aref.yCorRow = current_integer[3];
                      plchold_aref.xCorCol = current_integer[4];
                      plchold_aref.yCorCol = current_integer[5];
                    } else {
                      cout << "Error: Missing X or Y co-ordinate" << endl;
                    }
                    break;
                  case GDS_PROPATTR:
                    plchold_aref.propattr = current_integer[0];
                    break;
                  case GDS_PROPVALUE:
                    plchold_aref.propvalue = current_words;
                    break;
                  case GDS_ENDEL:

                    break;
                  default:
                    cout << "Error: Unrecognized record." << endl;
                    break;
                  }
                }
              } while (current_GDSKey != GDS_ENDEL);
              plchold_str.AREF.push_back(plchold_aref);
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              break;
            case GDS_TEXT:
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ secondary nest [TEXT]
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              // PLEX LAYER TEXTTYPE PRESENTATION PATHTYPE WIDTH STRANS MAG
              // ANGLE XY STRING PROPATTR PROPVALUE
              plchold_text.reset();
              do {
                current_readBlk = new char[2];
                gdsFile.read(current_readBlk, 2);

                current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                                   (unsigned char)current_readBlk[1]);

                gdsFile.seekg(-2, ios::cur);
                current_readBlk = new char[current_sizeBlk];
                gdsFile.read(current_readBlk, current_sizeBlk);

                if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                               current_integer, current_B8Real,
                               current_words)) {
                  cout << "Error: Unable to read GDS file." << endl;
                  break;
                } else {
                  switch (current_GDSKey) {
                  case GDS_PLEX:
                    plchold_text.plex = current_integer[0];
                    break;
                  case GDS_LAYER:
                    plchold_text.layer = current_integer[0];
                    break;
                  case GDS_TEXTTYPE:
                    plchold_text.text_type = current_integer[0];
                    break;
                  case GDS_PRESENTATION:
                    plchold_text.presentation_flags = current_bitarr;
                    break;
                  case GDS_PATHTYPE:
                    plchold_text.path_type = current_integer[0];
                    break;
                  case GDS_WIDTH:
                    plchold_text.width = current_integer[0];
                    break;
                  case GDS_STRANS:
                    plchold_text.text_transformation_flags = current_bitarr;
                    break;
                  case GDS_MAG:
                    plchold_text.text_transformation_flags.set(
                        13, 1); // Precaution incase other software forgot to
                                // set bit
                    plchold_text.scale =
                        current_B8Real[0]; // TODO: Check that distill actually
                                           // kicks outb8real here.
                    break;
                  case GDS_ANGLE:
                    plchold_text.text_transformation_flags.set(
                        14, 1); // Precaution incase other software forgot to
                                // set bit
                    plchold_text.angle = current_B8Real[0];
                    break;
                  case GDS_XY:
                    // confirm xy as pairs
                    if (current_integer.size() % 2 == 0) {
                      // x append
                      plchold_text.xCor = current_integer[0];
                      // y append
                      plchold_text.yCor = current_integer[1];
                    } else {
                      cout << "Error: Missing X or Y co-ordinate" << endl;
                    }
                    break;
                  case GDS_STRING:
                    plchold_text.textbody = current_words;
                    break;
                  case GDS_PROPATTR:
                    plchold_text.propattr = current_integer[0];
                    break;
                  case GDS_PROPVALUE:
                    plchold_text.propvalue = current_words;
                    break;
                  case GDS_ENDEL:

                    break;
                  default:
                    cout << "Error: Unrecognized record." << endl;
                    break;
                  }
                }
              } while (current_GDSKey != GDS_ENDEL);
              plchold_str.TEXT.push_back(plchold_text);
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              break;
            case GDS_NODE:
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ secondary nest [NODE]
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              // PLEX LAYER NODETYPE XY PROPATTR PROPVALUE
              plchold_node.reset();
              do {
                current_readBlk = new char[2];
                gdsFile.read(current_readBlk, 2);

                current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                                   (unsigned char)current_readBlk[1]);

                gdsFile.seekg(-2, ios::cur);
                current_readBlk = new char[current_sizeBlk];
                gdsFile.read(current_readBlk, current_sizeBlk);

                if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                               current_integer, current_B8Real,
                               current_words)) {
                  cout << "Error: Unable to read GDS file." << endl;
                  break;
                } else {
                  switch (current_GDSKey) {
                  case GDS_PLEX:
                    plchold_node.plex = current_integer[0];
                    break;
                  case GDS_LAYER:
                    plchold_node.layer = current_integer[0];
                    break;
                  case GDS_NODETYPE:
                    plchold_node.nodetype = current_integer[0];
                    break;
                  case GDS_XY:
                    // confirm xy as pairs
                    if (current_integer.size() % 2 == 0) {
                      current_int_iter = current_integer.begin();
                      odd_state = false;
                      while (current_int_iter != current_integer.end()) {
                        if (odd_state == false) {
                          // x append
                          plchold_node.xCor.push_back(*current_int_iter);
                        } else {
                          // y append
                          plchold_node.yCor.push_back(*current_int_iter);
                        }
                        odd_state = !odd_state;
                        current_int_iter++;
                      }
                    } else {
                      cout << "Error: XY co_ordinates uneven" << endl;
                    }
                    break;
                  case GDS_PROPATTR:
                    plchold_node.propattr = current_integer[0];
                    break;
                  case GDS_PROPVALUE:
                    plchold_node.propvalue = current_words;
                    break;
                  case GDS_ENDEL:

                    break;
                  default:
                    cout << "Error: Unrecognized record." << endl;
                    break;
                  }
                }
              } while (current_GDSKey != GDS_ENDEL);
              plchold_str.NODE.push_back(plchold_node);
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              break;
            case GDS_BOX:
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ secondary nest [BOX]
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              // PLEX LAYER BOXTYPE XY PROPATTR PROPVALUE
              plchold_box.reset();
              do {
                current_readBlk = new char[2];
                gdsFile.read(current_readBlk, 2);

                current_sizeBlk = (((unsigned char)current_readBlk[0] << 8) |
                                   (unsigned char)current_readBlk[1]);

                gdsFile.seekg(-2, ios::cur);
                current_readBlk = new char[current_sizeBlk];
                gdsFile.read(current_readBlk, current_sizeBlk);

                if (GDSdistill(current_readBlk, current_GDSKey, current_bitarr,
                               current_integer, current_B8Real,
                               current_words)) {
                  cout << "Error: Unable to read GDS file." << endl;
                  break;
                } else {
                  switch (current_GDSKey) {
                  case GDS_PLEX:
                    plchold_box.plex = current_integer[0];
                    break;
                  case GDS_LAYER:
                    plchold_box.layer = current_integer[0];
                    break;
                  case GDS_BOXTYPE:
                    plchold_box.boxtype = current_integer[0];
                    break;
                  case GDS_XY:
                    // confirm xy as pairs
                    if (current_integer.size() % 2 == 0) {
                      current_int_iter = current_integer.begin();
                      odd_state = false;
                      while (current_int_iter != current_integer.end()) {
                        if (odd_state == false) {
                          // x append
                          plchold_box.xCor.push_back(*current_int_iter);
                        } else {
                          // y append
                          plchold_box.yCor.push_back(*current_int_iter);
                        }
                        odd_state = !odd_state;
                        current_int_iter++;
                      }
                    } else {
                      cout << "Error: XY co_ordinates uneven" << endl;
                    }
                    break;
                  case GDS_PROPATTR:
                    plchold_box.propattr = current_integer[0];
                    break;
                  case GDS_PROPVALUE:
                    plchold_box.propvalue = current_words;
                    break;
                  case GDS_ENDEL:

                    break;
                  default:
                    cout << "Error: Unrecognized record." << endl;
                    break;
                  }
                }
              } while (current_GDSKey != GDS_ENDEL);
              plchold_str.BOX.push_back(plchold_box);
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              break;
            case GDS_ENDSTR:

              break;
            default:
              cout << "Error: Unrecognized record." << endl;
              break;
            }
          }
        } while (current_GDSKey != GDS_ENDSTR);
        setSTR(plchold_str);
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ END FIRST NEST
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        break;
      case GDS_ENDLIB:
        cout << "Reached end of library." << endl;
        break;

      default:
        cout << "Error: Unrecognized record." << endl;
        break;
      }
    }
  } while (current_GDSKey != GDS_ENDLIB);
  delete[] current_readBlk;
  identify_heirarchy();
  cout << "GDS file successfully imported." << endl;
  return 0;
}

/**
 * [gdscpp::identify heirarchy]
 * Populates gdscpp::map<int, vector<string>> Heirarchy
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */
int gdscpp::identify_heirarchy()
{
  vector<string> emptyvec;
  Heirarchy.insert({1, emptyvec});
  auto struct_it = STR.begin();
  while (struct_it != STR.end()) {
    if ((struct_it->SREF.empty() && (struct_it->AREF.empty()))) {
      Heirarchy[1].push_back(struct_it->name);
    } else {
      if (Heirarchy.count(2)) {
        Heirarchy[2].push_back(struct_it->name);
      } else {
        Heirarchy.insert({2, emptyvec});
        Heirarchy[2].push_back(struct_it->name);
      }
    }
    struct_it++;
  }
  // At this stage all structures that use references are on level 2.
  // We need to separate those that reference secondary structures and so on
  // If there are no references there will only be one level of heirarchy.
  if (Heirarchy.count(2)) {}
  return EXIT_SUCCESS;
}

bool gdscpp::compare_name(const string &ref, int level)
{
  if (Heirarchy.count(level)) // Safeguard
  {
    auto level_searcher = Heirarchy[level].begin();
    while (level_searcher != Heirarchy[level].end()) {
      if (*level_searcher == ref) {
        /* code */
      }

      level_searcher++;
    }
  }
  return false;
}
