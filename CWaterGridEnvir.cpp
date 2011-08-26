//---------------------------------------------------------------------------


#pragma hdrstop

#include "CWaterGridEnvir.h"

//---------------------------------------------------------------------------
void CWaterGridEnvir::CellsInit(){
   using SRunPara::RunPara;
   int index;int SideCells=RunPara.CellNum;
      for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
      CCell* cell = CGrid::CellList[i];
      delete cell;
   }
   delete[] CGrid::CellList;

   CellList = new (CWaterCell* [SideCells*SideCells]);

   for (int x=0; x<SideCells; x++){
      for (int y=0; y<SideCells; y++){
         index=x*SideCells+y;
         CWaterCell* cell = new CWaterCell(x,y,
                 CEnvir::AResMuster[index],CEnvir::BResMuster[index]);
         CellList[index]=cell;
     }
  }
//lösche alte CellList aus CGrid
//erstelle neue Liste
}//end CellsInit
CWaterGridEnvir::~CWaterGridEnvir(){
  for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
      CWaterCell* cell = CellList[i];
      delete cell;
   }
   delete[] CellList;

}
//---------------------------------------------------------------------------

#pragma package(smart_init)
