//---------------------------------------------------------------------------

#ifndef CclonalSeedH
#define CclonalSeedH
#include "clonalTraits.h"
#include "CSeed.h"
//---------------------------------------------------------------------------
class CclonalPlant;
/**\brief clonal seeds inherit from CSeed

   \since clonal version
*/
class CclonalSeed: public CSeed
{
public:
SclonalTraits* clonalTraits; ///<clonal traits
//Konstruktoren
//CclonalSeed(double x, double y, int ID, double m, double estab, int maxage, SclonalTraits clonalTraits);
CclonalSeed(double x, double y, double estab, SPftTraits* traits,  SclonalTraits* clonalTraits);
CclonalSeed(double x, double y, CclonalPlant* plant);
CclonalSeed(double estab, SPftTraits* traits,  SclonalTraits* clonalTraits,CCell* cell);
CclonalSeed(CclonalPlant* plant,CCell* cell);
CclonalSeed(CclonalSeed& seed);
virtual ~CclonalSeed(){};//destructor

virtual string type(); ///<say what you are
virtual string pft(); ///<say what you are

};

#endif


