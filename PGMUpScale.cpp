//---------------------------------------------------------------------------
//#include <vcl.h>
#include <iostream>
#include "TimeUtils.h"

#pragma hdrstop
///the only information the console needs from the model
#include "SiteScale.h"

//---------------------------------------------------------------------------
/**\mainpage Grassland Model -UpScale (for console) - documentation

\author
Katrin Koerner; basic model is a cowork of
Felix May, Ines Steinhauer and Katrin Koerner

\date 2010-07 (Lina's real-PFT Simulations)
\date 2010-08 Upscaling to site-scale

\par Verbal description of what the code does:
this code runs real (field-observed) PFT-combinations
at site scale

See also: documentation of GMLinaRealPFTKonsole and
publications of May(2008) and Steinhauer(2008) for the basic model

\par Type (function, class, unit, form, ...):
application with some classes

\par Flow chart (for complex code):
\image html flow.jpg "FlowChart of the clonal ModelVersion. In: Steinhauer(2008)"
Flussdiagramm des lokalen Graslandmodells inklusive Wachstumsprozessen klonaler Pflanzen.
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

#pragma argsused
int main(int argc, char* argv[])
{
//init...
//für alle Zellen
  //Berechne Wochen 1-x
  //trage LDD-Seeds in globales Raster
//Verteile LDD-Seeds auf globales Seed-Rain Raster
//für alle Zellen
  //seed migration
  //Berechne Wochen x-end_of_year
//Auswertung
        return 0;
}
//---------------------------------------------------------------------------
