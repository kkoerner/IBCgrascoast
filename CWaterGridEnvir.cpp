//---------------------------------------------------------------------------


//#pragma hdrstop

#include "CWaterGridEnvir.h"
#include "CWaterPlant.h"
#include <iomanip>
#include <sstream>


   map<string,SWaterTraits*> CWaterGridEnvir::WLinkList=
     map<string,SWaterTraits*>();
   vector<double> CWaterGridEnvir::weeklyWL=vector<double>();
   vector<double> CWaterGridEnvir::weeklySAL=vector<double>();
   vector<double> CWaterGridEnvir::weeklySAT=vector<double>();
// double CWaterGridEnvir::salinity=0;
//---------------------------------------------------------------------------
void CWaterGridEnvir::CellsInit(){
}//end CellsInit
//---------------------------------------------------------------------------
/**
  Load CWaterGridEnvir from file(s).
  \param id file name ID to load from

  \note no water grid specific data to load
*/
CWaterGridEnvir::CWaterGridEnvir(string id):CClonalGridEnvir(id)
{
  //open file..
 string dummi=(string)"Save/G_"+id+".sav";
 ifstream loadf(dummi.c_str());
 string d; getline(loadf,d);
 int x=0,y=0;int xmax=SRunPara::RunPara.CellNum-1;
   //load cells..
   //loop over cell entries
   do{
     loadf>>x>>y;
     getline(loadf,d);getline(loadf,d);
     while(d!="CE"){
       //set seeds of type x
       stringstream mstr(d);string type;int num;
       mstr>>type>>num;
        InitSeeds(type,num,x,y,0);
       getline(loadf,d);
     }

   }while(!(x==xmax&&y==xmax));
   //load Plants..
   int num;
 loadf>>d>>d>>d>>num;getline(loadf,d);
 cout<<"lade "<<num<<"plant individuals.."<<endl;
 do {getline(loadf,d);}while(InitInd(d));

cout<<"grid "<<id<<" initiated ---------------\n" ;
//load type definitions
}//load from file
//---------------------------------------------------------------------------
CWaterGridEnvir::~CWaterGridEnvir(){
//  for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
//      CWaterCell* cell = CellList[i];
//      delete cell;
//   }
//   delete[] CellList;

}
//-Save.. and Load.. --------------------------------------------------------------------------
/**
 Saves the current state of the grid and parameters.

 -does it have to be in one file?
 -how to store all information

 \note not saved: abiotic and weather data
\autor KK
\date 130214
*/
void CWaterGridEnvir::Save(string ID){
  //open file(s)
//  string fname="Save\\W_"+ID+".sav";
//  ofstream SaveFile(fname.c_str());
//  if (!SaveFile.good()) {cerr<<("Fehler beim Öffnen InitFile");exit(3); }
//  cout<<"SaveFile: "<<fname<<endl;

////environmental parameters CEnvir, CClonalGridEnvir
//  SaveFile<<endl;

  CClonalGridEnvir::Save(ID);

}//Save
//---------------------------------------------------------------------------
/**
 With which plant types the system to start.
 How are COMTESS plant types defined?

 \author KK
*/
void CWaterGridEnvir::InitInds()
{
  SWaterTraits::ReadWaterStrategy();
  int no_init_seeds=10;

  if(SRunPara::RunPara.species=="R" ||SRunPara::RunPara.species=="M"
  ||SRunPara::RunPara.species=="M1"||SRunPara::RunPara.species=="M2"){   //true
  //reed
    SPftTraits*    traits  =SPftTraits::PftList[15]; //15
    traits->pEstab=0.01;   //geringe Etablierung
    traits->MaxMass=200000;//hohe Individualmasse (100gAG+100gBG)
    traits->LMR=0.25;       //reed growth form
    traits->RAR=0.5;       //root densed (weniger Aufnahme, mehr Strukturgewebe)
    SclonalTraits* cltraits=SclonalTraits::clonalTraits[6];
    cltraits->mSpacer=500; //analog zu cheight mehr ressourcen pro cm rhizom
    SWaterTraits*  wtraits =SWaterTraits::PFTWaterList[0];
    InitWaterInds(traits,cltraits,wtraits,no_init_seeds,80000); //com out
    string spft="null";
    spft=this->PlantList.back()->pft();                         //com out
    PftInitList[spft]+=no_init_seeds;                           //com out
    addPftLink(spft,traits);//?noch mal genauer gucken
    addClLink(spft,cltraits);
    WLinkList[spft]=wtraits;
  }//end reed type

  if(SRunPara::RunPara.species=="G1"
  ||SRunPara::RunPara.species=="M"||SRunPara::RunPara.species=="M1"){ //true
  //further types
    SPftTraits*   traits  =SPftTraits::PftList[42]; //PFT43
//    traits->RAR=1.5;
    //  cltraits=SclonalTraits::clonalTraits[1]; //clonal2 Resshare0
    SclonalTraits* cltraits=SclonalTraits::clonalTraits[0]; //clonal1 Resshare1
    cltraits->mSpacer=70; //standard value
    SWaterTraits*  wtraits =SWaterTraits::PFTWaterList[2];
//  InitWaterSeeds(traits,cltraits,wtraits,no_init_seeds);
    InitWaterInds(traits,cltraits,wtraits,no_init_seeds,2000);    //com out
//  spft="null";
    string spft=this->PlantList.back()->pft();                          //com out
    PftInitList[spft]+=no_init_seeds;                            //com out
    addPftLink(spft,traits);//?noch mal genauer gucken
    addClLink(spft,cltraits);
    WLinkList[spft]=wtraits;
  }//grass type
  if(SRunPara::RunPara.species=="G2"
  ||SRunPara::RunPara.species=="M"||SRunPara::RunPara.species=="M2"){ //true
  //further types
    SPftTraits*   traits  =SPftTraits::PftList[71]; //PFT72  klein, stress
//    traits->RAR=1.5;
    //  cltraits=SclonalTraits::clonalTraits[1]; //clonal2 Resshare0
    SclonalTraits* cltraits=SclonalTraits::clonalTraits[0]; //clonal1 Resshare1
    cltraits->mSpacer=70; //standard value
    SWaterTraits*  wtraits =SWaterTraits::PFTWaterList[2];
//  InitWaterSeeds(traits,cltraits,wtraits,no_init_seeds);
    InitWaterInds(traits,cltraits,wtraits,no_init_seeds,400);    //com out
//  spft="null";
    string spft=this->PlantList.back()->pft();                          //com out
    PftInitList[spft]+=no_init_seeds;                            //com out
    addPftLink(spft,traits);//?noch mal genauer gucken
    addClLink(spft,cltraits);
    WLinkList[spft]=wtraits;
  }//grass type

}//end InitInds
bool CWaterGridEnvir::InitInd(string def){
  //initiate clonal plant
  CClonalGridEnvir::InitInd(def);
  //upgrade..
  stringstream d(def);
  //get dummi cell
  int x,y;  d>>x>>y;
  //frage streamzustand ab ; wenn nicht good, beende Funktion
  if (!d.good())
  return false;

  string type;
  d>>type;

  //for CWaterPlant
  CPlant* clplant=PlantList.back();
  CWaterPlant* plant = new CWaterPlant((CclonalPlant*)clplant,this->getWLink(type));
  DeletePlant( clplant); //erase and delete old clonal plant
   plant->getCell()->occupied=true;
   plant->getCell()->PlantInCell = plant;
  PlantList.pop_back(); PlantList.push_back(plant);           //append water plant
  cout<<"update "<<type<<" at "<<x<<":"<<y<<endl;
  return true;

} //end load water plant
/**
 init n seeds at position xy
*/
void CWaterGridEnvir::InitSeeds(const string  PftName,const int n,int x, int y,double estab)
{
     InitSeeds(this->getPftLink(PftName),
        this->getClLink(PftName),
        this->getWLink(PftName),n,x,y); //com out

}

/**
 init n seeds at random positions.
*/
void CWaterGridEnvir::InitSeeds(const string PftName,const int n,double estab)
{
     for (int i =0; i<n;i++)
     InitSeeds(this->getPftLink(PftName),
        this->getClLink(PftName),
        this->getWLink(PftName),1,this->getPftLink(PftName)->pEstab); //com out

}//end initWaterSeeds(string,...)

/**
 randomly distribute seeds of a given plant type (CWaterPlant)

 \param traits   link to basic PFT
 \param cltraits link to clonal traits
 \param wtraits  link to water traits
 \param n        number of seeds to disperse
 \param estab    establishment (default is 1 for initial conditions)
*/
void CWaterGridEnvir::InitSeeds(SPftTraits* traits,SclonalTraits* cltraits,
     SWaterTraits* wtraits, const int n,double estab,int x, int y)
{
//   using CEnvir::nrand;using SRunPara::RunPara;
//   int x,y;
   int SideCells=SRunPara::RunPara.CellNum;
   if(x<0 || y<0)   //set random position
     for (int i=0; i<n; ++i){
        x=nrand(SideCells);
        y=nrand(SideCells);
     }
   CCell* cell = CellList[x*SideCells+y];
   new CWaterSeed(estab,traits,cltraits,wtraits,cell);
//      cout<<"disp "<<cell->SeedBankList.back()->pft()
//          <<" at "<<cell->x<<";"<<cell->y<<endl;

}//end distribute seed rain
void CWaterGridEnvir::InitWaterInds(SPftTraits* traits,SclonalTraits* cltraits,
   SWaterTraits* wtraits,const int n,double mass)
{
//   using CEnvir::nrand;using SRunPara::RunPara;
   int x,y;//teste mal..
   x=12;y=13;
   int SideCells=SRunPara::RunPara.CellNum;

   for (int i=0; i<n; ++i){
        x=nrand(SideCells);
        y=nrand(SideCells);

        CCell* cell = CellList[x*SideCells+y];
        //set Plant
        CWaterPlant* plant= new CWaterPlant(traits,cltraits,wtraits,cell,mass/2.0,mass/2.0);
        PlantList.push_back(plant);

        //set Genet
        CGenet *Genet= new CGenet();
        GenetList.push_back(Genet);
        plant->setGenet(Genet);

//      cout<<"disp "<<plant->pft()
//          <<" at "<<cell->x<<";"<<cell->y<<endl;
   }

}
//------------------------------------------------------------------------------
/**
 Runs once for the Water Grid Environment. After an init phase of x years
 environmental conditions (WaterLevel) is changed.

 Migration, additional Winter mortality and salt-toxidity are implemented.
\todo add additional Winter mortality and salt-toxidity

 To allow a type to reestablish, once a year one Ind of each type is addad to
 the grid (individual drift / migration).
 \sa SRunPara::RunPara.WaterLevel
*/
void CWaterGridEnvir::OneRun(){
 //  int year_of_change=50;
   double WLstart=SRunPara::RunPara.WaterLevel;
   //run simulation until YearsMax
//   for (year=1; year<=5; ++year){
//   for (year=1; year<=SRunPara::RunPara.Tmax; ++year){
   while(year<SRunPara::RunPara.Tmax){
      this->NewWeek();
//      cout<<" y"<<year;

  //drift of little individuals -anually-
  if (SRunPara::RunPara.Migration>0){
    typedef map<string, int> mapType;

 //      for_each(PftInitList.begin(),PftInitList.end(),InitWaterSeeds);     //funkt nicht

      //streue für jeden Typ einen Samen aufs Grid
       for (std::map<const string,long>::iterator it = PftInitList.begin();
            it != PftInitList.end(); ++it)
      {
//cout<<"Migration: "<<SRunPara::RunPara.Migration<<" seeds of "<< it->first<<endl;
        InitSeeds(it->first,
          SRunPara::RunPara.Migration,
          this->getPftLink(it->first)->pEstab );
      }

}//if migration
//--------------------
      OneYear();
//--------------------
//-apply salt toxidicy effect


//aprupt climate change (WL)
//      if (year==year_of_change) SRunPara::RunPara.WaterLevel+=SRunPara::RunPara.changeVal;

//file output
      if (true)//(year>=20)
      {
        WriteGridComplete(false);//report last year
        WriteSurvival();
 //       WriteClonalOutput();
      }
   //save grid after init time
      if (false){//(year==20) {
        stringstream v; v<<"B"<<CEnvir::SimNr<<setw(2)<<setfill('0')<<CEnvir::RunNr;
        this->Save(v.str());
      }
   //if all done
      if (endofrun)break;
   }//years
//   //save grid at end of run
//   stringstream v; v<<"B"<<CEnvir::SimNr<<setw(3)<<setfill('0')<<CEnvir::RunNr<<"E"<<CEnvir::SimNr;
//   this->Save(v.str());
//WL zurücksetzen
//    if (year>=year_of_change)SRunPara::RunPara.WaterLevel-=SRunPara::RunPara.changeVal;//5cm weniger für nächste Sim
     SRunPara::RunPara.WaterLevel=WLstart;
}  // end OneSim
//------------------------------------------------------------------------------
/**
winter dieback and additional mortality due to prolonged inundation
as external disurbance

interpretes winterInundation as disturbance signal
**/
void CWaterGridEnvir::Winter(){
  //calculate additional mortality due to prolonged inundation (ditch breaching)
  //effected types: all non adapted to water and the other to smaller extend
   //mass removal and mortality in case of Disturbance
   for (int i=0; i<PlantList.size(); ++i){
      ((CWaterPlant*)PlantList[i])->winterDisturbance(winterInundation);
   }
  CGrid::Winter(); //remove dead plants and calc winter dieback
}//end winter() - winter mortality
//------------------------------------------------------------------------------
/**
    \param file file name of source file
*/
void CWaterGridEnvir::getEnvirCond(string file){
  weeklyWL.clear();weeklySAL.clear();weeklySAT.clear();
   ifstream EnvFile;
   //open parameter file
     EnvFile.open(file.c_str());
//   ifstream PftFile(SRunPara::RunPara.PftFile);
   if (!EnvFile.good()) {std::cerr<<("Fehler beim Öffnen EnvFile");exit(3); }
   string line1;
   getline(EnvFile,line1); //ignore header
   //*******************
    //loop for all weeks
   for (int lweek=0; lweek<30; ++lweek){
      int wl,sal,sat;
      //read plant parameter from inputfile
      EnvFile>>wl>>sat>>sal;
      weeklyWL.push_back(wl);
      weeklySAL.push_back(sal);
      weeklySAT.push_back(sat);
   }
  EnvFile>>winterInundation;//get winter inundation weeks in last line
}
double CWaterGridEnvir::getSAL(){
  return weeklySAL.at(week-1);
}//<get current salinity
double CWaterGridEnvir::getSAT(){
  return weeklySAT.at(week-1);
}//<get current soil saturation
double CWaterGridEnvir::getWL(){
  return weeklyWL.at(week-1);
}//<get current water level

/**
  \warning this work only with 30 weeks a year
*/
void CWaterGridEnvir::genAutokorrWL(double hurst)
{
  weeklyWL.clear();weeklyWL.assign(33,0);
  double mean=SRunPara::RunPara.WaterLevel;
  double sigma=SRunPara::RunPara.WLsigma;//5;
  int D=32,N=32, d=D/2;
  //first values
  weeklyWL.at(0)=CEnvir::rand01()*2*sigma+(mean-sigma);
  weeklyWL.at(32)=CEnvir::rand01()*2*sigma+(mean-sigma);
  double delta=sigma;
  //generate between
  for (int step=1;step<=5;step++){
    delta*= pow(0.5,0.5*hurst);
    for(int x=d;x<=N-d;x+=D){
    	weeklyWL.at(x)=(weeklyWL.at(x-d)+weeklyWL.at(x+d))/2.0
                  +delta*(2.0*rand01()-1);
    }
    D/=2;d/=2;
  }
}
/**
  \warning this work only with 30 weeks a year
*/
void CWaterGridEnvir::genSeasonWL()
{
  weeklyWL.clear();weeklyWL.assign(30,0);
  double mean=SRunPara::RunPara.WaterLevel;
  double sigma=SRunPara::RunPara.WLsigma; //5;//
  for (unsigned int i=0; i<30; i++)
    weeklyWL[i]=mean+sigma*cos(i/2.0/Pi);
}
/**
  \warning this work only with 30 weeks a year
*/
void CWaterGridEnvir::genConstWL()
{
//   weeklyWL.clear();weeklyWL.assign(30,0);
   weeklyWL.clear();weeklyWL.assign(30,0);
   weeklySAL.clear();weeklySAL.assign(30,0);
   weeklySAT.clear();weeklySAT.assign(30,0);

  for (unsigned int i=0; i<30; i++) {
	  weeklyWL[i]=SRunPara::RunPara.WaterLevel;
	  weeklySAL[i]=SRunPara::RunPara.salt;
//	  weeklySAT[i]=SRunPara::RunPara.sat;
  }
}


/**
  weekly change grid water level
  \author KK
  \date 11/10/10
*/
void CWaterGridEnvir::SetCellResource(){
  CGrid::SetCellResource();
  //  ChangeMeanWaterLevel(5);
  if (week==1)//generate new year's WaterLevels
  {
    if(SRunPara::RunPara.WLseason=="random")
    // generate autocorrelated Wl-series
    genAutokorrWL(0.1);
    else if(SRunPara::RunPara.WLseason=="season")
    // generate seasonal Wl-series
    genSeasonWL();
    else if(SRunPara::RunPara.WLseason=="file")
    // generate seasonal Wl-series
    getEnvirCond((string)"Input\\env_con.txt");
    else
//if(SRunPara::RunPara.WLseason=="const")
    // generate const Wl-series
    genConstWL(); //default
  }
//  this->SetMeanWaterLevel(SRunPara::RunPara.WaterLevel);
  this->SetMeanWaterLevel(getWL());

  //salinity
//  salinity=SRunPara::SRunPara::RunPara.salt;
//  if (week==1) cout<<"\n";
//  cout<<"\n w"<<week<<" WL:" <<getWL();
//  cout              <<" Sat:"<<getSAT();
//  cout              <<" Sal:"<<getSAL();
}

//-------------------------------------------------------------
CclonalPlant* CWaterGridEnvir::newSpacer(int x,int y, CclonalPlant* plant){
     return new CWaterPlant(x,y,(CWaterPlant*)plant);
}
//-------------------------------------------------------------
int CWaterGridEnvir::exitConditions()
{
     int currTime=1;//GetT();
//    if no more individuals existing
//     if (this->PlantList.size()==0)

//   seed input will probably let PFTs reestablish
     if (false)//(this->PlantList.size()==0)
     {
        endofrun=true;
        cout<<"no more inds";
        return currTime; //extinction time
     }
//     cout<<"\n  << "<<this->PlantList.size()<<"plants left";
     return 0;
}//end CClonalGridEnvir::exitConditions()
//---------------------------------------------------------
void CWaterGridEnvir::GetOutput(){
   CClonalGridEnvir::GetOutput();
   this->GridOutData.back()->WaterLevel=this->GetMeanWaterLevel(); //oder:
//   this->GridOutData.back()->WaterLevel=this->weeklyWL[week-1];

}    //run in 20th week of year
//---------------------------------------------------------
/**
   anually output of type-status

   two types and bare ground
   \author KK
   \date 11/10/25
*/
  void CWaterGridEnvir::WriteWaterOutput(){


  }   // end write results

//---------------------------------------------------------
/**
  CWaterGridEnvir version of this function. Only CWaterPlant and
  \ref CWaterSeed "-Seed" types are used in the COMTESS Model

  A new Plant and a new Genet are defined and appended to the grid-wide lists
  (PlantList and GenetList).

*/
void CWaterGridEnvir::EstabLott_help(CSeed* seed){
//  cout<<"estabLott_help - CWaterGridEnvir";
  CWaterPlant* plant= new CWaterPlant((CWaterSeed*)seed);
  CGenet *Genet= new CGenet();
  GenetList.push_back(Genet);
  plant->setGenet(Genet);
  PlantList.push_back(plant);
}
void CWaterGridEnvir::DispSeeds_help(CPlant* plant,CCell* cell)
{
  new CWaterSeed((CWaterPlant*) plant,cell);
}    //

//---------------------------------------------------------------------------
/**
Each plant's ressource uptake is corrected for it's water conditions.
Ressources are lost, if current water conditions doesn't suit the plant's
requirements.
\author KK
\date 11/10/10
*/
void CWaterGridEnvir::DistribResource(){
   //collect resouces according to local competition
   CGrid::DistribResource();
   //for each cell: coorect for water conditions
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant)
      if (!(*iplant)->dead)((CWaterPlant*) (*iplant))->DistrRes_help();

   Resshare();  // resource sharing betwen connected ramets
}
//---------------------------------------------------------------------------
/**
  \todo make fkt constant
  \return mean current water level of the grid
*/
double CWaterGridEnvir::GetMeanWaterLevel(){
   double sum_Level=0.0;
   //get cell number
   long ncells= SRunPara::RunPara.GetSumCells();
   for (int i=0; i<ncells; ++i){
      CWaterCell* cell = CellList[i];
   //sum all waterlevel values
      sum_Level+=cell->GetWaterLevel();
   }//end for
   // return mean
   return sum_Level/ncells;
}//end GetMeanWaterLevel
//---------------------------------------------------------------------------

/**
   Changes all WaterLevel values relatively to the current value.
   Differences absolutely stay constant.

   \param val  water level change (in cm)
*/
void CWaterGridEnvir::ChangeMeanWaterLevel(double val=0){
   //for each cell: add change
   long ncells= SRunPara::RunPara.GetSumCells();
   for (int i=0; i<ncells; ++i){
      CWaterCell* cell = CellList[i];
      cell->SetWaterLevel(cell->GetWaterLevel()+val);
   }//end for
}//end changeMeanWaterLevel
//---------------------------------------------------------------------------
/**
   Lowers or sinks all WaterLevel values at the same time.
   Differences absolutely stay constant.

   \param val new mean water level
*/
void CWaterGridEnvir::SetMeanWaterLevel(double val=0){
   //get current mean value
   double curr_mLevel=GetMeanWaterLevel();
   //get difference
   double diff=val-curr_mLevel;
   //for each cell: add difference
   ChangeMeanWaterLevel(diff);
   //   long ncells= SRunPara::RunPara.GetSumCells();
//   for (int i=0; i<ncells; ++i){
//      CWaterCell* cell = CellList[i];
//      cell->SetWaterLevel(cell->GetWaterLevel()+diff);
//   }//end for
}//end SetMeanWaterLevel

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
std::vector<SWaterTraits*> SWaterTraits::PFTWaterList;//(8,new SclonalTraits());


SWaterTraits::SWaterTraits():name("default"),
  assimBelWL(false),assimAnoxWL(0),saltTol(0)
{}//end constructor

/**
Translates Ellenberg Value saltTol to respiratory costs
for an adaptation to salt.

\return fraction of uptake as costs for  adaptation. \[0 - 1\]

\todo validate rule/values
\date 13.3. set costs to 5/10% for med/well adapted spec (20/40% before)
*/
double SWaterTraits::saltTolCosts(){
  if (saltTol<2) return 1;
  if (saltTol<=5) return 0.95;
  return 0.9;
} // salt tolerance costs
/**
Translates Ellenberg Value saltTol to tolerance level of salt content.

\return fraction of uptake as loss due to salinity. \[1e-1 - 1\]

\todo validate rule/values
*/
double SWaterTraits::saltTolEffect(double salinity){
 double min_lim=0.1;
  if (saltTol<2)  {if (salinity<1.0) return 1.0;else return min_lim;}
  if (saltTol<=5) {if(salinity<7.0) return 1.0;else return min_lim;}
  if (saltTol>5)  {return 1.0;}
  return min_lim;  //should not be reached
} // salt tolerance effect

//---------------------------------------------------------------------------
void SWaterTraits::ReadWaterStrategy(char* file)
{
  string name;SWaterTraits* temp=new SWaterTraits;

      //read plant parameter from here
      name="swamp";
//      temp->WL_Optimum=30;temp->WL_Tolerance= 30;//15;//30;
      temp->assimAnoxWL=0.3;
      temp->name=name;
      PFTWaterList.push_back(temp);

      temp=new SWaterTraits;
      name="moist";
//      temp->WL_Optimum=-10;temp->WL_Tolerance= 30;
      temp->assimAnoxWL=0.0;
      temp->name=name;
      PFTWaterList.push_back(temp);

      temp=new SWaterTraits;
      name="dry";
//      temp->WL_Optimum=-50;temp->WL_Tolerance= 15;
      temp->assimAnoxWL=0.0;
      temp->name=name;
      PFTWaterList.push_back(temp);

}//end ReadWaterStrategy(
//------------------------------------------------------------------------------
/**
  Initiate new Run: reset grid and randomly set initial individuals.
*/
void CWaterGridEnvir::InitRun(){
  CEnvir::InitRun();
  resetGrid();

  //set initial plants on grid...
  InitInds("Input\\RSpec59WP3_131114.txt"); //all species simultanously
//  InitInds("Input\\RSpec28S.txt"); //all species simultanously
//  InitInds("Input\\RSpec20.txt",SimNr);

}
/**
  this function reads a file, introduces PFTs and initializes seeds on grid
  after file data.

  (each PFT on file gets 10 seeds randomly set on grid)
  \since 2012-04-18

  \param file name of init file
  \param n initiate only n'th species for monoculture exps
*/
void CWaterGridEnvir::InitInds(string file,int n){
  const int no_init_seeds=10;//10;
  //Open InitFile,
  ifstream InitFile(file.c_str());
  if (!InitFile.good()) {cerr<<("Fehler beim Öffnen InitFile");exit(3); }
//  cout<<"InitFile: "<<file<<endl;
  string line;
  getline(InitFile,line);//skip header line
  //skip first lines if only one Types should be initiated
  if (n>-1) for (int x=0;x<n;x++)getline(InitFile,line);
  int dummi1; string dummi2; int PFTtype; string Cltype;
  do{
  //erstelle neue traits
    SPftTraits* traits=new SPftTraits();
    SclonalTraits* cltraits=new SclonalTraits();
    SWaterTraits* wtraits=new SWaterTraits();

// file structure
// [1] "ID"      "Species" "alSeed"   "LMR"     "maxMass" "mSeed" "Dist"
// [8] "pEstab"  "Gmax"    "SLA1"     "palat"   "memo"    "RAR"   "respAnox"
//[15] "PropSex" "meanSpacerLength" "Resshare" "mSpacer"
    //get type definitions from file
    InitFile>> dummi1;
    InitFile>>dummi2;
    InitFile>> traits->AllocSeed
            >> traits->LMR
            >> traits->MaxMass
            >> traits->m0 //mSeed
            >> traits->Dist
            >> traits->pEstab
            >> traits->Gmax
            >> traits->SLA
            >> traits->palat
            >> traits->memory
            >> traits->RAR
            >> wtraits->assimAnoxWL
            >> cltraits->PropSex
            >> cltraits->meanSpacerlength
            >> cltraits->Resshare
            >> cltraits->mSpacer
            >> cltraits->clonal
            >> wtraits->saltTol;
     traits->SeedMass=traits->m0;
//     if (traits->AllocSeed>0.1)traits->MaxAge=2; //Bienne
     cltraits->sdSpacerlength=cltraits->meanSpacerlength;
     //\todo test reduced RAR (moderate root efficiency)
      traits->RAR=0.5;

    //namen und IDs
    traits->name=cltraits->name=wtraits->name=dummi2;
    traits->TypeID=dummi1;
    //in Listen einfügen..
    addPftLink(dummi2,traits);
    addClLink(dummi2,cltraits);
    addWLink(dummi2,wtraits);
    SPftTraits::PftList.push_back(traits);
    SclonalTraits::clonalTraits.push_back(cltraits);
    SWaterTraits::PFTWaterList.push_back(wtraits);
    //print imported trait combination
//    traits->print();cltraits->print();wtraits->print();

    // initialization
    InitWaterInds(traits,cltraits,wtraits,no_init_seeds,traits->MaxMass/10.0); // /2.0 com out
//    InitWaterSeeds(traits,cltraits,wtraits,no_init_seeds);

    PftInitList[traits->name]+=no_init_seeds;
//    cout<<" init "<<no_init_seeds<<" seeds of Pft: "<<dummi2<<endl;

    if (n>-1) SRunPara::RunPara.species=dummi2;
    if(!InitFile.good()||n>-1) {
      SRunPara::RunPara.NPft=PftInitList.size(); return;}

  }while(!InitFile.eof());
}//initialization based on file

//---------------------------------------------------------------------------
void SWaterTraits::print(){
   std::cout<<"\nWater Type: "<<this->name;
   std::cout<<"\n  assimBelWL: "<<this->assimBelWL;
   std::cout<<"\n  assimAnoxWL: "<<this->assimAnoxWL;
   std::cout<<"\n  saltTol: "<<this->saltTol
   <<endl;
} //print water traits
//---------------------------------------------------------------------------

//#pragma package(smart_init)
