/**\file
   \brief definition of class CGrid and some typedefs
*/
//---------------------------------------------------------------------------
#ifndef GridBaseH
#define GridBaseH
//---------------------------------------------------------------------------
#include "CWaterCell.h"//"Cell.h"
#include "Plant.h"
#include "RunPara.h"
#define CCELL CWaterCell //CCell for old version

struct PftOut;
struct GridOut;

//!iterator type for plant list
typedef vector<CPlant*>::iterator plant_iter;

//!size type for plant list
typedef vector<CPlant*>::size_type plant_size;

//!structure for distance classes of seed output
struct LDD_Dist
{
   long NSeeds[5];
};

//! Class with all spatial algorithms where plant individuals interact in space
/*! Functions for competition and plant growth are overwritten by inherited classes
    to include different degrees of size asymmetry and differen concepts of niche differentiation
*/
class CGrid
{
//   map<string,long>* LDDSeeds; ///< list of seeds to dispers per LDD; has to be managed manually
   double cutted_BM;  ///< biomass removed by mowing
   double grazed_BM;  ///< biomass removed by grazing (aboveground)
   //clonal..
   void RametEstab(CPlant* plant);///< establish ramets
   virtual void EstabLott_help(CSeed* seed);
   virtual void DispSeeds_help(CPlant* plant,CCell* cell);    ///<clonal version

protected:
   ///List of Genets on Grid
   vector<CGenet*> GenetList;
   static const int NDistClass = 5; ///< number of distance classes for seed dispersal
   map<string,LDD_Dist>* LDDSeeds; ///< list of seeds to disperse per LDD; has to be managed manually
//   double mort_seeds;    //!< annual seed mortality  (constant)
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
   void Resshare();                     ///< share ressources among connected ramets
   ///virually generate new spacer
   virtual CPlant* newSpacer(int x,int y, CPlant* plant){
     return new CPlant(x,y,plant);};

//   virtual void DispersSeeds(CPlant* plant);        //!<  seed dispersal
   virtual int DispersSeeds(CPlant* plant);        //!<  seed dispersal
   //!  lottery competition for seedling establishment
   virtual void EstabLottery();
   //! calls seed mortality and mass removal of plants
   virtual void Winter();

   //overload...
//   void DistribResource();              ///<clonal version
//   void PlantLoop();                    ///<clonal version
//   int DispersSeeds(CPlant* plant);    ///<clonal version
//   void EstabLottery();                 ///<clonal version
//   void DeletePlant(CPlant* plant1);    ///<clonal version


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
   vector<CCELL*> CellList;    //!<array of pointers to CCell

   CGrid(); //!< Konstruktor
   CGrid(string id); //!< Konstruktor for loading a file saved grid
   virtual ~CGrid();                 //!< Destruktor
   virtual void resetGrid();

   //! initalization of plants
   virtual void InitPlants(SPftTraits* traits,const int n);
   //! initalization of seeds
   virtual void InitSeeds(SPftTraits* traits,const int n,double estab=1.0);
   //! initalization of seeds
   virtual void InitSeeds(SPftTraits* traits,const int n,int x, int y,double estab=1.0);
//   ///add seeds to ldd-pool of grid
//   void addLDDSeeds(string pft,int nb){(*LDDSeeds)[pft]+=nb;};
//   ///get ldd-pool of grid and clear buffer
//   map<string,long>* getLDDSeeds(){map<string,long>*ldd=LDDSeeds;LDDSeeds=new map<string,long>;return ldd;};
   void GetPftNInd(vector<int>&);
   void GetPftNSeed(vector<int>&);
   void resetCuttedBM(){cutted_BM=0;};
   double getCuttedBM(){return cutted_BM;};
   void resetGrazedBM(){grazed_BM=0;};
   double getGrazedBM(){return grazed_BM;};

   double GetTotalAboveMass();
   double GetTotalBelowMass();
//clonal..   CGridclonal();                       //!< Constructor
//   CGridclonal(string id);              //!< load saved grid
//   virtual ~CGridclonal();              //!< Destructor
//   virtual void resetGrid();

   //new...
   //! initalization of clonal plants
   virtual void InitClonalPlants(SPftTraits* traits,//SclonalTraits* cltraits,
     const int n);
   //! initalization of clonal seeds
   virtual void InitClonalSeeds(SPftTraits* traits,//SclonalTraits* cltraits,
     const int n,double estab=1.0);
   void DispersRamets(CPlant* plant); ///<initiate new ramets
   //service functions...
   void Save(string fname);  ///< file save of entire grid
   int GetNclonalPlants();   ///< number of living clonal plants
   int GetNPlants();         ///< number of living non-clonal plants
   int GetNMotherPlants();   ///< number of living genets
   int GetNdeadPlants();     ///< number of dead plants
   int GetCoveredCells();    ///< number of cells covered
   int GetRootedSoilarea();    ///< number of cells covered
   double GetNGeneration();  ///< number of Generations


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
