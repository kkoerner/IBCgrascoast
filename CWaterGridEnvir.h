//---------------------------------------------------------------------------

/** \page changes Changes for the COMTESS-version
  \section secWatStr Water stress option
  \image html ZOIs_Water_middle.png "New Rules' imagination"
  \par Extend class CCell with WaterLevel.
  A makro swiches in CGrid between an array of CCell and CWaterCell.
  \par Write derived class CWaterGridEnvir.
  This class is derrived from CClonalGridEnvir. At the moment it has a new
  variable WaterFlow, that has information on the weekly frequency/ intensity
  of water flow (enum CWaterGridEnvir::ETide).
  It has an own init-function and get/set functions for mean water level
  on grid.
  GridCells of the basic lattice have to be of type CWaterCell.
  \sa CWaterGridEnvir, CWaterCell
  \par Implement WaterLevel-related rules
   -# aboveground plant parts don't assimilate if they are submersed
   (but plant type may be adapted to assim below WL)
   -# root parts of the anoxic soil don't uptake resources
      if plant is not adapted; else the entire root gains a constant
      proportion of the resources available.

   \sa CWaterPlant::DistrRes_help and  CWaterPlant::comp_coef

  \section secWintMort Additional Winter Mortality
  Additional winter mortality due to prolonged inundation or disturbance events
  as dike breaking are calculated calling the Plant's function
  CWaterPlant::winterDisturbance in CWaterGridEnvir::Winter.
  A prolonged winter inundation let plants die by oxygen deficit. Types that are
  adapted to high water levels in summer, here also perform better.

  \verbatim
  mort=min(0.95, (MonthsInundation-Thresh_by_adapt )/4)
  Thresh_by_adapt(no,yes)=(2,8)
  \endverbatim

  \section secSalt Salt stress option
  Salt stress reduces belowground resource uptake to zero.
  Adaptation comes with costs that increase with tolerated salinity level
  (S0,1: 30% for <6g/l  and S2-5:60% for <10g/l; to date salinity levels above are not
  considered).

  \image html salinity_rules.jpg "salinity rules"

  \sa SWaterTraits::saltTol salt value after Ellenberg

   */

#ifndef CWaterGridEnvirH
#define CWaterGridEnvirH
#include <iostream>
#include "CGridEnvir.h"
#include "SPftTraits.h"
//---------------------------------------------------------------------------
class SWaterTraits;
class CWaterPlant;
///  Envir-Class for Water-stresds scenarios
/**
 This class is an Envir-class, i.e. weekly functions are scheduled here and
 in- and output is coordinated.

 \author KK
 \date 2011/09/15 COMTESS version

 \todo Weitere Funktion einfügen, die ausgibt, in wie vielen Zellen(\%)
   ein Typ in der AbovePlantList auftaucht
*/
class CWaterGridEnvir: public CGridEnvir{
//public CEnvir, public CWaterGrid{
 // / water flow intensity categories
// enum ETide {no,easy,tide,turbulent};
 // / flow intensity variable
// ETide WaterFlow;
// static map<string,SWaterTraits*> WLinkList;  //!< links of Pfts(SclonalTraits) used
 ///helping function for EstabLottery()
 void EstabLott_help(CSeed* seed);
 void DispSeeds_help(CPlant* plant,CCell* cell);    ///<clonal version
 virtual CPlant* newSpacer(int x,int y, CPlant* plant);
// virtual CclonalPlant* newSpacer(int x,int y, CclonalPlant* plant);
 ///weekly water levels
 /**2^5 for generating autocorellation
 \sa genAutokorrWL()
 */
 static vector<double> weeklyWL;
 static vector<double> weeklySAL;///< weekly salinity levels
 static vector<double> weeklySAT;///< weekly soil saturation levels
 /// generate autocorrelated Wl-series
 void genAutokorrWL(double hurst);
 /// generate seasonal Wl-series
 void genSeasonWL();
 /// generate const Wl-series
 void genConstWL();
 /// read annual environmental conditions from file
 void getEnvirCond(string file);
 /// inundation time in winter season \[weeks\] 0..12
 int winterInundation;
protected:
 void DistribResource();    ///<water impact on ressource allocation
 void SetCellResource();     ///< set amount of resources the cells serve
    //! calls seed mortality and mass removal of plants
 virtual void Winter();

public:
 /// plot's salinity level
 /**
   Global salinity value for the whole grid. Function CWaterPlant::rootEfficiency
   can read this.

   Unit is g/l (Thresholds in Ellenberg are in \'\%\').
 */
static double getSAL();///<get current salinity
// static double salinity;
static double getSAT();///<get current soil saturation
double getWL(); ///<get current water level

  CWaterGridEnvir():winterInundation(0){CellsInit();//WaterFlow(no),
// cout<<"\nCWaterGrid() ";
  };
  CWaterGridEnvir(string id); ///< load from file(s)
  ~CWaterGridEnvir();
 ///initialization of grid cells - no functionality
 void CellsInit();
 virtual void InitRun();///<Init current run
// virtual void InitInds();///<Initialization of individuals on grid
// virtual void InitInds(string file);///<initialization of inds based on file data
// virtual void InitSeeds(string, int);
  virtual void InitInds(string n="");///<initialization of inds based on file data
  /// Initialization of one Individual while loading a saved grid
  virtual bool InitInd(string def);
 //! initalization of clonal seeds
 virtual void InitSeeds(const string  PftName,const int n=1,double estab=1.0);
   //! initalization of seeds
   virtual void InitSeeds(const string  PftName,const int n,int x, int y,double estab=1.0);
 //! initalization of clonal seeds
 virtual void InitSeeds(//SPftTraits* traits,//SclonalTraits* cltraits,
   SWaterTraits* wtraits,const int n,double estab=1.0,int x=-1, int y=-1);
 virtual void InitWaterInds(//SPftTraits* traits,//SclonalTraits* cltraits,
   SWaterTraits* wtraits,const int n,double mass);

 virtual void Save(string ID);//<save current grid state

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

 // /get link to trait pointer
 //static SWaterTraits* getWLink(string type){return WLinkList.find(type)->second;};
 // /add new Link to Trait pointer list
 //static void addWLink(string type,SWaterTraits* link){WLinkList[type]=link;};
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
struct SWaterTraits: public SPftTraits   //plant functional traits
{
//   static vector<SWaterTraits*> PFTWaterList;        //!< List for Pft parameters

//   string name;   ///< name of functional type
   bool assimBelWL;///<plant can assimilate light below WaterLevel
   ///\brief ability to uptake resources in the anoxic root zone
   /// If zero, no uptake below waterlevel;
   /// else it's efficiency is reduced by this value for all WL
   ///
   double assimAnoxWL;
   double saltTol; ///< salt tolerance of species (Ellenberg salt value)
   double saltTolCosts(); ///< salt tolerance costs
   double saltTolEffect(double salinity); ///< salt tolerance effect

 //  void SetDefault();   ///< set default trait values (eq. 'PFT1')
   SWaterTraits();///< neutral constructor
   virtual ~SWaterTraits(){};///< destructor
   static bool ReadWPFTDef(const string& file, int n=-1);///< read PFT defs from file
   virtual void print();										///<print content to prompt
};

//---------------------------------------------------------------------------
#endif
