//---------------------------------------------------------------------------
#include <iostream>

//the only information the GUI needs from the model
#include "CGridEnvir.h"
#include <ctime>
//------------------------------------------------------------------------------
/**\mainpage Grassland Model (for console) - documentation

\authors
 - Florian Jeltsch (idea, basic model concept, and supervision),
 - Felix May (basic model, ZOI, cutting option, and seed rain option),
 - Ines Steinhauer (clonal option),
 - Lina Weiss (sensitivity analysis, adoption to real communities, and validation),
 - Katrin Koerner (revision and rebuilt Felix' grazing experiments, and
  belowground grazing option)

\par Verbal description of what the code does:
The model describes the development of a community of
perennial plant functional types (PFTs).
It is based on the grassland model developed by May et al. (2009),
augmented by the incorporation of clonal plant types.

See also: \ref ODDbase "ODD description" (by L.Weiss), also in publications of
Weiss et al. (2014), Koerner et al. (2014) and May(2009)

\par Type (function, class, unit, form, ...):
console application with some classes

\par Flow chart (for complex code):
\image html IBC-Grass_2x-flow.png "FlowChart of the clonal ModelVersion. In: Weiss et al.(2014)"

Flow chart of the model's processes, which are run for each individual plant. 
Processes related to clonal growth are marked by grey lines and boxes. 
Weekly schedule is coordinated in function CGridEnvir::OneWeek().

\par Expected input (type and range of values, units):
- an input file for successive simulations (SRunPara::NameSimFile,e.g. 'SimFile.txt', given as program argument)
with following structure:
  -# Simulation ID (int)
  -# Tmax number of years to simulate
  -# ARes aboveground resources (per cell)
  -# BRes belowground resources
  -# GrazProb weekly grazing probability (CGrid::Grazing(), \ref sgraz ODD)
  -# PropRemove portion biomass removed if grazed
  -# MassUngraz: Residual Mass ungrazable
  -# DistAreaYear: Trampling
  -# AreaEvent: Trampling
  -# NCut: Cutting number of Cuttings
  -# CutMass: residual Mass - Cutting depth
  -# SeedRainType SRunPara::SeedRainType
  -# SeedInput SRunPara::SeedInput
  -# file name of PFT-definitions (SRunPara::NamePftFile)
- input file (SRunPara::NamePftFile) with parameter settings for each plant functional type used

\sa CEnvir::GetSim()

\par Output (type and range of values, units):
- some ASCII-coded *.txt-files with weekly or yearly numbers
  of individuals or other summarizing variables on PFT or Grid level
- the Output is coordinated by CEnvir::WriteOutput()

\par Requirements and environment (libraries, headers, Borland Builder):
- IDE: Eclipse Compiler: MinGW and standard libraries on Win-based system

\par Sensitivity analysis (or reference to publication):
see Weiss et al (2014), ...

\par Validation (or reference to publication):
ongoing ...

\par Sources or reasons for parameter values, methods, equations:
See publications of May(2008) and Steinhauer(2008) and \ref ODDbase (\ref straits and \ref spftdef).

\par bugs and todos
See additional pages for solved and unsolved bugs (\ref bug) and todos (\ref todo)

\par Model History

\date 2008-02-13 (model)
\date 2009-05 (revision)
\date 2010-01 (Felix' grazing rebuilt and belowground grazing)
\date 2010-03 (Ines' clonal plants' rebuilt)
\date 2010-07 (Lina's real-PFT Simulations)
\date 2014-04 (Update2, Felix' seed rain option)

\copyright All rights belong to the Plant Ecology and Conservation
Biology group at the University of Potsdam

\par Publications or applications referring to the code:
- Weiss L, H Pfestorf, F May, K Koerner, S Boch, M Fischer, J Mueller,
  D Prati, S Socher , F Jeltsch (2014)
  Grazing response patterns indicate isolation of semi-natural
  European grasslands. Oikos 123 (5) 599-612.
- Koerner, K., Pfestorf, H., May, F., Jeltsch, F., 2014.
  Modelling the effect of belowground herbivory on grassland diversity.
  Ecological Modelling 273, 79-85.
- May, Felix, Grimm, Volker and Jeltsch, Florian (2009): Reversed effects of
  grazing on plant diversity: the role of belowground competition
  and size symmetry. Oikos 118: 1830-1843.
- Steinhauer, Ines (2008): KOEXISTENZ IN GRASLANDGESELLSCHAFTEN -
  Modellgestuetzte Untersuchungen unter Beruecksichtigung klonaler Arten.
  Diplomarbeit Universitaet Potsdam
- May, Felix (2008): Modelling coexistence of plant functional types
  in grassland communities - the role of above- and below-ground competition.
  Diploma thesis Potsdam University.
*/
//---------------------------------------------------------------------------
CGridEnvir* Envir;   ///<environment in which simulations are run
using namespace std;

void Init();
void Run();

/**
 * Program launch - Model Design is defined.
 * Here the model design is read from file.
 * Grid size is constant and 1m� (100*100cells) at moment.
 * @param argc number of program parameters (>1 if parameters are given)
 * @param argv list of program parameters (SimFileName is expected)
 * \sa SRunPara::NameSimFile
 * @return zero if simulations are done
 * \sa CGridEnvir
 * \sa CGridEnvir::OneRun()
 */
int main(int argc, char* argv[])
{
	initLCG(time(NULL), 3487234); // setze den Zufallsgenerator auf einen neuen Wert, 3487234 ist 'zuf�llig' gew�hlt

	if (argc>=2) {
      string file = argv[1];
      SRunPara::NameSimFile="Input\\"+file;
   }else SRunPara::NameSimFile="Input\\SimFile.txt";
   cout<<"new Environment...clonal community...\n";
//      //change gridsize to 3qm
//   SRunPara::RunPara.GridSize=SRunPara::RunPara.CellNum=173; //if pot experiments are to be reproduced than set to 12
   Envir=new CGridEnvir();

   //do simulations specified in input-file
   int lpos=Envir->GetSim(); //extern file input
   do{
      {
         cout<<"\n  Simulation No. "<<Envir->SimNr;
         for (Envir->RunNr=0;Envir->RunNr<Envir->NRep;Envir->RunNr++) //def:20
         {
            cout<<"\nrun "<<Envir->RunNr+1<<"; ";
            Init();
            Run();
         }
      }
      lpos=Envir->GetSim(lpos);
   }while (lpos!=-1);

   delete Envir;
   //delete static pointer vectors
   for (map<string,SPftTraits*>::iterator i=SPftTraits::PftLinkList.begin();
		   i!=SPftTraits::PftLinkList.end(); ++i) delete i->second;
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
   Envir->OneRun();
   exitcond=Envir->GetT();
}
//---------------------------------------------------------------------------
//eof---------------------------------------------------------------------------
