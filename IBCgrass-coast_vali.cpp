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
double GrazProb2=0;      ///<2nd grazing probability
double DistAreaYear2=0;  ///<2nd trampling intensity
int NCut2 =0;            ///<2nd mowing management
//-----------------------
/**
  Design of the 'vali' version of the IBC-grass_coast model:

  On an 100 by 100cm square grid 10 for each of x PFTs
  (parameterized according to real species) are initiated.
  One environmental setting is tested for 50 years (1 repitition).
  Salinity is zero/constant.
  No block design.

  \par Parameters
  facultative program parameters are:
   -# belowground resource supply <100>
   -# Migration (amount of annual seed rain) <0>
   -# grazing probability = Dist Area Year (Trampling) <0>
   -# NCut (cutting) <0>
   -# WaterLevel <-60>
   -# salinity (PSU)

  \par Input
   -species definitions
   -no sim file

  \par Output
   -annual grid information
   -annual type information

  \author KK
  \date 13/05/29
  */
int main(int argc, char* argv[])
{
    initLCG(time(NULL), 3487234); // 3487234 is chosen randomly
//RSpec59WP3_131114.txt
    SRunPara::NamePftFile="Input\\comtessKoerner_131114.txt";
//  bool endsim=false;
  SRunPara::RunPara.WaterLevel=-60; //default, unless set otherwise
  SRunPara::RunPara.Tmax=51;//100;250//Laufzeit
  SRunPara::RunPara.WLseason="const";//const - constant weather conditions
 // SRunPara::RunPara.CutLeave=15;
  int nruns=1;//3//10
  /// 0-abandoned; 1-grazing; 2-mowing
  CEnvir::SimNr=0;
  //sim-loop
  if (argc>1){
    SRunPara::RunPara.meanBRes=atoi(argv[1]); //belowground resources
    SRunPara::RunPara.Migration=5;//atoi(argv[2]);  //init types
    SRunPara::RunPara.GrazProb=atof(argv[2]); //grazing
//    SRunPara::RunPara.DistAreaYear=atof(argv[4]); //trampling
    SRunPara::RunPara.AreaEvent=atof(argv[2])*0.2; //trampling
    SRunPara::RunPara.NCut=atoi(argv[3]); //number of cuttings
    SRunPara::RunPara.WaterLevel=atoi(argv[4]); //number of cuttings
    SRunPara::RunPara.salt=atof(argv[5]); //soil salinity
    SRunPara::NamePftFile=argv[6];
}
  //      //change gridsize
     SRunPara::RunPara.GridSize=SRunPara::RunPara.CellNum=100;//default: 100

    //Run-loop
    for(Envir->RunNr=1;Envir->RunNr<=nruns;Envir->RunNr++){
//      cout<<"new Environment...\n";
      Envir=new CWaterGridEnvir();
      Init();
//-----------------
    //filenames
    string idstr= SRunPara::RunPara.getRunID();
    stringstream strd;
    strd<<"Output\\Mix_Grid_log_"<<idstr
      <<".txt";
    Envir->NameLogFile=strd.str();     // clear stream
    strd.str("");strd<<"Output\\Mix_gridO_"<<idstr
      <<".txt";
    Envir->NameGridOutFile=strd.str();
    strd.str("");strd<<"Output\\Mix_survO_"<<idstr
      <<".txt";
    Envir->NameSurvOutFile= strd.str();
    strd.str("");strd<<"Output\\Mix_typeO_"<<idstr
      <<".txt";
    Envir->NamePftOutFile= strd.str();
 //   SRunPara::RunPara.print();
 //-----------------
      Run();

      //Application output:
      cout<<Envir->year
    		  <<"\t"<<Envir->GridOutData.back()->PftCount
    		  <<"\t"<<Envir->GridOutData.back()->shannon
    		  <<"\t"<<Envir->GridOutData.back()->above_mass<<endl<<flush;

      delete Envir;

    Envir->SimNr++;
    }//end run

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
