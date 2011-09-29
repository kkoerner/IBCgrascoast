//---------------------------------------------------------------------------

#ifndef CWaterGridEnvirH
#define CWaterGridEnvirH
#include <iostream>
#include "environment.h"

//---------------------------------------------------------------------------
///  Envir-Class for Water-stresds scenarios
/** 
 This class is an Envir-class, i.e. weekly functions are scheduled here and
 in- and output is coordinated.
 
*/
class CWaterGridEnvir: public CClonalGridEnvir{
//public CEnvir, public CWaterGrid{
 /// water flow intensity categories
 enum ETide {no,easy,tide,turbulent};
 /// flow intensity variable
 ETide WaterFlow;
public:
  CWaterGridEnvir():WaterFlow(no){CellsInit();
 cout<<"\nCWaterGrid() ";};
  ~CWaterGridEnvir();
 void CellsInit();
 /// returns grid's mean water level
 double GetMeanWaterLevel();
 /// lowers or rises mean water level
 void SetMeanWaterLevel(double val);
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
#endif
