//---------------------------------------------------------------------------


#pragma hdrstop

#include "CWaterGridEnvir.h"

//---------------------------------------------------------------------------
void CWaterGridEnvir::CellsInit(){
}//end CellsInit
//---------------------------------------------------------------------------

CWaterGridEnvir::~CWaterGridEnvir(){
  for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
      CWaterCell* cell = CellList[i];
      delete cell;
   }
   delete[] CellList;

}
//---------------------------------------------------------------------------
/**
  \todo make fkt constant
*/
double CWaterGridEnvir::GetMeanWaterLevel(){
   double sum_Level=0.0;
   //get cell number
   long ncells= SRunPara::RunPara.GetSumCells();
   for (int i=0; i<ncells; ++i){
      CWaterCell* cell = CellList[i];
   //sum all waterlevel values
      sum_Level+=cell->GetWaterLevel();
   }//end for
   // return mean
   return sum_Level/ncells;
}//end GetMeanWaterLevel

/**
   Lowers or sinks all WaterLevel values at the same time.
   Differences absolutely stay constant.

   \param val new mean water level
*/
void CWaterGridEnvir::SetMeanWaterLevel(double val=0){
   //get current mean value
   double curr_mLevel=GetMeanWaterLevel();
   //get difference
   double diff=val-curr_mLevel;
   //for each cell: add difference
   long ncells= SRunPara::RunPara.GetSumCells();
   for (int i=0; i<ncells; ++i){
      CWaterCell* cell = CellList[i];
      cell->SetWaterLevel(cell->GetWaterLevel()+diff);
   }//end for
}//end SetMeanWaterLevel

//---------------------------------------------------------------------------

#pragma package(smart_init)
