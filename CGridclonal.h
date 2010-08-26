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
private:
   ///List of Genets on Grid
   vector<CGenet*> GenetList;
   //new...
   void RametEstab(CclonalPlant* plant);///< establish ramets
   void Resshare();                     ///< share ressources among connected ramets
protected:
   //overload...
   void DistribResource();              ///<clonal version
   void PlantLoop();                    ///<clonal version
   void DispersSeeds(CPlant* plant);    ///<clonal version
   void EstabLottery();                 ///<clonal version
   void DeletePlant(CPlant* plant1);    ///<clonal version

public:
   CGridclonal();                       //!< Constructor
   virtual ~CGridclonal();              //!< Destructor
   virtual void resetGrid();

   //new...
   //! initalization of clonal plants
   virtual void InitClonalPlants(SPftTraits* traits,SclonalTraits* cltraits,const int n);
   //! initalization of clonal seeds
   virtual void InitClonalSeeds(SPftTraits* traits,SclonalTraits* cltraits,const int n);
   void DispersRamets(CclonalPlant* plant); ///<initiate new ramets
   //service functions...
   int GetNclonalPlants();   ///< number of living clonal plants
   int GetNPlants();         ///< number of living non-clonal plants
   int GetNMotherPlants();   ///< number of living genets
   int GetNdeadPlants();     ///< number of dead plants
   int GetCoveredCells();    ///< number of cells covered
   double GetNGeneration();  ///< number of Generations
};
//---------------------------------------------------------------------------
#endif

