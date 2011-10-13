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
#define CCELL CWaterCell //CCell for old version

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
   map<string,long>* LDDSeeds; ///< list of seeds to dispers per LDD; has to be managed manually
   double cutted_BM;  ///< biomass removed by mowing

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

//   virtual void DispersSeeds(CPlant* plant);        //!<  seed dispersal
   virtual int DispersSeeds(CPlant* plant);        //!<  seed dispersal
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

   virtual void SetCellResource();     ///< set amount of resources the cells serve

public:

   vector<CPlant*> PlantList;    //!< List of plant individuals
   CCELL** CellList;    //!<array of pointers to CCell
//  CWaterCell** CellList;    //!<array of pointers to CCell

   CGrid(); //!< Konstruktor
   virtual ~CGrid();                 //!< Destruktor
   virtual void resetGrid();

   //! initalization of plants
   virtual void InitPlants(SPftTraits* traits,const int n);
   //! initalization of seeds
   virtual void InitSeeds(SPftTraits* traits,const int n,double estab=1.0);
   ///add seeds to ldd-pool of grid
   void addLDDSeeds(string pft,int nb){(*LDDSeeds)[pft]+=nb;};
   ///get ldd-pool of grid and clear buffer
   map<string,long>* getLDDSeeds(){map<string,long>*ldd=LDDSeeds;LDDSeeds=new map<string,long>;return ldd;};
   void GetPftNInd(vector<int>&);
   void GetPftNSeed(vector<int>&);
   void resetCuttedBM(){cutted_BM=0;};
   double getCuttedBM(){return cutted_BM;};

   double GetTotalAboveMass();
   double GetTotalBelowMass();
};
   ///vector of cell indices increasing in distance to grid center
static vector<int> ZOIBase;
   //! periodic boundary conditions
void Boundary(int& xx,int& yy);
   /// test for emmigration
bool Emmigrates(int& xx,int& yy);
   ///dispersal kernel for seeds
void getTargetCell(int& xx,int& yy,const float mean,const float sd,double cellscale=0);
   //! distance between two points using Pythagoras
double Distance(const double& xx, const double& yy,
                    const double& x=0, const double& y=0);
  ///compare two index-values in their distance to the center of grid
bool CompareIndexRel(int i1, int i2);

//---------------------------------------------------------------------------
#endif
