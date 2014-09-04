/** \file
\brief coordinating main.cpp of the GrasslandModell for COMTESS water stress
experiments

This file contains the main() function to start and coordinate the designed
experiments.
*/
//---------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <ctime>


//the only information the GUI needs from the model
//#include "environment.h"
#include "CWaterGridEnvir.h"
//------------------------------------------------------------------------------
/**\mainpage Grassland Model (for console) - documentation

\authors
 - Florian Jeltsch (idea, basic model concept, and supervision),
 - Felix May (basic model, ZOI, cutting option, and seed rain option),
 - Ines Steinhauer (clonal option),
 - Lina Weiss (sensitivity analysis, adoption to real communities, and validation),
 - Katrin Koerner (revision and rebuilt Felix' grazing experiments, and
  belowground grazing option, COMTESS)

\par Verbal description of what the code does:
The model describes the development of a community of
perennial plant functional types (PFTs).
It is based on the grassland model developed by May et al. (2009),
augmented by the incorporation of clonal plant types.

<b>Model Version IBC-grass_coast with respect to wet and/or salty plot conditions.</b>

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
- input file for COMTESS-specific PFT-definitions (species list)
- (optional) five input variables
  - Belowground resources
  - Migration [0,1]
  - Aboveground grazing probability
  - Trampling (disturbed area per year [\%])
  - Mowing management [0..3]

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
\date 2011-08 ff COMTESS
\date 2012-04 COMTESS real spec (1)
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
CWaterGridEnvir* Envir;   ///<environment in which simulations are run
using namespace std;

void Init();
void Run();
const int Tinit=100;//100;
const int Tmax=200;//200;
const int nruns=3;//3//10
//-----------------------
/**
  Design of the main trunk version of the IBC-grass_coast model:

  On an 1 by 1m square grid 10 for each of x PFTs
  (parameterized according to real species) are initiated.
  One environmental setting is tested for 100 years (1 repitition).
  Salinity is zero/constant.
  No block design.

  \par Parameters
  facultative program parameters are:
   -# file name of sim file
   -# file name of pft definition file (regional species pool)


  \par Input
   -species definitions
   -sim file with definition of 'init' and 'effect' environmental settings
     -# belowground resource supply <100>
     -# Migration (amount of annual seed rain) <0>
     -# grazing probability = Dist Area Year (Trampling) <0>
     -# NCut (cutting) <0>
     -# WaterLevel <-60>
     -# salinity

  \par Output
   -annual grid information
   -annual type information

  \author KK
  \date 14/09/03
  */
int main(int argc, char* argv[])
{
    initLCG(time(NULL), 3487234); // 3487234 is chosen randomly
  SRunPara::RunPara.Tmax=50;//50;//Init time
  int tmax=150;//200;//time to run
  SRunPara::RunPara.WLseason="const";//const - constant weather conditions
  CEnvir::SimNr=0;
  //      //change gridsize
     SRunPara::RunPara.GridSize=SRunPara::RunPara.CellNum=100;//default: 100
//     bool endsim=false;
      if (argc>=2) {
         string file = argv[1];
         SRunPara::NameSimFile=file;
         file=argv[2];
         SRunPara::NamePftFile=file;
      }//else CEnvir::NameSimFile="Input\\comtest.txt";
      int maxRun=1; if (argc>3) {maxRun = atoi(argv[3]);}
      //fill PftLinkList
    SWaterTraits::ReadPFTDef(SRunPara::NamePftFile,-1);

    //hier: loop verschiedener Grids
     for (CEnvir::RunNr=1;CEnvir::RunNr<=maxRun;CEnvir::RunNr++){
       //erstes Grid und Kontrolle
       cout<<"start init Environment...\n";
       Envir=new CWaterGridEnvir();  //erstelle neues Grid
       int lpos=Envir->GetSim();
       int startID=CEnvir::SimNr;
             Init();
       CEnvir::ResetT();
       //-----------------
           //filenames
           string idstr= SRunPara::NameSimFile;SRunPara::RunPara.getRunID();
           stringstream strd;
           strd<<"Output\\Mix_Grid_log_"<<idstr;
//             <<".txt";
           Envir->NameLogFile=strd.str();     // clear stream
           strd.str("");strd<<"Output\\Mix_gridO_"<<idstr;
//             <<".txt";
           Envir->NameGridOutFile=strd.str();
           strd.str("");strd<<"Output\\Mix_survO_"<<idstr;
//             <<".txt";
           Envir->NameSurvOutFile= strd.str();
           strd.str("");strd<<"Output\\Mix_typeO_"<<idstr;
//             <<".txt";
           Envir->NamePftOutFile= strd.str();
        //   SRunPara::RunPara.print();
        //-----------------
           Run();//until end of init time
           SRunPara::RunPara.Tmax=tmax; //set max time
               //do simulations specified in input-file
           lpos=Envir->GetSim(lpos);
           int controlID=CEnvir::SimNr;CEnvir::SimNr=startID;
              Run();//until end
          cout<<"control Environment...\n";
          // start control run
          delete Envir; Envir=new CWaterGridEnvir();
          //reset year and week
          CEnvir::ResetT();CEnvir::SimNr=controlID;
          Init();//init
          Run(); //control run
     delete Envir;
     }//end for x master grids

    //delete static pointer vectors
    for (map<string,SPftTraits*>::iterator i=SPftTraits::PftLinkList.begin();
 		   i!=SPftTraits::PftLinkList.end(); ++i) delete i->second;
  return 0;
}//end main
//---------------------------------------------------------------------------
/**
      high belowground ressources
*/
void Init(){
      Envir->InitRun();
}
//------------------------------------------------
/**\brief one run of simulation

Core function is like CClonalGridEnvir::OneRun().
The changes are
 #1 after init time the management might change
 #2 every year one unspecified migration occurs
    to prevent species loss

*/
void Run(){
   Envir->OneRun();
}
//---------------------------------------------------------------------------
//eof---------------------------------------------------------------------------
