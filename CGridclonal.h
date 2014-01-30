/**\file
   \brief definition of class CGridclonal which inherits from CGrid
*/
//---------------------------------------------------------------------------
#ifndef CGridclonalH
#define CGridclonalH
//---------------------------------------------------------------------------
#include "GridBase.h"
#include "CGenet.h"
#include "CclonalSeed.h"
#include "ClonalPlant.h"

/**\brief clonal grid class inherits from CGrid

   \since clonal version
*/
class CGridclonal : public CGrid
{
protected:
   void Resshare();                     ///< share ressources among connected ramets

private:
   //new...
   void RametEstab(CclonalPlant* plant);///< establish ramets
   virtual void EstabLott_help(CSeed* seed);
   virtual void DispSeeds_help(CPlant* plant,CCell* cell);    ///<clonal version
   virtual CclonalPlant* newSpacer(int x,int y, CclonalPlant* plant){
     return new CclonalPlant(x,y,plant);};
protected:
   ///List of Genets on Grid
   vector<CGenet*> GenetList;
   //overload...
   void DistribResource();              ///<clonal version
   void PlantLoop();                    ///<clonal version
   int DispersSeeds(CPlant* plant);    ///<clonal version
   void EstabLottery();                 ///<clonal version
   void DeletePlant(CPlant* plant1);    ///<clonal version

public:
   CGridclonal();                       //!< Constructor
   CGridclonal(string id);              //!< load saved grid
   virtual ~CGridclonal();              //!< Destructor
   virtual void resetGrid();

   //new...
   //! initalization of clonal plants
   virtual void InitClonalPlants(SPftTraits* traits,SclonalTraits* cltraits,
     const int n);
   //! initalization of clonal seeds
   virtual void InitClonalSeeds(SPftTraits* traits,SclonalTraits* cltraits,
     const int n,double estab=1.0, int x=-1, int y=-1);
   void DispersRamets(CclonalPlant* plant); ///<initiate new ramets
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
//---------------------------------------------------------------------------
#endif

