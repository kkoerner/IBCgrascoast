//---------------------------------------------------------------------------
#pragma package(smart_init)
#include <iostream>
#include <map>
#include <string>
//---------------------------------------------------------------------------
#include "Cell.h"
#include "GridBase.h"
#include "environment.h"
#pragma hdrstop

//-----------------------------------------------------------------------------
CCell::CCell(const unsigned int xx,const unsigned int yy){
  CCell(xx,yy,0,0);
  const unsigned int index=xx*SRunPara::RunPara.CellNum+yy;
  AResConc=CEnvir::AResMuster.at(index);
  BResConc=CEnvir::BResMuster[index];
}
//-----------------------------------------------------------------------------
CCell::CCell(const unsigned int xx,const unsigned int yy, double ares, double bres)
:x(xx),y(yy),AResConc(ares),BResConc(bres),NPftA(0),NPftB(0),
occupied(false),PlantInCell(NULL)
{
   AbovePlantList.clear();
   BelowPlantList.clear();
   SeedBankList.clear();
   SeedlingList.clear();

   using SRunPara::RunPara;
   PftNIndA.clear();
   PftNIndB.clear();
   PftNSeedling.clear();
}
//---------------------------------------------------------------------------
void CCell::clear(){
   AbovePlantList.clear();
   BelowPlantList.clear();
   for (unsigned int i=0; i<SeedBankList.size();i++) delete SeedBankList[i];
   for (unsigned int i=0; i<SeedlingList.size();i++) delete SeedlingList[i];
   SeedBankList.clear();   SeedlingList.clear();

   PftNIndA.clear();
   PftNIndB.clear();
   PftNSeedling.clear();
   NPftA=(0);NPftB=(0);
   occupied=(false);PlantInCell=(NULL);
}
//---------------------------------------------------------------------------
CCell::~CCell()
{
   for (unsigned int i=0; i<SeedBankList.size();i++) delete SeedBankList[i];
   for (unsigned int i=0; i<SeedlingList.size();i++) delete SeedlingList[i];
   SeedBankList.clear();   SeedlingList.clear();
   AbovePlantList.clear();
   BelowPlantList.clear();
   PftNSeedling.clear();
}
//---------------------------------------------------------------------------
///not used
///
void CCell::SetResource(double Ares, double Bres)
{
   double SideLength=SRunPara::RunPara.CellScale();
   AResConc=Ares*(SideLength*SideLength);       //resource units per cell
   BResConc=Bres*(SideLength*SideLength);
}//end setResource
//---------------------------------------------------------------------------
double CCell::Germinate()
{
   //double rnum;
   double sumseedmass=0;
   unsigned int sbsize=SeedBankList.size();
   //Germination
   for (unsigned int i =0; i<sbsize;i++)
   {
      CSeed* seed = SeedBankList[i];// *iter;
      //rnum=CEnvir::rand01();
      if (CEnvir::rand01()< seed->estab)
      {
         //make a copy in seedling list
         SeedlingList.push_back(seed);//new CSeed(*seed));
         int dummi=seed->Traits->TypeID-1;
         PftNSeedling[seed->pft()]++;
         seed->remove=true;
         sumseedmass+=seed->mass;
      }
   }
   //remove germinated seeds from SeedBankList
   seed_iter iter_rem = partition(SeedBankList.begin(),SeedBankList.end(),GetSeedRemove);
   SeedBankList.erase(iter_rem,SeedBankList.end());

   return sumseedmass;
}//end Germinate()
//---------------------------------------------------------------------------
void CCell::RemoveSeedlings()
{
   PftNIndA.clear();
   PftNIndB.clear();
   PftNSeedling.clear();

   for (seed_iter iter=SeedlingList.begin(); iter!=SeedlingList.end();++iter){
      CSeed* seed = *iter;
      delete seed;
   }
   SeedlingList.clear();
}//end removeSeedlings
//---------------------------------------------------------------------------
void CCell::RemoveSeeds()
{
   seed_iter irem = partition(SeedBankList.begin(),SeedBankList.end(),GetSeedRemove);

   for (seed_iter iter=irem; iter!=SeedBankList.end(); ++iter){
      CSeed* seed = *iter;
      delete seed;
   }
   SeedBankList.erase(irem,SeedBankList.end());
}//end removeSeeds

//---------------------------------------------------------------------------
/**
  Updates Numbers of Plant functional traits on grid
  (refreshes NPftA and NPftB)

  \todo adapt for map  -done
*/
void CCell::GetNPft()
{
   NPftA=PftNIndA.size();
   NPftB=PftNIndB.size();
}
//-----------------------------------------------------------------------------
/**
  Returns the cell-cover of the plant type given (only aboveground).
  To get bare ground area, type has to be 'bare'.

  \param type name of PFT to look for

  \note achtung: arbeitet nur correkt, wenn die Typen pro zelle
  gleichberechtigt sind
  \todo finde die höchste Pflanze in der AbovePlantList und nehme deren Typ

  \warning zeitraubend!!
*/
double CCell::getCover(const string type) const{
//wenn zelle besetzt
  if (occupied) {
    string ltype=PlantInCell->pft();
    if (ltype==type) return 1;else return 0;}
//wenn zelle leer
  if (AbovePlantList.empty()){
    if (type=="bare")return 1;else return 0;
//sonst gehe durch AboveGround-Liste
  }else{
    unsigned int noplants = AbovePlantList.size();
    unsigned int nonodead = noplants;
    unsigned int notype   = 0;
    for (unsigned int i=0;i<noplants;i++){
      //wenn  Pflanze tot, zähle sie nicht (oder als Streu)
      if (AbovePlantList.at(i)->dead) --nonodead;
      //sonst...
      if (AbovePlantList.at(i)->pft()==type) ++notype;
    }
    //...berechne Anteil
    if (nonodead>0)return notype/(double)nonodead;
    else if (type=="bare")return 1;else return 0;
  }

}
//-----------------------------------------------------------------------------
/**
  Returns cellstate. To adapt for individual use.
  Here Methods of Felix and Ines are indicated.

  \param layer one is for aboveground, two for belowground Layer

  \note has to be called before dead plants are deleted
      (access-violateion else because Cell's
       Above- and BelowPlantLists are used)
*/
int CCell::getCover(const int layer)const{
  if (occupied) return 99;
  if (layer==1){
    if (AbovePlantList.empty())return 0;
    if (AbovePlantList.back()->dead)return 98;
    //Felix...
    //return AbovePlantList.back()->Traits->TypeID;
    //Ines...
    string typ=AbovePlantList.back()->type();
    if(typ=="CPlant")return 102;
//    else return 101;
    //end Ines / revision variant:
    return (((CclonalPlant*)AbovePlantList.back())->getGenet()->number%20)*2+1;
  }else{ //if layer==2
    if (BelowPlantList.empty())return 0;
    if (BelowPlantList.back()->dead)return 98;
    //beide
    return BelowPlantList.back()->Traits->TypeID;
  }
//  return 9999;//default - should not be reached
}//get Acover und getBcover
//-----------------------------------------------------------------------------
/**
ABOVEground competition takes global information on symmetry and version to
distribute the cell's resources. Resource competition Version is 1.

virtual function will be substituted by comp function from sub class
*/
void CCell::AboveComp()
{
   if (AbovePlantList.empty()) return;
   if (SRunPara::RunPara.AboveCompMode==asymtot){
     //total asymmetry only for above plant competition
      sort(AbovePlantList.begin(),AbovePlantList.end(),CompareShoot);
      CPlant* plant=*AbovePlantList.begin(); //biggest plant
      plant->Auptake+=AResConc;
      return;
   }
   int symm=1; if (SRunPara::RunPara.AboveCompMode==asympart) symm=2;
   double comp_tot=0, comp_c=0;

   //1. sum of resource requirement
   for (plant_iter iter=AbovePlantList.begin(); iter!=AbovePlantList.end(); ++iter){
      CPlant* plant=*iter;
      comp_tot+=plant->comp_coef(1,symm)
               *prop_res(plant->pft(),1,SRunPara::RunPara.Version);
   }
   //2. distribute resources
   for (plant_iter iter=AbovePlantList.begin(); iter!=AbovePlantList.end(); ++iter){
      CPlant* plant=*iter;
      comp_c=plant->comp_coef(1,symm)
           *prop_res(plant->pft(),1,SRunPara::RunPara.Version);
      plant->Auptake+=AResConc*comp_c/comp_tot;
   }
}//end above_comp
//-----------------------------------------------------------------------------
/**
BELOWground competition takes global information on symmetry and version to
distribute the cell's resources.
 Resource competition Version is 1.

virtual function will be substituted by comp function from sub class
*/
void CCell::BelowComp()
{
   if (BelowPlantList.empty()) return;
   if (SRunPara::RunPara.BelowCompMode==asymtot){
     cerr<<"CCell::BelowComp() - "
         <<"no total asymetric belowground competition allowed"; exit(3);
   }
   int symm=1; if (SRunPara::RunPara.BelowCompMode==asympart) symm=2;
   double comp_tot=0, comp_c=0;

   //1. sum of resource requirement
   for (plant_iter iter=BelowPlantList.begin(); iter!=BelowPlantList.end(); ++iter){
      CPlant* plant=*iter;
      comp_tot+=plant->comp_coef(2,symm)
               *prop_res(plant->pft(),2,SRunPara::RunPara.Version);
   }
   //2. distribute resources
   for (plant_iter iter=BelowPlantList.begin(); iter!=BelowPlantList.end(); ++iter){
      CPlant* plant=*iter;
      comp_c=plant->comp_coef(2,symm)
              *prop_res(plant->pft(),2,SRunPara::RunPara.Version);
      plant->Buptake+=BResConc*comp_c/comp_tot;
   }
}//end below_comp
//---------------------------------------------------------------------------
/**
  \param type     Plant_functional_Type-ID
  \param layer    above(1)- or below(2)ground
  \param version  one of [0,1,2]
  \since revision
  \todo adapt for map-version
*/
double CCell::prop_res(const string type,const int layer,const int version)const{
   switch (version){
     case 0:  return 1;
     case 1:  if (layer==1){
         map<string,int>::const_iterator noa =PftNIndA.find(type);
         if (noa!=PftNIndA.end())
          return 1.0/sqrt(noa->second);
        }
              if (layer==2){
         map<string,int>::const_iterator nob =PftNIndB.find(type);
         if (nob!=PftNIndB.end())
          return 1.0/sqrt(nob->second);
        }
     case 2:  if (layer==1)return NPftA/(1.0+NPftA);
              if (layer==2)return NPftB/(1.0+NPftB);
     default: cerr<<"CCell::prop_res() - wrong input";exit(3);
   }
   return -1; //should not be reached
}//end CCell::prop_res
//---------------------------------------------------------------------------
void CCell::SortTypeID()
{
   sort(SeedlingList.begin(),SeedlingList.end(),CompareTypeID);
}
void CCell::print_map(map<string,int> &mymap){
   typedef map<string,int>::const_iterator CI;
   for (CI p=mymap.begin();p!=mymap.end();++p){
     cout<<p->first<<'\t' <<p->second<<endl;
   }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "CWaterPlant.h"

//! Konstruktor
CWaterCell::CWaterCell(const unsigned int xx,const unsigned int yy):
  CCell(xx,yy),WaterLevel(0){
  //std::cout<<"CWaterCell; ";
  }
//! Konstruktor
CWaterCell::CWaterCell(const unsigned int xx,const unsigned int yy,
  double ares,double bres):CCell(xx,yy,ares,bres),WaterLevel(0)
  {//cout<<"CWaterCell; ";
  }
//! Konstruktor
CWaterCell::CWaterCell(const unsigned int xx,const unsigned int yy,
  double ares,double bres, double wl):CCell(xx,yy,ares,bres),WaterLevel(wl)
  {//cout<<"CWaterCell; ";
  }
//-----------------------------------------------------------------------------
/**
BELOWground competition takes global information on symmetry and version to
distribute the cell's resources.
 Resource competition Version is 1.

virtual function will be substituted by comp function from sub class

soil resources are meant as res per 50cm rooting depth

\sa CWaterPlant::RootGrow
*/
void CWaterCell::BelowComp()
{
   if (BelowPlantList.empty()) return;
   if (SRunPara::RunPara.BelowCompMode==asymtot){
     cerr<<"CCell::BelowComp() - "
         <<"no total asymetric belowground competition allowed"; exit(3);
   }
   int symm=1; if (SRunPara::RunPara.BelowCompMode==asympart) symm=2;
   double comp_tot=0, comp_c=0, max_depth_eff=0;

   //1. sum of resource requirement
   for (plant_iter iter=BelowPlantList.begin(); iter!=BelowPlantList.end(); ++iter){
      CPlant* plant=*iter;
      comp_tot+=plant->comp_coef(2,symm)
               *plant->getDepth() * ((CWaterPlant*) plant)->rootEfficiency()
               *prop_res(plant->pft(),2,SRunPara::RunPara.Version);
       max_depth_eff=max( max_depth_eff,plant->getDepth()
                    * ((CWaterPlant*) plant)->rootEfficiency());
   }
   //2. distribute resources
   for (plant_iter iter=BelowPlantList.begin(); iter!=BelowPlantList.end(); ++iter){
      CPlant* plant=*iter;
      comp_c=plant->comp_coef(2,symm)
              *plant->getDepth() * ((CWaterPlant*) plant)->rootEfficiency()
              *prop_res(plant->pft(),2,SRunPara::RunPara.Version);
      plant->Buptake+=BResConc/50.0* max_depth_eff
                        * comp_c /comp_tot;
   }
}//end below_comp

//-eof---------------------------------------------------------------------






