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
using namespace std;

SRunPara SRunPara::RunPara=SRunPara();
//-------------------------------------------------------------------
SRunPara::SRunPara():Version(version2),AboveCompMode(asympart),BelowCompMode(sym),
  mort_base(0.007),LitterDecomp(0.5),DiebackWinter(0.5),EstabRamet(1),
  GridSize(128),CellNum(128),Tmax(10),NPft(81),GrazProb(0),PropRemove(0.5),BitSize(0.5),
  BelGrazProb(0),BelPropRemove(0),BelGrazMode(0),BGThres(1),HetBG(false),
  CutLeave(10),NCut(0),torus(true),salt(0),//CutMass(5000),
  DistAreaYear(0),AreaEvent(0.1),mort_seeds(0.5),meanARes(100),meanBRes(100),
  PftFile("Input/PftTraits2304.txt"){}

/**
\note  es fehlen: CellNum,NPft
\autor KK
\date  120831
*/
std::string SRunPara::asString(){
  std::stringstream mystream;
      mystream
      <<"\n"<<Version<<"\t"<<AboveCompMode<<"\t"<<BelowCompMode
      <<"\t"<<GridSize <<"\t"<<Tmax        <<"\t"<<torus
      <<"\t"<<mort_seeds<<"\t"<<EstabRamet
      <<"\t"<<mort_base<<"\t"<<LitterDecomp<<"\t"<<DiebackWinter
      <<"\nRes: \t"<<meanARes     <<"\t"<<meanBRes
      <<"\nGrazing:\t"<<GrazProb    <<"\t"<<PropRemove <<"\t"<<BitSize
      <<"\nBGHerb:\t"<<BelGrazProb <<"\t"<<BelPropRemove
      <<"\t"<<BelGrazMode  <<"\t"<<BGThres   <<"\t"<<HetBG
      <<"\nCutting:\t"<<NCut         <<"\t"<<CutLeave  //<<"\nCutLeave:\t"<<CutLeave
      <<"\nTrampling:\t"<<DistAreaYear<<"\t"<<AreaEvent
      <<"\t"<<DistAreaYear<<"\t"<<AreaEvent
//      <<"\t"<<mort_seeds
 //     <<"\nspecies:\t"<<species   <<"\nWaterLevel:\t"<<WaterLevel
 //     <<"\nWLseason:\t"<<WLseason <<"\nWLsigma:\t"<<WLsigma
 //     <<"\nchangeVal:\t"<<changeVal<<"\n Migration:\t"<<Migration
      <<"\n"<<PftFile;
 return mystream.str();
}//end print
void SRunPara::setRunPara(std::string def){
  stringstream dummi; dummi<<def;
  string d;
//  dummi.clear();
//  dummi>>Version;  //forum entries suggest BB6-Bug here
                     //probably BB can't cope with enums
  dummi>>d;Version=atoi(d.c_str());
  dummi>>d;AboveCompMode=atoi(d.c_str());
  dummi>>d;BelowCompMode=atoi(d.c_str());
  dummi>> GridSize; CellNum=GridSize;
  dummi>> Tmax>> torus;
  dummi>>mort_seeds>> EstabRamet>>mort_base>>LitterDecomp>>DiebackWinter;
  dummi>> GrazProb>> PropRemove>>BitSize>> BelGrazProb >>BelPropRemove>> BelGrazMode
       >> BGThres>> HetBG>> NCut>> CutLeave>> meanARes>> meanBRes>>DistAreaYear
       >> AreaEvent  >>PftFile;
}
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
      <<"\n Salinity: "<<salt
      <<"\nPFTFile:"<<PftFile<<std::endl;
}//end print
///
/// currently for Reed-Mix experiments
/// \date 2011/11/14
///
string SRunPara::getRunID(){
      stringstream dummi;
      dummi<<"_R"<<this->meanBRes<<"_"<<Migration//species
//           <<"_"<<this->WLseason
           <<"_"<<this->GrazProb
 //          <<"_"<<this->DistAreaYear
           <<"_"<<this->NCut
           <<"_WL"<<this->WaterLevel
           <<"_S"<<this->salt
 //          <<"_"<<this->changeVal
//           <<"_WLc"<<WLsigma
           ;
      return dummi.str();
}// string for file name generation

//eof  ---------------------------------------------------------------------
