//---------------------------------------------------------------------------
/**\file implement functions related to water ressources*/


#pragma hdrstop

#include "CWaterPlant.h"
#include "CWaterGridEnvir.h"

//---------------------------------------------------------------------------
string CWaterPlant::type()
{
        return "CWaterPlant";
}
string CWaterPlant::pft(){
        string dummi=CclonalPlant::pft() + this->waterTraits->name;
//       cout<<dummi;
        return dummi;
}   //say what a pft you are
//---------------------------------------------------------------------------
/***/
CWaterPlant::CWaterPlant(CWaterSeed* seed)
  :CclonalPlant(seed),
  waterTraits(seed->waterTraits)
{
}
//---------------------------------------------------------------------------
/***/
CWaterPlant::CWaterPlant(double x, double y, CWaterPlant* plant)
  :CclonalPlant(x,y,plant),
  waterTraits(plant->waterTraits)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/***/
CWaterSeed::CWaterSeed(double estab, SPftTraits* traits,
      SclonalTraits* clonalTraits,
      SWaterTraits* waterTraits, CCell* cell)
  :CclonalSeed(estab,traits,clonalTraits,cell),
  waterTraits(waterTraits)
{}
//---------------------------------------------------------------------------
/***/
CWaterSeed::CWaterSeed(CWaterPlant* plant,CCell* cell)
  :CclonalSeed(plant,cell),
  waterTraits(plant->waterTraits)
{}
//---------------------------------------------------------------------------
string CWaterSeed::type()
{
        return "CWaterSeed";
}
string CWaterSeed::pft(){
        string dummi=CclonalSeed::pft() + this->waterTraits->name;
//       cout<<dummi;
        return dummi;
}   //say what a pft you are
//---------------------------------------------------------------------------

#pragma package(smart_init)
