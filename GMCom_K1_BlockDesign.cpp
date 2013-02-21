/** \file
\brief coordinating main.cpp of the GrasslandModell for COMTESS water stress
experiments

This file contains the main() function to start and coordinate the designed
experiments.
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#include <iomanip>
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

This version uses a Block design: after Init Time the control-Grid is saved and
all designs are started at that position.
\ref loadsave "see also page for loading and saving"

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
- input file for COMTESS-specific PFT-definitions (species list)
- (optional) five input variables
  - Belowground resources
  - Migration [0,1]
  - Aboveground grazing probability
  - Trampling (disturbed area per year [\%])
  - Mowing management [0..3]

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

  teste 'reale' Arten -
  \depreciated jede Art ein Jahr: wie entwickeln sich die Keimlinge, entstehen Rameten / neue Keimlinge?


  \par Parameters  GrazProb, Trampling(=Grazprob), NCut
  \author KK
  \date 12/01/26
  */
int main(int argc, char* argv[])
{
  bool endsim=false;
   if (argc>=2) {
      string file = argv[1];
      CEnvir::NameSimFile="Input\\"+file;
   }else CEnvir::NameSimFile="Input\\comtest.txt";
   int maxRun=1; if (argc>=3) {maxRun = atoi(argv[2]);}
//   SRunPara::RunPara.GridSize=SRunPara::RunPara.CellNum=50; //make smaller for develloping process
   SRunPara::RunPara.Migration=2;
   //hier: loop verschiedener Grids
   for (CEnvir::RunNr=1;CEnvir::RunNr<=maxRun;CEnvir::RunNr++){
     //erstes Grid und Kontrolle
     cout<<"start master Environment...\n";
     Envir=new CWaterGridEnvir();  //lade hier gespeicherte Version
     int lpos=Envir->GetSim();
           Init();
     CEnvir::ResetT();
     //do simulations specified in input-file
     do{ //one run per grid - block-design
            Run();
        cout<<"new Environment...\n";
        //lade hier gespeicherte Version
        stringstream v; v<<"B"<<setw(3)<<setfill('0')<<CEnvir::RunNr;
        delete Envir; Envir=new CWaterGridEnvir(v.str());
      lpos=Envir->GetSim(lpos);
   }while (lpos!=-1);

   delete Envir;
   }//end for x master grids
   //delete static pointer vectors
  for (unsigned int i=0;i<SPftTraits::PftList.size();i++)
    delete SPftTraits::PftList[i];
  for (unsigned int j=0;j<SclonalTraits::clonalTraits.size();j++)
    delete SclonalTraits::clonalTraits[j];
  for (unsigned int i=0;i<SWaterTraits::PFTWaterList.size();i++)
    delete SWaterTraits::PFTWaterList[i];
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
   Envir->OneRun();
}
//---------------------------------------------------------------------------
//eof---------------------------------------------------------------------------
