//---------------------------------------------------------------------------


//#pragma hdrstop

#include "CclonalSeed.h"
#include "clonalPlant.h"
//---------------------------------------------------------------------------

//#pragma package(smart_init)
/*
CclonalSeed::CclonalSeed(double x, double y, int ID, double m, double estab,
  int maxage, CDTclonalTraits clonalTraits):CSeed(x,y,ID,m,estab,maxage)
{
   this->clonalTraits=clonalTraits;
}
*/
//---------------------------------------------------------------------------
CclonalSeed::CclonalSeed(CclonalSeed& seed)
:CSeed(seed),clonalTraits(seed.clonalTraits)
{}
//---------------------------------------------------------------------------
CclonalSeed::CclonalSeed(double x, double y, double estab,
  SPftTraits* traits, SclonalTraits* clonalTraits)
  :CSeed(x,y,estab,traits),clonalTraits(clonalTraits)
{}
//---------------------------------------------------------------------------
CclonalSeed::CclonalSeed(double x, double y, CclonalPlant* plant):
  CSeed(x,y,(CPlant*)plant),clonalTraits(plant->clonalTraits)
{}
//---------------------------------------------------------------------------
CclonalSeed::CclonalSeed(double estab,
  SPftTraits* traits,  SclonalTraits* clonalTraits, CCell* cell)
  :CSeed(estab,traits,cell),clonalTraits(clonalTraits)
{}
//---------------------------------------------------------------------------
CclonalSeed::CclonalSeed(CclonalPlant* plant,CCell* cell):
  CSeed((CPlant*)plant,cell),clonalTraits(plant->clonalTraits)
{}
//---------------------------------------------------------------------------
string CclonalSeed::type()
{
        return "CclonalSeed";
}
string CclonalSeed::pft(){
        string dummi=CSeed::pft() +  this->clonalTraits->name;
//        std::cout<<dummi;
        return dummi;
}   //say what a pft you are

//---------------------------------------------------------------------------
