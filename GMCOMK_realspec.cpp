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
\date 2012-04 COMTESS real spec (1)


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
- model parameter
  - belowground resources
  - inital species set (M, R, G1, G2)
  - intraannual WaterLevel variation (const , , )
  - variation of above (std 0)

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
 - add resource storage by plants ?!
 - add disturbance impact: cutting, grazing, trampling

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
 - Koerner, Katrin et al. (in prep): belowground herbivory
 - Weiss, Lina et al. (in prep): clonal growth
*/
//---------------------------------------------------------------------------
CWaterGridEnvir* Envir;   ///<environment in which simulations are run
using namespace std;
#pragma argsused

void Init();
void Run();
double GrazProb2=0;      ///<2nd grazing probability
double DistAreaYear2=0;  ///<2nd trampling intensity
int NCut2 =0;            ///<2nd mowing management
//-----------------------
/**
  the new water ressource environment can be tested here

  Tests für Änderung des Management nach 20 Jahren.

  \par Parameters  GrazProb, Trampling(=Grazprob), NCut
  \author KK
  \date 12/01/26
  */
int main(int argc, char* argv[])
{
  bool endsim=false;
  SRunPara::RunPara.WaterLevel=-30; //start-WL   100
  SRunPara::RunPara.Tmax=1;//40;//20Jahre Laufzeit
//  SRunPara::RunPara.Migration=true;
  int nruns=1;//3
  /// 0-abandoned; 1-grazing; 2-mowing
  int management=0;CEnvir::SimNr=1;
  //sim-loop
  do{
//  if (argc>1){
  //für jeden Run neu einlesen, da sonst veränderte Daten übernommen werden
    SRunPara::RunPara.GrazProb=0; //grazing
    SRunPara::RunPara.DistAreaYear=SRunPara::RunPara.GrazProb; //trampling
    SRunPara::RunPara.NCut=0;//atoi(argv[2]); //number of cuttings
//  }
    //Run-loop
    for(Envir->RunNr=1;Envir->RunNr<=nruns;Envir->RunNr++){ //15Runs per Sim
      cout<<"new Environment...\n";
      Envir=new CWaterGridEnvir();
      Init();
      if (SRunPara::RunPara.species.length()<2)continue;
//-----------------
//    //simNr
//    Envir->SimNr=SRunPara::RunPara.WaterLevel+1000;
    //filenames
    string idstr= SRunPara::RunPara.getRunID();
    stringstream strd;
    strd<<"Output\\Mix_Grid_log_"<<idstr
      <<".txt";
    Envir->NameLogFile=strd.str();     // clear stream
    strd.str("");strd<<"Output\\Mix_gridO_"<<idstr
      <<".txt";
    Envir->NameGridOutFile=strd.str();
    strd.str("");strd<<"Output\\Mix_typeO_"<<idstr
      <<".txt";
    Envir->NameSurvOutFile= strd.str();
    SRunPara::RunPara.print();
 //-----------------
      Run();

      delete Envir;

//  if (argc>1){
  //für jeden Run neu einlesen, da sonst veränderte Daten übernommen werden
//    SRunPara::RunPara.GrazProb=atof(argv[1]); //grazing
//    SRunPara::RunPara.DistAreaYear=SRunPara::RunPara.GrazProb; //trampling
//    SRunPara::RunPara.NCut=atoi(argv[2]); //number of cuttings
//  }
    Envir->SimNr++;
    }//end run
//    SRunPara::RunPara.WaterLevel-=15;//5cm weniger für nächste Sim
//    if(SRunPara::RunPara.WaterLevel< -50)
//    endsim=true;
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
/**\brief one run of simulation

Core function is like CClonalGridEnvir::OneRun().
The changes are
 #1 after 20 years the management changes
 #2 every year one little Individual of each Type arrives/establishes
    to prevent species loss

*/
void Run(){
   for (Envir->year=1; Envir->year<=SRunPara::RunPara.Tmax; ++Envir->year){
      cout<<" y"<<Envir->year;
//drift of little individuals -anually-
if (SRunPara::RunPara.Migration){

}//if migration

      Envir->OneYear();
//        WriteSurvival();
        Envir->WriteGridComplete(false);//report last year
//        clonalOutput();
        Envir->WriteSurvival();
      if (Envir->endofrun)break;
   }//years
}
//---------------------------------------------------------------------------
//eof---------------------------------------------------------------------------
