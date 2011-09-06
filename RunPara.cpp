/**\file
\brief constructor of struct SRunPara and Initialization of static Variables
*/
//---------------------------------------------------------------------------
#pragma hdrstop

#include "RunPara.h"
#include <iostream>
//---------------------------------------------------------------------------
#pragma package(smart_init)

SRunPara SRunPara::RunPara=SRunPara();
//-------------------------------------------------------------------
SRunPara::SRunPara():Version(version2),AboveCompMode(asympart),BelowCompMode(sym),
  GridSize(128),CellNum(128),Tmax(10),NPft(81),GrazProb(0),PropRemove(0.5),
  BelGrazProb(0),BelPropRemove(0),BelGrazMode(0),BGThres(1),HetBG(false),
  CutMass(5000),NCut(0),torus(true),
  DistAreaYear(0),AreaEvent(0.1),mort_seeds(0.5),meanARes(100),meanBRes(100),
  PftFile("Input/PftTraits2304.txt"){}
void SRunPara::print(){
  std::cout<<"\n  Parameter setting:\n";
  std::cout<<"Version: "<<Version<<"\tAcomp:"<<AboveCompMode
      <<"\tBcomp:"<<BelowCompMode
      <<"\nGridSize:"<<GridSize<<"\tTmax:"<<Tmax<<"Torus:"<<torus
      <<"\nAGraz:"<<GrazProb<<"\tRemoval:"<<PropRemove
      <<"\nBGraz:"<<BelGrazProb<<"\tRemoval:"<<BelPropRemove
      <<"\tMode:"<<BelGrazMode<<"\tThresh:"<<BGThres<<"\tHetGraz:"<<HetBG
      <<"\nNCut:"<<NCut<<"\tCutMass:"<<CutMass
      <<"\nARes:"<<meanARes<<"\tBres:"<<meanBRes
      <<"\nTramplingArea:"<<DistAreaYear<<"\tTrampl_Prob:"<<AreaEvent
      <<"\nMort_Seeds:"<<mort_seeds<<"\tPFTFile:"<<PftFile
      <<std::endl;
}//end print
  //eof  ---------------------------------------------------------------------
