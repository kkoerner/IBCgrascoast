//---------------------------------------------------------------------------
#ifndef CellH
#define CellH
//---------------------------------------------------------------------------
#include "plant.h"
#include "CSeed.h"
#include "CclonalSeed.h"
#include <algorithm>
#include <map>
//#include <string>

using namespace std;

//! iterator type for seed list
typedef vector<CSeed*>::iterator seed_iter;

//! class for cell objects (surprisingly)
class CCell
{
public:
   int x;  //!< x location [grid cells]
   int y;  //!< y location [grid cells]

   double AResConc;  //!< above-ground resource availability
   double BResConc;  //!< below-ground resource availability
//   double WaterLevel;
//   int Acover;    //!< above-ground cell state used for plotting the grid -> no ecological meaning
//   int Bcover;   //!< below-ground cell state used for plotting the grid -> no ecological meaning
   /// returns cell-cover (int-coded)
   int getCover(const int layer)const;
   /// returns cell's cover of the given type
   double getCover(const string type) const;

   bool occupied;  //!< is the cell occupied by any plant?

   CPlant* PlantInCell;  //!< pointer to plant individual that has its central point in the cell (if any)
//   CSeed* seed;     //!< pointer to seed object

   vector<CPlant*> AbovePlantList;  //!< List of all plant individuals that cover the cell ABOVE ground
   vector<CPlant*> BelowPlantList;  //!< List of all plant individuals that cover the cell BELOW ground

   vector<CSeed*> SeedBankList;  //!< List of all (ungerminated) seeds in the cell
   vector<CSeed*> SeedlingList;  //!< List of all freshly germinated seedlings in the cell

   //! array with individual numbers of each PFT covering the cell above-ground
   /*! necessary for niche differentiation version 2
   */
   map<string,int> PftNIndA;
   //! array with individual numbers of each PFT covering the cell below-ground
   /*! necessary for niche differentiation version 2
   */
   map<string,int> PftNIndB;

   //! array of seedling number of each PFT
//   int *PftNSeedling;
   map<string,int> PftNSeedling;
   //! number of different PFTs covering the cell above-ground
   /*! necessary for niche differentiation version 3 */
   int NPftA;
   //! number of different PFTs covering the cell
   /*! necessary for niche differentiation version 3 */
   int NPftB;

//   int *PftNclonalSeedling; ///<array of clonal seedling number of each PFT -not used
    CCell(const unsigned int xx,const unsigned int yy);
    CCell(const unsigned int xx,const unsigned int yy, double ares, double bres); //!< Konstruktor
   virtual ~CCell(); //!< Destruktor

   void clear();
   void SetResource(double Ares, double Bres);
   double Germinate();
   void RemoveSeedlings();
   void RemoveSeeds();
   void GetNPft();     //!< calculates number of individuals of each PFT
   //! competition function for size symmetric above-ground resource competition
   /*! function is overwritten if inherited class with different competitive
    size-asymmetry of niche differentiation is used*/
   virtual void AboveComp();
   //! competition function for size symmetric below-ground resource competition
   /*! function is overwritten if inherited class with different competitive
     size-asymmetry of niche differentiation is used*/
   virtual void BelowComp();

   ///portion cell resources the plant is gaining
//   double prop_res(const int type,const int layer,const int version)const;
   double prop_res(const string type,const int layer,const int version)const;

   void SortTypeID();  //!< sort individuals after PFT ID
   void print_map(map<string,int> &mymap);  //!< print map content for debugging - can be deleted
   std::string asString();///<return content for file saving
};

/**\brief Water-Cell class inherits from CCell

   \since COMTESS version
*/
class CWaterCell : public CCell
{
 double WaterLevel;
public:
    CWaterCell(const unsigned int xx,const unsigned int yy);
    CWaterCell(const unsigned int xx,const unsigned int yy, double ares, double bres); //!< Konstruktor
    CWaterCell(const unsigned int xx,const unsigned int yy, double ares, double bres, double wl); //!< Konstruktor
//    CCell(xx,yy);
//    CCell(xx,yy,ares,bres); //!< Konstruktor

 CWaterCell();
 ~CWaterCell(){};
    //! competition function for size symmetric below-ground resource competition
   /*! function is overwritten if inherited class with different competitive
     size-asymmetry of niche differentiation is used*/
   virtual void BelowComp();

 double GetWaterLevel(){return WaterLevel;};
 void SetWaterLevel(double val){WaterLevel=val;};
   std::string asString();///<return content for file saving
};//end CWaterCell

//---------------------------------------------------------------------------
#endif
