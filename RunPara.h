/**\file
\brief definition of struct SRunPara and enums CompMode and CompVersion
*/
//---------------------------------------------------------------------------

#ifndef RunParaH
#define RunParaH
//---------------------------------------------------------------------------
//! Enumeration type to specify size asymmetry/symmetry of competition
enum CompMode {sym, asympart, asymtot, clonal1, clonal2, clonal3};

//! Enumeration type to specify the competition version describing interspecific niche differentiation
/**
  \arg version1 \c no difference between intra- and interspecific competition
  \arg version2 \c higher effects of intraspecific competition
  \arg version3 \c lower resource availability for intraspecific competition
*/
enum CompVersion {version1, version2, version3};

//---------------------------------------------------------------------------
//! Structure with all scenario parameters
struct SRunPara
{
   static SRunPara RunPara;  //!> scenario parameters
   CompMode AboveCompMode;   //!<0 = symmetric; 1 = partial asymmetry; 2 = total asymmetry
   CompMode BelowCompMode;   //!<0 = symmetric; 1 = partial asymmetry; 2 = total asymmetry
   //!niche differentiation
   /*!0 = no difference between intra- and interspecific competition
      1 = higher effects of intraspecific competition
      2 = lower resource availability for intraspecific competition
   */
   CompVersion Version;
   int BelGrazMode; ///< mode of belowground grazing \sa CGrid::GrazingBelGr
   int GridSize;   //!< side length in cm
   int CellNum; //!< side length in cells
   bool torus; //!< boundary behavior
   int	Tmax;  //!< simulation time
   int	NPft;   ///< number of plant functional types
   double GrazProb;        //!< grazing probability per week
   double PropRemove;     //!< proportion of above ground mass removed by grazing
   ///belowground grazing probability per week
   ///\since belowground herbivory simulations
   double BelGrazProb;
   //! proportion of belowground biomass removed by grazing
   ///\since belowground herbivory simulations
   double BelPropRemove;
   double CutMass;       //!< plant aboveground biomass for plants with LMR = 1.0
   int    NCut;          //!< number cuts per year
   double DistAreaYear;   //!< fraction of grid area disturbed per year;
   double AreaEvent;      //!< fraction of grid area disturbed in one event
   double mort_seeds;     //!< seed mortality per year
   double meanARes;       //!< mean above-ground resource availability
   double meanBRes;       //!< below-ground resourcer availability
   double cv_res;     //!< coefficient of resource variation between years (not used)
   double Aampl;      //!< within year above-ground resource amplitude (not used)
   double Bampl;      //!<  within year above-ground resource amplitude (not used)
   char* PftFile;     //!< File with PFT trait parameter in Folder "Input"

   SRunPara();
   inline double DistProb(){return DistAreaYear/AreaEvent/30.0;};
   inline double CellScale(){return GridSize/(double)CellNum;};
   inline int GetGridSize() const {return CellNum;};
   inline int GetSumCells() const {return CellNum*CellNum;};

};
//---------------------------------------------------------------------------
#endif
