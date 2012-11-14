/**\file
   \brief definition of class CGenet
*/
//---------------------------------------------------------------------------
#ifndef CGenetH
#define CGenetH

#include "clonalPlant.h"
//---------------------------------------------------------------------------
/**\brief a genet consists of several ramets

   \since clonal version
*/
class CGenet
{
public:
   static int staticID;
   vector<CclonalPlant*> AllRametList;  ///<list of ramets
   int number;                         ///<ID of genet

   CGenet():number(++staticID){};
   ~CGenet(){};
   virtual double GetMass();///< return BM of Genet
   void ResshareA();     ///< share above-ground resources
   void ResshareB();     ///< share below-ground resources
};
//---------------------------------------------------------------------------
#endif

