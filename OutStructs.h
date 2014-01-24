//---------------------------------------------------------------------------

#ifndef OutStructsH
#define OutStructsH

//#include <vector>
#include <map>
#include <string>
using namespace std;
//---------------------------------------------------------------------------
//! Structure with output data for each PFT
struct SPftOut
{
   struct SPftSingle{
//     string name;        //!<list of active functional type names
     double totmass;     //!< total biomass
     double shootmass;
     double rootmass;
     double cover;
     int Nind;        //!< population size
     int Nseeds;       //!< number of seeds
     SPftSingle();
     ~SPftSingle(){};
   };

   int week;                  //!< week of the year (1-30)
//   vector<SPftSingle*> PFT;    //!< list of active PFTs
   map<string,SPftSingle*> PFT;                 //!< list of active PFTs
//   vector<string> PFT;
//   vector<double> totmass;
//   vector<double> shootmass;
//   vector<double> rootmass;
//   vector<int> Nind;        //!< population size
//   vector<int> Nseeds;       //!< number of seeds
//can be activated as needed:
//  --be careful to update constructors and destructors as well
//   vector<double> meanmass;  //!< mean individual mass
//   vector<double> ssmass;   //!< sum of squares of individual masses (for calculation of std)
//   vector<double> sdmass;   //!< standard deviation (std) of individual masses
//   vector<double> alloc;    //!< mean allocation coefficient
//   vector<double> stress;   //!< mean stress counter
//   vector<double> Ashoot;   //!< shoot area [cm²]
//   vector<double> Aroot;    //!< root area  [cm²]
//   vector<double> repromass; //!< reproductive mass (which can be cobverted to seeds
//   vector<int> Ash_disc;     //!< discrete shoot area (in #cells covered instead of cm²)
//   vector<int> Art_disc;     //!< discrete root area (in #cells covered instead of cm²)
   SPftOut();
   ~SPftOut();
};
//---------------------------------------------------------------------------

//! Structure with output data for the whole grid
struct SGridOut
{
   int week;                 //!< week of the year (1-30)
   int PftCount;            //!< number surviving PFTs
   double totmass;          //!< total biomass (sum over all PFTs
   int Nind;                //!< number of plants
   double shannon;          //!< shannon diversity index
   double above_mass;       //!< total above-ground mass
   double below_mass;       //!< total below-ground mass
   double aresmean;         //!< mean above-ground resource availability
   double bresmean;         //!< mean below-ground resource availability
   double cutted;           ///< cutted biomass
   double grazed;           ///< cutted biomass
   double WaterLevel;      ///< current mean WaterLevel on grid
   double bareGround;      ///< portion bare ground
   double bareSoil;      ///< portion bare soil
   double above_litter;    ///< aboveground litter DW
   double below_litter;    ///< belowground litter DW
   SGridOut();
};

//---------------------------------------------------------------------------

//! Structure with output data for clonal plants
struct SClonOut
{
   int week;                 //!< week of the year (1-30)
   int NPlants;              ///< nb non-clonal plants
   int NclonalPlants;       ///< nb clonal plants
   int NGenets;             ///< Number of living genets
   double MPlants;          //!< total biomass non-clonal plants
   double MclonalPlants;          //!< total biomass clonal plants
   double MeanGenetsize;          ///< mean size of genets
   double MeanGeneration;         ///< mean number of generations
   SClonOut();
};
#endif
