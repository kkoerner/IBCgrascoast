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
     ///competition coefficient for a plant -needed for AboveComp and BelowComp
   virtual double comp_coef(const int layer,const int symmetry)const;

public:
   SWaterTraits* waterTraits;                ///<traits of water preferences
   //Constructors
   ///make a plant from a seed object
   CWaterPlant(CWaterSeed* seed);
   ///clonal growth
   CWaterPlant(double x, double y, CWaterPlant* plant);
   CWaterPlant(double mass, SPftTraits* traits,
      SclonalTraits* clonalTraits,
      SWaterTraits* waterTraits, CCell* cell);
   virtual ~CWaterPlant(){};  //!<destructor


///add water impact on ressource allocation
  virtual void DistrRes_help();
  ///corrective value for root uptake and competition
  double rootEfficiency();
  
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
