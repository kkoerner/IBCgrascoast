/**\file
   \brief definition of environmental classes and result structures PftOut and GridOut
*/
//---------------------------------------------------------------------------
#ifndef environmentH
#define environmentH

#include "GridBase.h"
#include "CGridclonal.h"

#include "rng.h"
#include "OutStructs.h"
#include <vector>
#include <fstream>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/// virtual Basic Results Class with general static simulation parameters
/** The class contains
    - simulation-wide (static) information on
      - Names of in- and output-files,
      - an Random Number Generator (plus some service functions), and
      - a template for above-and belowground resources as well as
      - current simulation status (current year, week etc.)
    - variables storing result information on grid and single pfts
    - functions
      - collecting and writing results to output-files
      - reading-in Resource data
      - core function OneWeek(), running a week of the simulation
   \par time scales of the simulations:
      - 1 step = 1 week
      - 1 year = 30 weeks
*/
class CEnvir{
protected:
   map<string,int> PftSurvTime;    //!< array for survival times of PFTs [years];
   map<string,int> PftInitList;  //!< list of Pfts used

public:
   //Input Files
   static char* NamePftFile;   ///< Filename of PftTrait-File
   static string NameBResFile; ///< Filename of Belowground-Resource-File
   static string NameSimFile;  ///< Filename of Simulation-File

   //Output Files
   static string NamePftOutFile;   ///< Filename of Pft-Output
   static string NameGridOutFile;  ///< Filename of Grid-Output
   static char* NameSurvOutFile;   ///< Filename of Survival-Output

//   static int NumPft;              ///<Number of Plant functional Types
   static vector<double> AResMuster;     //!< mean above-ground resource availability [resource units per cm^2]
   static vector<double> BResMuster;     //!< mean below-ground resource availability [resource units per cm^2]
   static KW_RNG::RNG RandNumGen;   //!< random number generator

   static int week;        ///< current week (0-30)
   static int year;        ///< current year
   static int WeeksPerYear;///< nb of weeks per year (constantly at value 30)
   static int NRep;        //!< number of replications -> read from SimFile;
   static int SimNr;       ///< simulation-ID
   static int RunNr;       ///< repitition number

   bool endofrun;
   int init;               ///< flag for simulation issue (init time)
   vector<SPftOut*> PftOutData;   //!< Vector for Pft output data
   vector<SGridOut*> GridOutData;  //!< Vector for Grid output data

  //result variables - non-clonal
   vector<int> ACover;     //!< mean above-ground resource availability [resource units per cm^2]
   vector<int> BCover;     //!< mean below-ground resource availability [resource units per cm^2]
   map<string,double> PftCover;  //!< current Grid-cover of Pfts used
   int NPlants;            ///< Number of Plants on Grid
   int NdeadPlants;        ///< Number of dead Plants on Grid
   int CoveredCells;       ///< Number of Cells covered by plants on Grid
   double NCellsAcover;    ///< Number of Cells shaded by plants on ground
//   double MeanShannon;     ///< mean Shannon Diversity
   double Mortalitaetsrate;///< mortality rate

   //Functions
   CEnvir();
   virtual ~CEnvir();

   //! read in fractal below-ground resource distribution (not used)
   static void ReadLandscape();
   /// returns absolute time horizon
   static int GetT(){return (year-1)*WeeksPerYear+week;};
   /// reset time
   static void ResetT(){ year=1;week=0;};
   /// set new week
   static void NewWeek(){week++;if (week>WeeksPerYear){week=1;year++;};};
   ///round a doube value
   inline static int Round(const double& a){return floor(a+0.5);};
   ///get a uniformly distributed random number (0-n)
   inline static int nrand(int n){return RandNumGen.rand_int32()/(double)ULONG_MAX*n;};
   ///get a uniformly distributed random number (0-1)
   inline static double rand01(){return RandNumGen.rand_halfclosed01();};

   virtual void InitRun();   ///<init a new run
   virtual void clearResults(); ///<reset result storage
   virtual void OneWeek()=0;  //!< calls all weekly processes
   virtual void OneYear()=0; ///<runs one year in default mode
   virtual void OneRun()=0;  ///<runs one simulation run in default mode
   ///collect and write Output to an output-file
   virtual void GetOutput()=0;//PftOut& PftData, GridOut& GridData)=0;
   //! returns number of surviving PFTs
   /*! a PFT is condsidered as a survivor if individuals or
        at least seeds are still there
   */
   virtual int PftSurvival()=0;

   //! write suvival time of each PFT to the output file
   /*! For each scenario the year in which each PFT went extinct is written
     to the output file. If the year is equal to the simulation time the PFT survived
     In addition the number or surviving PFTs and the mean shannon index of the last
     quarter of the simulation is saved to the file.
   */
   void WriteSurvival();
   void WriteSurvival(int runnr, int simnr);
   //! writes detailed data for the modelled community to output file
   void WriteGridComplete();
   //! writes detailed data for each PFT to output file
   void WritePftComplete();
   /// get mean shannon diversity over several years
   double GetMeanShannon(int years);
};
//---------------------------------------------------------------------------
/// simulation service class including grid-, result- and environmental information
/**
   The class collects simulation environment with clonal properties.
   CGridclonal and CEnvir are connected, and some Clonal-specific
   result-variables added.
*/
class CClonalGridEnvir: public CEnvir, public CGridclonal{
protected:
public:
  static char* NameClonalPftFile; ///< Filename of clonal Pft-File
  static char* NameClonalOutFile; ///< Filename of clonal Output-File
  static int clonaltype;          ///< current clonal type (number in List)
  static int Pfttype;             ///< current Pft-Type (number in List)
//  static int sim;                 ///< current number of  repititions

  //Constructors, Destructors ...
  CClonalGridEnvir();
  virtual ~CClonalGridEnvir();//delete clonalTraits;

  //annual Results variables -clonal
  vector<SClonOut*>  ClonOutData; ///< Vector of clonal Output data

  ///\name reimplemented Functions from CEnvir
  //@{
  void InitRun();   ///< from CEnvir
  void OneYear();   ///< runs one year in default mode
  void OneRun();    ///< runs one simulation run in default mode
  void OneWeek();   //!< calls all weekly processes
  int PftSurvival();    ///< from CEnvir
  /// from CEnvir: collect and write general results
  void GetOutput();    //run in 20th week of year
  void GetClonOutput(); //run in 30th week of year
  //@}
  //new...
  ///reads simulation environment from file
  int GetSim(const int pos=0,string file=NameSimFile);
  void InitInds();///<Initialization of individuals on grid
  void InitInds(string file);///<initialization of inds based on file data
  void clonalOutput();   ///< write clonal results collected last
  int exitConditions(); ///< get exit conditions //first implemented by Ines
   ///\name Functions to get Acover and Bcover of cells.
   /** It is assumed that coordinates/indices match grid size.
       Functions have to be called after function CGrid::CoverCells and before
       first function calling delete for established plants in the same week.
       else undefined behavior including access violation is possible.

     \note depends (at least) on an inherited subclass of CGrid
   */
   //@{
   int getACover(int x, int y);
   int getBCover(int x, int y);
   double getTypeCover(const int i, const string type)const;
private:
   int getGridACover(int i);
   int getGridBCover(int i);
   ///set cell state information
   void setCover();
   //@}
};
//---------------------------------------------------------------------------
#endif
