/**\file
   \brief definition of clonal plants' class
*/
//---------------------------------------------------------------------------
#ifndef clonalPlantH
#define clonalPlantH

#include "Plant.h"
#include "clonalTraits.h"
//---------------------------------------------------------------------------
class CclonalSeed; class CGenet;
/** \brief clonal plants inherit from CPlant

   \since clonal version
*/
class CclonalPlant: public CPlant
{
protected:
   ///helping function to set allocation to reproduction
   double ReproGrow(double uptake);
   double mReproRamets;                         ///<resources for ramet growth
   CGenet* genet;                               ///<genet of the clonal plant

public:
   SclonalTraits* clonalTraits;                ///<traits of clonal PFT
   vector<CclonalPlant*> growingSpacerList;     ///<List of growing Spacer
   double Spacerlength;                         ///<real spacer length
   double Spacerdirection;                      ///<spacer direction
   double SpacerlengthToGrow;                   ///<length to grow
   int Generation;                              ///<clonal generation

   //constructors for plant objects
   ///make a plant from a clonal-seed object
   CclonalPlant(CclonalSeed* seed);
   ///clonal growth
   CclonalPlant(double x, double y, CclonalPlant* plant);
   ///keine vordefinierten Eigenschaften
   CclonalPlant(double x, double y, SPftTraits* PlantTraits, SclonalTraits* clonalTraits);
   CclonalPlant(SPftTraits* PlantTraits, SclonalTraits* clonalTraits,CCell* cell);
   virtual ~CclonalPlant();  //!<destructor

   virtual string type();              ///<say what you are
   virtual string pft();   ///<say what a pft you are
   virtual string asString(); ///<report plant's status

   ///set genet and add ramet to its list
   void setGenet(CGenet* genet);
   CGenet* getGenet(){return genet;};
   void SpacerGrow();                  ///<spacer growth
   virtual int GetNRamets();           ///<return number of ramets
   virtual double GetBMSpacer();  ///<returns clonal mass
   ///returns BM of ramet+spacer
   virtual double GetMass(){return CPlant::GetMass()+GetBMSpacer();};
};
#endif
//--------------------------------------------------------------------------
