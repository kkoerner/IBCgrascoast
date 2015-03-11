/**\file
\brief constructor of struct SRunPara and Initialization of static Variables
*/
//---------------------------------------------------------------------------
//#pragma hdrstop

#include "RunPara.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
//---------------------------------------------------------------------------
//#pragma package(smart_init)
//Input Files
   std::string SRunPara::NamePftFile="Input\\PftTraits.experiment.txt"; // trait file for experiment species
   std::string SRunPara::NameSimFile = "Input\\SimFile.txt";  //file with simulation scenarios
   std::string SRunPara::NameEnvFile="Input\\env-con.txt"; // trait file for experiment species

SRunPara SRunPara::RunPara=SRunPara();
//-------------------------------------------------------------------
SRunPara::SRunPara():Version(version2),AboveCompMode(asympart),BelowCompMode(sym),
  mort_base(0.007),LitterDecomp(0.5),DiebackWinter(0.5),EstabRamet(1),
  GridSize(100),CellNum(100),Tmax(10),NPft(81),GrazProb(0),PropRemove(0.5),BitSize(0.5),MassUngraz(15300),
  BelGrazProb(0),BelPropRemove(0),BelGrazMode(0),BGThres(1),HetBG(false),
  CutLeave(10),NCut(0),torus(true),salt(0),//CutMass(5000),
  DistAreaYear(0),AreaEvent(0),mort_seeds(0.5),meanARes(100),meanBRes(100),
  PftFile("Input/RSpec59WP3_131114.txt"),SeedInput(0),SeedRainType(1),
  species("M"),WaterLevel(0),WLsigma(0),changeVal(0),
  Migration(0),Aampl(0),Bampl(0),//cv_res(0),
  WLseason("file"){}

/**
\note  es fehlen: CellNum,NPft
\author KK
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
      <<"\n"<<SRunPara::NamePftFile;
 return mystream.str();
}//end print
void SRunPara::setRunPara(std::string def){
  stringstream dummi; dummi<<def;
  string d;
  using namespace std;
//  dummi.clear();
//  dummi>>Version;  //forum entries suggest BB6-Bug here
                     //probably BB can't cope with enums
  dummi>>d;
  switch(atoi(d.c_str())){
  case 0: Version=version1;break;
  case 1:Version=version2;break;
  case 2:Version=version3;break;
  default:break;
  }
  dummi>>d;//AboveCompMode=atoi(d.c_str());
  switch(atoi(d.c_str())){
  case 0: AboveCompMode=sym;break;
  case 1: AboveCompMode=asympart;break;
  case 2: AboveCompMode=asymtot;break;
  default:break;
  }
  dummi>>d;//BelowCompMode=atoi(d.c_str());
  switch(atoi(d.c_str())){
  case 0: BelowCompMode=sym;break;
  case 1: BelowCompMode=asympart;break;
  case 2: BelowCompMode=asymtot;break;
  default:break;
  }
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
      string envname=((string)this->NameEnvFile);
      string envid=envname.substr(envname.find('.')+1);
      std::size_t pos=envid.find('t');
      std::size_t length = NamePftFile.size();
      std::size_t pos1 = min(length,NamePftFile.find("/"))+1;
      std::size_t pos2 = NamePftFile.find(".");
      if (pos1>=length) pos1=0;
      string name=NamePftFile.substr(pos1,pos2-pos1);
      dummi<<"_R"<<this->meanBRes<<"_"<<Migration//species
//           <<"_"<<this->WLseason
           <<"_"<<this->GrazProb
 //         <<"_"<<this->AreaEvent
 //          <<"_"<<this->DistAreaYear
           <<"_"<<this->NCut
//           <<"_WL"<<this->WaterLevel
           <<"_S"<<this->salt
           <<"_"<<name
           <<"_"<<envid.substr(0,pos-1)

 //          <<"_"<<this->changeVal
//           <<"_WLc"<<WLsigma
           ;
      cout<< dummi.str()<<endl;
      return dummi.str();
}// string for file name generation

//eof  ---------------------------------------------------------------------
