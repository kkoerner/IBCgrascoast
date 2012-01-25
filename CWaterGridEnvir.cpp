//---------------------------------------------------------------------------


#pragma hdrstop

#include "CWaterGridEnvir.h"
#include "CWaterPlant.h"
   map<string,SWaterTraits*> CWaterGridEnvir::WLinkList=
     map<string,SWaterTraits*>();

//---------------------------------------------------------------------------
void CWaterGridEnvir::CellsInit(){
}//end CellsInit
//---------------------------------------------------------------------------

CWaterGridEnvir::~CWaterGridEnvir(){
//  for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
//      CWaterCell* cell = CellList[i];
//      delete cell;
//   }
//   delete[] CellList;

}
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
/**
 randomly distribute seeds of a given plant type (CWaterPlant)

 \param traits   link to basic PFT
 \param cltraits link to clonal traits
 \param wtraits  link to water traits
 \param n        number of seeds to disperse
 \param estab    establishment (default is 1 for initial conditions)
*/
void CWaterGridEnvir::InitWaterSeeds(SPftTraits* traits,SclonalTraits* cltraits,
     SWaterTraits* wtraits, const int n,double estab)
{
   using CEnvir::nrand;using SRunPara::RunPara;
   int x,y;
   int SideCells=RunPara.CellNum;

   for (int i=0; i<n; ++i){
        x=nrand(SideCells);
        y=nrand(SideCells);

        CCell* cell = CellList[x*SideCells+y];
        new CWaterSeed(estab,traits,cltraits,wtraits,cell);
      cout<<"disp "<<cell->SeedBankList.back()->pft()
          <<" at "<<cell->x<<";"<<cell->y<<endl;
   }
}//end distribute seed rain
void CWaterGridEnvir::InitWaterInds(SPftTraits* traits,SclonalTraits* cltraits,
   SWaterTraits* wtraits,const int n,double mass)
{
   using CEnvir::nrand;using SRunPara::RunPara;
   int x,y;
   int SideCells=RunPara.CellNum;

   for (int i=0; i<n; ++i){
        x=nrand(SideCells);
        y=nrand(SideCells);

        CCell* cell = CellList[x*SideCells+y];
        //set Plant
        CWaterPlant* plant= new CWaterPlant(mass,traits,cltraits,wtraits,cell);
        PlantList.push_back(plant);

        //set Genet
        CGenet *Genet= new CGenet();
        GenetList.push_back(Genet);
        plant->setGenet(Genet);

      cout<<"disp "<<plant->pft()
          <<" at "<<cell->x<<";"<<cell->y<<endl;
   }

}
//------------------------------------------------------------------------------
/**
 Runs once for the Water Grid Environment. After an init phase of 10 years
 environmental conditions (WaterLevel) is changed.
 To allow a type to reestablish, once a year one Ind of each type is addad to
 the grid (individual drift / migration).
 \sa SRunPara::RunPara.WaterLevel
*/
void CWaterGridEnvir::OneRun(){
//   double teval=0.2;  //fraction of Tmax that is used for evaluation
   //get initial conditions
   init=1; //for init the second plant (for the invasion experiments)
   int year_of_change=20;
   double WLstart=SRunPara::RunPara.WaterLevel;
   //run simulation until YearsMax
   for (year=1; year<=SRunPara::RunPara.Tmax; ++year){
      cout<<" y"<<year;

//drift of little individuals -anually-
if (SRunPara::RunPara.Migration){
//  if(SRunPara::RunPara.species=="R"||SRunPara::RunPara.species=="M") //true
//    InitWaterSeeds(
    InitWaterInds(
      SPftTraits::PftList[15],SclonalTraits::clonalTraits[6],
      SWaterTraits::PFTWaterList[0],1,8000);
//  if(SRunPara::RunPara.species=="G"||SRunPara::RunPara.species=="M") //true
    InitWaterInds(
      SPftTraits::PftList[42],SclonalTraits::clonalTraits[0],
      SWaterTraits::PFTWaterList[2],1,200);
    InitWaterInds(
      SPftTraits::PftList[71],SclonalTraits::clonalTraits[0],
      SWaterTraits::PFTWaterList[2],1,40);

}//if migration
//--------------------
      OneYear();
//--------------------
//aprupt climate change (WL)
      if (year==year_of_change) SRunPara::RunPara.WaterLevel+=SRunPara::RunPara.changeVal;

//file output
      if (true)//(year>=20)
      {
        WriteGridComplete(false);//report last year
        WriteSurvival();
      }
      if (endofrun)break;
   }//years
//WL zur�cksetzen
//    if (year>=year_of_change)SRunPara::RunPara.WaterLevel-=SRunPara::RunPara.changeVal;//5cm weniger f�r n�chste Sim
     SRunPara::RunPara.WaterLevel=WLstart;
}  // end OneSim
//------------------------------------------------------------------------------
/**
  \warning this work only with 30 weeks a year
*/
void CWaterGridEnvir::genAutokorrWL(double hurst)
{
  double mean=SRunPara::RunPara.WaterLevel;
  double sigma=SRunPara::RunPara.WLsigma;//5;
  int D=32,N=32, d=D/2;
  //first values
  weeklyWL[0]=CEnvir::rand01()*2*sigma+(mean-sigma);
  weeklyWL[32]=CEnvir::rand01()*2*sigma+(mean-sigma);
  double delta=sigma;
  //generate between
  for (int step=1;step<=5;step++){
    delta*= pow(0.5,0.5*hurst);
    for(int x=d;x<=N-d;x+=D){
      weeklyWL[x]=(weeklyWL[x-d]+weeklyWL[x+d])/2.0
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
  for (unsigned int i=0; i<30; i++) weeklyWL[i]=SRunPara::RunPara.WaterLevel;
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
    else
//if(SRunPara::RunPara.WLseason=="const")
    // generate const Wl-series
    genConstWL(); //default
  }
//  this->SetMeanWaterLevel(SRunPara::RunPara.WaterLevel);
  this->SetMeanWaterLevel(weeklyWL[week-1]);
  if (week==1)cout<<"\n";
  cout<<"\n w"<<week<<" "<<weeklyWL[week-1];
}

//-------------------------------------------------------------
CclonalPlant* CWaterGridEnvir::newSpacer(int x,int y, CclonalPlant* plant){
     return new CWaterPlant(x,y,(CWaterPlant*)plant);
}
//-------------------------------------------------------------
int CWaterGridEnvir::exitConditions()
{
     int currTime=GetT();
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

  \todo define a NULL-clone type
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
  WL_Optimum(0),WL_Tolerance(0),assimBelWL(false),assimAnoxWL(0)
{}//end constructor

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

//---------------------------------------------------------------------------

#pragma package(smart_init)
