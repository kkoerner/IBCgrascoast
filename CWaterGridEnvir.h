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

 \todo Weitere Funktion einfügen, die ausgibt, in wie vielen Zellen(\%)
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
 static double salinity;

  CWaterGridEnvir():WaterFlow(no),winterInundation(0){CellsInit();
 cout<<"\nCWaterGrid() ";};
  CWaterGridEnvir(string id); ///< load from file(s)
  ~CWaterGridEnvir();
 ///initialization of grid cells - no functionality
 void CellsInit();
 virtual void InitRun();///<Init current run
 virtual void InitInds();///<Initialization of individuals on grid
// virtual void InitInds(string file);///<initialization of inds based on file data
// virtual void InitSeeds(string, int);
  virtual void InitInds(string file,int n=-1);///<initialization of inds based on file data
  /// Initialization of one Individual while loading a saved grid
  virtual bool InitInd(string def);
 //! initalization of clonal seeds
 virtual void InitWaterSeeds(const string  PftName,const int n=1,double estab=1.0);
   //! initalization of seeds
   virtual void InitWaterSeeds(const string  PftName,const int n,int x, int y,double estab=1.0);
 //! initalization of clonal seeds
 virtual void InitWaterSeeds(SPftTraits* traits,SclonalTraits* cltraits,
   SWaterTraits* wtraits,const int n,double estab=1.0,int x=-1, int y=-1);
 virtual void InitWaterInds(SPftTraits* traits,SclonalTraits* cltraits,
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
   bool assimBelWL;///<plant can assimilate light below WaterLevel
   ///\brief ability to uptake resources in the anoxic root zone
   /// If zero, no uptake below waterlevel;
   /// else it's efficiency is reduced by this value for all WL
   ///
   double assimAnoxWL;
   int saltTol; ///< salt tolerance of species (Ellenberg salt value)
   double saltTolCosts(); ///< salt tolerance costs
   double saltTolEffect(double salinity); ///< salt tolerance effect

   void SetDefault();   ///< set default trait values (eq. 'PFT1')
   SWaterTraits();
   virtual ~SWaterTraits(){};
   void print();
};

//---------------------------------------------------------------------------
#endif
