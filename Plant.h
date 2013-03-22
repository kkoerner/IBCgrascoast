#ifndef PlantH
#define PlantH

#include <math.h>
#include "CObject.h"
#include "RunPara.h"
//#include "Seed.h"
#include <vector>
using namespace std;


//---------------------------------------------------------------------------

const double Pi=3.14159265358979323846;

//---------------------------------------------------------------------------
//! Structure to store all PFT Parameters
struct SPftTraits   //plant functional traits
{
   static vector<SPftTraits*> PftList;        //!< List for Pft parameters
//   static map<string,SPftTraits*> PftList;        //!< List for Pft parameters
   static void ReadPftStrategy(char* file="");    //!> read PFT parameters from the input file

   int TypeID;     //!< PFT ID same number for all individuals of one PFT
   string name;   ///< name of functional type
   int N0;         ///< number of initial individuals
   int MaxAge;     ///< maximum age of plants
   double LMR;     //!< leaf mass ratio (LMR) (leaf mass per shoot mass) [0;1] 1 -> only leafs, 0 -> only stem
   double SLA;     //!< specific leaf area (SLA) equal to cshoot in the model description (leaf area per leaf mass)
   double RAR;     //!< root area ratio (root area per root mass) equal to croot in the model description
   double m0;      ///< initial masses of root and shoot
   double MaxMass; //!< maximum individual mass
   double AllocSeed;  //!< constant proportion that is allocated to seeds between FlowerWeek and DispWeek
   double SeedMass;   //!< Seed mass (mass of ONE seed)
   double Dist;    //!< mean dispersal distance (and standard deviation of the dispersal kernel
   int    Dorm;    //!< maximum seed longevity
   double pEstab;  //!< annual probability of establishment

   //! maximal resource utilization per ZOI area per time step
   /*!< (optimum uptake for two layers : LimRes=2*Gmax)
   (optimum uptake for one layer : LimRes=Gmax)
   */
   double Gmax;
   //! above-ground competitive ability
   inline double CompPowerA()const {return 1.0/LMR*Gmax;};
   //! below-ground competitive ability
   inline double CompPowerB()const {return Gmax;};
    //! fraction of above-ground biomass removal if a plant is grazed
   inline double GrazFac()const {return 1.0/LMR*palat;};

   double palat;   //!< Palatability -> susceptability towards grazing
   int    memory;    //!< equal to surv_max in the model description -> maximal time of survival under stress
   double mThres;  //!< Fraction of maximum uptake that is considered as resource stress
   double growth;  //!< concersion rate  resource -> biomass [mass/resource unit]
   int    FlowerWeek; //!< week of start of seed production
   int    DispWeek;   //!< week of seed dispersal (and end of seed production)
   void SetDefault();   ///< set default trait values (eq. 'PFT1')
   SPftTraits();
   virtual ~SPftTraits(){};
   void print();

};
//-----------------------------------------------
class CSeed;class CCell;
//! Class that describes plant individuals
class CPlant : public CObject
{
protected:
   CCell* cell;      ///<cell where it sits
   virtual double ReproGrow(double uptake);
   virtual double ShootGrow(double shres);
   virtual double RootGrow(double rres);

public:
   SPftTraits* Traits;///<PFT Traits
   double xcoord;   //!< location of plant's central point
   double ycoord;   //!< location of plant's central point

   double mshoot;   //!< shoot mass
   double mroot;    //!< root mass
   double mRepro;    //!< reproductive mass (which is converted to seeds)

//   double alloc_shoot;     //!< root-shoot resource partition coefficient

//   double Ashoot;   //!< area of above-ground ZOI [cm^2]
//   double Aroot;    //!< area of below-ground ZOI [cm^2]

   double Ash_disc; //!< discrete above-ground ZOI area [number of cells covered * area of one cell]
   double Art_disc; //!< discrete below-ground ZOI area [number of cells covered * area of one cell]
   double Aroots_all;  ///<area of all species' roots in ZOI
   double Aroots_type; ///<area of all PFT's roots in ZOI

//   double rsh;      //!< radius of above-ground ZOI [cm]
//   double rrt;      //!< radius of below-ground ZOI [cm]

   double Auptake; //!< uptake of above-ground resource in one time step
   double Buptake; //!< uptake below-ground resource one time step

   bool dead;      //!< plant dead or alive?
   bool remove;    //!< trampled or not - should the plant be removed?

   int stress;     //!< counter for weeks with resource stress exposure
   int Age;        ///< age of (established) plant in years (ageing in winter)

//   int LimitRes;   //!< limiting resource: 1->above, 2->below 0->equal (not used)

   //functions
   //! constructor for plant objects
   CPlant(double x, double y,SPftTraits* Traits);
   CPlant(SPftTraits* Traits,CCell* cell,
     double mshoot=0, double mroot=0, double mrepro=0,
     int stress=0, bool dead=false);
   ///make a plant from a clonal-seed object
   CPlant(CSeed* seed);
   //! initalization of one plant
//   CPlant(SPftTraits* traits,CCell* cell,
//     double mshoot, double mroot, double mrepro, int stress, bool dead);
   virtual ~CPlant();  //!<destruktor
   virtual string type();  ///<say what you are
   virtual string pft();   ///<say what a pft you are
   virtual string asString(); ///<report plant's status
//   void Allometrics(); //!< calculates ZOI areas (above and below) from shoot and root mass
   double Area_shoot();
   double Area_root();
   double Radius_shoot();
   double Radius_root();
/// \brief get plant's height
/// \param cheight mg vegetative plant mass per cm height
/// \return plant height in cm
/// \include SLA in some way (as is in Area_shoot())?
///
  virtual double getHeight(double const cheight = 6.5){
    return pow(mshoot/(Traits->LMR),1/3.0)*cheight;};
/// \brief get plant's depth
/// \param cdepth mg vegetative root mass per cm ground layer  (root density)
/// \return depth of root cylinder in cm
/// calculation: devide root mass by ZOI-area
/// (calibrated: 2500mg DWroot beeing 50cm)
/// \note That is more quick and dirty. One might include a new type
///  parameter analogous to LMR.
///
///
  virtual double getDepth(double const cdepth = 4.0){   //6.5
    return (mroot*this->Traits->RAR)/this->Area_root()*cdepth;};

   ///competition coefficient for a plant -needed for AboveComp and BelowComp
   virtual double comp_coef(const int layer,const int symmetry)const;
   //! shoot-root resource allocation and plant growth in two layers (one-layer option obsolete now)
   virtual void Grow2(); //!< shoot-root resource allocation and plant growth in two layers
   virtual bool stressed();///< return true if plant is stressed
/// lower threshold of aboveground resource uptake (light stress thresh.)
   virtual double minresA(){return Traits->mThres*Ash_disc*Traits->Gmax*2;}
/// lower threshold of belowground resource uptake (nutrient stress thresh.)
   virtual double minresB(){return Traits->mThres*Art_disc*Traits->Gmax*2;}
   void DecomposeDead();     //!< calculate mass shrinkage of dead plants
   void Kill();  //!< Mortality due to resource shortage or at random

   void WinterLoss(); //!< removal of above-ground biomass in winter

   double RemoveMass();  //!< removal of above-ground biomass by grazing
   /// removal of belowground biomass by grazing
   double RemoveRootMass(const double prop_remove=0.5);
//   void Decompose();    //!< decomposition of dead plants (mass shrinkage)

   void setCell(CCell* cell); ///<define cell for plant
   inline CCell* getCell(){return cell;};///<returns central cell
   virtual double GetMass(){return mshoot+mroot+mRepro;};//!< returns plant mass
   virtual int GetNSeeds();     //!< returns number of seeds of one plant individual
};

//functions that are used for STL algorithms (sort + partition)
//-----------------------------------------------------------------------------
/// sort plant individuals descending after shoot size * palatability

/// (necessary to apply algorithms from STL)
///
bool ComparePalat(const CPlant*& plant1, const CPlant*& plant2)
{
   return ((plant1->mshoot*plant1->Traits->GrazFac())
      > (plant2->mshoot*plant2->Traits->GrazFac()));
}
//-----------------------------------------------------------------------------
/// sort plants descending after shoot size (mass*1/LMR)
/**\bug '*' in secound line solved 100202*/
bool CompareShoot(const CPlant*& plant1, const CPlant*& plant2)
{
   return ((plant1->mshoot/plant1->Traits->LMR)
       > (plant2->mshoot/plant2->Traits->LMR));
}
//-----------------------------------------------------------------------------
/// sort plants descending after root mass
bool CompareRoot(const CPlant*& plant1, const CPlant*& plant2)
{
   return ((plant1->mroot) > (plant2->mroot));
}
//-----------------------------------------------------------------------------
//! return if plant should be removed (necessary to apply algorithms from STL)
bool GetPlantRemove(const CPlant*& plant1)
{
   return (!plant1->remove);
}
//---------------------------------------------------------------------------
bool is_left(const CPlant*& plant1)
{
   return (plant1->xcoord < SRunPara::RunPara.CellNum/2);
}
//-----------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
