/**\file
\brief contains GUI-behaviour of the program*/
//------------------------------------------------------------------------------
#include <iomanip>

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MMColorGrid"
#pragma resource "*.dfm"

#include "GrassGui.h"

#pragma link "MMLegendView"
#pragma hdrstop
//------------------------------------------------------------------------------
/**\mainpage Grassland Model (for console) - documentation

\author
Felix May (basic model, ZOI) and Ines Steinhauer (clonal option),
Katrin Koerner (revision and rebuilt Felix' grazing experiments)


\date 2008-02-13 (model)
\date 2009-05 (revision)
\date 2010-01 (Felix' grazing rebuilt and belowground grazing)
\date 2010-03 (Ines' clonal plants' rebuilt)

\par Verbal description of what the code does:
This code rebuilds the clonal experiments describes by Steinhauer (2008).
For better imagine and debugging purposes this version is
implemented as GUI-Version. It displays on time weekly grid-
information. Charts display temporal devellopment of the populations.

In choosing a simulation file the user can define the modelling environment
according to the console version of the model.

See also: publications of May(2008) and Steinhauer(2008)

\par Type (function, class, unit, form, ...):
application with some classes

\par Flow chart (for complex code):
\image html flow.jpg "FlowChart of the clonal ModelVersion. In: Steinhauer(2008)"
Flussdiagramm des Graslandmodells inklusive Wachstumsprozessen klonaler Pflanzen.
Prozesse mit abgerundeten Ecken sind Prozesse der Zelle.
Die anderen Prozesse werden fuer alle Pflanze/Samen ausgefuehrt.
Erweiterungen des Modells von May (2008) sind in grau dargestellt.

\par Expected input (type and range of values, units):
- input file for 81 plant functional traits
- input file for 8 clonal trait syndomes
- optionally an input file for successive simulations

\par Output (type and range of values, units):
- some ASCII-coded *.txt-files with weekly or yearly numbers
  of individuals or other summarizing variables

\par Requirements and environment (libraries, headers, Borland Builder):
- BorlandBuilder6.0
- random number generator <i>r250</i> (included), STL

\par Sensitivity analysis (or reference to publication):
...

\par Validation (or reference to publication):
...

\par Sources or reasons for parameter values, methods, equations:
see publications of May(2008) and Steinhauer(2008)

\bug
see additional page for solved and unsolved bugs

\par Publications or applications referring to the code:
- May, Felix, Grimm, Volker and Jeltsch, Florian (2009): Reversed effects of
  grazing on plant diversity: the role of belowground competition
  and size symmetry. Oikos 118: 1830-1843.
- May, Felix (2008): Modelling coexistence of plant functional types
  in grassland communities - the role of above- and below-ground competition.
  Diploma thesis Potsdam University.
- Steinhauer, Ines (2008): KOEXISTENZ IN GRASLANDGESELLSCHAFTEN -
  Modellgestuetzte Untersuchungen unter Beruecksichtigung klonaler Arten.
  Diplomarbeit Universitaet Potsdam
*/

//----------------------------------------------------------------------
//globale Variablen
TForm1 *Form1;
CClonalGridEnvir* Envir;
//------------------------------------------------------------------------------
__fastcall TForm1::~TForm1(){
  delete Envir;
//delete static pointer vectors
  for (unsigned int i=0;i<SPftTraits::PftList.size();i++)
    delete SPftTraits::PftList[i];
  for (unsigned int j=0;j<SclonalTraits::clonalTraits.size();j++)
    delete SclonalTraits::clonalTraits[j];
}
//------------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{
   using SRunPara::RunPara;
   ETmax->Text =RunPara.Tmax;
   EPgraz->Text=RunPara.GrazProb;
   EAres->Text =RunPara.meanARes;
   EBres->Text =RunPara.meanBRes;

   multisim=0;
   Memo1->Clear();
   Envir=new CClonalGridEnvir();
   CEnvir::NameSimFile="Input\\ClType_a50_b50_all.txt"; //default value
   //load sim file
//   if (ODSimFile->Execute())CEnvir::NameSimFile=ODSimFile->FileName.c_str();
   //cluster simulations...
//   Button7->Click();  //Start
//   BExit->Click();  //Exit
}
//------------------------------------------------
void writeBenchmark(int pft,int clpft,int run,int week, double time){
  ofstream of("Output\\extTimes.txt",ios_base::app);
  of<<pft<<'\t'<<clpft<<'\t'<<run<<'\t'<<week<<'\t'<<time<<'\n';
}
//Init--------------------------------------------------------------------------
void __fastcall TForm1::InitSim()
{
   ChClonality->SeriesList->Series[0]->Clear();
   ChClonality->SeriesList->Series[1]->Clear();

   ChPFTs->SeriesList->Series[0]->Clear();
   ChPFTs->SeriesList->Series[1]->Clear();

   ChBiomass->SeriesList->Series[0]->Clear();
   ChBiomass->SeriesList->Series[1]->Clear();

   using SRunPara::RunPara;
   MMColorGrid1->Xcells=RunPara.GridSize;// Grid1->GetGridSize();
   MMColorGrid1->Ycells=RunPara.GridSize;//Grid1->GetGridSize();
   MMColorGrid2->Xcells=RunPara.GridSize;//Grid1->GetGridSize();
   MMColorGrid2->Ycells=RunPara.GridSize;//Grid1->GetGridSize();

   //Comment out for Cluster Simulations
   PaintGrid();

   ProgressBar1->Position=0;
   ProgressBar1->Max=RunPara.Tmax*CEnvir::WeeksPerYear;
   ProgressBar1->Step=1;
}//end initSim
//Init--------------------------------------------------------------------------
void __fastcall TForm1::GUIInit(){
 //für später
   using SRunPara::RunPara;
   RunPara.Tmax=StrToInt(Form1->ETmax->Text);
   RunPara.GrazProb=StrToFloat(Form1->EPgraz->Text);
   RunPara.meanARes=StrToFloat(Form1->EAres->Text);
   RunPara.meanBRes=StrToFloat(Form1->EBres->Text);

   Envir->ResetT();
   Envir->InitRun();
   Form1->InitSim();

} //end GUIInit
//------------------------------------------------------------------------------
/**
*/
void __fastcall TForm1::BWeekClick(TObject *Sender)
{
  //init
  if (Envir->GetT()==0
     ||Envir->endofrun
     &&CEnvir::year<SRunPara::RunPara.Tmax)
       GUIInit();//...
  //this week
  Envir->NewWeek();
  LWeek->Caption=Envir->GetT();
  Envir->OneWeek();
    //generate output
  Envir->GetOutput();//clonalOutput()
  Envir->GetClonOutput();//clonalOutput()

    //GUI
    clonalOutput();
    PaintGrid();
    PaintTimeSeries();
    ProgressBar1->StepIt();

  //ask for end-of-run
  exitcond=Envir->exitConditions();//exit conditions
  if (exitcond) Application->MessageBoxA("ExitCondition break","Info",0);
}
//------------------------------------------------------------------------------
/**
  Simulation of one year in GUI-mode.
  After each week some GUI-processes as grid-painting and process-bar-stepping
  are called.
  \sa PaintGrid(), clonalOutput(), and CClonalGridEnvir::OneWeek(),
      but also CClonalGrid::OneYear()

  A general console-mode variant is included
  in CEnvir and inheriting classes.
*/
void __fastcall TForm1::OneYear()
{
  do{
     Form1->BWeek->Click();

     if (Envir->endofrun)break;
     Application->ProcessMessages();
  }//weeks
  while (CEnvir::week<CEnvir::WeeksPerYear);
  Envir->PftSurvival();
}
void __fastcall TForm1::BYearClick(TObject *Sender)
{
  OneYear();
}

//Run---------------------------------------------------------------------------
/**
  Calls yearly processes until exit conditions are met or maximum
  simulation time is reached. Subsequently some result- and output-
  functions are called.
*/
void __fastcall TForm1::BRunClick(TObject *Sender)
{
   double start=HRTimeInSec();

   //Simulation;
     //run simulation until YearsMax
     while (CEnvir::year<=SRunPara::RunPara.Tmax)
       {
      OneYear();
      if (Envir->endofrun)break;
      Application->ProcessMessages();
   }//years

   double end=HRTimeInSec();
   Memo1->Lines->Add(FloatToStr(end-start));

   //Output
   Envir->WriteGridComplete();
   Envir->WritePftComplete();
//   Envir->WriteSurvival(CClonalGridEnvir::clonaltype,CClonalGridEnvir::sim);
//   writeBenchmark(CClonalGridEnvir::Pfttype,CClonalGridEnvir::clonaltype,
//               CClonalGridEnvir::sim,exitcond,end-start);
}  //end BRunClick
//Exit--------------------------------------------------------------------------
void __fastcall TForm1::BExitClick(TObject *Sender)
{
   exit(0);
}  //end button3
//------------------------------------------------------------------------------
///multiple simulations (read file)
/**
  An input-file is opened and first simulation
  specified therein is loaded.
*/
void __fastcall TForm1::BLoadFileClick(TObject *Sender)
{
   if (ODSimFile->Execute())CEnvir::NameSimFile=ODSimFile->FileName.c_str();
   int lpos=Envir->GetSim();
}//end BLoadFileClick
//------------------------------------------------------------------------------
///paint grids on tab 'Grids'
void TForm1::PaintGrid()
{
   int N=SRunPara::RunPara.CellNum;

   for (int x=0; x<N; ++x) for(int y=0;y<N;y++)
   {

      MMColorGrid1->SetXY(x,y,Envir->getACover(x,y));
      MMColorGrid2->SetXY(x,y,Envir->getBCover(x,y));
   }
   MMColorGrid1->Refresh();
   MMColorGrid2->Refresh();
}//end PaintGrid
//------------------------------------------------------------------------------
///update TimeSeries on charts
void TForm1::PaintTimeSeries()
{
   int t=CEnvir::GetT();
   vector<double> value=Envir->PftOutData.back()->totmass;

   ChPFTs->SeriesList->Series[0]->AddXY(t,
     Envir->GridOutData.back()->PftCount,"",clTeeColor);
   ChPFTs->SeriesList->Series[1]->AddXY(t,
     Envir->GridOutData.back()->Nind,"",clTeeColor);

   ChBiomass->SeriesList->Series[0]->AddXY(t,
     Envir->GridOutData.back()->above_mass,"",clTeeColor);
   ChBiomass->SeriesList->Series[1]->AddXY(t,
     Envir->GridOutData.back()->below_mass,"",clTeeColor);

   SClonOut* co=Envir->ClonOutData.back();
   ChClonality->SeriesList->Series[0]->AddXY(t,co->NclonalPlants+1);
   ChClonality->SeriesList->Series[1]->AddXY(t,co->NPlants+1);

   ChPFTs->Refresh();
   ChBiomass->Refresh();
   ChClonality->Refresh();
}//end PaintTimeSeries
//------------------------------------------------------------------------------
///generates output for clonal version of the model
/**
  \since clonal version
*/
void TForm1::clonalOutput()
{
   int t=CEnvir::GetT();

  SClonOut* co=Envir->ClonOutData.back();
  Label21->Caption=IntToStr(co->NclonalPlants);
  Label23->Caption=IntToStr(co->NGenets);
  Label21->Refresh();
  Label23->Refresh();

  Label44->Caption=IntToStr(co->NPlants);
  Label44->Refresh();

  Label29->Caption=IntToStr(Envir->CoveredCells);
  Label33->Caption=FloatToStr(co->MeanGeneration);
  Label29->Refresh();
  Label33->Refresh();
}//end TForm1::ClonalOutput
//------------------------------------------------------------------------------
//-eof--------------------------------------------------------------------------





