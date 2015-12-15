/**\file
   \brief definition of class CGenet
*/
//---------------------------------------------------------------------------
#ifndef CGenetH
#define CGenetH

#include <vector>
//#include "Plant.h"
using namespace std;

namespace ibc{
class CPlant;
//---------------------------------------------------------------------------
/**
 * Class organizing ramets of a genet.
 */
class CGenet
{
	   static int staticID;
public:
   vector<CPlant*> AllRametList;  ///<list of ramets
   int number;                         ///<ID of genet

   CGenet():number(++staticID){};
   virtual ~CGenet(){};
   virtual double GetMass();///< return BM of Genet
   virtual unsigned int GetNRamets();///< return number of alive Ramets
   void ResshareA();     ///< share above-ground resources
   void ResshareB();     ///< share below-ground resources

	int getNumber() const {
		return number;
	}

	static int getStaticId() {
		return staticID;
	}

	static void setStaticId(int staticId=0) {
		staticID = staticId;
	}

//	string getPFT(){return allRametList.begin->pft()};
};
}//namespace ibc
//---------------------------------------------------------------------------
#endif

