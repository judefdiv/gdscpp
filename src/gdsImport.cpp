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
// Element records form part of a structure
// --------------------------------- 1: BOUNDARY        [ ]--------------------------
//              PLEX       //Ignore for now - seems to only be used with nodes
//              LAYER
//              DATATYPE
//              XY
// ----------------------------------- 2: PATH          [ ]--------------------------
//              PLEX       //Ignore for now - seems to only be used with nodes
//              LAYER
//              DATATYPE
//              PATHTYPE
//              WIDTH
//              XY
// --------------------------- 3: STRUCTURE REFERENCE   [ ]--------------------------
//              SNAME
//              STRANS Transformation )
//              MAG                   )
//              ANGLE                 )
//              XY
// ----------------------------- 4: ARRAY REFERENCE     [ ]--------------------------
//              PLEX       //Ignore for now - seems to only be used with nodes
//              SNAME
//              STRANS Transformation )
//              MAG                   )
//              ANGLE                 )
//              COLROW
//              XY
// ----------------------------------- 5: TEXT          [ ]--------------------------
//              PLEX       //Ignore for now - seems to only be used with nodes
//              LAYER
//              TEXTTYPE
//              PRESENTATION
//              PATHTYPE
//              WIDTH
//              STRANS Transformation )
//              MAG                   )
//              ANGLE                 )
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
//              PLEX       //Ignore for now - seems to only be used with nodes
//              LAYER
//              BOXTYPE
//              XY

#include "gdsClass.hpp"

//Load GDS
