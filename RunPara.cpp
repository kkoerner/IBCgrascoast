/**\file
\brief constructor of struct SRunPara and Initialization of static Variables
*/
//---------------------------------------------------------------------------
#pragma hdrstop

#include "RunPara.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

SRunPara SRunPara::RunPara=SRunPara();
//-------------------------------------------------------------------
SRunPara::SRunPara():Version(version1),AboveCompMode(sym),BelowCompMode(sym),
  GridSize(128),CellNum(128),Tmax(10),NPft(81),GrazProb(0),PropRemove(0.5),
  BelGrazProb(0),BelPropRemove(0),BelGrazMode(0),
  CutMass(5000),NCut(0),
  DistAreaYear(0),AreaEvent(0.1),mort_seeds(0.5),meanARes(100),meanBRes(100),
  PftFile("Input/PftTraits2304.txt"){}
//eof  ---------------------------------------------------------------------
