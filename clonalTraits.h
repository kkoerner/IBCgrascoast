//---------------------------------------------------------------------------

#ifndef clonalTraitsH
#define clonalTraitsH
//#include "clonalPlant.h"

#include <string>
#include <vector>
//class string;
//---------------------------------------------------------------------------
/**\brief Structure with clonal preferences

   \since clonal version
*/
struct SclonalTraits   //clonal plant functional traits
{
   /// List of available Plant traits
   static std::vector<SclonalTraits*> clonalTraits;
   /// read file with clonal traits
   static void ReadclonalTraits(char* file="");

   std::string name;         ///<name of PFT
   ///allocation to sexual reproduction during time of seed production
   double PropSex;
   double meanSpacerlength;  ///<mean spacer length [cm]
   double sdSpacerlength;    ///<sd spacer length [cm]
   bool Resshare;            ///<do established ramets share their ressources?
   double mSpacer;  ///<resources for 1 cm spacer (default=70)

   SclonalTraits();   ///<constructor with default values
   ~SclonalTraits(){};
   void print();
};
#endif
