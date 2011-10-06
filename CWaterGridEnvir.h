//---------------------------------------------------------------------------

#ifndef CWaterGridEnvirH
#define CWaterGridEnvirH
#include <iostream>
#include "environment.h"

//---------------------------------------------------------------------------
class SWaterTraits;
///  Envir-Class for Water-stresds scenarios
/**
 This class is an Envir-class, i.e. weekly functions are scheduled here and
 in- and output is coordinated.

 \author KK
 \date 2011/09/15 COMTESS version
*/
class CWaterGridEnvir: public CClonalGridEnvir{
//public CEnvir, public CWaterGrid{
 /// water flow intensity categories
 enum ETide {no,easy,tide,turbulent};
 /// flow intensity variable
 ETide WaterFlow;
 static map<string,SWaterTraits*> WLinkList;  //!< links of Pfts(SclonalTraits) used
public:
  CWaterGridEnvir():WaterFlow(no){CellsInit();
 cout<<"\nCWaterGrid() ";};
  ~CWaterGridEnvir();
 void CellsInit();
 virtual void InitInds();///<Initialization of individuals on grid
// virtual void InitInds(string file);///<initialization of inds based on file data
// virtual void InitSeeds(string, int);
   //! initalization of clonal seeds
   virtual void InitWaterSeeds(SPftTraits* traits,SclonalTraits* cltraits,
     SWaterTraits* wtraits,const int n,double estab=1.0);

 /// returns grid's mean water level
 double GetMeanWaterLevel();
 /// lowers or rises mean water level relatively to current value
 void ChangeMeanWaterLevel(double val);
 /// lowers or rises mean water level
 void SetMeanWaterLevel(double val);

 static SWaterTraits* getWLink(string type){return WLinkList.find(type)->second;};
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
   double WL_Optimun; ///< optimal mean water level (positive is flooded)
   double WL_Tolerance; ///< SD of optimal water level


   void SetDefault();   ///< set default trait values (eq. 'PFT1')
   SWaterTraits();
   virtual ~SWaterTraits(){};
};

//---------------------------------------------------------------------------
#endif
