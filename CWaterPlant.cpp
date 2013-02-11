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
double oldmass=this->GetMass();
 //standard growth
 CPlant::Grow2();
 //detailed output - for grid plots
// if (this->genetID%%4==1)   //nur jeder 4. Genet
// if (CEnvir::week==21) //Samen fertig, aber noch nicht released
// if (CEnvir::week==2||CEnvir::week==22||CEnvir::week==29)
//enable again for more detailed spatial information
//if (false)
if (CEnvir::week==20&&CEnvir::year==SRunPara::RunPara.Tmax)
//if (CEnvir::year<3)
//if (true)
{
 string filename=CEnvir::NameLogFile;
 CEnvir::AddLogEntry(CEnvir::SimNr,filename);
 CEnvir::AddLogEntry(CEnvir::RunNr,filename);
 CEnvir::AddLogEntry(CEnvir::year,filename);
 CEnvir::AddLogEntry(CEnvir::week,filename);
 CEnvir::AddLogEntry(xcoord,filename);
 CEnvir::AddLogEntry(ycoord,filename);
 CEnvir::AddLogEntry(GetMass(),filename);    //biomass
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
/**
    root growth

    dm/dt = growth*(c*m^p - m^q / m_max^r)

    with respect of rooting depth (Gmax now represents resources per 50cm rooting depth)

    \since 27/04/2012
*/
double CWaterPlant::RootGrow(double rres){
   double Assim_root, Resp_root;
   double p_depth= this->getDepth();
   double p=2.0/3.0, q=2.0, r=4.0/3.0; //exponents for growth function
   Assim_root=Traits->growth*min(rres,Traits->Gmax*p_depth/50.0*Art_disc);    //growth limited by maximal resource per area -> similar to uptake limitation
   Resp_root=Traits->growth*Traits->Gmax*p_depth/50.0*Traits->RAR
            *pow(mroot,q)/pow(Traits->MaxMass*0.5,r);  //respiration proportional to root^2

   return max(0.0,Assim_root-Resp_root);
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
 if(this->waterTraits->assimAnoxWL>0.0)
   return min(1.0,this->waterTraits->assimAnoxWL);  //0.5
 double retval=  max(min(depth,-wl)/depth,1e-10); //0.0

//salt stress
 retval*=this->waterTraits->saltTolEffect(CWaterGridEnvir::salinity)
     - this->waterTraits->saltTolCosts();
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

#pragma package(smart_init)
