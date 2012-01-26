/**\file
\brief constructor of struct SRunPara and Initialization of static Variables
*/
//---------------------------------------------------------------------------
#pragma hdrstop

#include "RunPara.h"
#include <iostream>
#include <sstream>
//---------------------------------------------------------------------------
#pragma package(smart_init)

SRunPara SRunPara::RunPara=SRunPara();
//-------------------------------------------------------------------
SRunPara::SRunPara():Version(version2),AboveCompMode(asympart),BelowCompMode(sym),
  GridSize(128),CellNum(128),Tmax(10),NPft(81),GrazProb(0),PropRemove(0.5),
  BelGrazProb(0),BelPropRemove(0),BelGrazMode(0),BGThres(1),HetBG(false),
  CutLeave(10),NCut(0),torus(true),//CutMass(5000),
  DistAreaYear(0),AreaEvent(0.1),mort_seeds(0.5),meanARes(100),meanBRes(100),
  PftFile("Input/PftTraits2304.txt"),
  species("M"),WaterLevel(0),WLsigma(0),WLseason("const"),changeVal(0),
  Migration(true){}
void SRunPara::print(){
  std::cout<<"\n  Parameter setting:\n";
  std::cout
//      <<"Version: "<<Version<<"\tAcomp:"<<AboveCompMode
//      <<"\tBcomp:"<<BelowCompMode
      <<"\nGridSize:"<<GridSize<<"\tTmax:"<<Tmax<<"Torus:"<<torus
      <<"\nAGraz:"<<GrazProb<<"\tRemoval:"<<PropRemove
//      <<"\nBGraz:"<<BelGrazProb<<"\tRemoval:"<<BelPropRemove
//      <<"\tMode:"<<BelGrazMode<<"\tThresh:"<<BGThres<<"\tHetGraz:"<<HetBG
      <<"\nNCut:"<<NCut<<"\tCutLeave:"<<CutLeave//<<"\tCutMass:"<<CutMass
      <<"\nARes:"<<meanARes<<"\tBres:"<<meanBRes
      <<"\nTramplingArea:"<<DistAreaYear<<"\tTrampl_Prob:"<<AreaEvent
//      <<"\nMort_Seeds:"<<mort_seeds
      <<"\nSpecInit: "<<species<<"\tWLmean: "<<WaterLevel
      <<"\tWL mode: "<<WLseason<<"\tWL change"<<WLsigma
      <<"\n change Param by "<<changeVal<<"\n Migration: "<<Migration
      <<"\nPFTFile:"<<PftFile<<std::endl;
}//end print
///
/// currently for Reed-Mix experiments
/// \date 2011/11/14
///
string SRunPara::getRunID(){
      stringstream dummi;
      dummi<<"_R"<<this->meanBRes<<"_"<<species
//           <<"_"<<this->WLseason
           <<"_"<<this->GrazProb
 //          <<"_"<<this->DistAreaYear
           <<"_"<<this->NCut
           <<"_WL"<<this->WaterLevel
 //          <<"_"<<this->changeVal
//           <<"_WLc"<<WLsigma
           ;
      return dummi.str();
}// string for file name generation

  //eof  ---------------------------------------------------------------------
