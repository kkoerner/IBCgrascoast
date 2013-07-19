//---------------------------------------------------------------------------
/**\file implement functions related to water ressources*/


//#pragma hdrstop

#include "CWaterPlant.h"
#include "CWaterGridEnvir.h"
#include <sstream>
//---------------------------------------------------------------------------
string CWaterPlant::type()
{
        return "CWaterPlant";
}
///
/// \return name of current pft
///
string CWaterPlant::pft(){
//        string dummi=CclonalPlant::pft() + this->waterTraits->name;
        string dummi=this->waterTraits->name;
//       cout<<dummi;
        return dummi;
}   //say what a pft you are

/// print traits
void CWaterPlant::print_type(){
   cout<<"\nCoordinates "<<xcoord<<";"<<ycoord<<endl;
   this->Traits->print();
   this->clonalTraits->print();
   this->waterTraits->print();
}
//---------------------------------------------------------------------------
/***/
CWaterPlant::CWaterPlant(CWaterSeed* seed)
  :CclonalPlant(seed),
  waterTraits(seed->waterTraits)
{
//this->print_type();
//cout<<"\n"<<this->pft()<<" Coords: "<<xcoord<<";"<<ycoord<<endl;
}
//---------------------------------------------------------------------------
/***/
CWaterPlant::CWaterPlant(double x, double y, CWaterPlant* plant)
  :CclonalPlant(x,y,plant),
  waterTraits(plant->waterTraits)
{
//cout<<"\nnew Ramet "<<this->pft()<<": "<<x<<";"<<y<<endl;
}
//---------------------------------------------------------------------------
/**
initialization of adult plants
\param mass initial biomass of Plant (above- + belowground)
*/
CWaterPlant::CWaterPlant(SPftTraits* traits,
      SclonalTraits* clonalTraits,
      SWaterTraits* waterTraits, CCell* cell,
     double mshoot,double mroot,double mrepro,
     int stress,bool dead,int generation,int genetnb,
     double spacerl,double spacerl2grow):
      CclonalPlant(traits,clonalTraits,cell,mshoot,mroot,mrepro,stress,dead,generation, genetnb,
      spacerl,spacerl2grow),waterTraits(waterTraits)
{
}
/**
Copies the data from an existing CclonalPlant and appends water related traits.
\note the original clonal plant is still existing.
*/
CWaterPlant::CWaterPlant(CclonalPlant* clplant, SWaterTraits* waterTraits):
      CclonalPlant(clplant->Traits,clplant->clonalTraits,clplant->getCell(),
      clplant->mshoot,clplant->mroot,clplant->mRepro,clplant->stress,
      clplant->dead,clplant->Generation, //clplant->genetnb,
      clplant->Spacerlength,clplant->SpacerlengthToGrow),waterTraits(waterTraits)
{
this->setGenet(clplant->getGenet());
}

//--SAVE-----------------------------------------------------------------------
/**
  CWaterPlant-Version of plant report

  -no additional state variables
  \autor KK
  \date 130214
*/
string CWaterPlant::asString(){
  std::stringstream dummi;
  // CclonalPlant part
  dummi<<CclonalPlant::asString();
  return dummi.str();
} //<report plant's status
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
//double oldmass=this->GetMass();
 //standard growth
 CPlant::Grow2();
 //detailed output - for grid plots
// if (this->genetID%%4==1)   //nur jeder 4. Genet
// if (CEnvir::week==21) //Samen fertig, aber noch nicht released
// if (CEnvir::week==2||CEnvir::week==22||CEnvir::week==29)
//enable again for more detailed spatial information
//if (false)
//if (CEnvir::week==20&&CEnvir::year==SRunPara::RunPara.Tmax)
//if (CEnvir::year==21&CEnvir::week<10)
if (true)
{
 string filename=CEnvir::NameLogFile;
 CEnvir::AddLogEntry(CEnvir::SimNr,filename);
 CEnvir::AddLogEntry(CEnvir::RunNr,filename);
 CEnvir::AddLogEntry(CEnvir::year,filename);
 CEnvir::AddLogEntry(CEnvir::week,filename);
 CEnvir::AddLogEntry(CWaterGridEnvir::getSAL(),filename);
 CEnvir::AddLogEntry(xcoord,filename);
 CEnvir::AddLogEntry(ycoord,filename);
 CEnvir::AddLogEntry(CPlant::GetMass(),filename);    //biomass
 CEnvir::AddLogEntry(this->GetBMSpacer(),filename);    //biomass
// CEnvir::AddLogEntry(GetMass()-oldmass,filename);
 CEnvir::AddLogEntry(this->mRepro,filename); //growing seed mass

  CEnvir::AddLogEntry(this->stress,filename);
// CEnvir::AddLogEntry(this->Area_shoot(),filename);
 CEnvir::AddLogEntry(this->getHeight(),filename);
// CEnvir::AddLogEntry(this->getDepth(),filename);
 if (this->growingSpacerList.size()>0)
 CEnvir::AddLogEntry(this->growingSpacerList.front()->Spacerlength
 - this->growingSpacerList.front()->SpacerlengthToGrow ,filename);
 else
 CEnvir::AddLogEntry(0.0,filename);
 CEnvir::AddLogEntry(this->getGenet()->number,filename);
 string dummi=" "+this->pft()+"\n";
 CEnvir::AddLogEntry(dummi,filename);
 //CEnvir::AddLogEntry(" ",filename);
 //CEnvir::AddLogEntry(this->pft(),filename);

// CEnvir::AddLogEntry(height,"C-reed-out.txt");
 //CEnvir::AddLogEntry("\n",filename);
}
}//end Grow2
double CWaterPlant::RootCosts(){
  double p_depth= this->getDepth();
  double p=2.0/3.0, q=2.0, r=4.0/3.0; //exponents for growth function
  return    Traits->growth*Traits->Gmax*p_depth/50.0*Traits->RAR
          *pow(mroot,q)/pow(Traits->MaxMass*0.5,r);  //respiration proportional to root^2
}

/**
    root growth

    dm/dt = growth*(c*m^p - m^q / m_max^r)

    with respect of rooting depth (Gmax now represents resources per 50cm rooting depth)

    \since 27/04/2012

    \since 05/03/2013 NEW:dieback of roots due to salinity; negative output possible
*/
double CWaterPlant::RootGrow(double rres){
   double Assim_root;
   double p_depth= this->getDepth();
   double p=2.0/3.0, q=2.0, r=4.0/3.0; //exponents for growth function
   Assim_root=Traits->growth*min(rres,Traits->Gmax*p_depth/50.0*Art_disc);    //growth limited by maximal resource per area -> similar to uptake limitation
   double grow=Assim_root;
   //salinity dieback
   if (this->waterTraits->saltTolEffect(CWaterGridEnvir::getSAL())<1.0)
     grow -=0.1*this->mroot;
   return grow;
}

//-------------------------------------------------------------

/**
Get root efficiency. 'Normal' plants are not able to uptake
resources in the anoxic root layer. Here the proportion of roots
in the oxic root zone is returned.

Adapted plants are not restricted to WaterLevel depth, but have to pay
with restricted general uptake rate.

\note returns 1e-10 minimal (to exclude 'devide by zero')

Reduce Plant's root efficiency in case of salt stress.
\author KK
\date 13/02/07


*/
double CWaterPlant::rootEfficiency(){
 double wl= ((CWaterCell*) cell)->GetWaterLevel(); ///<plant's water level
 double depth=this->getDepth();
 //a) logistic formula
// ...
 //b) Wenn-Dann
 double retval=1.0, WLcost= this->waterTraits->assimAnoxWL;
 //oxygen deficit
 //costs..
 if(this->waterTraits->assimAnoxWL>0.0)
   retval= min(1.0,WLcost);  //0.5
 //effect..
 else retval=  max(min(depth,-wl)/depth,1e-10); //0.0

//salt stress
 retval*=this->waterTraits->saltTolEffect(CWaterGridEnvir::getSAL())
     * this->waterTraits->saltTolCosts();
 return retval;
}
//-------------------------------------------------------------
/**
calc effect of winter disturbance on plant
depending on water-related traits

\verbatim
 mort=min(0.95, max(0,MonthsInundation-Thresh_by_adapt )/4)
 Thresh_by_adapt(no,yes)=(2,8)
\endverbatim
no additional dieback at the moment
*/
void CWaterPlant::winterDisturbance(int weeks_of_dist){
  double mortality=0;
  int aThresh=2; if (this->waterTraits->assimAnoxWL>0) aThresh=8;
  if (!dead){
    mortality=min(0.95, max(0,weeks_of_dist-aThresh)/4.0);
    if (CEnvir::rand01()<mortality) dead=true;
  }
} //end CWaterPlant::winterDisturbance
//-------------------------------------------------------------
/**
Helping function for CWaterGridEnvir::DistribRessource

Corrects plant's resource uptake for current Water conditions.
\note no stochasticity
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
// double diff=wl-this->waterTraits->WL_Optimum;
// double sigma=this->waterTraits->WL_Tolerance;
 //Ressourcennutzung nach Normalverteilung
// this->Buptake*=min(1.0,max(0.0,exp(-0.5*(diff/sigma)*(diff/sigma))));
// schon in CWaterCell::BelowComp() enthalten ...
// this->Buptake*=rootEfficiency();

}
/* *
  CWater - version of competion strength of a plant.
  Belowground strength varies with distance to WL-Optimum of plant type.
  \bugnot sure for returning zero
  \todo this function line3 to comment out for (not)using rule 3
* /
double CWaterPlant::comp_coef(const int layer, const int symmetry)const{
  bool rule3=true;  //true
  double cplantval=CPlant::comp_coef(layer,symmetry);
  if (!rule3 || layer==1) //comment out to disable changed competitive power
    return cplantval;
  //korrekturwert durch WaterLevel
  else {   //comment out here too
//    double wl= ((CWaterCell*) cell)->GetWaterLevel(); ///<plant's water level
//    double diff=wl-this->waterTraits->WL_Optimum;
//    double sigma=this->waterTraits->WL_Tolerance;
//    return cplantval* min(1.0,max(0.0,exp(-0.5*(diff/sigma)*(diff/sigma))));
    return cplantval* rootEfficiency();
  }        //comment out here too
}
*/
//---------------------------------------------------------------------------
/**
  Reimplemented from CCell::Germinate(). Prior to germination a seed mortality due to
  salinity is calculated. Survival responds to saltTolEffect() of seed's PFT.
*/
double CWaterCell::Germinate(){
   //seed mortality due to salinity
   unsigned int sbsize=SeedBankList.size();
   for (unsigned int i =0; i<sbsize;i++)
   {
     CSeed* seed = SeedBankList[i];// *iter;
     if (CEnvir::rand01()>
       ((CWaterSeed*)seed)->waterTraits->saltTolEffect(CWaterGridEnvir::getSAL()))
       seed->remove=true;
   }
   this->RemoveSeeds();
   return CCell::Germinate();
}//end Germinate
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
///
/// \return current pft name
///
string CWaterSeed::pft(){
//        string dummi=CclonalSeed::pft() + this->waterTraits->name;
        string dummi=this->waterTraits->name;
//       cout<<dummi;
        return dummi;
}   //say what a pft you are
//---------------------------------------------------------------------------

//#pragma package(smart_init)
