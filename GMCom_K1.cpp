/** \file
\brief coordinating main.cpp of the GrasslandModell for COMTESS water stress
experiments

This file contains the main() function to start and coordinate the designed
experiments.
*/
//---------------------------------------------------------------------------
//#include <vcl.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

//the only information the GUI needs from the model
#include "environment.h"
#include "CWaterGridEnvir.h"
//------------------------------------------------------------------------------
/**\mainpage Grassland Model (for console) - documentation

\author
Felix May (basic model, ZOI) and Ines Steinhauer (clonal option),
Katrin Koerner (COMTESS functionality)


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

void Init();
void Run();
const int Tinit=100;//100;
const int Tmax=100;//200;
const int nruns=1;//3//10
//-----------------------
/**
  Design of the main trunk version of the IBC-grass_coast model:

  On an 1.28 by 1.28cm square grid 10 for each of 28 PFTs
  (parameterized according to real species) are initiated.
  One environmental setting is tested for 100 years (1 repitition).
  Salinity is zero/constant.
  No block design.

  \par Parameters
  facultative program parameters are:
   -# belowground resource supply <100>
   -# Migration (amount of annual seed rain) <0>
   -# grazing probability = Dist Area Year (Trampling) <0>
   -# NCut (cutting) <0>
   -# WaterLevel <-60>

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
    initLCG(time(NULL), 3487234); // 3487234 ist 'zufällig' gewählt
    CEnvir::NamePftFile="Input\\RSpec59WP3_131114.txt";
  SRunPara::RunPara.Tmax=Tmax;//run time
  SRunPara::RunPara.Tinit=Tinit;//init time
  SRunPara::RunPara.WLseason="const";//const - constant weather conditions
  SRunPara::RunPara.changeVal=5;
  int WLset=0;//originally set WL
  // SRunPara::RunPara.CutLeave=15;
  /// 0-abandoned; 1-grazing; 2-mowing
  //sim-loop
  if (argc>1){
    SRunPara::RunPara.meanBRes=atoi(argv[1]); //belowground resources
    nruns=atoi(argv[2]);  //init types
    SRunPara::RunPara.GrazProb=atof(argv[3]); //grazing
//    SRunPara::RunPara.DistAreaYear=atof(argv[4]); //trampling
    SRunPara::RunPara.AreaEvent=atof(argv[4]); //trampling
    SRunPara::RunPara.NCut=atoi(argv[5]); //number of cuttings
    WLset=atoi(argv[6]); //Water Level set
    SRunPara::RunPara.WaterLevel=WLset; //
    SRunPara::RunPara.salt=atof(argv[7]); //soil salinity
//    SRunPara::RunPara.changeVal=atoi(argv[8]);
  }else exit(99);

    //Run-loop
    for(Envir->RunNr=1;Envir->RunNr<=nruns;Envir->RunNr++){
    	  SRunPara::RunPara.changeVal=0;
//-----------------
    //filenames
    string idstr= SRunPara::RunPara.getRunID();
    stringstream strd;
    strd<<"Output\\Mix_Grid_log_"<<idstr<<"_"<<Envir->RunNr
      <<".txt";
    Envir->NameLogFile=strd.str();     // clear stream
    strd.str("");strd<<"Output\\Mix_gridO_"<<idstr<<"_"//<<Envir->RunNr
      <<".txt";
    Envir->NameGridOutFile=strd.str();
    strd.str("");strd<<"Output\\Mix_typeO_"<<idstr<<"_"//<<Envir->RunNr
      <<".txt";
    Envir->NameSurvOutFile= strd.str();
 //   SRunPara::RunPara.print();
 //-----------------
    //use env params as set in argv
    do{ //one run per grid and deltaWL setting
//        SRunPara::RunPara.WaterLevel+=deltaWL; //set changed conditions
        Envir->SimNr=100+SRunPara::RunPara.changeVal;
        SRunPara::RunPara.Migration+=SRunPara::RunPara.changeVal; //set 2nd control WL

      //sown conditions
      cout<<"changing conditions: SimNr "<<Envir->SimNr<<endl;
      SRunPara::RunPara.Tinit=Tinit;//init time
      Envir=new CWaterGridEnvir(); //generate control grid
      Init(); CEnvir::ResetT();
      Envir->sew();//ploughing and sewing
      Run();
      delete Envir;

      //control conditions (2nd control)
      Envir->SimNr+=1000;//to identify controls
      cout<<"control conditions: SimNr "<<Envir->SimNr<<endl;
 //     SRunPara::RunPara.Tinit=Tmax;//init time
      Envir=new CWaterGridEnvir(); //generate control grid
      Init(); CEnvir::ResetT();
      Run();
      delete Envir;
      Envir->SimNr-=1000;//reset SimNr
//      SRunPara::RunPara.WaterLevel=WLset;//reset base WL

      SRunPara::RunPara.changeVal+=5;//change for next setting
     }while(SRunPara::RunPara.changeVal<=25);//stop if WL increase is more than 20cm
  }//end run

    //delete static pointer vectors
  for (unsigned int i=0;i<SPftTraits::PftList.size();i++)
    delete SPftTraits::PftList[i];
  for (unsigned int j=0;j<SclonalTraits::clonalTraits.size();j++)
    delete SclonalTraits::clonalTraits[j];
  return 0;
}//end main
//---------------------------------------------------------------------------
/**\brief initiate one run
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
