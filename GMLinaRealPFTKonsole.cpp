//---------------------------------------------------------------------------
#include <vcl.h>
#include <iostream>
#include "TimeUtils.h"

#pragma hdrstop

///the only information the GUI needs from the model
#include "environment.h"
//------------------------------------------------------------------------------
/**\mainpage Grassland Model (for console) - documentation

\author
Felix May (basic model, ZOI, cutting option) and
Ines Steinhauer (clonal option),
Katrin Koerner (revision and rebuilt Felix' grazing experiments,
  belowground grazing)

\date 2008-02-13 (model)
\date 2009-05 (revision)
\date 2010-01 (Felix' grazing rebuilt and belowground grazing)
\date 2010-03 (Ines' clonal plants' rebuilt)
\date 2010-07 (Lina's real-PFT Simulations)

\par Verbal description of what the code does:
this code runs real (field-observed) PFT-combinations

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
- input file for plant types initially used
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
//---------------------------------------------------------------------------
CClonalGridEnvir* Envir;   ///<environment in which simulations are run
using namespace std;
#pragma argsused

void Init();
void Run();

/**
  The experimental design is read from file
  <TT>'Input/SimFileTest1.txt'</TT>
*/
int main(int argc, char* argv[])
{
   if (argc>=2) {
      string file = argv[1];
      CEnvir::NameSimFile="Input\\"+file;
   }else CEnvir::NameSimFile="Input\\SimFile.txt";
   cout<<"new Environment...clonal community...\n";
   Envir=new CClonalGridEnvir();
   using CClonalGridEnvir::RunNr;

   //do invasion experiments after ines steinhauer
   //do simulations specified in input-file
   int lpos=Envir->GetSim();
   do{
      {
         cout<<"\n  Simulation No. "<<Envir->SimNr;
//         cout<<"plant type .. "<<Pfttype+1<<endl;
//         cout<<"clonal type .. "<<clonaltype+1<<endl;
         for (RunNr=0;RunNr<Envir->NRep;RunNr++) //def:20
         {
            cout<<"\nrun "<<RunNr+1<<"; ";
            Init();
            Run();
         }
      }
      lpos=Envir->GetSim(lpos);
   }while (lpos!=-1);

   delete Envir;
   //delete static pointer vectors
   for (unsigned int i=0;i<SPftTraits::PftList.size();i++)
    delete SPftTraits::PftList[i];
   for (unsigned int j=0;j<SclonalTraits::clonalTraits.size();j++)
    delete SclonalTraits::clonalTraits[j];
   return 0;
}
//---------------------------------------------------------------------------
void Init(){
      Envir->InitRun();
}
//------------------------------------------------
void writeBenchmark(int pft,int clpft,int run,int week, double time){
  ofstream of("Output\\extTimes.txt",ios_base::app);
  of<<pft<<'\t'<<clpft<<'\t'<<run<<'\t'<<week<<'\t'<<time<<'\n';
}
//------------------------------------------------
void Run(){
   int exitcond=0;
   double start=HRTimeInSec();
   Envir->OneRun();
   exitcond=Envir->GetT();
   double end=HRTimeInSec();
}
//---------------------------------------------------------------------------
//eof---------------------------------------------------------------------------
