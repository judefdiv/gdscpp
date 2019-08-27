/**
 * Author:  		Heinrich Herbst
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:				Supertools, Coldflux Project - IARPA
 * Created: 		2019-08-26
 * Modified:
 * license: 
 * Description: .   This file holds the functions which populate the gds class
 * File:			gdsImport.cpp
 */

// ############################ DETAILED DESCRIPTION ################################
// To understand format better: http://boolean.klaasholwerda.nl/interface/bnf/gdsformat.html
// =============================== GENERAL RECORDS ==================================
/*                                      <Record>     < Done? >
*                                       [HEADER]        [ ]
*                                       [BGNLIB]        [ ]
*                                       [LIBNAME]       [ ]
*                                       [REFLIBS]       [ ](Don't bother here unless needed.)
*                                       [FONTS]         [ ](Don't bother here unless needed.)
*                                       [ATTRTABLE]     [ ](Don't bother here unless needed.)
*                                       [GENERATIONS]   [ ]
*                                       [<FormatType>]  [ ](Don't bother here unless needed.)
*                                       [UNITS]         [ ]
*                                       [BGNSTR]        [ ]
*                                       [STRNAME]       [ ]
*                                       [ENDLIB]        [ ]
*/
// =============================== ELEMENT RECORDS ==================================
// --------------------------------- 1: BOUNDARY        [ ]--------------------------
// ----------------------------------- 2: PATH          [ ]--------------------------
// --------------------------- 3: STRUCTURE REFERENCE   [ ]--------------------------
// ----------------------------- 4: ARRAY REFERENCE     [ ]--------------------------
//              PLEX
//              SNAME
//              STRANS Transformation
//              COLROW
//              XY
// ----------------------------------- 5: TEXT          [ ]--------------------------
//              PLEX
//              LAYER
//              TEXTTYPE
//              PRESENTATION
//              PATHTYPE
//              WIDTH
//              STRANS
//              MAG
//              ANGLE
//              XY
//              STRING
//              PROPATTR
//              PROPVALUE
// ----------------------------------- 6: NODE          [ ]--------------------------
//              PLEX
//              LAYER
//              NODETYPE
//              XY
// ----------------------------------- 7: BOX           [ ]--------------------------
//              PLEX
//              LAYER
//              BOXTYPE
//              XY

#include "gdsClass.hpp"

//Load GDS
