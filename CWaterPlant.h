//---------------------------------------------------------------------------
/**\file classes and functions related to water ressources*/
#ifndef CWaterPlantH
#define CWaterPlantH
//class CclonalPlant; class CclonalSeed;
#include "clonalPlant.h"
#include "CclonalSeed.h"
class SWaterTraits; class CWaterSeed;
/** \brief water dependent plants

 \author KK
 \date 2011/10/6 COMTESS version
*/
class CWaterPlant:public CclonalPlant
{
//protected:
  virtual void Grow2();
//     ///competition coefficient for a plant -needed for AboveComp and BelowComp
//   virtual double comp_coef(const int layer,const int symmetry)const;
   virtual double RootGrow(double rres);

public:
   SWaterTraits* waterTraits;                ///<traits of water preferences
   //Constructors
   ///make a plant from a seed object
   CWaterPlant(CWaterSeed* seed);
   ///clonal growth
   CWaterPlant(double x, double y, CWaterPlant* plant);
   /// general constructor
//   CWaterPlant(double mass, SPftTraits* traits,
//      SclonalTraits* clonalTraits,
//      SWaterTraits* waterTraits, CCell* cell);
   CWaterPlant(SPftTraits* PlantTraits, SclonalTraits* clonalTraits,SWaterTraits* waterTraits,CCell* cell,
     double mshoot=0,double mroot=0,double mrepro=0,
     int stress=0,bool dead=false,int generation=1,int genetnb=0,
     double spacerl=0,double spacerl2grow=0);
     ///upgrade clonal plant
   CWaterPlant(CclonalPlant* clplant, SWaterTraits* waterTraits);
   virtual ~CWaterPlant(){};  //!<destructor
   void print_type();
   virtual string asString(); ///<report plant's status

//   virtual bool stressed();///< return true if plant is stressed
/// \brief lower threshold of belowground resource uptake (nutrient stress thresh.)
///    with respect of rooting depth (Gmax now represents optimal
///    resource uptake per 50cm rooting depth)
///
   virtual double minresB(){return CPlant::minresB()*this->getDepth()/50.0;}


///add water impact on ressource allocation
  virtual void DistrRes_help();
  ///corrective value for root uptake and competition
  double rootEfficiency();
  ///calc effect of winter disturbance on plant
  void winterDisturbance(int weeks_of_dist);
  
   virtual string type();  ///<say what you are
   virtual string pft();   ///<say what a pft you are

};
//---------------------------------------------------------------------------
/**\brief water seeds inherit from CclonalSeed

   \author KK
   \since 2011/10/06 COMTESS version
*/
class CWaterSeed: public CclonalSeed
{
public:
SWaterTraits* waterTraits; ///<clonal traits
//Konstruktoren

///for initialzation
CWaterSeed(double estab, SPftTraits* traits,
      SclonalTraits* clonalTraits,
      SWaterTraits* waterTraits, CCell* cell);
///seed set
CWaterSeed(CWaterPlant* plant,CCell* cell);
virtual ~CWaterSeed(){};//destructor

virtual string type(); ///<say what you are
virtual string pft(); ///<say what you are

};

//---------------------------------------------------------------------------
#endif
