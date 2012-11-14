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
  double LDDEffect;  ///<dispersal factor for LDD
protected:
//variables
  vector<CClonalGridEnvir*> site;  ///<site composed of single square-metre grids
  typedef map<string,long> TpftList; ///<type combining currently used PFTs with a number
  vector<TpftList*> seed_to_disperse; ///<covering grid of seeds to disperse widely (LDD)
  vector<TpftList*> seed_rain;      ///<covering grid of seed rain to add to lokal cells
  int xLim;                         ///<number of single grid cells at x-axis
  int yLim;                         ///<number of single grid cells at y-axis
//konstructors
  CSiteScale(int size=75); ///<constructor with default size
//functions...
  void SeedDispersal();    ///<long distance seed dispersal
//  void OneYear();          ///<one year calkulations
  double GetDiversity();   ///<get global PFT-Diversity
  ///set-function
  void setLDDEffect(double effect){LDDEffect=effect;};
public:
  ///get-function
  double getLDDEffect(){return LDDEffect;};
};
//---------------------------------------------------------------------------
/** \brief UpScaling service class

*/
class CSiteScaleEnvir: public CEnvir, public CSiteScale{

   virtual void OneWeek(){};  //!< weekly processes - not needed for UpScale
   void first_weeks(int); ///<weeks 1 to seed_set
   void last_weeks(int);  ///<weeks seed_set+1 to end_of_year
   double GetMeanMSDivGrid(); ///< mean Shannon Diversity on grid
   double GetSumBiomassGrid(); ///< biomass on grid
   void writeOverview(string);  ///< file-write overview
   void writeDiversity(string); ///< file-write shannon diversity per cell
   void writeTypeCover(string,string);  ///< file-write type cover per cell
public:
   CSiteScaleEnvir();  ///<constructor
   CSiteScaleEnvir(int,int,int,int,double,double,
     double lddeffect=100,double grazing=0,double ncut=0,double trampling=0);  ///<constructor
   void InitInds(string file);///<initialization of inds based on file data
   virtual void OneYear(); ///<runs one year in default mode
   virtual void OneRun();  ///<runs one simulation run in default mode
   ///collect and write Output to an output-file (to fill yet)
   virtual void GetOutput();
   //! returns number of surviving PFTs (to fill yet)
   virtual int PftSurvival(){return 0;};

} ;
//---------------------------------------------------------------------------
#endif
