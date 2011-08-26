//---------------------------------------------------------------------------

#ifndef CWaterGridEnvirH
#define CWaterGridEnvirH
#include <iostream>
#include "environment.h"

//---------------------------------------------------------------------------
class CWaterGridEnvir: public CClonalGridEnvir{
//public CEnvir, public CWaterGrid{
public:
  CWaterGridEnvir():WaterFlow(no){CellsInit();
 cout<<"\nCWaterGrid() ";};
  ~CWaterGridEnvir();
///\todo  überlade grid mit **CWaterCell
 CWaterCell** CellList;    //!<array of pointers to CWaterCell
 void CellsInit();
 /// water flow intensity categories
 enum ETide {no,easy,tide,turbulent};
 ETide WaterFlow;
// /// returns grid's mean water level
// double GetMeanWaterLevel();
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
