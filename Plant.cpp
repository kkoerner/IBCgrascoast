//---------------------------------------------------------------------------
#pragma hdrstop
#include <iostream>

#include "Plant.h"
#include "environment.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
SPftTraits::SPftTraits():TypeID(999),name("default"),N0(1),MaxAge(100),
  AllocSeed(0.05),LMR(0),m0(0),MaxMass(0),SeedMass(0),Dist(0),
  pEstab(0.5),Gmax(0),memory(0),SLA(0),palat(0),RAR(1),growth(0.25),
  mThres(0.2),Dorm(1),FlowerWeek(16),DispWeek(20)
{}//end constructor
//---------------------------------------------------------------------------
///not used
///
void SPftTraits::SetDefault(){
        TypeID=1;name="PFT1";N0=20;
        MaxAge=100;
        AllocSeed=0.05;
        LMR=1;
        m0=1;MaxMass=5000;SeedMass=1;
        Dist=0.1;
        pEstab=0.5;
        Gmax=60;
        memory=2;
        SLA=1;
        palat=1;
        RAR=1;
        growth=0.25;
        mThres=0.2;
        Dorm=1;
        FlowerWeek=16;DispWeek=20;
}//end setDefault
//------------------------------------------------------------------------------
vector<SPftTraits*> SPftTraits::PftList;//(CRunPara::RunPara.NPft,new SPftTraits);
//int SPftTraits::NPft=81;
void SPftTraits::ReadPftStrategy(char* file)
{
//open parameter file
   string sfile=file;   ifstream PftFile;
   //open parameter file
   if (sfile=="") sfile=(SRunPara::RunPara.PftFile);
   PftFile.open(sfile.c_str());
//   ifstream PftFile(SRunPara::RunPara.PftFile);
   if (!PftFile.good()) {std::cerr<<("Fehler beim Öffnen PftFile");exit(3); }
   string line1;
   getline(PftFile,line1);
   //*******************

    //loop for all plant types
   for (int pft=0; pft<SRunPara::RunPara.NPft; ++pft){
      int dummi;SPftTraits* temp =new SPftTraits;
      //read plant parameter from inputfile
      PftFile>>temp->TypeID
             >>temp->name
             >>dummi//temp->yearstart
             >>temp->N0
             >>temp->MaxAge
             >>temp->AllocSeed
             >>temp->LMR
             >>temp->m0
             >>temp->MaxMass
             >>temp->SeedMass
             >>temp->Dist
             >>temp->pEstab
             >>temp->Gmax
             >>temp->memory
             >>temp->SLA
             >>temp->palat
             >>temp->RAR
             >>temp->growth
             >>temp->mThres
             >>temp->Dorm
             >>temp->FlowerWeek
             >>temp->DispWeek
             ;
     PftList.push_back(temp);
   }
   PftFile.close();
}//end ReadPftStrategy
//---------------------------------------------------------------------------
CPlant::CPlant(double x, double y, SPftTraits* Traits):
  xcoord(x),ycoord(y),Traits(Traits),mshoot(Traits->m0),mroot(Traits->m0),
  Aroots_all(0),Aroots_type(0),mRepro(0),Ash_disc(0),Art_disc(0),
  Auptake(0),Buptake(0),dead(false),remove(false),stress(0),cell(NULL)
{
//   mRepro=0;

// //   Allometrics();
//   Ash_disc=0;
//   Art_disc=0;
}
//-----------------------------------------------------------------------------
CPlant::CPlant(SPftTraits* Traits, CCell* cell):
  xcoord(0),ycoord(0),Traits(Traits),mshoot(Traits->m0),mroot(Traits->m0),
  Aroots_all(0),Aroots_type(0),mRepro(0),Ash_disc(0),Art_disc(0),
  Auptake(0),Buptake(0),dead(false),remove(false),stress(0),cell(NULL)
{
  setCell(cell);
  if (cell){
    xcoord=(cell->x*SRunPara::RunPara.CellScale());
    ycoord=(cell->y*SRunPara::RunPara.CellScale());
  }
//  mRepro=0;
// //  Allometrics();
//  Ash_disc=0;
//  Art_disc=0;
}
//-----------------------------------------------------------------------------
CPlant::CPlant(CSeed* seed):
  xcoord(seed->xcoord),ycoord(seed->ycoord),Traits(seed->Traits),
  mshoot(seed->Traits->m0),mroot(seed->Traits->m0),cell(NULL),
  Aroots_all(0),Aroots_type(0),mRepro(0),Ash_disc(0),Art_disc(0),
  Auptake(0),Buptake(0),dead(false),remove(false),stress(0)
{
   //establish this plant on cell
   setCell(seed->getCell());
   if (cell){
     xcoord=(cell->x*SRunPara::RunPara.CellScale());
     ycoord=(cell->y*SRunPara::RunPara.CellScale());
   }
//   mRepro=0;

// //   Allometrics();
//   Ash_disc=0;
//   Art_disc=0;
}
//-----------------------------------------------------------------------------
CPlant::~CPlant(){
//  cout<<'~'<<type();
}
//-----------------------------------------------------------------------------
void CPlant::setCell(CCell* cell){
   if (this->cell==NULL&&cell!=NULL){
     this->cell=cell;
     this->cell->occupied=true;
     this->cell->PlantInCell = this;
   }
}//end setCell
//-----------------------------------------------------------------------------
string CPlant::type(){
        return "CPlant";
}
string CPlant::pft(){
        return this->Traits->name;
}   //say what a pft you are

//---------------------------------------------------------------------------
///
/// \return uptake portion available for vegetative growth
///
double CPlant::ReproGrow(double uptake){
   double SeedRes,VegRes,dm_seeds;
   int pweek=CEnvir::week;
   //fixed Proportion of resource to seed production
   if ((pweek>=Traits->FlowerWeek) && (pweek<Traits->DispWeek) &&
       (mRepro<=Traits->AllocSeed*mshoot)){
      SeedRes =uptake*Traits->AllocSeed;
      VegRes  =uptake*(1-Traits->AllocSeed);
       //reproductive growth
      dm_seeds=max(0.0,Traits->growth*SeedRes);
      mRepro+=dm_seeds;
   }
   else VegRes=uptake;
   return VegRes;
}//end reprogrow
//-----------------------------------------------------------------------------
/**
  two-layer growth
  -# Resources for fecundity are allocated
  -# According to the resources allocated and the respiration needs
  shoot- and root-growth are calculated.
  -# Stress-value is in- or decreased according to the uptake

*/
void CPlant::Grow2()         //grow plant one timestep
{
   double dm_shoot, dm_root,alloc_shoot;
   double Assim_shoot, Resp_shoot, Assim_root, Resp_root;
   double LimRes, ShootRes, RootRes, VegRes;
   double p=2.0/3.0, q=2.0, r=4.0/3.0; //exponents for growth function

   /********************************************/
   /*  dm/dt = growth*(c*m^p - m^q / m_max^r)  */
   /********************************************/

   //which resource is limiting growth ?
   LimRes=min(Buptake,Auptake);   //two layers
   VegRes=ReproGrow(LimRes);

   //allocation to shoot and root growth
   alloc_shoot= Buptake/(Buptake+Auptake); //allocation coefficient

   ShootRes=alloc_shoot*VegRes;
   RootRes =VegRes-ShootRes;
   //   RootRes=(1-alloc_shoot)*VegRes;

   //Shoot growth
   Assim_shoot=Traits->growth*min(ShootRes,Traits->Gmax*Ash_disc);    //growth limited by maximal resource per area -> similar to uptake limitation
   Resp_shoot=Traits->growth*Traits->SLA
              *pow(Traits->LMR,p)*Traits->Gmax
              *pow(mshoot,q)/pow(Traits->MaxMass*0.5,r);       //respiration proportional to mshoot^2

   dm_shoot=max(0.0,Assim_shoot-Resp_shoot);

   //Root growth
   Assim_root=Traits->growth*min(RootRes,Traits->Gmax*Art_disc);    //growth limited by maximal resource per area -> similar to uptake limitation
   Resp_root=Traits->growth*Traits->Gmax*Traits->RAR
            *pow(mroot,q)/pow(Traits->MaxMass*0.5,r);  //respiration proportional to root^2

   dm_root=max(0.0,Assim_root-Resp_root);

//   if (dm_shoot<0) dm_shoot=0;  //no negative growth
//   if (dm_root<0)  dm_root=0;  //no negative growth

   mshoot+=dm_shoot;
   mroot+=dm_root;

   if ((Auptake<Traits->mThres*Ash_disc*Traits->Gmax*2)
       || (Buptake<Traits->mThres*Art_disc*Traits->Gmax*2))++stress;
   else if (stress>0) --stress;
   cout<<"\n"<<this->xcoord<<";"<<this->ycoord<<"\tBM "<<this->GetMass();
//   if (stress>Traits->memory) stress=Traits->memory;
//   Auptake=0;
//   Buptake=0;
}

//-----------------------------------------------------------------------------
void CPlant::Kill()
{
//   double pmort;//,rnumber;

   //resource deficiency mortality  ; pmin->random background mortality
   const double pmin=0.007;
   double pmort= (double)stress/Traits->memory  + pmin;  //stress mortality + random background mortality
//   rnumber = CEnvir::rand01();//(double )rand()/(RAND_MAX+1);
   if (CEnvir::rand01()<pmort) dead=true;
}
//-----------------------------------------------------------------------------
void CPlant::DecomposeDead()
{
   const double minmass=10; // mass at which dead plants are removed
   const double rate=0.5;

   if (dead)
   {
      mRepro=0;
      mshoot*=rate;
      mroot*=rate;
      if (GetMass() < minmass) remove=true;
   }
}//end DecomposeDead
//-----------------------------------------------------------------------------
/**
  If the plant is alive and it is dispersal time, the function returns
  the number of seeds produced during the last weeks.
  Subsequently the allocated resources are reset to zero.
*/
int CPlant::GetNSeeds()
{
   int NSeeds=0;
   double prop_seed=1.0;  //proportion of reproductive biomass that are seeds

   if (!dead){
      if ((mRepro>0)&&(CEnvir::week>Traits->DispWeek)){
         NSeeds=floor(mRepro*prop_seed/Traits->SeedMass);
         mRepro=0;
      }
   }
   return NSeeds;
}

//-----------------------------------------------------------------------------
/**
  Remove half shoot mass and seed mass from a plant.

  \return mass that was removed
*/
double CPlant::RemoveMass()
{
   double mass_removed=0;
   const double prop_remove=0.5; //proportion of mass removed

   if (mshoot+mRepro>1){   //only remove mass if shoot mas > 1mg
      mass_removed=prop_remove*mshoot+mRepro;
      mshoot*=1-prop_remove;
      mRepro=0;
   }
   return mass_removed;
}//end removeMass
//-----------------------------------------------------------------------------
/**
  Remove root mass from a plant.
  \param prop_remove   proportion of mass to be removed
  \return mass that was removed
  \since belowground herbivory simulations
*/
double CPlant::RemoveRootMass(const double prop_remove){
   double mass_removed=0;
   if (mroot>1){   //only remove mass if root mass > 1mg
      mass_removed=prop_remove*mroot;
      mroot-=mass_removed;//*=1-prop_remove;
   }
   return mass_removed;
}//end RemoveRootMass
//-----------------------------------------------------------------------------
void CPlant::WinterLoss()
{
   double prop_remove=0.5;
   mshoot*=1-prop_remove;
   mRepro=0;
}//end WinterLoss
//-----------------------------------------------------------------------------
//double CPlant::GetMass()
//{
//   return mshoot+mroot+mRepro;
//}
//-----------------------------------------------------------------------------
/*
void CPlant::Decompose()
{
   double rate=0.5;

    mRepro=0;
    mshoot=rate*mshoot;
    mroot=rate*mroot;
}
*/
//-----------------------------------------------------------------------------
double CPlant::Radius_shoot(){
   return sqrt(Traits->SLA*pow(Traits->LMR*mshoot,2.0/3.0)/Pi);
}
//-----------------------------------------------------------------------------
double CPlant::Radius_root(){
   return sqrt(Traits->RAR*pow(mroot,2.0/3.0)/Pi);
}
//-----------------------------------------------------------------------------
double CPlant::Area_shoot(){
  return Traits->SLA*pow(Traits->LMR*mshoot,2.0/3.0);
}
//-----------------------------------------------------------------------------
double CPlant::Area_root(){
  return Traits->RAR*pow(mroot,2.0/3.0);
}
//-----------------------------------------------------------------------------
/*
void CPlant::Allometrics()
{
   double p=2.0/3.0;

   double Ashoot=Traits->SLA*pow(Traits->LMR*mshoot,p);
   double Aroot=Traits->RAR*pow(mroot,p);

   rsh=sqrt(Ashoot/Pi);
   rrt=sqrt(Aroot/Pi);
}
*/
//-----------------------------------------------------------------------------
/**
  \since revision
*/
double CPlant::comp_coef(const int layer, const int symmetry)const{
   switch (symmetry){
     case 1: if (layer==1) return Traits->Gmax;//CompPowerA();
             if (layer==2) return Traits->Gmax;//CompPowerB();
     case 2: if (layer==1) return mshoot*Traits->CompPowerA();
             if (layer==2) return mroot *Traits->CompPowerB();
     default: cerr<<"CPlant::comp_coef() - wrong input"; exit(3);
   }
   return -1;  //should not be reached
}//end comp_coef
//-eof----------------------------------------------------------------------------



