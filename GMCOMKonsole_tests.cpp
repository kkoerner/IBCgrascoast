/** \file
\brief coordinating main.cpp of the GrasslandModell for COMTESS water stress
experiments

This file contains the main() function to start and coordinate the designed
experiments.
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#include <iostream>
#include <sstream>
#include "TimeUtils.h"

#pragma hdrstop

//the only information the GUI needs from the model
#include "environment.h"
#include "CWaterGridEnvir.h"
//------------------------------------------------------------------------------
/**\mainpage Grassland Model (for console) - documentation

\author
Felix May (basic model, ZOI) and Ines Steinhauer (clonal option),
Katrin Koerner (revision and rebuilt Felix' grazing experiments)


\date 2008-02-13 (original model)
\date 2009-05 (revision)
\date 2010-01 (Felix' grazing rebuilt)
\date 2010-11 - 2011-03 (Belowground Herbivory)
\date 2011-08 ff COMTESS


\par Verbal description of what the code does:
This code simulates pft-related wetland-grassland dynamics for different
environmental conditions --develloping process----

See also: publications of May(2008) and Steinhauer(2008)

\par Type (function, class, unit, form, ...):
application with some classes

\par Flow chart
\image html SteinhFlow1.png "FlowChart of the clonal ModelVersion. In: Steinhauer(2008)"
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

\todo
 -add water-stress feature
 -add opt for water-stress adapted plant types
 -add resource storage by plants ?!

\section bib Publications or applications referring to the code:
- May, Felix, Grimm, Volker and Jeltsch, Florian (2009): Reversed effects of
  grazing on plant diversity: the role of belowground competition
  and size symmetry. Oikos 118: 1830-1843.
- May, Felix (2008): Modelling coexistence of plant functional types
  in grassland communities - the role of above- and below-ground competition.
  Diploma thesis Potsdam University.
- Steinhauer, Ines (2008): KOEXISTENZ IN GRASLANDGESELLSCHAFTEN -
  Modellgestuetzte Untersuchungen unter Beruecksichtigung klonaler Arten.
  Diplomarbeit Universitaet Potsdam
- Körner, Katrin et al. (in prep): belowground herbivory
- Weiß, Lina et al. (in prep): clonal growth
*/
//---------------------------------------------------------------------------
CWaterGridEnvir* Envir;   ///<environment in which simulations are run
using namespace std;
#pragma argsused

void Init();
void Run();

/**
  the new water ressource environment can be tested here
  \todo baue eine automatische Abfolge je 20 Jahre für WL 0..30[5er schritte]
  ein. !ändere SimNr und LogFileName
  \author KK
  \date 11/10/6
  */
int main(int argc, char* argv[])
{
  if (argc>1){
    SRunPara::RunPara.meanBRes=atoi(argv[1]); //belowground resources
    SRunPara::RunPara.species=argv[2];  //init types
    SRunPara::RunPara.WLseason=argv[3]; //schwankungen //not yet implemented
    SRunPara::RunPara.WLsigma=atof(argv[4]);
  }
  bool endsim=false;
  SRunPara::RunPara.WaterLevel=40; //start-WL   100
  SRunPara::RunPara.Tmax=20;//20Jahre Laufzeit
  //sim-loop
  do{
    //simNr
    Envir->SimNr=SRunPara::RunPara.WaterLevel+1000;
    //filenames
//    Envir->NameLogFile=((AnsiString)"Mix_Grid_log_"+IntToStr(Envir->SimNr)+".txt").c_str();
    string idstr= SRunPara::RunPara.getRunID();
    stringstream strd;
    strd<<"Output\\Mix_Grid_log_"<<idstr<<".txt";
    Envir->NameLogFile=strd.str();     // clear stream
    strd.str("");
    strd<<"Output\\Mix_clonO_"<<idstr<<".txt";
    Envir->NameClonalOutFile=strd.str();
    strd.str("");strd<<"Output\\Mix_gridO_"<<idstr<<".txt";
    Envir->NameGridOutFile=strd.str();
    strd.str("");strd<<"Output\\Mix_typeO_"<<idstr<<".txt";
    Envir->NameSurvOutFile= strd.str();
    SRunPara::RunPara.print();
    //Run-loop
    for(Envir->RunNr=1;Envir->RunNr<=3;Envir->RunNr++){ //15Runs per Sim
      cout<<"new Environment...\n";
      Envir=new CWaterGridEnvir();

      //SRunPara::RunPara.WaterLevel=30;
      //CEnvir::NameLogFile="C-reed-out.txt";
      cout<<"\n test water types ";
      Init();
      Run();

      delete Envir;
    }//end run
    SRunPara::RunPara.WaterLevel-=5;//5cm weniger für nächste Sim
    if(SRunPara::RunPara.WaterLevel< -40)
    endsim=true;
  }while(!endsim);//end sim
   //delete static pointer vectors
  for (unsigned int i=0;i<SPftTraits::PftList.size();i++)
    delete SPftTraits::PftList[i];
  for (unsigned int j=0;j<SclonalTraits::clonalTraits.size();j++)
    delete SclonalTraits::clonalTraits[j];
//   string dummi;
//   cin>>dummi;
  return 0;
}//end main
//---------------------------------------------------------------------------
/**
      high belowground ressources
*/
void Init(){
      Envir->InitRun();
//      SRunPara::RunPara.meanBRes=200;
}
//------------------------------------------------
void Run(){
//   int exitcond=0;
//   double start=HRTimeInSec();
   //do one run
   Envir->OneRun();
//   exitcond=Envir->GetT();
//   double end=HRTimeInSec();
}
//---------------------------------------------------------------------------
//eof---------------------------------------------------------------------------
