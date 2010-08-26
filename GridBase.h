/**\file
   \brief definition of class CGrid and some typedefs
*/
//---------------------------------------------------------------------------
#ifndef GridBaseH
#define GridBaseH
//---------------------------------------------------------------------------
#include "Cell.h"
#include "Plant.h"
#include "RunPara.h"

struct PftOut;
struct GridOut;

//!iterator type for plant list
typedef vector<CPlant*>::iterator plant_iter;

//!size type for plant list
typedef vector<CPlant*>::size_type plant_size;


//! Class with all spatial algorithms where plant individuals interact in space
/*! Functions for competition and plant growth are overwritten by inherited classes
    to include different degrees of size asymmetry and differen concepts of niche differentiation
*/
class CGrid
{
protected:
   double mort_seeds;    //!< annual seed mortality  (constant)
   //! assigns grid cells to plants - which cell is covered by which plant
   virtual void CoverCells();
   void CalcRootInteraction();
   void CalcRootInteraction(CPlant* plant);
   //! removes dead plants from the grid and deletes them
   virtual void RemovePlants();
   virtual void DeletePlant(CPlant* plant1); //!< delete plant object
    //!  loop over all plants including growth, seed dispersal and mortality
   virtual void PlantLoop();
   //! distributes resource to each plant --> calls competition functions
   virtual void DistribResource();

   virtual void DispersSeeds(CPlant* plant);        //!<  seed dispersal
   //!  lottery competition for seedling establishment
   virtual void EstabLottery();
   //! calls seed mortality and mass removal of plants
   virtual void Winter();

   void ResetWeeklyVariables();//!< clears list of plants that cover each cell
   void SeedMortAge();         //!<  seed mortality in winter due to ageing of seeds
   void SeedMortWinter();      //!< random seed mortality in winter
   //! disturbance --> calls grazing and gap formation functions
   bool Disturb();
   /// simulates aboveground herbivory
   void Grazing();
   /// simulates belowground grazing
   void GrazingBelGr(const int mode=0);
   void Trampling();           //!< gap formation
   void Cutting();             //!< cutting of all plants to equal aboveground mass
   void CellsInit();           //!< initalization of cells

   void SetCellResource();     ///< set amount of resources the cells serve

public:

   vector<CPlant*> PlantList;    //!< List of plant individuals
   CCell** CellList;    //!<array of pointers to CCell

   CGrid(); //!< Konstruktor
   virtual ~CGrid();                 //!< Destruktor
   virtual void resetGrid();

   //! initalization of plants
   virtual void InitPlants(SPftTraits* traits,const int n);
   //! initalization of seeds
   virtual void InitSeeds(SPftTraits* traits,const int n);

   void GetPftNInd(vector<int>&);
   void GetPftNSeed(vector<int>&);

   double GetTotalAboveMass();
   double GetTotalBelowMass();
};
   ///vector of cell indices increasing in distance to grid center
vector<int> ZOIBase;
   //! periodic boundary conditions
void Boundary(int& xx,int& yy);
   //! distance between two points using Pythagoras
double Distance(const double& xx, const double& yy,
                    const double& x=0, const double& y=0);
  ///compare two index-values in their distance to the center of grid
bool CompareIndexRel(int i1, int i2);

//---------------------------------------------------------------------------
#endif
