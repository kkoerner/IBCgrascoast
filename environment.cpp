/**\file
   \brief functions and static defaults of environmental classes

   Version for Ines' invasion experiments: adapt GetSim, Run and Init functions
   of class CClonalGridEnvir (further: CClonalGridEnvir::exitConditions()).
   \date 03/2010

   Version for Linas exploratoria experiments - no invasion; realistic PFTs
   \date 07/2010
*/
/** \page changes Changes for the COMTESS-version
  \section secWatStr Water stress option
  \par Extend class CCell with WaterLevel.
  A makro swiches in CGrid between an array of CCell and CWaterCell.
  \par Write derived class CWaterGridEnvir.
  This class is derrived from CClonalGridEnvir. At the moment it has a new
  variable WaterFlow, that has information on the weekly frequency/ intensity
  of water flow (enum CWaterGridEnvir::ETide).
  It has an own init-function and get/set functions for mean water level
  on grid.
  GridCells of the basic lattice have to be of type CWaterCell.
  \sa CWaterGridEnvir, CWaterCell
  \par Implement WaterLevel-related rules
   -# aboveground plant parts don't assimilate if they are submersed
   (but plant type may be adapted to assim below WL)
   -# root parts of the anoxic soil don't uptake resources
      if plant is not adapted; else the entire root gains a constant
      proportion of the resources available.

   \sa CWaterPlant::DistrRes_help and  CWaterPlant::comp_coef

  \section secWintMort Additional Winter Mortality
  Additional winter mortality due to prolonged inundation or disturbance events
  as dike breaking are calculated calling the Plant's function
  CWaterPlant::winterDisturbance in CWaterGridEnvir::Winter.
  A prolonged winter inundation let plants die by oxygen deficit. Types that are
  adapted to high water levels in summer, here also perform better.

  \verbatim
  mort=min(0.95, (MonthsInundation-Thresh_by_adapt )/4)
  Thresh_by_adapt(no,yes)=(2,8)
  \endverbatim

  \section secSalt Salt stress option
  Salt stress reduces belowground resource uptake to zero.
  Adaptation comes with costs that increase with tolerated salinity level
  (S0,1: 30% for <6g/l  and S2-5:60% for <10g/l; to date salinity levels above are not
  considered).

  \image html salinity_rules.jpg "salinity rules"

  \sa SWaterTraits::saltTol salt value after Ellenberg

   */
//---------------------------------------------------------------------------

#pragma package(smart_init)

#pragma hdrstop

#include "environment.h"
#include <iostream>

using namespace std;
//-CEnvir: Init static variables--------------------------------------------------------------------------
   int CEnvir::week=0;
   int CEnvir::year=1;
   int CEnvir::WeeksPerYear=30;
//   int CEnvir::NumPft=81;

   //Input Files
   char* CEnvir::NamePftFile="Input\\PftTraits2304.txt";
   string CEnvir::NameBResFile="";
   string CEnvir::NameSimFile = "Input\\SimFileTest1.txt";  //file with simulation scenarios

   //Output Files
   string CEnvir::NamePftOutFile= "Output\\PftOut.txt";
   string CEnvir::NameGridOutFile= "Output\\GridOut.txt";
//   char* CEnvir::NameSurvOutFile= "Output\\SurvOutGraz.txt";
   string CEnvir::NameSurvOutFile= "Output\\SurvOutGraz.txt";
   string CEnvir::NameLogFile="Output\\LogUpSc.log";

   int CEnvir::NRep=1;        //!> number of replications -> read from SimFile;
   int CEnvir::SimNr=0;
   int CEnvir::RunNr=0;
   KW_RNG::RNG CEnvir::RandNumGen;   //!< random number generator object pointer
   vector<double> CEnvir::AResMuster;
   vector<double> CEnvir::BResMuster;
   map<string,long> CEnvir::PftInitList;  //!< list of Pfts used

//---------------------------------------------------------------------------
CEnvir::CEnvir()://NdeadPlants(0),NPlants(0),
  CoveredCells(0),//MeanShannon(0),
  //NCellsAcover(0),//Mortalitaetsrate(0),
  init(1),endofrun(false)
{
   ReadLandscape();
   ACover.assign(SRunPara::RunPara.GetSumCells(),0);
   BCover.assign(SRunPara::RunPara.GetSumCells(),0);
}
//------------------------------------------------------------------------------
CEnvir::~CEnvir(){
  for (unsigned int i=0;i<GridOutData.size();i++) delete GridOutData[i];
  for (unsigned int i=0;i<PftOutData.size();i++)  delete PftOutData[i];
}
//------------------------------------------------------------------------------
/**
  Function defined global muster resources, set to gridcells at beginning
  of each Year. At the moment only evenly dirtributed single values for above-
  and belowground ressources are implemented.
  Later the function can read source files of values <100\% autocorrelated or
  generate some noise around fixed values etc..
*/
void CEnvir::ReadLandscape(){
  //100% autocorrelated values
  using SRunPara::RunPara;
  AResMuster.clear();BResMuster.clear();
  AResMuster.assign(RunPara.GetSumCells(),RunPara.meanARes);
  BResMuster.assign(RunPara.GetSumCells(),RunPara.meanBRes);
}//end ReadLandscape

//------------------------------------------------------------------------------
/**
  \param pos file position to start reading
  \param file name of input file (obsolete, NameSimFile is used)
  \return file position of next simulation set
*/
int CClonalGridEnvir::GetSim(const int pos,string file){
  //Open SimFile,
  ifstream SimFile(NameSimFile.c_str());
  if (!SimFile.good()) {cerr<<("Fehler beim Öffnen SimFile");exit(3); }
  cout<<"SimFile: "<<NameSimFile<<endl;
  int lpos=pos;
  if (pos==0){  //read header
    string line,file_id;
    getline(SimFile,line);
    int SimNrMax;//dummi
    SimFile>>SimNrMax>>NRep>>file_id;
    //set file names
    NamePftOutFile= (string)"Output\\Pft-"+file_id+".txt";
    NameGridOutFile=(string)"Output\\Grd-"+file_id+".txt";
    NameSurvOutFile=(string)"Output\\Srv-"+file_id+".txt";
    NameLogFile=    (string)"Output\\Log-"+file_id+".log";

    getline(SimFile,line);
    getline(SimFile,line);
    lpos=SimFile.tellg();
  }
  SimFile.seekg(lpos);
  //ist position gültig?, wenn nicht -- Abbruch
  if (!SimFile.good()) return -1;

  int version, acomp, bcomp;
   using SRunPara::RunPara;
   //RunNr=0; int dummi;
   //for (int i=0; i<SimNrMax; ++i)

      SimFile>>SimNr
       //      >>dummi     //RunPara.Layer
       //      >>version   //>>RunPara.Version - enum types cannot be read with >>
       //      >>acomp     //>>RunPara.AboveCompMode
       //      >>bcomp    //>>RunPara.BelowCompMode
       //      >>RunPara.BelGrazMode   //mode of belowground grazing
        //     >>RunPara.GridSize
        //     >>RunPara.CellNum
             >>RunPara.Tmax
        //     >>RunPara.NPft
             >>RunPara.GrazProb
         //    >>RunPara.PropRemove
         //    >>RunPara.BelGrazProb     //for belowground simulations
         //    >>RunPara.BelPropRemove   //for belowground simulations
             >>RunPara.NCut
             >>RunPara.CutLeave//Mass
        //     >>RunPara.DistAreaYear
        //     >>RunPara.AreaEvent
        //     >>RunPara.mort_seeds
             >>RunPara.meanARes
             >>RunPara.meanBRes
     //        >>RunPara.PftFile
         //    >>RunPara.BGThres
             ;

      //---------standard parameter:
      //grazing intensity
      RunPara.PropRemove=0.5;
      //no trampling
      RunPara.DistAreaYear=0;
      //above- and belowground competition
      acomp=1;bcomp=0;
      //--------------------------------
      // set version and competition  modes - in this way because of enum types!
//      RunPara.Version=version;
//      RunPara.AboveCompMode=acomp;
//      RunPara.BelowCompMode=bcomp;
//      //Größenänderung des Grids auf 3qm
//      RunPara.GridSize=RunPara.CellNum=173;
  RunPara.print();
  return SimFile.tellg();
}//end  CEnvir::GetSim
//------------------------------------------------------------------------------
void CEnvir::clearResults(){
   InitRun();
   ACover.assign(SRunPara::RunPara.GetSumCells(),0);
   BCover.assign(SRunPara::RunPara.GetSumCells(),0);
   //MeanShannon=0;
   //Mortalitaetsrate=0;
   //NCellsAcover=0;
   //NdeadPlants=0; NPlants=0;
}
//------------------------------------------------------------------------------
void CEnvir::InitRun(){
   for (unsigned int i=0;i<GridOutData.size();i++) delete GridOutData[i];
   for (unsigned int i=0;i<PftOutData.size();i++)  delete PftOutData[i];
   PftInitList.clear();
   PftSurvTime.clear();
//   PftWeek=NULL;GridWeek=NULL;
   PftOutData.clear();
   GridOutData.clear();
   endofrun=false;
  //set resources
  ReadLandscape();
}
//------------------------------------------------------------------------------
void CEnvir::WriteGridComplete(bool allYears)
{
   //Open GridFile, write header
   ofstream GridOutFile(NameGridOutFile.c_str(),ios::app);
   if (!GridOutFile.good()) {cerr<<("Fehler beim Öffnen GridOutFile");exit(3); }
   GridOutFile.seekp(0, ios::end);
   long size=GridOutFile.tellp();
   if (size==0){
     GridOutFile<<"Sim\tRun\tTime\t"
              <<"totMass\tNInd\t"
              <<"abovemass\tbelowmass\t"
              <<"pcBare\t"
              <<"mean_WL\tmean_bres\t"
              <<"shannon\tmeanShannon\t"
              <<"NPFT\tmeanNPFT\tCutted"
              ;
     GridOutFile<<"\n";
   }

   vector<SGridOut>::size_type i=0;
   if (!allYears)i= GridOutData.size()-1;
   for (i; i<GridOutData.size(); ++i){
      GridOutFile<<SimNr<<'\t'<<RunNr<<'\t'<<i //<<'\t'<<GridOutData[i]->week
                 <<'\t'<<GridOutData[i]->totmass
                 <<'\t'<<GridOutData[i]->Nind
                 <<'\t'<<GridOutData[i]->above_mass
                 <<'\t'<<GridOutData[i]->below_mass
                 <<'\t'<<GridOutData[i]->bareGround
//                 <<'\t'<<GridOutData[i]->aresmean
                 <<'\t'<<GridOutData[i]->WaterLevel
                 <<'\t'<<GridOutData[i]->bresmean
                 <<'\t'<<GridOutData[i]->shannon
                 <<'\t'<<GetMeanShannon(10)//25 nach 100J
                 <<'\t'<<GridOutData[i]->PftCount
                 <<'\t'<<GetMeanNPFT(10)
                 <<'\t'<<GridOutData[i]->cutted
                 <<"\n";
   }
   GridOutFile.close();
}//WriteGridComplete
//------------------------------------------------------------------------------
/**
   Saves Pft survival times and returns number of surviving PFTs

   changed in version 100716 for type-flexible Output
   \return number of surviving PFTs
*/
int CClonalGridEnvir::PftSurvival()
{
    typedef map<string,long> mapType;
    for(mapType::const_iterator it = PftInitList.begin();
          it != PftInitList.end(); ++it)
    {
       if (PftOutData.back()->PFT.find(it->first)
       ==PftOutData.back()->PFT.end()){  //PFT is extinct
       //vergleiche mit letztem Stand
          if(PftSurvTime.find(it->first)->second==0)//wenn vorher noch existent
            PftSurvTime[it->first]=CEnvir::year;//GetT(); //get current time
       }else{                            //PFT still exists
          PftSurvTime[it->first]=0;
       } // [it->first];
   }
   return PftOutData.back()->PFT.size();//count_pft;
}//endPftSurvival
//------------------------------------------------------------------------------
/**
  changed in version 100715 for type-flexible Output
*/
void CEnvir::WritePftComplete()
{
   //Open PftFile , write header and initial conditions
   ofstream PftOutFile(NamePftOutFile.c_str(),ios_base::app);
   if (!PftOutFile.good()) {cerr<<("Fehler beim Öffnen PftOutFile");exit(3); }
   PftOutFile.seekp(0, ios::end);
   long size=PftOutFile.tellp();
   if (size==0){
     PftOutFile<<"Sim\tRun\tTime";
     PftOutFile<<"\tInds\tseeds\tshootmass\tPFT";
     PftOutFile<<"\n";
   }

   for (vector<PftOut>::size_type i=0; i<PftOutData.size();++i){
   typedef map<string, SPftOut::SPftSingle*> mapType;

     for(mapType::const_iterator it = PftOutData[i]->PFT.begin();
          it != PftOutData[i]->PFT.end(); ++it)
     {
//        cout << "Who(key = first): " << it->first;
//        cout << " Score(value = second): " << it->second << '\n';
//    }

         PftOutFile<<SimNr<<'\t'<<RunNr<<'\t'<<i;
         PftOutFile<<'\t'<<it->second->Nind;
         PftOutFile<<'\t'<<it->second->Nseeds;
         PftOutFile<<'\t'<<it->second->shootmass;
         PftOutFile<<'\t'<<it->first; //pft-name at last
         //add cover here

         PftOutFile<<"\n";
     }
   }
   PftOutFile.close();
}//end WritePftComplete()
//------------------------------------------------------------------------------
void CEnvir::WriteSurvival(){
  WriteSurvival(RunNr,SimNr);
}
void CEnvir::WriteSurvival(string str){
  string dummi=NameSurvOutFile.substr(0,NameSurvOutFile.length()-4);
  NameSurvOutFile=dummi+str+".txt";
  WriteSurvival();
}
/**
  File-Documentation of type-specific survival statistics.
*/
void CEnvir::WriteSurvival(int runnr, int simnr)
{
   if(PftOutData.size()==0)return;  //no output if no list element exists

   ofstream SurvOutFile(NameSurvOutFile.c_str(),ios_base::app);
   if (!SurvOutFile.good()) {cerr<<("Fehler beim Öffnen SurvFile");exit(3); }
   SurvOutFile.seekp(0, ios::end);
   long size=SurvOutFile.tellp();
   if (size==0){
     SurvOutFile<<"Sim\tRun\tT\t";
     SurvOutFile<<"BM\tcPop\tcover\tPFT";
     SurvOutFile<<"\n";
   }
     SPftOut*  PftWeek=PftOutData.back();
    typedef map<string,SPftOut::SPftSingle*> mapType;
//    typedef map<string,int> mapType;
//    for(mapType::const_iterator it = PftSurvTime.begin();
//          it != PftSurvTime.end(); ++it)
    for(mapType::const_iterator it = PftWeek->PFT .begin();
          it != PftWeek->PFT.end(); ++it)
    {
     SPftOut::SPftSingle*  Pft =it->second;

     SurvOutFile<<simnr<<'\t'<<runnr<<'\t'<<year;
     SurvOutFile<<'\t'<<Pft->shootmass;//type's AGbiomass
     SurvOutFile<<'\t'<<Pft->Nind;//number ramets
     SurvOutFile<<'\t'<<Pft->cover;//cover of type

//     SurvOutFile<<'\t'<<GetMeanPopSize(it->first,10);//mean of 10years
//     SurvOutFile<<'\t'<<GetCurrPopSize(it->first);
//     SurvOutFile<<'\t'<<it->second
      SurvOutFile<<'\t'<<it->first<<endl;
    }
//     SurvOutFile<<"\n";
}//end writeSurvival
//----------------------------------
void CEnvir::AddLogEntry(string text,string filename)
{
   ofstream LogFile(filename.c_str(),ios_base::app);
   if (!LogFile.good()) {cerr<<("Fehler beim Öffnen LogFile");exit(3);}

   LogFile<<text;
}
//----------------------------------
void CEnvir::AddLogEntry(float nb,string filename)
{
   ofstream LogFile(filename.c_str(),ios_base::app);
   if (!LogFile.good()) {cerr<<("Fehler beim Öffnen LogFile");exit(3);}

   LogFile<<" "<<nb;
}

//---------------------------------------------------------------------------
double CEnvir::GetMeanShannon(int years)
{
   double sum=0, count=0;

   //int start=(GridOutData.size()-1)-years*32;
   int start=(GridOutData.size()-1)-years;

   for (vector<SGridOut>::size_type i=start+1; i<GridOutData.size(); ++i){
      sum+=GridOutData[i]->shannon;
      count++;
   }
   return sum/count;
}
//---------------------------------------------------------------------------
double CEnvir::GetMeanNPFT(int years)
{
   double sum=0, count=0;

   //int start=(GridOutData.size()-1)-years*32;
   int start=(GridOutData.size()-1)-years;

   for (vector<SGridOut>::size_type i=start+1; i<GridOutData.size(); ++i){
      sum+=GridOutData[i]->PftCount;
      count++;
   }
   return sum/count;
}
//---------------------------------------------------------------------------
double CEnvir::GetCurrPopSize(string pft){
   if (PftOutData.back()->PFT.find(pft)== PftOutData.back()->PFT.end())
     return 0;
   SPftOut::SPftSingle* entry
      =PftOutData.back()->PFT.find(pft)->second;
   return entry->Nind;
}
//-------------------------------------------------------
double CEnvir::GetMeanPopSize(string pft,int x){
      //only if output once a year
   if(PftOutData.size()<x) return -1;
   double sum=0;
   for (int i= (PftOutData.size()-x);i<PftOutData.size();i++)
    if (PftOutData[i]->PFT.find(pft)!=PftOutData[i]->PFT.end())
    {
     SPftOut::SPftSingle* entry
       =PftOutData[i]->PFT.find(pft)->second;
     sum+= entry->Nind;
     }
    return (sum/x);
}
//---------------------------------------------------------------------------

//-CClonalEnvir: Init static variables--------------------------------------

   /// \var char* CClonalGridEnvir::NameClonalPftFile
   /// \since 2010-07-06 slightly changed clonal traits
   ///
   char* CClonalGridEnvir::NameClonalPftFile="Input\\clonalTraits.explodat.txt";
//   char* CClonalGridEnvir::NameClonalPftFile="Input\\clonalTraits.txt";
   string CClonalGridEnvir::NameClonalOutFile="Output\\clonalOut.txt";
   int CClonalGridEnvir::clonaltype=0;
   int CClonalGridEnvir::Pfttype=40;//?default  41th PFT with average parameters

   map<string,SPftTraits*> CClonalGridEnvir::PftLinkList=
//   map<string,SPftTraits*> CSiteScaleEnvir::PftLinkList=
     map<string,SPftTraits*>();
   map<string,SclonalTraits*> CClonalGridEnvir::ClLinkList=
//   map<string,SclonalTraits*> CSiteScaleEnvir::ClLinkList=
     map<string,SclonalTraits*>();
   //   int CClonalGridEnvir::sim=0;
//------------------------------------------------------------------------------
CClonalGridEnvir::CClonalGridEnvir():CGridclonal(),CEnvir()
{
   ReadLandscape();
}
//------------------------------------------------------------------------------
/**
\todo move function to struct SPftTraits ?
*/
SPftTraits* CClonalGridEnvir::getPftLink(string type)
{
  SPftTraits* traits=NULL;
  map<string,SPftTraits*>::iterator pos = PftLinkList.find(type);
  if (pos==(PftLinkList.end())) cerr<<"type not found\n";
  else traits=pos->second;
  if (traits==NULL) cerr<<"NULL-pointer error\n";
  return traits;

}
//------------------------------------------------------------------------------
CClonalGridEnvir::~CClonalGridEnvir()
{
  for (unsigned int i=0;i<ClonOutData.size();i++)  delete ClonOutData[i];
}
/**
  Initiate new Run: reset grid and randomly set initial individuals.
*/
void CClonalGridEnvir::InitRun(){
  CEnvir::InitRun();
  resetGrid();

  //set initial plants on grid...
  InitInds("Input\\RSpec28.txt");
}
//------------------------------------------------------------------------------
/**
  This is an adaptable function to set initial numbers of individuals on grid.
  At the moment Ines' invasion experiment (1) is implemented:
  put 100 seeds of a non-clonal plant type randomly on the grid

  (Felix' grazing experiments:
  put ten individuals of each plant type randomly on the grid)

  \todo put this function in an extra file together with other adaptable functions
*/
void CClonalGridEnvir::InitInds(){
  SclonalTraits* cltraits=SclonalTraits::clonalTraits[clonaltype];
     SPftTraits* traits=SPftTraits::PftList[Pfttype];
  //non-clonal plants
   int Pfttypebegin=0;//(StrToInt(Form1->Edit13->Text))-1;
   int Pfttypeend=80; //(StrToInt(Form1->Edit14->Text))-1;
   for(int Pfttype=Pfttypebegin;Pfttype<=Pfttypeend;Pfttype++)
   {
     // cout<<"plant type .. "<<Pfttype+1<<endl;
     SPftTraits* traits=SPftTraits::PftList[Pfttype];
      CGrid::InitPlants(traits,10);
      PftInitList[traits->name]+=10;
   }
  //non-clonal seeds
//  CGrid::InitSeeds(traits,100);
//  PftInitList[traits->name]+=100;
//  CGridclonal::InitClonalPlants(traits,cltraits,0);
//  PftInitList[traits->name+cltraits->name]+=0;  //clonal seeds
//  CGridclonal::InitClonalSeeds(traits,cltraits,100);
//  PftInitList[traits->name+cltraits->name]+=100;
}//Initialization of individuals on grid
//------------------------------------------------------------------------------
/**
  this function reads a file and initializes seeds on grid after
  file data.

  (each PFT on file gets 10 seeds randomly set on grid)
  \since 2010-07-07
  \todo put search for pft-links in SclonalTraits and SPftTraits
*/
void CClonalGridEnvir::InitInds(string file){
  const int no_init_seeds=10;
  //Open InitFile,
  ifstream InitFile(file.c_str());
  if (!InitFile.good()) {cerr<<("Fehler beim Öffnen InitFile");exit(3); }
  cout<<"InitFile: "<<file<<endl;
  string line;
  getline(InitFile,line);//skip header line
  int dummi1; string dummi2; int PFTtype; string Cltype;
  do{
    //get from file
    InitFile>> dummi1>>dummi2
            >> PFTtype
            >> Cltype;
    if(!InitFile.good()) return;
    // interpretation
    //suche in einer Schleife den nichtklonalen Typ
    SPftTraits* traits=SPftTraits::PftList[PFTtype-1];
    //suche in einer Schleife den Klonalen Typ
    //(beachte Ausnahme 'clonal0')
    if (Cltype=="clonal0"){
    // initialization
       CGrid::InitSeeds(traits,no_init_seeds);
       PftInitList[traits->name]+=no_init_seeds;
//       cout<<"\n init "<<no_init_seeds<<" seeds of Pft"<<PFTtype<<Cltype;
    }
    else{
       SclonalTraits* cltraits=NULL;//=SclonalTraits::clonalTraits[Cltype];
       //...
       for(int i=0;i<SclonalTraits::clonalTraits.size();i++){
         if (SclonalTraits::clonalTraits[i]->name==Cltype){
           cltraits=SclonalTraits::clonalTraits[i];break;
         }
       }
    // initialization
       if(cltraits==NULL){
         cerr<<"no clonal type "<<Cltype<<" found\n";exit(3);
       }
       CGridclonal::InitClonalSeeds(traits,cltraits,no_init_seeds);
       PftInitList[traits->name+Cltype]+=no_init_seeds;
       PftLinkList[traits->name+Cltype]=traits;
       ClLinkList[traits->name+Cltype]=cltraits;
//       cout<<"\n init "<<no_init_seeds<<" seeds of Pft"<<PFTtype<<Cltype;
    }

  }while(!InitFile.eof());
}//initialization based on file
//------------------------------------------------------------------------------
/**
 This function initiates a number of seeds of the specified type on the grid.

 \param type string naming the type to be set
 \param number number of seeds to set
*/
void CClonalGridEnvir::InitSeeds(string type, int number)
{
   //searching the type
   SclonalTraits *cltraits=getClLink(type);//=SclonalTraits::clonalTraits[Cltype];
   SPftTraits *pfttraits=getPftLink(type);//=SclonalTraits::clonalTraits[Cltype];
   bool clonal= (cltraits!=NULL);//(cltraits->name=="default");

   //set seeds...
   if (clonal)
       CGridclonal::InitClonalSeeds(pfttraits,cltraits,number,pfttraits->pEstab);
   else
       CGrid::InitSeeds(pfttraits,number,pfttraits->pEstab);
}//InitSeeds
//------------------------------------------------------------------------------
void CClonalGridEnvir::OneRun(){
//   double teval=0.2;  //fraction of Tmax that is used for evaluation
   //get initial conditions
//   init=1; //for init the second plant (for the invasion experiments)

   //run simulation until YearsMax
   for (year=1; year<=SRunPara::RunPara.Tmax; ++year){
      cout<<" y"<<year;
      OneYear();
//      if (year%10==1){//(year==11||year==31){ modulo
//        WriteSurvival();
        WriteGridComplete(false);//report last year
//        clonalOutput();
        WriteSurvival();
//      }
      if (endofrun)break;
   }//years
      //file-write output     -now in main.cpp
//   WriteGridComplete();
//   WriteSurvival();
//   WritePftComplete();

}  // end OneSim
//------------------------------------------------------------------------------
void CClonalGridEnvir::OneYear(){
   for (week=1;week<=WeeksPerYear; ++week)
   {
     OneWeek();
     //exit conditions
      exitConditions();
      if (endofrun)break;
  }//weeks
} // end OneYear
//------------------------------------------------------------------------------
/**
  calculation of one week's todos

  \since 2010-07-07 no invasion version
*/
void CClonalGridEnvir::OneWeek(){
//   cout<<"week "<<week<<endl;
   ResetWeeklyVariables(); //cell loop, removes data from cells
   SetCellResource();      //variability between weeks

   CoverCells();           //plant loop
   setCover();             //set ACover und BCover lists, as well as type cover
   DistribResource();      //cell loop, resource uptake and competition

   PlantLoop();            //Growth, Dispersal, Mortality

   if (year>1) Disturb();  //grazing and disturbance

   RemovePlants();         //remove trampled plants

   //if (((week>=1) && (week<4)) || ((week>21)&&(week<=25)))  //establishment only between week 1-4 and 21-25
   EstabLottery();         //for seeds and ramets

   if (week==20) SeedMortAge(); //necessary to remove non-dormant seeds before autumn
   if (week==WeeksPerYear){     //at end of year ...
      Winter();            //removal of above ground biomass and of dead plants
      SeedMortWinter();    //winter seed mortality
   }

   if (week==20){        //general output
      GetOutput();   //calculate output variables
   }
   if (week==30){
      //get cutted biomass
      GetOutputCutted();
      //clonal output
      // comment out since 11/11/17
      //      GetClonOutput();   //calculate output variables
   }
}//end CClonalGridEnvir::OneWeek()
//---------------------------------------------------------------------------
/**
  Exit conditions for runs,
  where clonal Plants migrate into a non-clonal community or
  vice versa.
  \since clonal version

  changed for Lina's realistic-pft-experiments
*/
int CClonalGridEnvir::exitConditions()
{
     int currTime=GetT();
     int NPlants=GetNPlants();//+GetNclonalPlants();
     int NClPlants=GetNclonalPlants();

//    if no more individuals existing
     if ((NPlants + NClPlants)==0)
     {
        endofrun=true;
        return currTime; //extinction time
     }
     return 0;
}//end CClonalGridEnvir::exitConditions()
//-Auswertung--------------------------------------------------------------------------
int CClonalGridEnvir::getACover(int x, int y){
  return ACover[x*SRunPara::RunPara.CellNum+y];}
int CClonalGridEnvir::getBCover(int x, int y){
  return BCover[x*SRunPara::RunPara.CellNum+y];}
int CClonalGridEnvir::getGridACover(int i){return CellList[i]->getCover(1);}
int CClonalGridEnvir::getGridBCover(int i){return CellList[i]->getCover(2);}

/// Anteil des erfragten Typs type an der Zelle i
/// \warning ohne Adressüberprüfung
/// \sa CCell::getCover()
///
double CClonalGridEnvir::getTypeCover(const int i, const string type)const{
  return CellList[i]->getCover(type);}

///returns grid cover of a given type
double CClonalGridEnvir::getTypeCover(const string type)const{
  //for each cell
  double number=0;
  const long int sumcells=SRunPara::RunPara.GetSumCells();
  for (long int i=0; i<sumcells; ++i){
         number+=getTypeCover(i,type);

  }
  return number/sumcells;
}

/**
  \todo ermittle cover für alle typen auf dem Grid
  (dazu evtl neue klassenvariable deklarieren)
  dazu cover der einzelnen Typen in das aktuelle PFTOut eintragen
   -- Funktion z.Zt. auskommentiert

  \bug double PftCover muss mit 0 initialisiert werden
  \warning PftCover is very time consuming
*/
void CClonalGridEnvir::setCover(){
  typedef map<string, int> mapType;
  const int sum=SRunPara::RunPara.GetSumCells();
  for(int i=0;i<sum;i++){
    ACover.at(i)=getGridACover(i);
    BCover[i]   =getGridBCover(i);
  }
//  if(this->PftOutData.size()>0){
//    typedef map<string, SPftOut::SPftSingle*> mapType;
//    for(mapType::const_iterator it = PftWeek->PFT.begin();
//          it != PftWeek->PFT.end(); ++it)
  if (week==20){
    typedef map<string, long> mapType;
    for(mapType::const_iterator it = this->PftInitList.begin();
          it != this->PftInitList.end(); ++it)
    {
    //in folgender Zeile der Fehler?
          this->PftCover[it->first]=getTypeCover(it->first);
    }
   }//end if week=20
    //  }//end if output size >0
  //  if (week==WeeksPerYear){
//    PftCover.clear(); //new year
//    for(int i=0;i<sum;i++){
//      for(mapType::const_iterator it = PftInitList.begin();
//          it != PftInitList.end(); ++it)
//      {
//         //addiere den type-cover der aktuellen Zelle
//         PftCover[it->first]+=getTypeCover(i,it->first);
//      }

//    }
//  }//end if at end of year
}

//---------------------------------------------------------------------------
void CClonalGridEnvir::clonalOutput(){
    //write data in the clonalOut file
    ofstream clonOut(NameClonalOutFile.c_str(),ios_base::app);
//    {
   if (ClonOutData.size()==0) {cerr<<("no data found in ClonalOutData");return; }
    if (!clonOut.good()) {cerr<<("Fehler beim Öffnen ClonalOutFile");exit(3); }
//funktioniert nicht!?:
    clonOut.seekp(0, ios::end);
    long size=clonOut.tellp();
    if (size==0) clonOut<<"SimNr"
    //<<"\tPFT\tclType"
    <<"\tRun\tweek"
    //<<"\tnon-ClPlants"
    <<"\tramets\tBM\tgenets\n";
   SClonOut* ClonWeek=ClonOutData.back();
    clonOut<<SimNr<<"\t"
//        <<Pfttype+1<<"\t"
//        <<clonaltype+1<<"\t"
        <<RunNr<<"\t"
        <<GetT()<<"\t"
//        <<GetNPlants()<<"\t"
        <<ClonWeek->NclonalPlants<<"\t"   //GetNclonalPlants()
        <<ClonWeek->MclonalPlants<<"\t"
        <<ClonWeek->NGenets              //GetNMotherPlants()
        <<endl;    //schreibt z.b: 1 4
//    }
    clonOut.close();
}//end CClonalGridEnvir::clonalOutput()
//---------------------------------------------------------------------------
/**
  calculate Output-variables and store in intern 'database'

  changed in Version 100715 - for  type-flexible Output
*/
void CClonalGridEnvir::GetOutput()//PftOut& PftData, SGridOut& GridData)
{
   int pft, df;
   string pft_name;
   double mean, prop_PFT;

   SPftOut*  PftWeek =new SPftOut();

   SGridOut* GridWeek=new SGridOut();
   //calculate sums
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      CPlant* plant = *iplant;
//      pft=plant->Traits->TypeID;
      pft_name=plant->pft();
      //suche pft...
      if (!plant->dead){
//following lines adapted from Internet
// http://stackoverflow.com/questions/936999/what-is-the-default-constructor-for-c-pointer
//CHANGED BY ME
        map<string,SPftOut::SPftSingle*>::const_iterator pos = PftWeek->PFT.find(pft_name);
        SPftOut::SPftSingle* mi;
        if (pos==PftWeek->PFT.end())
           PftWeek->PFT[pft_name] = new SPftOut::SPftSingle();
        mi =PftWeek->PFT.find(pft_name)->second;
        mi->totmass+=plant->GetMass();
        ++mi->Nind;
        mi->shootmass+=plant->mshoot;
        mi->rootmass+=plant->mroot;
      }
   }
   //calculate mean values
   typedef map<string, SPftOut::SPftSingle*> mapType;

   for(mapType::const_iterator it = PftWeek->PFT.begin();
          it != PftWeek->PFT.end(); ++it)
    {
    //in folgender Zeile der Fehler?
    //      it->second->cover=getTypeCover(it->first);
         string type=it->first;
         double cover=this->PftCover.find(it->first)->second ;
          it->second->cover=cover;

//   for (int i=0; i<SRunPara::RunPara.NPft; ++i){
      if (it->second->Nind>=1){
//         PftWeek->shootmass[i]=PftWeek->shootmass[i]/PftWeek->Nind[i];
//         PftWeek->rootmass[i]=PftWeek->rootmass[i]/PftWeek->Nind[i];

         //calculate shannon index and proportion of each PFT
         prop_PFT=(double) it->second->Nind/PlantList.size();
         GridWeek->shannon+=(-1)*prop_PFT*log(prop_PFT);
      }
///\todo cover mit Funktion find()  füllen -- da sonst evtl adressierungsfehler
//      it->second->cover=PftCover[it->first];
      GridWeek->totmass+=it->second->totmass;
      GridWeek->above_mass+=it->second->shootmass;
      GridWeek->below_mass+=it->second->rootmass;
      GridWeek->Nind+=it->second->Nind;
   }

   //summarize seeds on grid...
   int sumcells=SRunPara::RunPara.GetSumCells();
      for (int i=0; i<sumcells; ++i){
         CCell* cell = CellList[i];
         for (seed_iter iter=cell->SeedBankList.begin();
              iter<cell->SeedBankList.end(); ++iter){
            string pft=(*iter)->pft();
            map<string,SPftOut::SPftSingle*>::const_iterator
              pos = PftWeek->PFT.find(pft);
            if (pos==PftWeek->PFT.end())
            PftWeek->PFT[pft] = new SPftOut::SPftSingle();
//         if (!PftWeek->PFT[pft])
//           PftWeek->PFT[pft] = new SPftOut::SPftSingle();
            ++PftWeek->PFT[pft]->Nseeds;
         }
      }

//      GridWeek->PftCount=0;

//   for(mapType::const_iterator it = PftWeek->PFT.begin();
//          it != PftWeek->PFT.end(); ++it)
//    {
// //      for (int pft=0; pft<SRunPara::RunPara.NPft; ++pft){
// //         if ((PftWeek->Nind[pft] > 0) || (PftWeek->Nseeds[pft] > 0)){
//         if ((it->second->Nind > 0) || (it->second->Nseeds > 0)){
//            ++GridWeek->PftCount;
//         }
//      }

   double sum_above=0, sum_below=0;
   for (int i=0; i<sumcells; ++i){
      CCell* cell = CellList[i];
      sum_above+=cell->AResConc;
      sum_below+=cell->BResConc;
   }
   GridWeek->aresmean=sum_above/sumcells;
   GridWeek->bresmean=sum_below/sumcells;

    double NCellsAcover=GetCoveredCells();
    if (NCellsAcover>0){
      CoveredCells=(NCellsAcover/SRunPara::RunPara.GetSumCells());
    }
    GridWeek->bareGround=1- CoveredCells;

   PftOutData.push_back(PftWeek);
   GridWeek->PftCount=PftSurvival(); //get PFT results

   GridOutData.push_back(GridWeek);
}//end CClonalGridEnvir::GetOutput(

//---------------------------------------------------------------------------
void CClonalGridEnvir::GetOutputCutted(){
   SGridOut* GridWeek=GridOutData.back();
   //store cutted biomass and reset value for next mowing
   GridWeek->cutted=this->getCuttedBM();this->resetCuttedBM();

}
//---------------------------------------------------------------------------
void CClonalGridEnvir::GetClonOutput()//PftOut& PftData, SGridOut& GridData)
{
   SClonOut* ClonWeek=new SClonOut();
    ClonWeek->NclonalPlants=GetNclonalPlants();
    ClonWeek->NGenets=GetNMotherPlants();
    ClonWeek->MeanGeneration=GetNGeneration();
    ClonWeek->NPlants=GetNPlants();

   ClonOutData.push_back(ClonWeek);

}
//---------------------------------------------------------------------------
//eof
