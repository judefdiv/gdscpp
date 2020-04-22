/**
 * Author:      J.F. de Villiers & H.F. Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-10-14
 * Modified:
 * license:     MIT License
 * Description: Import function for storing gds in memory
 * File:				gdsCpp.hpp
 */

// ========================= Includes =========================
#include "gdsCpp.hpp"

// ====================== Miscellanious =======================
using namespace std;

// ================= Structure Declarations ===================
struct POINT
{
  double x = 0;
  double y = 0;
};
// ================== Function Declarations ===================
int rotate_point(double cx, double cy, double angle, POINT &subject);

// ====================== Function Code =======================

/**
 * [gdscpp::read description]
 * @param  fileName [The file name of the GDS file that is going to be read in]
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */
int gdscpp::import(string fileName)
{
  // Variable declarations
  std::ifstream gdsFile;
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
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end secondary nest
              //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
  STR_Lookup.insert(
      {"\0", 1000000000}); // Add null character to structure map with index 1
                           // billion. Unlikely to be 1 billion structures
  delete[] current_readBlk;
  resolve_heirarchy_and_bounding_boxes();
  cout << "GDS file successfully imported." << endl;
  return 0;
}

/**
 * [gdscpp::resolve_heirarchy_and_bounding_boxes]
 * Populates vector<vector<string>> heirarchy
 * then uses heirarchy to set heirarchy property of structures.
 * Finally sets the bounding boxes of each structure.
 * @return          [0 - Exit Success; 1 - Exit Failure]
 */
int gdscpp::resolve_heirarchy_and_bounding_boxes()
{
  // ========== Part 1: Populate heirarchy ===========
  int heirarchical_levels = 0;
  vector<vector<string>> heirarchy;
  heirarchy.clear();
  vector<string> string_vector;
  vector<string> allowed_structures_list; // Allowes structure names
  heirarchy.push_back(string_vector);     // Creates first level
  vector<vector<string>>::iterator heir_iter = heirarchy.begin();
  auto struct_it = STR.begin();
  while (struct_it != STR.end()) {
    if ((struct_it->SREF.empty() && (struct_it->AREF.empty()))) {
      heir_iter = heirarchy.begin();
      heir_iter->push_back(struct_it->name);
    } else {
      heir_iter = heirarchy.begin();
      heir_iter++;
      if (heir_iter == heirarchy.end()) {
        heir_iter--;
        string_vector.push_back(struct_it->name);
        heirarchy.push_back(string_vector); // Creates second level
        string_vector.clear();
        heir_iter = heirarchy.end(); // | Point back to last line of heirarchy.
        heir_iter--;                 // |
      } else {
        heir_iter->push_back(struct_it->name);
      }
    }
    struct_it++;
  }
  // At this stage all structures that use references are on level 2.
  // We need to separate those that reference secondary structures and so on
  // If there are no references there will only be one level (no heirarchy).
  if (heirarchy.size() > 1) {
    bool nothing_added = false;
    while ((nothing_added == false) && (heir_iter != heirarchy.end())) {
      // Add the previous level to allowable names
      heir_iter--; // Go to previous level to fetch allowed names
      auto structures_iterator = heir_iter->begin();
      while (structures_iterator != heir_iter->end()) {
        allowed_structures_list.push_back(*structures_iterator);
        structures_iterator++;
      }
      // Compare every name on current level's references to list of allowable
      // names
      heir_iter++; // Return to current level to compare this level's names vs
                   // allowed
      structures_iterator = heir_iter->begin();
      int names_shifted = 0;
      while (structures_iterator !=
             heir_iter->end()) // For each name in the set
      {
        int fetch_index =
            STR_Lookup[*structures_iterator]; // Get what STR vector index
                                              // *structures_iterator references
        vector<gdsSREF> srefs = STR[fetch_index].SREF;
        vector<gdsAREF> arefs = STR[fetch_index].AREF;
        auto SREF_it = srefs.begin();
        bool failed = false;
        while (SREF_it != srefs.end()) {
          if (!check_name(SREF_it->name, allowed_structures_list)) {
            failed = true;
            break;
          }
          SREF_it++;
        }
        auto AREF_it = arefs.begin();
        while (AREF_it != arefs.end()) {
          if (!check_name(AREF_it->name, allowed_structures_list)) {
            failed = true;
            break;
          }
          AREF_it++;
        }
        // if any of the SREF or aref of target structure fail test, the
        // structure must be moved down
        if (failed == true) {
          string struct_to_move = *structures_iterator;
          structures_iterator = heir_iter->erase(structures_iterator);
          heir_iter++; // Move to level below
          if (heir_iter ==
              heirarchy.end()) // if level below doesn't exist, make it.
          {
            heir_iter--; // Reverse iterator to last element.
            string_vector.push_back(struct_to_move);
            heirarchy.push_back(string_vector); // Push back the set
            string_vector.clear(); // Clear the temporary set container
          } else {
            // string_vector.push_back(
            //     struct_to_move); // Next level already exists, simply add the
            //                      // word
            heir_iter->push_back(struct_to_move);//Adding the word [22 Apr]
          }
          heir_iter = heirarchy.end(); // |
          heir_iter--;                 // | Return to current level.
          heir_iter--;                 // |
          names_shifted++;
        } else {
          structures_iterator++;
        }
      }
      if (names_shifted == 0) {
        nothing_added = true;
      }
      heir_iter++; // Move onto next level and repeat
    }
    // Reverse order of heirarchy function
    reverse(heirarchy.begin(), heirarchy.end());
  }
  // ========== Part 2: Set heirarchy into structures ===========
  // ==========      Also calculate bounding boxes    ===========
  heir_iter = heirarchy.end();
  heir_iter--;
  while (!(heir_iter < heirarchy.begin())) {
    auto structure_iterator = heir_iter->begin();
    while (structure_iterator != heir_iter->end()) {
      int STR_index = STR_Lookup[*structure_iterator];
      STR[STR_index].heirarchical_level = heir_iter - heirarchy.begin();
      int b_box[4] = {0, 0, 0, 0};
      calculate_STR_bounding_box(STR_index, b_box);
      STR[STR_index].bounding_box[0] = b_box[0];
      STR[STR_index].bounding_box[1] = b_box[1];
      STR[STR_index].bounding_box[2] = b_box[2];
      STR[STR_index].bounding_box[3] = b_box[3];
      if (STR[STR_index].heirarchical_level > highest_heirarchical_level)
        highest_heirarchical_level = STR[STR_index].heirarchical_level;
      structure_iterator++;
    }
    heir_iter--;
  }
  return EXIT_SUCCESS;
}

/**
 * [gdscpp::check name]
 * @param  name       [Set which name will be compared against]
 * @param  ref_vector [Vector to search through]
 * @return            [true - Name allowed; false - name not allowed]
 */
bool gdscpp::check_name(string name, vector<string> ref_vector)
{
  if (find(ref_vector.begin(), ref_vector.end(), name) != ref_vector.end())
    return true;
  else
    return false;
}

/**
 * [gdscpp::calculate_STR_bounding_box - Calculates the bounding box
 *  for the structure at the specified index. Places result in destination
 * array.]
 * @param  structure_index  [Index of structure in gdscpp object]
 * @param  destination      [Vector to save results in]
 * @return                  [0 - Function completed]
 */
int gdscpp::calculate_STR_bounding_box(int structure_index, int *destination)
{
  int bound_box[4]; // xmin, ymin, xmax, ymax of structure
  bool box_initialized = false;
  // ======================= Look through boundaries =======================
  auto boundary_iterator = STR[structure_index].BOUNDARY.begin();
  while (boundary_iterator != STR[structure_index].BOUNDARY.end()) {
    int STR_bound_box[4];
    if (box_initialized == false) {
      // Initialize bounding box
      fetch_boundary_bounding_box(*boundary_iterator, bound_box);
      box_initialized = true;
    } else {
      // Compare for minimum/maximums
      fetch_boundary_bounding_box(*boundary_iterator, STR_bound_box);
      if (STR_bound_box[0] < bound_box[0])
        bound_box[0] = STR_bound_box[0];
      if (STR_bound_box[1] < bound_box[1])
        bound_box[1] = STR_bound_box[1];
      if (STR_bound_box[2] > bound_box[2])
        bound_box[2] = STR_bound_box[2];
      if (STR_bound_box[3] > bound_box[3])
        bound_box[3] = STR_bound_box[3];
    }
    boundary_iterator++;
  }
  // ========================= Look through boxes ==========================
  auto box_iter = STR[structure_index].BOX.begin();
  while (box_iter != STR[structure_index].BOX.end()) {
    int BOX_bound_box[4];
    if (box_initialized == false) {
      fetch_box_bounding_box(*box_iter, bound_box);
      box_initialized = true;
    } else {
      fetch_box_bounding_box(*box_iter, BOX_bound_box);
      if (BOX_bound_box[0] < bound_box[0])
        bound_box[0] = BOX_bound_box[0];
      if (BOX_bound_box[1] < bound_box[1])
        bound_box[1] = BOX_bound_box[1];
      if (BOX_bound_box[2] > bound_box[2])
        bound_box[2] = BOX_bound_box[2];
      if (BOX_bound_box[3] > bound_box[3])
        bound_box[3] = BOX_bound_box[3];
    }
    box_iter++;
  }
  // ========================= Look through paths ==========================
  auto path_iter = STR[structure_index].PATH.begin();
  while (path_iter != STR[structure_index].PATH.end()) {
    // Neglect path type. Simplify work by assuming max half-width protrusion.
    // Worst case scenario is that the bounding box is ever so slightly
    // bigger than actually needed for a flat-cap situation.
    int offset = (int)(round(((double)path_iter->width * 0.5)));
    int local_bbox[4] = {
        (*min_element(path_iter->xCor.begin(), path_iter->xCor.end()) - offset),
        (*min_element(path_iter->yCor.begin(), path_iter->yCor.end()) - offset),
        (*max_element(path_iter->xCor.begin(), path_iter->xCor.end()) + offset),
        (*max_element(path_iter->yCor.begin(), path_iter->yCor.end()) +
         offset)};
    if (box_initialized == false) {
      bound_box[0] = local_bbox[0];
      bound_box[1] = local_bbox[1];
      bound_box[2] = local_bbox[2];
      bound_box[3] = local_bbox[3];
      box_initialized = true;
    }
    if (local_bbox[0] < bound_box[0])
      bound_box[0] = local_bbox[0]; // new minimum
    if (local_bbox[2] > bound_box[2])
      bound_box[2] = local_bbox[2]; // new maximum
    if (local_bbox[1] < bound_box[1])
      bound_box[1] = local_bbox[1];
    if (local_bbox[3] > bound_box[3])
      bound_box[3] = local_bbox[3];
    path_iter++;
  }
  // ================== Look through structure references ==================
  auto SREF_iter = STR[structure_index].SREF.begin();
  while (SREF_iter != STR[structure_index].SREF.end()) {
    // Warn user if specified structure's bounding box is not yet initialized
    int target_structure_index = STR_Lookup[SREF_iter->name];
    if (target_structure_index == 1000000000) {
      cout << "Error: reference to structure with no name." << endl;
      cout << "Terminating SREF bounding box check." << endl;
      break;
    }
    int referred_bound_box[4];
    referred_bound_box[0] = STR[target_structure_index].bounding_box[0];
    referred_bound_box[1] = STR[target_structure_index].bounding_box[1];
    referred_bound_box[2] = STR[target_structure_index].bounding_box[2];
    referred_bound_box[3] = STR[target_structure_index].bounding_box[3];

    if ((referred_bound_box[0] == 0) && (referred_bound_box[1] == 0) &&
        (referred_bound_box[2] == 0) && (referred_bound_box[3] == 0)) {
      cout << "Warning: Structure being referenced does not have an initialized"
           << endl;
      cout << "bounding box. Therefore, bounding boxes of references will be "
              "inaccurate."
           << endl;
      cout << "Calculate bounding boxes from the lowest level (unreferenced) "
              "upwards."
           << endl;
    }
    if (SREF_iter->reflection == true) // Reflect about x-axis
    {
      // Reflect the box about x-axis and swap because max becomes min
      int placeholder = referred_bound_box[1];
      referred_bound_box[1] = -1 * referred_bound_box[3];
      referred_bound_box[3] = -1 * placeholder;
    }
    double x_1 = referred_bound_box[0];
    double y_1 = referred_bound_box[1];
    double x_2 = referred_bound_box[2];
    double y_2 = referred_bound_box[3];
    if (SREF_iter->scale != 1) // Multiply by scale factor
    {
      x_1 = x_1 * SREF_iter->scale;
      x_2 = x_2 * SREF_iter->scale;
      y_1 = y_1 * SREF_iter->scale;
      y_2 = y_2 * SREF_iter->scale;
    }
    // Rotate
    int angle = SREF_iter->angle;
    if (angle > 0) {
      while (angle >= 360) {
        angle = angle - 360;
      }
      double x_1_new = x_1 * cos(angle * 3.141592653589 / 180) -
                       y_1 * sin(angle * 3.141592653589 / 180);
      double y_1_new = x_1 * sin(angle * 3.141592653589 / 180) +
                       y_1 * cos(angle * 3.141592653589 / 180);
      double x_2_new = x_2 * cos(angle * 3.141592653589 / 180) -
                       y_2 * sin(angle * 3.141592653589 / 180);
      double y_2_new = x_2 * sin(angle * 3.141592653589 / 180) +
                       y_2 * cos(angle * 3.141592653589 / 180);
      x_1 = x_1_new;
      y_1 = y_1_new;
      x_2 = x_2_new;
      y_2 = y_2_new;
      // convert back to vertical box by getting the new minimum and maximum
      if (x_1 > x_2) {
        double temp_holder = x_2;
        x_2 = x_1;
        x_1 = temp_holder;
      }
      if (y_1 > y_2) {
        double temp_holder2 = y_2;
        y_2 = y_1;
        y_1 = temp_holder2;
      }
    }
    // Offset shape according to translation
    referred_bound_box[0] = (int)(round(x_1)) + SREF_iter->xCor;
    referred_bound_box[1] = (int)(round(y_1)) + SREF_iter->yCor;
    referred_bound_box[2] = (int)(round(x_2)) + SREF_iter->xCor;
    referred_bound_box[3] = (int)(round(y_2)) + SREF_iter->yCor;

    if (box_initialized == false) {
      bound_box[0] = referred_bound_box[0];
      bound_box[1] = referred_bound_box[1];
      bound_box[2] = referred_bound_box[2];
      bound_box[3] = referred_bound_box[3];
      box_initialized = true;
    }
    // See if min, max x,y becomes the structures min, max x,y
    if (referred_bound_box[0] < bound_box[0])
      bound_box[0] = referred_bound_box[0]; // new minimum
    if (referred_bound_box[2] > bound_box[2])
      bound_box[2] = referred_bound_box[2]; // new maximum
    if (referred_bound_box[1] < bound_box[1])
      bound_box[1] = referred_bound_box[1];
    if (referred_bound_box[3] > bound_box[3])
      bound_box[3] = referred_bound_box[3];

    SREF_iter++;
  }
  // ==================== Look through array references ====================
  auto AREF_iter = STR[structure_index].AREF.begin();
  while (AREF_iter != STR[structure_index].AREF.end()) {
    // Warn user if specified structure's bounding box is not yet initialized
    int target_structure_index = STR_Lookup[AREF_iter->name];
    if (target_structure_index == 1000000000) {
      cout << "Error: reference to structure with no name." << endl;
      cout << "Terminating AREF bounding box check." << endl;
      break;
    }
    // fetch bounding box of the array reference structure
    int a_referred_bound_box[4] = {STR[target_structure_index].bounding_box[0],
                                   STR[target_structure_index].bounding_box[1],
                                   STR[target_structure_index].bounding_box[2],
                                   STR[target_structure_index].bounding_box[3]};
    if ((a_referred_bound_box[0] == 0) && (a_referred_bound_box[1] == 0) &&
        (a_referred_bound_box[2] == 0) && (a_referred_bound_box[3] == 0)) {
      cout << "Warning: Structure being referenced for AREF does not have an "
              "initialized"
           << endl;
      cout << "bounding box. Therefore, bounding boxes of references will be "
              "inaccurate."
           << endl;
      cout << "Calculate bounding boxes from the lowest level (unreferenced) "
              "upwards."
           << endl;
    }
    // Rotate array reference back to original dimensions
    POINT cor, corRow, corCol, true_max;
    POINT enclosed_array[4];
    int aref_angle = AREF_iter->angle;
    cor.x = (double)AREF_iter->xCor;
    cor.y = (double)AREF_iter->yCor;
    corRow.x = (double)AREF_iter->xCorRow;
    corRow.y = (double)AREF_iter->yCorRow;
    corCol.x = (double)AREF_iter->xCorCol;
    corCol.y = (double)AREF_iter->yCorCol;
    rotate_point(cor.x, cor.y, (-aref_angle), corRow);
    rotate_point(cor.x, cor.y, (-aref_angle), corCol);
    // Determine the true endpoints (without blank space at end)
    int referred_structure_width =
        a_referred_bound_box[2] - a_referred_bound_box[0];
    int referred_structure_height =
        a_referred_bound_box[3] - a_referred_bound_box[1];
    double column_spacing = (corRow.x - cor.x) / AREF_iter->colCnt;
    double row_spacing = (corCol.y - cor.y) / AREF_iter->rowCnt;
    true_max.x = corRow.x - column_spacing + referred_structure_width;
    true_max.y = corCol.y - row_spacing + referred_structure_height;
    enclosed_array[0].x = cor.x;
    enclosed_array[0].y = cor.y;
    enclosed_array[1].x = true_max.x;
    enclosed_array[1].y = cor.y;
    enclosed_array[2].x = true_max.x;
    enclosed_array[2].y = true_max.y;
    enclosed_array[3].x = cor.x;
    enclosed_array[3].y = true_max.y;
    // Rotate the 4 points of the box to their correct positions
    rotate_point(cor.x, cor.y, aref_angle, enclosed_array[0]);
    rotate_point(cor.x, cor.y, aref_angle, enclosed_array[1]);
    rotate_point(cor.x, cor.y, aref_angle, enclosed_array[2]);
    rotate_point(cor.x, cor.y, aref_angle, enclosed_array[3]);
    // Determine min/maxes for new box
    int x_min, y_min, x_max, y_max;
    x_min = (int)enclosed_array[0].x;
    if (enclosed_array[1].x < (double)x_min)
      x_min = (int)enclosed_array[1].x;
    if (enclosed_array[2].x < (double)x_min)
      x_min = (int)enclosed_array[2].x;
    if (enclosed_array[3].x < (double)x_min)
      x_min = (int)enclosed_array[3].x;

    x_max = (int)enclosed_array[0].x;
    if (enclosed_array[1].x > (double)x_max)
      x_max = (int)enclosed_array[1].x;
    if (enclosed_array[2].x > (double)x_max)
      x_max = (int)enclosed_array[2].x;
    if (enclosed_array[3].x > (double)x_max)
      x_min = (int)enclosed_array[3].x;

    y_min = (int)enclosed_array[0].y;
    if (enclosed_array[1].y < (double)y_min)
      y_min = (int)enclosed_array[1].y;
    if (enclosed_array[2].y < (double)y_min)
      y_min = (int)enclosed_array[2].y;
    if (enclosed_array[3].y < (double)y_min)
      y_min = (int)enclosed_array[3].y;

    y_max = (int)enclosed_array[0].y;
    if (enclosed_array[1].y > (double)y_max)
      y_max = (int)enclosed_array[1].y;
    if (enclosed_array[2].y > (double)y_max)
      y_max = (int)enclosed_array[2].y;
    if (enclosed_array[3].y > (double)y_max)
      y_max = (int)enclosed_array[3].y;

    // Do the comparison against the structure bounding box
    if (box_initialized == false) {
      bound_box[0] = x_min;
      bound_box[1] = y_min;
      bound_box[2] = x_max;
      bound_box[3] = y_max;
      box_initialized = true;
    }
    // See if min, max x,y becomes the structures min, max x,y
    if (x_min < bound_box[0])
      bound_box[0] = x_min; // new minimum
    if (x_max > bound_box[2])
      bound_box[2] = x_max; // new maximum
    if (y_min < bound_box[1])
      bound_box[1] = y_min;
    if (y_max > bound_box[3])
      bound_box[3] = y_max;
    AREF_iter++;
  }
  // =============== Send the bounding box to specified array ==============
  destination[0] = bound_box[0];
  destination[1] = bound_box[1];
  destination[2] = bound_box[2];
  destination[3] = bound_box[3];
  return EXIT_SUCCESS;
}

/**
 * [gdscpp::fetch_boundary_bounding_box - Returns the bounding box
 *  of a BOUNDARY into the specified destination.]
 * @param  target_boundary  [Boundary to fetch bounding box for]
 * @param  destination      [Array to save results in]
 * @return                  [0 - Function completed]
 */
int gdscpp::fetch_boundary_bounding_box(gdsBOUNDARY target_boundary,
                                        int *destination)
{
  // Must start as a point in the geometry
  int bounding_box[4] = {target_boundary.xCor[0], target_boundary.yCor[0],
                         target_boundary.xCor[0], target_boundary.yCor[0]};
  auto x_iter = target_boundary.xCor.begin();
  auto y_iter = target_boundary.yCor.begin();
  while (x_iter != target_boundary.xCor.end()) {
    if (*x_iter < bounding_box[0])
      bounding_box[0] = *x_iter; // new minimum
    if (*x_iter > bounding_box[2])
      bounding_box[2] = *x_iter; // new maximum
    if (*y_iter < bounding_box[1])
      bounding_box[1] = *y_iter;
    if (*y_iter > bounding_box[3])
      bounding_box[3] = *y_iter;
    x_iter++;
    y_iter++;
  }
  destination[0] = bounding_box[0];
  destination[1] = bounding_box[1];
  destination[2] = bounding_box[2];
  destination[3] = bounding_box[3];
  return EXIT_SUCCESS;
}

/**
 * [gdscpp::fetch_box_bounding_box - Returns the bounding box
 *  of a BOX into the specified destination.]
 * @param  target_box       [Box to fetch bounding box for]
 * @param  destination      [Array to save results in]
 * @return                  [0 - Function completed]
 */
int gdscpp::fetch_box_bounding_box(gdsBOX target_box, int *destination)
{
  // Must start as a point in the geometry
  int bounding_box[4] = {target_box.xCor[0], target_box.yCor[0],
                         target_box.xCor[0], target_box.yCor[0]};
  auto x_iter = target_box.xCor.begin();
  auto y_iter = target_box.yCor.begin();
  while (x_iter != target_box.xCor.end()) {
    if (*x_iter < bounding_box[0])
      bounding_box[0] = *x_iter; // new minimum
    if (*x_iter > bounding_box[2])
      bounding_box[2] = *x_iter; // new maximum
    if (*y_iter < bounding_box[1])
      bounding_box[1] = *y_iter;
    if (*y_iter > bounding_box[3])
      bounding_box[3] = *y_iter;
    x_iter++;
    y_iter++;
  }
  destination[0] = bounding_box[0];
  destination[1] = bounding_box[1];
  destination[2] = bounding_box[2];
  destination[3] = bounding_box[3];
  return EXIT_SUCCESS;
}

/**
 * [Rotate Point]
 * Rotates a specified POINT's co-ordinates by reference.
 * @param  cx     [ The x co-ordinate of the point to rotate around ]
 * @param  cy     [ The y co-ordinate of the point to rotate around ]
 * @param angle   [ The angle (in degrees) which the point must be rotated by
 * (anticlockwise from x-axis)]
 * @param subject [ The point which must be rotated]
 * @return        [0 - Exit Success]
 */
int rotate_point(double cx, double cy, double angle, POINT &subject)
{
  POINT p = subject;
  angle = angle * 3.141592653589 / 180; // convert angle to radians
  double s = sin(angle);
  double c = cos(angle);

  // translate point back to origin:
  p.x -= cx;
  p.y -= cy;

  // rotate point
  double xnew = p.x * c - p.y * s;
  double ynew = p.x * s + p.y * c;

  // translate point back:
  p.x = round(xnew + cx);
  p.y = round(ynew + cy);
  subject = p;         // Overwrite the original point with the rotated one.
  return EXIT_SUCCESS; // Indicate successful conversion.
}