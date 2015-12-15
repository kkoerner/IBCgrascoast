/**
 * CWaterCell.cpp
 *
 *  \date 27.01.2015
 *  \author KatrinK
 */
//---------------------------------------------------------------------------
#include "CWaterPlant.h"
#include "CWaterGridEnvir.h"
namespace ibc_coast{
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
   double comp_tot=0, comp_c=0, max_depth_eff=0, max_saltTol_eff=0;
//   cout<<"Cell "<<this->x<<":"<<this->y<<" - "<<BelowPlantList.size()<<" Inds\n"<<flush;

   //1. sum of resource requirement
   for (plant_iter iter=BelowPlantList.begin(); iter!=BelowPlantList.end(); ++iter){
      CWaterPlant* plant= (CWaterPlant*)*iter;
 //cout<<plant->type()<<endl;
      double dTre=plant->getDepth() * plant->rootEfficiency();
      double dTse=((SWaterTraits*) plant->Traits)->saltTolEffect(CWaterGridEnvir::getSAL());
      comp_tot+=plant->comp_coef(2,symm)
               *dTre*dTse
               *prop_res(plant->pft(),2,SRunPara::RunPara.Version);
       max_depth_eff=max( max_depth_eff,dTre);
       max_saltTol_eff=max( max_saltTol_eff,dTse);
   }
   //2. distribute resources
   for (plant_iter iter=BelowPlantList.begin(); iter!=BelowPlantList.end(); ++iter){
	      CWaterPlant* plant= (CWaterPlant*)*iter;
	 //cout<<plant->type()<<endl;
	  double dTre=plant->getDepth()* plant->rootEfficiency();
	  double dTse=((SWaterTraits*) plant->Traits)->saltTolEffect(CWaterGridEnvir::getSAL());
      comp_c=plant->comp_coef(2,symm)
              *dTre*dTse
              *prop_res(plant->pft(),2,SRunPara::RunPara.Version);

      plant->Buptake+=BResConc/50.0 * max_depth_eff
              //correction by respiration costs for sal tolerance
    		  * max_saltTol_eff
              * comp_c /comp_tot;
   }
}//end below_comp
/**
report cell's content without plants

  - no abiotic data documented
  - no additional biotic data to document
\autor KK
\date 1209xx
*/
std::string CWaterCell::asString(){
return CCell::asString();
} //return content for file saving

/**
 * initiate cells on grid
 */
CWaterCell::CWaterCell():
		  CCell(0,0),WaterLevel(0) {
}

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
       ((SWaterTraits*)((CWaterSeed*)seed)->Traits)->saltTolEffect(CWaterGridEnvir::getSAL()))
       seed->remove=true;
   }
   this->RemoveSeeds();
   return CCell::Germinate();
}//end Germinate

}//namespace ibc_coast
//eof




