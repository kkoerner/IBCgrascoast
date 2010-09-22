/**\file code for UpScale-related classes
*/
//---------------------------------------------------------------------------
#pragma hdrstop

#include "SiteScale.h"
#include <iostream>
#include <sstream>

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
/**
Constructor for class CSiteScale. You have to give the size of the upscaled site.
\param size side length in number of cells. The grid is a sqare.

*/
CSiteScale::CSiteScale(int size):xLim(size),yLim(size),LDDEffect(0)
{
  long length=size*size;
  //fill Grids with empty cells
  for (unsigned long i=0; i<length; i++){
cout<<"init cell"<<i<<endl;
     CClonalGridEnvir * dummi=new CClonalGridEnvir();
     site.push_back(dummi);
     TpftList * el1=new TpftList();
     seed_to_disperse.push_back(el1);// assign(length);
     TpftList * el2=new TpftList();
     seed_rain.push_back(el2);//assign(length);
  }
}//Constructor
//---------------------------------------------------------------------------
/**
\warning mean dispersal distance for LDD seeds differs to normal dispersal
  by 100fold -variable via parameter LDDEffect
\bug getting dispersal type-pecific properties of seeds is troubling
\param lddfactor effect of LDD on type-specific dispersal distance (default=100)
*/
void CSiteScale::SeedDispersal()
{
     //test for no dispersal
  if (LDDEffect==0){
      //copy seed_to_disperse to seed_rain
      for (unsigned long i=0;i<seed_to_disperse.size();i++)
        for(TpftList::const_iterator it = seed_to_disperse[i]->begin();
         it != seed_to_disperse[i]->end(); ++it)
           (seed_rain[i])->operator[](it->first)+=it->second;
  }else
      //disperse seeds...
  //for all cells..
 for (unsigned long i=0;i<seed_to_disperse.size();i++){
   TpftList * to_disp=seed_to_disperse[i];
   int x=i%xLim;int y=i/xLim;   //richtig?
   for(TpftList::const_iterator it = to_disp->begin();
    it != to_disp->end(); ++it){
    string type=it->first;
    //get mean and sd
    SPftTraits* traits=CClonalGridEnvir::getPftLink(type);
    double mean=traits->Dist*100*LDDEffect;//lddfactor;
    double sd=mean;
    //for all seeds of that type...
    for (unsigned int seednb=0;seednb<it->second;seednb++){
     int lx=x,ly=y;
     getTargetCell(lx,ly,mean,sd,SRunPara::RunPara.GridSize);
     //test for touching boundaries (absorbing condition)
     if (lx<0||lx>=xLim||ly<0||ly>=yLim) continue;
     unsigned long index = lx*xLim+ly; //richtig?
     //add seed to seed_rain
     (seed_rain[index])->operator[](type)++;
    }
   }
 }
//clear
 for (int i=0;i<seed_to_disperse.size();i++){
  seed_to_disperse[i]->clear();
  //delete seed_to_disperse[i]; seed_to_disperse[i]=NULL;
 }
}//SeedDispersal
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/**
Constructor for the CSiteScaleEnvir class.
At the moment environmental parameters for the model are set here.

\bug - has to move to CClonalGridEnvir
*/
CSiteScaleEnvir::CSiteScaleEnvir(int nr,int run,int size,int tmax,
     double ares,double bres,
     double lddeffect,double grazing,
     double ncut,double trampling):CSiteScale(size),CEnvir()
{
   cout<<"new environment: SimId_"<<nr<<" Run_"<<run<<" Size_"<<size;
   cout<<" TMax_"<<tmax<<" Ares_"<<ares<<" Bres_"<<bres;
   cout<<" LDDEffect_"<<lddeffect<<" PGraz_"<<grazing<<" NCut_"<<ncut;
   cout<<" Trampling_"<<trampling<<endl;
   using SRunPara::RunPara;
   SimNr=nr;RunNr=run;
   RunPara.Version=version2; //const
   RunPara.Tmax=tmax;
   RunPara.GrazProb=grazing;
   RunPara.NCut=ncut;            //for cutting scenarios
   RunPara.DistAreaYear=trampling;     //for trampling scenarios
   RunPara.meanARes=ares;
   RunPara.meanBRes=bres;
   setLDDEffect(lddeffect);
   //set logfile-name
   stringstream strname;
   strname<<"Output\\LogUpSc"<<SimNr<<".log";
   NameLogFile=strname.str();

}
/**
Constructor for the CSiteScaleEnvir class.
At the moment environmental parameters for the model are set here.

*/
CSiteScaleEnvir::CSiteScaleEnvir():CSiteScale(2),CEnvir(){
  //CSiteScale(5); //zu testzwecken
  //set environmental preferences...
   using SRunPara::RunPara;
   SimNr=1004;RunNr=1;
   RunPara.Version=version2;
   RunPara.Tmax=15;
   RunPara.GrazProb=0;
   RunPara.NCut=0;            //for cutting scenarios
   RunPara.DistAreaYear=0;     //for trampling scenarios
   RunPara.meanARes=100;
   RunPara.meanBRes=100;
   setLDDEffect(100);
  //read file..
  }//constructor
//------------------------------------------------------------------------------
/**
  this function reads a file and initializes seeds on grid after
  file data.

  (each PFT on file gets 10 seeds randomly set on grid)
  \since 2010-07-07
  \todo put search for pft-links in SclonalTraits and SPftTraits
*/
void CSiteScaleEnvir::InitInds(string file){
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
    SclonalTraits* cltraits=NULL;//=SclonalTraits::clonalTraits[Cltype];
    if (Cltype!="clonal0"){
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
     }
//       CGridclonal::InitClonalSeeds(traits,cltraits,no_init_seeds);
     string mytype=traits->name;
     if(Cltype!="clonal0")mytype+=Cltype;
     PftInitList[mytype]+=no_init_seeds;
     CClonalGridEnvir::addPftLink(mytype,traits);
     CClonalGridEnvir::addClLink(mytype,cltraits);
//       cout<<"\n init "<<no_init_seeds<<" seeds of Pft"<<PFTtype<<Cltype;
//    }

  }while(!InitFile.eof());
}//initialization based on file
//---------------------------------------------------------------------------
/**
Calculates weekly processes from beginning of the year till global
seed dispersal time (at the moment: end of the year - week 30) for local cell i.
Seeds to be dispersed by LDD are written in the global 'seed_to_disperse'.

\param i index of cell to run
*/
void CSiteScaleEnvir::first_weeks(int i)
{
   cout<<" cell "<<i<<endl;
//   stringstream strstream;
  //Berechne Wochen 1-x
   clock_t beg =clock();
   for (week=1;week<=WeeksPerYear; ++week)
   {
     site[i]->OneWeek();
   }//weeks
   double difft=(clock()-beg)/CLOCKS_PER_SEC;
   cout<<difft<<" sec. needed\n";
//   strstream << " "<<difft;
//   AddLogEntry(strstream.str(),"Output\\LogUpSc.log");
   AddLogEntry(difft,NameLogFile);
  //get LDD-seeds
   seed_to_disperse[i]=site[i]->getLDDSeeds();
   //trage LDD-Seeds in globales Raster
}//first_weeks
//---------------------------------------------------------------------------
/**
Calculates global seed rain and weekly processes from global seed dispersal
event to end of the year(at the moment: only seed rain) for local cell i.

 \param i index of cell to run
\bug - has to move to CClonalGridEnvir
*/
void CSiteScaleEnvir::last_weeks(int i)
{
  //seed migration (init-function of CGridClonal and CGrid)
  TpftList * rain =seed_rain[i];
  unsigned long seeds=0;
    //for each type in list
  for(TpftList::const_iterator it = rain->begin();
    it != rain->end(); ++it){
    //set seeds...
    site[i]->InitSeeds(it->first,it->second);
    seeds+=it->second;
  }
    cout<<"seed rain on cell "<<i<<" ("<<seeds<<" seeds)\n";
//  stringstream strstream;
//  strstream <<" "<<seeds;
//  AddLogEntry(strstream.str(),"Output\\LogUpSc.log");
    AddLogEntry(seeds,NameLogFile);
//clear...seed rain
  rain->clear();// delete rain; rain=NULL;
  //Berechne Wochen x-end_of_year
//   for (week=1;week<=WeeksPerYear; ++week)
//   {
//     OneWeek();
//     //exit conditions
//      exitConditions();
//      if (endofrun)break;
//  }//weeks
}//last_weeks

//---------------------------------------------------------------------------
/**
Calculates one Year on site scale.
 - # run all single cells and collect their LDD-dispersed seeds.
 - # disperse seeds globally.
 - # initialize new seeds at each single gridcell.
*/
void CSiteScaleEnvir::OneYear()
{
//für alle Zellen
// for_each(site.begin(),site.end(),first_weeks);
   for (unsigned int i=0;i<site.size();i++) first_weeks(i);

//Verteile LDD-Seeds auf globales Seed-Rain Raster
  clock_t beg =clock();
  SeedDispersal();
  double difft=(clock()-beg)/CLOCKS_PER_SEC;
  cout<<"Dispersal:"<<difft<<" sec. needed\n";
//  stringstream strstream;
//  strstream <<" "<<difft;
//  AddLogEntry(strstream.str(),"Output\\LogUpSc.log");
  AddLogEntry(difft,NameLogFile);
     //für alle Zellen
// for_each(site.begin(),site.end(),last_weeks);
   for (unsigned int i=0;i<site.size();i++) last_weeks(i);
}//OneYear
//---------------------------------------------------------------------------
/**
Calculates one Run in the upscaled site.
 - # initializes seed composition on each single gridcell.
 - # runs yearly steps (number is given in SRunPara::RunPara) and writes Output
Files
\sa CSiteScaleEnvir::GetOutput()
*/
void CSiteScaleEnvir::OneRun()
{
//init...
  //new: read from File
  string mfile("");mfile="Input\\InitPFTdat.txt";
  InitInds(mfile);
  for (unsigned long i=0; i<site.size(); i++){
     site[i]->InitRun();
     for(TpftList::const_iterator it = PftInitList.begin();
       it != PftInitList.end(); ++it){
       site[i]->InitSeeds(it->first,it->second);
     }
  }
//for each year
  //oneyear
   for (year=1; year<=SRunPara::RunPara.Tmax; ++year){
      cout<<" y"<<year;
      AddLogEntry(year,NameLogFile);
      OneYear();
      GetOutput();
      AddLogEntry("\n",NameLogFile);

      if (endofrun)break;
   }//years
  //Auswertung

}//OneRun
//---------------------------------------------------------------------------
/**
Writes a line of Output files in directory 'Output'.
 - # Overview with model parameter and annual means
 - # Shannon diversity for each cell (annually)
 - # Population sizes for each PFT(files) and for each gridcell (annually)

*/
void CSiteScaleEnvir::GetOutput()
{
   string dir="Output\\",core="UpSc",ext=".txt";char simidc[8];
   string simid=itoa(SimNr,simidc,10);
   //write file(s)
   //generally
   writeOverview(dir+core+simid+"-O"+ext);

   //Diversity each cell
   writeDiversity(dir+core+simid+"-SD"+ext);

   //Type Composition
   //for all types (each type a file)
     for(TpftList::const_iterator it = PftInitList.begin();
       it != PftInitList.end(); ++it){
       string type=it->first;
       writeTypeCover(type,dir+core+simid+"-"+type+ext);
     }

}//GetOutput
//---------------------------------------------------------------------------
void CSiteScaleEnvir::writeOverview(string filename)
{
   //open file(s)
   //generally
   ofstream OutFile0(filename.c_str(),ios_base::app);
   if (!OutFile0.good()) {cerr<<("Fehler beim Öffnen OutFile1");exit(3); }
   //if beginning of run (i.e. year==0 or file is empty?)
   OutFile0.seekp(0, ios::end);
   long size=OutFile0.tellp();
   if (size==0){
     OutFile0<<"SIM_ID: "    <<SimNr<<endl;
     OutFile0<<"GRIDSIZE_X: "<<xLim <<endl;
     OutFile0<<"GRIDSIZE_Y: "<<yLim <<endl;
     OutFile0<<"YEAR_MAX: "<<SRunPara::RunPara.Tmax <<endl;
     OutFile0<<"RES_ABOVE: "<<SRunPara::RunPara.meanARes <<endl;
     OutFile0<<"RES_BELOW: "<<SRunPara::RunPara.meanBRes <<endl;
     OutFile0<<"P_GRAZING: "<<SRunPara::RunPara.GrazProb <<endl;
     OutFile0<<"N_CUTTING: "<<SRunPara::RunPara.NCut <<endl;
     OutFile0<<"LDD_EFFECT: "<<getLDDEffect() <<endl;
     OutFile0<<"Run\tYear\tmMSDiv\tSumBM_kg\n";
   }
   //output annually
   OutFile0<<RunNr<<'\t'<<year<<'\t'<<GetMeanMSDivGrid()<<'\t'
           <<GetSumBiomassGrid()/1e6<<endl;
}
//---------------------------------------------------------------------------
void CSiteScaleEnvir::writeDiversity(string filename)
{
   ofstream OutFile1(filename.c_str(),ios_base::app);
   if (!OutFile1.good()) {cerr<<("Fehler beim Öffnen OutFile1");exit(3); }
   //if beginning of run (i.e. year==0 or file is empty?)
   OutFile1.seekp(0, ios::end);
   long size1=OutFile1.tellp();
   if (size1==0){
     OutFile1<<"Sim\tRun\tTime";
     OutFile1<<"\tDiversity each cell";
     OutFile1<<"\n";
   }
   //output
   OutFile1<<SimNr<<'\t'<<RunNr<<'\t'<<year;

   //for all cells...
   for(unsigned long i =0; i<site.size();i++){
      double MSDiv=0;
      MSDiv=site[i]->GridOutData.back()->shannon;
      OutFile1<<"\t"<<MSDiv;
   }
   OutFile1<<endl;
}
//---------------------------------------------------------------------------
/**
  file writes population sizes of one type in a file
  \param type functional type to document
  \param filename file name to write in
*/
void CSiteScaleEnvir::writeTypeCover(string type,string filename)
{
   ofstream OutFile2(filename.c_str(),ios_base::app);
   if (!OutFile2.good()) {cerr<<("Fehler beim Öffnen OutFile1");exit(3); }
   //if beginning of run (i.e. year==0 or file is empty?)
   OutFile2.seekp(0, ios::end);
   long size=OutFile2.tellp();
   if (size==0){
     OutFile2<<"Sim\tRun\tTime";
     OutFile2<<"\tPopSize_"<<type;
     OutFile2<<"\n";
   }
   //output
   OutFile2<<SimNr<<'\t'<<RunNr<<'\t'<<year;

   //for all cells...
   for(int i =0; i<site.size();i++){
      double cover=0;
      map<string,SPftOut::SPftSingle*>::iterator pos=
        site[i]->PftOutData.back()->PFT.find(type);
      if (pos!=site[i]->PftOutData.back()->PFT.end())
        cover=pos->second->Nind;
      OutFile2<<"\t"<<cover;
   }
   OutFile2<<endl;

}
//---------------------------------------------------------------------------
/**
  \return current mean of all single grid-cell's Shannon Diversity
*/
double CSiteScaleEnvir::GetMeanMSDivGrid()
{
 double mean=0;
 //for all cells
 for(unsigned long i=0;i<site.size();i++){
   mean+=site[i]->GridOutData.back()->shannon;
 }
 return mean/site.size();
}
//---------------------------------------------------------------------------
/**
 \return overall-grid aboveground biomass
*/
double CSiteScaleEnvir::GetSumBiomassGrid()
{
  double sum=0;
  //for all cells
 for(unsigned long i=0;i<site.size();i++){
   sum+=site[i]->GridOutData.back()->above_mass;
 }
 return sum;
}
//---------------------------------------------------------------------------
//eof
