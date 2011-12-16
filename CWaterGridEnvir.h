//---------------------------------------------------------------------------

#ifndef CWaterGridEnvirH
#define CWaterGridEnvirH
#include <iostream>
#include "environment.h"

//---------------------------------------------------------------------------
class SWaterTraits;
class CWaterPlant;
///  Envir-Class for Water-stresds scenarios
/**
 This class is an Envir-class, i.e. weekly functions are scheduled here and
 in- and output is coordinated.

 \author KK
 \date 2011/09/15 COMTESS version

 \todo Weitere Funktion einf�gen, die ausgibt, in wie vielen Zellen(\%)
   ein Typ in der AbovePlantList auftaucht
*/
class CWaterGridEnvir: public CClonalGridEnvir{
//public CEnvir, public CWaterGrid{
 /// water flow intensity categories
 enum ETide {no,easy,tide,turbulent};
 /// flow intensity variable
 ETide WaterFlow;
 static map<string,SWaterTraits*> WLinkList;  //!< links of Pfts(SclonalTraits) used
 ///helping function for EstabLottery()
 void EstabLott_help(CSeed* seed);
 void DispSeeds_help(CPlant* plant,CCell* cell);    ///<clonal version
 virtual CclonalPlant* newSpacer(int x,int y, CclonalPlant* plant);
// virtual CclonalPlant* newSpacer(int x,int y, CclonalPlant* plant);
 ///weekly water levels
 /**2^5 for generating autocorellation
 \sa genAutokorrWL()
 */
 double weeklyWL[33];
 /// generate autocorrelated Wl-series
 void genAutokorrWL(double hurst);
 /// generate seasonal Wl-series
 void genSeasonWL();
 /// generate const Wl-series
 void genConstWL();
protected:
 void DistribResource();    ///<water impact on ressource allocation
 void SetCellResource();     ///< set amount of resources the cells serve
public:
  CWaterGridEnvir():WaterFlow(no){CellsInit();
 cout<<"\nCWaterGrid() ";};
  ~CWaterGridEnvir();
 ///initialization of grid cells - no functionality
 void CellsInit();
 virtual void InitInds();///<Initialization of individuals on grid
// virtual void InitInds(string file);///<initialization of inds based on file data
// virtual void InitSeeds(string, int);
 //! initalization of clonal seeds
 virtual void InitWaterSeeds(SPftTraits* traits,SclonalTraits* cltraits,
   SWaterTraits* wtraits,const int n,double estab=1.0);
 virtual void InitWaterInds(SPftTraits* traits,SclonalTraits* cltraits,
   SWaterTraits* wtraits,const int n,double mass);
//-----
  void OneRun();    ///< runs one simulation run in default mode

  virtual int exitConditions(); ///< get exit conditions
  virtual void GetOutput();    //run in 20th week of year
  void WriteWaterOutput();   ///< write water-experiment result output

 /// returns grid's mean water level
 double GetMeanWaterLevel();
 /// lowers or rises mean water level relatively to current value
 void ChangeMeanWaterLevel(double val);
 /// lowers or rises mean water level
 void SetMeanWaterLevel(double val);

 ///get link to trait pointer
 static SWaterTraits* getWLink(string type){return WLinkList.find(type)->second;};
 ///add new Link to Trait pointer list
 static void addWLink(string type,SWaterTraits* link){WLinkList[type]=link;};
  ///\name reimplemented Functions from CEnvir
  //@{
//  void InitRun();   ///< from CEnvir
//  void OneYear();   ///< runs one year in default mode
//  void OneRun();    ///< runs one simulation run in default mode
//  void OneWeek();   //!< calls all weekly processes
//  int PftSurvival();    ///< from CEnvir
  /// from CEnvir: collect and write general results
//  void GetOutput();
  //@}
  //new...
  ///reads simulation environment from file
//  int GetSim(const int pos=0,string file=NameSimFile);
//  void InitInds();///<Initialization of individuals on grid

};//end CWaterGridEnvir

//---------------------------------------------------------------------------
/**\bief structure to store all water related type specific parameters

\author KK
\date 2011/10/05 COMTESS version
*/
struct SWaterTraits   //plant functional traits
{
   static vector<SWaterTraits*> PFTWaterList;        //!< List for Pft parameters
//   static map<string,SPftTraits*> PftList;        //!< List for Pft parameters
   static void ReadWaterStrategy(char* file="");    //!> read PFT parameters from the input file

   string name;   ///< name of functional type
   double WL_Optimum; ///< optimal mean water level (positive is flooded)
   double WL_Tolerance; ///< SD of optimal water level
   bool assimBelWL;///<plant can assimilate light below WaterLevel
   bool assimAnoxWL;///<root can uptake Resources in anoxic root zone

   void SetDefault();   ///< set default trait values (eq. 'PFT1')
   SWaterTraits();
   virtual ~SWaterTraits(){};
};

//---------------------------------------------------------------------------
#endif
