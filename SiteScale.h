//---------------------------------------------------------------------------
/**\file
   \brief definition of UpScale-related classes
*/

#ifndef SiteScaleH
#define SiteScaleH

/// lokal cell's services
#include "environment.h"
//---------------------------------------------------------------------------
/**
  \brief class with variables and functions on site scale


*/
class CSiteScale{
//variables
  vector<CClonalGridEnvir*> site;  ///<site composed of single square-metre grids
  typedef map<string,int> TpftList; ///<type combining currently used PFTs with a number
  vector<TpftList*> seed_to_disperse; ///<covering grid of seeds to disperse widely (LDD)
  vector<TpftList*> seed_rain;      ///<covering grid of seed rain to add to lokal cells

//konstructors
  CSiteScale(int size=75); ///<constructor with default size
//functions...
  void SeedDispersal();    ///<long distance seed dispersal
  void OneYear();          ///<one year calkulations
  double getDiversity();   ///<get global PFT-Diversity
}
//---------------------------------------------------------------------------
/** \brief UpScaling service class

*/
class CSiteScaleEnvir: public CEnvir, public CSiteScale{

}
//---------------------------------------------------------------------------
#endif
