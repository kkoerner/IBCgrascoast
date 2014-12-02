/**\file
   \brief functions and static defaults of environmental classes

   Version for Ines' invasion experiments: adapt GetSim, Run and Init functions
   of class CClonalGridEnvir (further: CClonalGridEnvir::exitConditions()).
   \date 03/2010

   Version for Linas exploratoria experiments - no invasion; realistic PFTs
   \date 07/2010
*/

//---------------------------------------------------------------------------
#include "CEnvir.h"
#include <iostream>
#include <time.h>
#include <iomanip>
#include <sstream>

using namespace std;
//-CEnvir: Init static variables--------------------------------------------------------------------------
   int CEnvir::week=0;
   int CEnvir::year=1;
   int CEnvir::WeeksPerYear=30;

   //Output Files
   string CEnvir::NamePftOutFile= "Output\\PftOut.txt";
   string CEnvir::NameGridOutFile= "Output\\GridOut.txt";
   string CEnvir::NameSurvOutFile= "Output\\SurvOutGraz.txt";
   string CEnvir::NameLogFile="Output\\LogUpSc.log";
   string CEnvir::NameClonalOutFile="Output\\clonalOut.txt";

   string SSR::NameLDDFile1= "Output\\SeedOut1_2.txt";
   string SSR::NameLDDFile2= "Output\\SeedOut2_3.txt";
   string SSR::NameLDDFile3= "Output\\SeedOut4_5.txt";
   string SSR::NameLDDFile4= "Output\\SeedOut9_10.txt";
   string SSR::NameLDDFile5= "Output\\SeedOut19_20.txt";

   int CEnvir::NRep=1;        //!> number of replications -> read from SimFile;
   int CEnvir::SimNr=0;
   int CEnvir::RunNr=0;
   vector<double> CEnvir::AResMuster;
   vector<double> CEnvir::BResMuster;
   map<string,long> CEnvir::PftInitList;  //!< list of Pfts used
   map<string,double> SSR::PftSeedRainList;

//---------------------------------------------------------------------------
/**
 * constructor for virtual class
 */
 CEnvir::CEnvir():
  NCellsAcover(0),init(1),endofrun(false)
{
   ReadLandscape();
   ACover.assign(SRunPara::RunPara.GetSumCells(),0);
   BCover.assign(SRunPara::RunPara.GetSumCells(),0);
}
//---------------------------------------------------------------------------
/**
 * constructor -
 * Restore an previosly saved grid.
 * @param id file name stub of saves grid
 */
 CEnvir::CEnvir(string id):
  NCellsAcover(0),init(1),endofrun(false)
{
//read file
 string dummi=(string)"Save/E_"+id+".sav";
 ifstream loadf(dummi.c_str()); string d;
  loadf>>year>>week;
  string dummi2;
  loadf>>SRunPara::NamePftFile>>dummi2;//NameClonalPftFile must be read separately
//  getline(loadf,d);
  getline(loadf,dummi2); getline(loadf,dummi2);
  SRunPara::RunPara.setRunPara(dummi2);
   ReadLandscape();
   ACover.assign(SRunPara::RunPara.GetSumCells(),0);
   BCover.assign(SRunPara::RunPara.GetSumCells(),0);
}
//------------------------------------------------------------------------------
/**
 * destructor -
 * free summarizing data sets
 */
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
  AResMuster.clear();BResMuster.clear();
  AResMuster.assign(SRunPara::RunPara.GetSumCells(),
		  SRunPara::RunPara.meanARes);
  BResMuster.assign(SRunPara::RunPara.GetSumCells(),
		  SRunPara::RunPara.meanBRes);
}//end ReadLandscape

//------------------------------------------------------------------------------
/**
 * File input - get run parameters from file
 * - one line per setting;
 * nb repetitions can be specified.
 *
 * After reading one line of Simfile,
 * PFT definitions are read too via SPftTraits::ReadPFTDef()
 *
  \param pos file position to start reading
  \param file name of input file (obsolete, NameSimFile is used)
  \return file position of next simulation set
*/
int CEnvir::GetSim(const int pos,string file){
  //Open SimFile,
  ifstream SimFile(SRunPara::NameSimFile.c_str());
  if (!SimFile.good()) {cerr<<("error opening SimFile");exit(3); }
  cout<<"SimFile: "<<SRunPara::NameSimFile<<endl;
  int lpos=pos;
  if (pos==0){  //read header
    string line,file_id; //file_id not used here
    getline(SimFile,line);
    int tmax;//dummi
    SimFile>>tmax>>file_id;
    SRunPara::RunPara.Tmax=tmax;
//    file_id.erase (file_id.begin(), file_id.begin()+1);
//    file_id.erase (file_id.end()-1, file_id.end());

    getline(SimFile,line);
    getline(SimFile,line);
    lpos=SimFile.tellg();
  }
  SimFile.seekg(lpos);
  //ist position gültig?, wenn nicht -- Abbruch
  if (!SimFile.good()) return -1;

  // int version, acomp, bcomp;
    //RunNr=0; int dummi;
    //for (int i=0; i<SimNrMax; ++i)

       SimFile>>SimNr
             >>SRunPara::RunPara.meanBRes
              >>SRunPara::RunPara.GrazProb
              >>SRunPara::RunPara.NCut
              >>SRunPara::RunPara.WaterLevel
              >>SRunPara::RunPara.salt
               ;

       //---------standard parameter:
       //aboveground resources
       SRunPara::RunPara.meanARes=100;

       //grazing intensity
       SRunPara::RunPara.PropRemove=0.5;
       //trampling equals grazing
       SRunPara::RunPara.DistAreaYear=SRunPara::RunPara.GrazProb;
       //above- and belowground competition
  //     acomp=1;bcomp=0;
      //--------------------------------
      // set version and competition  modes - in this way because of enum types!
       //filenames
     string idstr= SRunPara::RunPara.getRunID();
     stringstream strd;
     strd<<"Output\\Mix_Grid_log_"<<idstr
       <<".txt";
     NameLogFile=strd.str();     // clear stream
     strd.str("");strd<<"Output\\Mix_gridO_"<<idstr
       <<".txt";
     NameGridOutFile=strd.str();
     strd.str("");strd<<"Output\\Mix_typeO_"<<idstr
       <<".txt";
     NameSurvOutFile= strd.str();

     //Open InitFile,
   	SPftTraits::ReadPFTDef(SRunPara::NamePftFile);

  //     SRunPara::RunPara.print();
   return SimFile.tellg();
}//end  CEnvir::GetSim
//------------------------------------------------------------------------------
/**
 * refresh environment
 *
 * \todo is it used?
 * /
void CEnvir::clearResults(){
   InitRun();
   ACover.assign(SRunPara::RunPara.GetSumCells(),0);
   BCover.assign(SRunPara::RunPara.GetSumCells(),0);
   NCellsAcover=0;
}
*/
//------------------------------------------------------------------------------
/**
 * refresh output data.
 * \todo used?
 */
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
/**
 * File Output..
 *
 * Choose whether to write every year, once at end of run, or discrete slots between.
 * At end of Run all data or only last year's data can be written
 */
void CEnvir::WriteOFiles() {
 if (year%10==1){// modulo... output every n time steps
	// if (year==11||year==31){//output in discrete time steps
	WriteGridComplete(false);
	WritePftComplete(false);
}
	//	WriteclonalOutput();
//	if (year == SRunPara::RunPara.Tmax){//output at end of run
//		WriteSurvival();
//		this->WritePftSeedOutput();//write all year's output at once
//	}
}
//------------------------------------------------------------------------------
/**
 * File Output - grid-wide summaries
 * @param allYears write all data or only the last entry?
 */
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
           // <<"abovemass\tbelowmass\tBareGround\t"
              <<"abovemass\tbelowmass\tBareGround\t"
              <<"pcBare\tpcBareSoil\t"
              <<"mean_WL\tmean_bres\t"
              <<"shannon\t"
              <<"NPFT\tCutted\tGrazed\t"
              <<"aLitter\tbLitter"
             <<"NNonClonal\tNClonal\tNGenets"
              ;
     GridOutFile<<"\n";
   }

   vector<SGridOut>::size_type i=0;
   if (!allYears) i= GridOutData.size()-1;
   for ((i); i<GridOutData.size(); ++i){
      GridOutFile<<SimNr<<'\t'<<RunNr<<'\t'<<i //<<'\t'<<GridOutData[i]->week
                 <<'\t'<<GridOutData[i]->totmass
                 <<'\t'<<GridOutData[i]->Nind
                 <<'\t'<<GridOutData[i]->above_mass
                 <<'\t'<<GridOutData[i]->below_mass
                 <<'\t'<<GridOutData[i]->bareGround
                 <<'\t'<<GridOutData[i]->bareSoil
//                 <<'\t'<<GridOutData[i]->aresmean
                 <<'\t'<<GridOutData[i]->WaterLevel
                 <<'\t'<<GridOutData[i]->bresmean
                 <<'\t'<<GridOutData[i]->shannon
//                 <<'\t'<<GetMeanShannon(10)//25 nach 100J
                 <<'\t'<<GridOutData[i]->PftCount
//                 <<'\t'<<GetMeanNPFT(10)
                 <<'\t'<<GridOutData[i]->cutted
                 <<'\t'<<GridOutData[i]->grazed
                 <<'\t'<<GridOutData[i]->above_litter
                 <<'\t'<<GridOutData[i]->below_litter
                 <<'\t'<<GridOutData.back()->NPlants
                 <<'\t'<<GridOutData.back()->NclonalPlants
                 <<'\t'<<GridOutData.back()->NGenets//GetNMotherPlants()
              <<"\n";
   }
   GridOutFile.close();
}//WriteGridComplete
//------------------------------------------------------------------------------
/**
 * File output - summarizing PFT information
  changed in version 100715 for type-flexible Output
  \param allYears write all data or only the last entry?
*/
void CEnvir::WritePftComplete(bool allYears)
{
   //Open PftFile , write header and initial conditions
   ofstream PftOutFile(NamePftOutFile.c_str(),ios_base::app);
   if (!PftOutFile.good()) {cerr<<("Fehler beim Öffnen PftOutFile");exit(3); }
   PftOutFile.seekp(0, ios::end);
   long size=PftOutFile.tellp();
   if (size==0){
     PftOutFile<<"Sim\tRun\tTime";
     PftOutFile<<"\tInds\tJuv\tseeds\tcover\tshootmass\tPFT";
     PftOutFile<<"\n";
   }

   vector<SPftOut>::size_type i=0;
   if (!allYears)i= PftOutData.size()-1;
   for ((i); i<PftOutData.size(); ++i){
   typedef map<string, SPftOut::SPftSingle*> mapType;

     for(mapType::const_iterator it = PftOutData[i]->PFT.begin();
          it != PftOutData[i]->PFT.end(); ++it)
     {
         PftOutFile<<SimNr<<'\t'<<RunNr<<'\t'<<i;
         PftOutFile<<'\t'<<it->second->Nind;
         PftOutFile<<'\t'<<it->second->Njuv;
         PftOutFile<<'\t'<<it->second->Nseeds;
         PftOutFile<<'\t'<<it->second->cover;
         PftOutFile<<'\t'<<it->second->shootmass;
         PftOutFile<<'\t'<<it->first; //pft-name at last
         //add cover here

         PftOutFile<<"\n";
     }
   }
   PftOutFile.close();
}//end WritePftComplete()
//------------------------------------------------------------------------------
/**
 * File Output - survival Info of PFTs
 */
void CEnvir::WriteSurvival(){
  WriteSurvival(RunNr,SimNr);
}
/**
 * File Output - survival Info of PFTs
 *
 * @param str Output file name stub
 */
void CEnvir::WriteSurvival(string str){
  string dummi=NameSurvOutFile.substr(0,NameSurvOutFile.length()-4);
  NameSurvOutFile=dummi+str+".txt";
  WriteSurvival();
}
/**
  File-Output - Documentation of type-specific survival statistics.
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
     SurvOutFile<<"BM\tPopAd\tPopJuv\tcover\tPFT";
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
   SurvOutFile<<'\t'<<Pft->Nind;//number adult ramets (older than 6 weeks)
   SurvOutFile<<'\t'<<Pft->Njuv;//number juvenile ramets
   SurvOutFile<<'\t'<<Pft->cover;//cover of type

//     SurvOutFile<<'\t'<<GetMeanPopSize(it->first,10);//mean of 10years
//     SurvOutFile<<'\t'<<GetCurrPopSize(it->first);
//     SurvOutFile<<'\t'<<it->second
    SurvOutFile<<'\t'<<it->first<<endl;
  }
//     SurvOutFile<<"\n";
}//end writeSurvival
//---------------------------------------------------------------------------
/**
 * Write clonal Output
 */
void CEnvir::WriteclonalOutput(){
	//get data
    //write data in the clonalOut file
    ofstream clonOut(NameClonalOutFile.c_str(),ios_base::app);
//    {
    if (!clonOut.good()) {cerr<<("Fehler beim Öffnen ClonalOutFile");exit(3); }
    clonOut.seekp(0, ios::end);
    long size=clonOut.tellp();
    if (size==0) clonOut<<"SimNr\tRun\tweek\tnon-ClPlants\tClPlants\tclones\n";
    clonOut<<SimNr<<"\t"
 //       <<Pfttype+1<<"\t"
 //       <<clonaltype+1<<"\t"
        <<RunNr+1<<"\t"
        <<GetT()<<"\t"
        <<	GridOutData.back()->NPlants<<"\t"
        <<GridOutData.back()->NclonalPlants<<"\t"//GetNclonalPlants()
        <<GridOutData.back()->NGenets//GetNMotherPlants()
        <<endl;    //schreibt z.b: 1 4
//    }
    clonOut.close();
}//end CClonalGridEnvir::clonalOutput()
//---------------------------------------------------------------------------
/**
 * file output of seed rain (current list)
 *
 * \author FM - seed rain option
 */
void CEnvir::WritePftSeedOutput()
{
   string PFTname;
//   size_t pos1;


   // File 1 ----------------------------------------------------------------
   //Open PftFiles , write header and initial conditions
   ofstream LDDFile1(SSR::NameLDDFile1.c_str(),ios_base::app);
   if (!LDDFile1.good()) {cerr<<("Fehler beim Öffnen LDDFile");exit(3); }

   LDDFile1.seekp(0, ios::end);
   long size=LDDFile1.tellp();
   if (size==0){
     LDDFile1<<"Sim\tRun\tTime";
     LDDFile1<<"\t";

     for (map<string, SPftTraits*>::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){
            PFTname = it->first;
         LDDFile1<<PFTname<<"\t";
     }
     LDDFile1<<"\n";
   }

   typedef map<string, SPftTraits*> mapType;
   map<string, SPftOut::SPftSingle*>::const_iterator pos;

   for (vector<PftOut>::size_type i=0; i<PftOutData.size();++i){

     LDDFile1<<SimNr<<'\t'<<RunNr<<'\t'<<i+1;

     for (mapType::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){

          string pft_name = it->first;
          pos = PftOutData[i]->PFT.find(pft_name);
          if (pos == PftOutData[i]->PFT.end())
            LDDFile1<<"\t"<<0;
          else
            LDDFile1<<"\t"<<PftOutData[i]->PFT[pft_name]->LDDseeds[0];

     }
     LDDFile1<<"\n";
   }
   LDDFile1.close();

   // File 2 ----------------------------------------------------------------
   //Open PftFiles , write header and initial conditions
   ofstream LDDFile2(SSR::NameLDDFile2.c_str(),ios_base::app);
   if (!LDDFile2.good()) {cerr<<("Fehler beim Öffnen LDDFile");exit(3); }

   LDDFile2.seekp(0, ios::end);
   size=LDDFile2.tellp();
   if (size==0){
     LDDFile2<<"Sim\tRun\tTime";
     LDDFile2<<"\t";

     for (mapType::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){
            PFTname = it->first;
         LDDFile2<<PFTname<<"\t";
     }
     LDDFile2<<"\n";
   }

   for (vector<PftOut>::size_type i=0; i<PftOutData.size();++i){

     LDDFile2<<SimNr<<'\t'<<RunNr<<'\t'<<i+1;

     for (mapType::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){

          string pft_name = it->first;
          pos = PftOutData[i]->PFT.find(pft_name);
          if (pos == PftOutData[i]->PFT.end())
            LDDFile2<<"\t"<<0;
          else
            LDDFile2<<"\t"<<PftOutData[i]->PFT[pft_name]->LDDseeds[1];

     }
     LDDFile2<<"\n";
   }
   LDDFile2.close();

   // File 3 ----------------------------------------------------------------
   //Open PftFiles , write header and initial conditions
   ofstream LDDFile3(SSR::NameLDDFile3.c_str(),ios_base::app);
   if (!LDDFile3.good()) {cerr<<("Fehler beim Öffnen LDDFile");exit(3); }

   LDDFile3.seekp(0, ios::end);
   size=LDDFile3.tellp();
   if (size==0){
     LDDFile3<<"Sim\tRun\tTime";
     LDDFile3<<"\t";

     for (map<string, SPftTraits*>::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){
            PFTname = it->first;
         LDDFile3<<PFTname<<"\t";
     }
     LDDFile3<<"\n";
   }

   for (vector<PftOut>::size_type i=0; i<PftOutData.size();++i){

     LDDFile3<<SimNr<<'\t'<<RunNr<<'\t'<<i+1;

     for (mapType::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){

          string pft_name = it->first;
          pos = PftOutData[i]->PFT.find(pft_name);
          if (pos == PftOutData[i]->PFT.end())
            LDDFile3<<"\t"<<0;
          else
            LDDFile3<<"\t"<<PftOutData[i]->PFT[pft_name]->LDDseeds[2];

     }
     LDDFile3<<"\n";
   }
   LDDFile3.close();

   // File 4 ----------------------------------------------------------------
   //Open PftFiles , write header and initial conditions
   ofstream LDDFile4(SSR::NameLDDFile4.c_str(),ios_base::app);
   if (!LDDFile4.good()) {cerr<<("Fehler beim Öffnen LDDFile");exit(3); }

   LDDFile4.seekp(0, ios::end);
   size=LDDFile4.tellp();
   if (size==0){
     LDDFile4<<"Sim\tRun\tTime";
     LDDFile4<<"\t";

     for (map<string, SPftTraits*>::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){
            PFTname = it->first;
         LDDFile4<<PFTname<<"\t";
     }
     LDDFile4<<"\n";
   }

   for (vector<PftOut>::size_type i=0; i<PftOutData.size();++i){

     LDDFile4<<SimNr<<'\t'<<RunNr<<'\t'<<i+1;

     for (mapType::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){

          string pft_name = it->first;
          pos = PftOutData[i]->PFT.find(pft_name);
          if (pos == PftOutData[i]->PFT.end())
            LDDFile4<<"\t"<<0;
          else
            LDDFile4<<"\t"<<PftOutData[i]->PFT[pft_name]->LDDseeds[3];

     }
     LDDFile4<<"\n";
   }
   LDDFile4.close();

   // File 3 ----------------------------------------------------------------
   //Open PftFiles , write header and initial conditions
   ofstream LDDFile5(SSR::NameLDDFile5.c_str(),ios_base::app);
   if (!LDDFile5.good()) {cerr<<("Fehler beim Öffnen LDDFile");exit(3); }

   LDDFile5.seekp(0, ios::end);
   size=LDDFile5.tellp();
   if (size==0){
     LDDFile5<<"Sim\tRun\tTime";
     LDDFile5<<"\t";

     for (map<string, SPftTraits*>::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){
            PFTname = it->first;
         LDDFile5<<PFTname<<"\t";
     }
     LDDFile5<<"\n";
   }

   for (vector<PftOut>::size_type i=0; i<PftOutData.size();++i){

     LDDFile5<<SimNr<<'\t'<<RunNr<<'\t'<<i+1;

     for (mapType::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){

          string pft_name = it->first;
          pos = PftOutData[i]->PFT.find(pft_name);
          if (pos == PftOutData[i]->PFT.end())
            LDDFile5<<"\t"<<0;
          else
            LDDFile5<<"\t"<<PftOutData[i]->PFT[pft_name]->LDDseeds[4];

     }
     LDDFile5<<"\n";
   }
   LDDFile5.close();




}//end WritePftSeedOutput()

//---------------------------------------------------------------------------
//----------------------------------
/**
 * File Output - append text to log file
 * @param text string to append
 * @param filename filename
 */
void CEnvir::AddLogEntry(string text,string filename)
{
   ofstream LogFile(filename.c_str(),ios_base::app);
   if (!LogFile.good()) {cerr<<("Fehler beim Öffnen LogFile");exit(3);}

   LogFile<<text;
}
//----------------------------------
/**
 * File Output - append numbers to log file
 * @param nb number to append
 * @param filename filename
 */
void CEnvir::AddLogEntry(float nb,string filename)
{
   ofstream LogFile(filename.c_str(),ios_base::app);
   if (!LogFile.good()) {cerr<<("Fehler beim Öffnen LogFile");exit(3);}

   LogFile<<" "<<nb;
}

//---------------------------------------------------------------------------
/**
 * extract mean Shannon Diversity
 * @param years time steps to accumulate
 * @return Shannon Diversity value
 */
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
/**
 * extract mean PFT number
 * @param years time steps to accumulate
 * @return number of PFTs
 */
double CEnvir::GetMeanNPFT(int years)
{
   double sum=0, count=0;
   int start=(GridOutData.size()-1)-years;

   for (vector<SGridOut>::size_type i=start+1; i<GridOutData.size(); ++i){
      sum+=GridOutData[i]->PftCount;
      count++;
   }
   return sum/count;
}
//---------------------------------------------------------------------------
/**
 * extract current population size
 * @param pft PFT asked for
 * @return population size of PFT pft
 */
double CEnvir::GetCurrPopSize(string pft){
   if (PftOutData.back()->PFT.find(pft)== PftOutData.back()->PFT.end())
     return 0;
   SPftOut::SPftSingle* entry
      =PftOutData.back()->PFT.find(pft)->second;
   return entry->Nind + entry->Njuv;
}
//-------------------------------------------------------
/**
 * extract mean population size
 * @param pft PFT asked for
 * @param x time steps to accumulate
 * @return mean population size of PFT pft
 */
double CEnvir::GetMeanPopSize(string pft,int x){
      //only if output once a year
   if(PftOutData.size()<x) return -1;
   double sum=0;
   for (unsigned int i= (PftOutData.size()-x);i<PftOutData.size();i++)
    if (PftOutData[i]->PFT.find(pft)!=PftOutData[i]->PFT.end())
    {
     SPftOut::SPftSingle* entry
       =PftOutData[i]->PFT.find(pft)->second;
     sum+= entry->Nind +entry->Njuv;
     }
    return (sum/x);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/**
 * summarize seed mass distribution in seed rain
 * \author FM for seed rain option (numbers oriented to PFT categories)
 */
void SSR::GetNPftSeedsize(){

   //calculate number of PFTs with small, medium and large seeds
   for (int i=0; i<3; ++i) NPftSeedSize[i] = 0;

   string PFT_ID;
//   size_t posc;
   SPftTraits *pfttraits;

   for (map<string, SPftTraits*>::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){

      PFT_ID = it->first;
      pfttraits=SPftTraits::getPftLink(PFT_ID);
      if (pfttraits->SeedMass <= 0.1) NPftSeedSize[0]++;       //small
      else if (pfttraits->SeedMass >= 1.0) NPftSeedSize[2]++;   //large
      else  NPftSeedSize[1]++;//if (pfttraits->SeedMass == 0.3) //medium size
   }
}

//---------------------------------------------------------------------------
/**
 * summarize clonality distribution in seed rain
 * \author FM for seed rain option
 */
void SSR::GetNPftSeedClonal(){

   //calculate number of PFTs with small, medium and large seeds
   for (int i=0; i<2; ++i) NPftClonal[i] = 0;

   string PFT_ID;
//   size_t posc;
   SPftTraits *traits;

   for (map<string, SPftTraits*>::const_iterator it = SPftTraits::PftLinkList.begin();
          it!= SPftTraits::PftLinkList.end(); ++it){

      PFT_ID = it->first;
      traits=SPftTraits::getPftLink(PFT_ID);
      if (!traits->clonal) NPftClonal[0]++; //non-clonal
      else NPftClonal[1]++;                 //clonal
   }
}


//eof
