/** \file
    \brief functions of class CclonalPlant
*/
//---------------------------------------------------------------------------
//#pragma hdrstop
#include <iostream>

#include "clonalPlant.h"
#include "environment.h"
//---------------------------------------------------------------------------
//#pragma package(smart_init)
//extern void Boundary(int&, int&);
//---------------------------------------------------------------------------
/**
  The new Plant inherits its parameters from 'plant'.
  Genet is the same as for plant, Generation is by one larger than
  that of plant.

  \note For clonal growth:
  cell has to be set and plant has to be added to genet list
  when ramet establishes.

  \since revision
*/
CclonalPlant::CclonalPlant(double x, double y, CclonalPlant* plant)
  :CPlant(x,y,plant->Traits)
{
   this->clonalTraits=plant->clonalTraits;
   growingSpacerList.clear();
   mReproRamets=0;

   Spacerlength=0;      //real spacer length
   Spacerdirection=0;    //spacer direction
   SpacerlengthToGrow=0;
  this->genet=plant->genet;
  this->Generation=plant->Generation+1;
}//<clonal growth constructor
//---------------------------------------------------------------------------
/**
  not used
*/
CclonalPlant::CclonalPlant(SPftTraits* PlantTraits,
  SclonalTraits* clonalTraits, CCell* cell)
  :CPlant(PlantTraits,cell),genet(NULL)//keine vordefinierten Eigenschaften
{
   this->clonalTraits=clonalTraits;
   growingSpacerList.clear();
   mReproRamets=0;

   Spacerlength=0;      //real spacer length
   Spacerdirection=0;    //spacer direction
   Generation=1;
   SpacerlengthToGrow=0;
}
//---------------------------------------------------------------------------
/**
  not used
*/
CclonalPlant::CclonalPlant(double x, double y,
  SPftTraits* PlantTraits, SclonalTraits* clonalTraits)
  :CPlant(x,y,PlantTraits),genet(NULL)//keine vordefinierten Eigenschaften
{
   this->clonalTraits=clonalTraits;
   growingSpacerList.clear();
   mReproRamets=0;

   Spacerlength=0;      //real spacer length
   Spacerdirection=0;    //spacer direction
   Generation=1;
   SpacerlengthToGrow=0;
}
//---------------------------------------------------------------------------
/**
  If a seed germinates, the new plant inherits its parameters.
  Genet has to be defined externally.

  \since revision
*/
CclonalPlant::CclonalPlant(CclonalSeed* seed):CPlant((CSeed*)seed){
   clonalTraits=seed->clonalTraits;
   growingSpacerList.clear();
   mReproRamets=0;

   Spacerlength=0;      //real spacer length
   Spacerdirection=0;    //spacer direction
   SpacerlengthToGrow=0;
   Generation=1;
   genet=NULL;
}
//---------------------------------------------------------------------------
///set genet and add ramet to its list
void CclonalPlant::setGenet(CGenet* genet){
  if (this->genet==NULL){
    this->genet=genet;
    this->genet->AllRametList.push_back(this);
  }
}//end setGenet
//---------------------------------------------------------------------------
///destructor for clonal plants
///
CclonalPlant::~CclonalPlant()
{
    for (unsigned int i=0;i<growingSpacerList.size();i++)
      delete growingSpacerList[i];
    growingSpacerList.clear();
//    cout<<'~'<<type();
}
//---------------------------------------------------------------------------
string CclonalPlant::type()
{
        return "CclonalPlant";
}
string CclonalPlant::pft(){
        string dummi=CPlant::pft() + this->clonalTraits->name;
//       cout<<dummi;
        return dummi;
}   //say what a pft you are

//---------------------------------------------------------------------------
/**
*/
double CclonalPlant::ReproGrow(double uptake){
   double SeedRes,VegRes,dm_seeds;
   //fixed Proportion of resource to seed production
   if (mRepro<=Traits->AllocSeed*mshoot)//calculate mRepro for every week
   {
      SeedRes =uptake*Traits->AllocSeed;
      VegRes  =uptake-SeedRes;
      //      VegRes  =uptake*(1-Traits->AllocSeed);
       //reproductive growth
      dm_seeds=max(0.0,Traits->growth*SeedRes);
      // distrubution of mRepro
      int pweek=CEnvir::week;
      if ((pweek>=Traits->FlowerWeek) && (pweek<Traits->DispWeek))
      {//during the seed-production-weeks
         //if non-clonal plant in fruiting season
         if (!this->clonalTraits->clonal){mRepro+=dm_seeds; return VegRes;}
         mRepro+=(dm_seeds*this->clonalTraits->PropSex);
         mReproRamets+=(dm_seeds*(1-this->clonalTraits->PropSex));
      }else
      { //during the other weeks (without seed production)
          //if non-clonal plant out of the fruiting season
         if (!this->clonalTraits->clonal){return uptake;}
        mReproRamets+=dm_seeds;
      }
   }else VegRes=uptake;
//   cout<<"\tVeg "<<VegRes<<"\tSpacer "<<mReproRamets<<"\tSeed"<<mRepro;
   return VegRes;
}//end reprogrow
//-----------------------------------------------------------------------------
/**
returns the number of new spacer to set: currently
 - 1 if there are clonal-growth-resources and spacer-lisdt is empty, and
 - 0 otherwise

Unlike CPlant::GetNSeeds() no resources are reset due to ongoing growth
*/
int CclonalPlant::GetNRamets()
{
//   int NRamets=0;
//   if (!dead){
      if ((mReproRamets>0)
         &&(!dead)
         &&(growingSpacerList.size()==0))
//      {
         return 1;
//      }
//   }
   return 0;
}
//-----------------------------------------------------------------------------
double CclonalPlant::GetBMSpacer(){
   int SpacerListSize=this->growingSpacerList.size();
  if (SpacerListSize==0)return 0.0;
  double mSpacer=0;
  for (int g=0; g<(SpacerListSize); g++)
    {  //loop for all growing Spacer of one plant
       CclonalPlant* Spacer = this->growingSpacerList[g];
       mSpacer+=(Spacer->Spacerlength-Spacer->SpacerlengthToGrow)* clonalTraits->mSpacer;
  }
  return this->mReproRamets+ mSpacer;
}
void CclonalPlant::SpacerGrow()
{
   double mGrowSpacer=0;
   int SpacerListSize=this->growingSpacerList.size();

   if (SpacerListSize==0)return;
   if ((mReproRamets>0))
   {
      mGrowSpacer=(mReproRamets/SpacerListSize);//resources for one spacer

      for (int g=0; g<(SpacerListSize); g++)
      {  //loop for all growing Spacer of one plant
         CclonalPlant* Spacer = this->growingSpacerList[g];

         double lengthtogrow=Spacer->SpacerlengthToGrow;
         lengthtogrow-=(mGrowSpacer/clonalTraits->mSpacer); //spacer growth
         Spacer->SpacerlengthToGrow=max(0.0,lengthtogrow);
//cout<<"\n <<Spacer: "<<Spacer->SpacerlengthToGrow<<"cm left";
         //Estab for all growing Spacers in the last week of the year
         if ((CEnvir::week==CEnvir::WeeksPerYear)
           && (Spacer->SpacerlengthToGrow>0))
         {
             double direction=Spacer->Spacerdirection;
             double complDist=Spacer->Spacerlength;//should be positive
             double dist=(complDist-Spacer->SpacerlengthToGrow);
             double CmToCell=1.0/SRunPara::RunPara.CellScale();
             int x2=CEnvir::Round(this->cell->x+cos(direction)*dist*CmToCell);
             int y2=CEnvir::Round(this->cell->y+sin(direction)*dist*CmToCell);

             /// \todo change boundary conditions
             //Boundary conditions (types MUST match exactly)
             Boundary(x2,y2);
//             CGrid::Boundary(x2,y2);

             //beachte dass CellNum nicht immer == CellSize
             Spacer->xcoord=x2/CmToCell;
             Spacer->ycoord=y2/CmToCell;
             Spacer->SpacerlengthToGrow=0;
         }  //end if pweek==WeeksPerYear
      }   //end List of Spacers
   }
   mReproRamets=0;
} //end SpacerGrow
//-eof---------------------------------------------------------------------------

