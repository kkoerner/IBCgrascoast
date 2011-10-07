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
  //reed
  SPftTraits*    traits  =SPftTraits::PftList[18];
  SclonalTraits* cltraits=SclonalTraits::clonalTraits[2];
  SWaterTraits*  wtraits =SWaterTraits::PFTWaterList[0];
  PftInitList["reed"]+=no_init_seeds;
  addPftLink("reed",traits);//?noch mal genauer gucken
  addClLink("reed",cltraits);
  WLinkList["reed"]=wtraits;
  InitWaterSeeds(traits,cltraits,wtraits,no_init_seeds);


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
//-------------------------------------------------------------
int CWaterGridEnvir::exitConditions()
{
     int currTime=GetT();
//    if no more individuals existing
     if (this->GenetList.size()==0)
     {
        endofrun=true;
        cout<<"no more inds";
        return currTime; //extinction time
     }
     return 0;
}//end CClonalGridEnvir::exitConditions()
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
  WL_Optimun(0),WL_Tolerance(0)
{}//end constructor

//---------------------------------------------------------------------------
void SWaterTraits::ReadWaterStrategy(char* file)
{
  string name;SWaterTraits* temp=new SWaterTraits;

      //read plant parameter from here
      name="swamp";
      temp->WL_Optimun=-30;temp->WL_Tolerance= 15;
      temp->name=name;
      PFTWaterList.push_back(temp);

      temp=new SWaterTraits;
      name="wet";
      temp->WL_Optimun=10;temp->WL_Tolerance= 15;
      temp->name=name;
      PFTWaterList.push_back(temp);

      temp=new SWaterTraits;
      name="moist";
      temp->WL_Optimun=30;temp->WL_Tolerance= 15;
      temp->name=name;
      PFTWaterList.push_back(temp);

      temp=new SWaterTraits;
      name="dry";
      temp->WL_Optimun=50;temp->WL_Tolerance= 15;
      temp->name=name;
      PFTWaterList.push_back(temp);

}//end ReadWaterStrategy(

//---------------------------------------------------------------------------

#pragma package(smart_init)
