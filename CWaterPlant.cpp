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
/**
initialization of adult plants
\param mass initial biomass of Plant (above- + belowground)
*/
CWaterPlant::CWaterPlant(double mass, SPftTraits* traits,
      SclonalTraits* clonalTraits,
      SWaterTraits* waterTraits, CCell* cell):
      CclonalPlant(traits,clonalTraits,cell),waterTraits(waterTraits)
{
  this->mshoot=mass/2.0;
  this->mroot=mass/2.0;
}
//---------------------------------------------------------------------------
/**
Overload CPlant::Grow2() for additional Effect of WaterLevel.
 *aboveground no ressources are allocated if plant is below water level.
 (algorithm is analog to \ref CGrid::Cutting() 'cutting biomass')
 *belowground ressources are lost relative to plant's properties.
 \author KK
\date 11/10/10
\todo check cheight-value [10mg per 1cm plant height]
*/
void CWaterPlant::Grow2()
{

 //standard growth
 CPlant::Grow2();
 //detailed output - for grid plots
if (CEnvir::week==2||CEnvir::week==20||CEnvir::week==25)
{
 string filename=CEnvir::NameLogFile;
 CEnvir::AddLogEntry(CEnvir::SimNr,filename);
 CEnvir::AddLogEntry(CEnvir::RunNr,filename);
 CEnvir::AddLogEntry(CEnvir::year,filename);
 CEnvir::AddLogEntry(CEnvir::week,filename);
 CEnvir::AddLogEntry(xcoord,filename);
 CEnvir::AddLogEntry(ycoord,filename);
 CEnvir::AddLogEntry(GetMass(),filename);
 CEnvir::AddLogEntry(this->getHeight(),filename);
 CEnvir::AddLogEntry(this->growingSpacerList.size(),filename);
 CEnvir::AddLogEntry(this->getGenet()->number,filename);
 CEnvir::AddLogEntry(" ",filename);
 CEnvir::AddLogEntry(this->pft(),filename);

// CEnvir::AddLogEntry(height,"C-reed-out.txt");
 CEnvir::AddLogEntry("\n",filename);
}
}//end Grow2
/**
Helping function for CWaterGridEnvir::DistribRessource

Corrects plant's resource uptake for current Water conditions.
\not not stochasticity
\author KK
\date 11/10/11
*/
void CWaterPlant::DistrRes_help(){
 //add water-effect
 double wl= ((CWaterCell*) cell)->GetWaterLevel(); ///<plant's water level

 //.. aboveground
 if (!this->waterTraits->assimBelWL){
 // submersed plant parts dont assimilate
    this->Auptake*=min(1.0,max(0.0,1-(wl )/getHeight()));
 }

 //.. belowground
 double diff=wl-this->waterTraits->WL_Optimum;
 double sigma=this->waterTraits->WL_Tolerance;
 //Ressourcennutzung nach Normalverteilung
// double cfac=CEnvir::RandNumGen.normal(diff,sigma);
// double cfac=exp(-0.5*(diff/sigma)*(diff/sigma));
// double factor= min(1.0,max(0.0,CEnvir::RandNumGen.normal(diff,sigma)));
// double factor= min(1.0,max(0.0,cfac));
 this->Buptake*=min(1.0,max(0.0,exp(-0.5*(diff/sigma)*(diff/sigma))));

}
/**
  CWater - version of competion strength of a plant.
  Belowground strength varies with distance to WL-Optimum of plant type.

  \todo this function line3 to comment out for (not)using rule 3
*/
double CWaterPlant::comp_coef(const int layer, const int symmetry)const{
  bool rule3=false;  //true
  double cplantval=CPlant::comp_coef(layer,symmetry);
  if (!rule3 || layer==1) //comment out to disable changed competitive power
    return cplantval;
  //korrekturwert durch WaterLevel
  else {   //comment out here too
    double wl= ((CWaterCell*) cell)->GetWaterLevel(); ///<plant's water level
    double diff=wl-this->waterTraits->WL_Optimum;
    double sigma=this->waterTraits->WL_Tolerance;
    return cplantval* min(1.0,max(0.0,exp(-0.5*(diff/sigma)*(diff/sigma))));
  }        //comment out here too
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
