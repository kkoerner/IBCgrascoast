/**\file
   \brief functions of class CGrid
*/

#pragma package(smart_init)
#pragma hdrstop
#define CCELL CWaterCell //CCell for old version
#include "GridBase.h"
#include "environment.h"
#include <iostream>
#include <map>
//---------------------------------------------------------------------------
CGrid::CGrid():cutted_BM(0)
{
   SPftTraits::ReadPftStrategy(); //get list of available Strategies
   CellsInit();
   LDDSeeds = new map<string,long>;
   //generate ZOIBase...
   ZOIBase.assign(SRunPara::RunPara.GetSumCells(),0);
   for (unsigned int i=0;i<ZOIBase.size();i++) ZOIBase[i]=i;
   sort(ZOIBase.begin(),ZOIBase.end(),CompareIndexRel);
}
//---------------------------------------------------------------------------
CGrid::CGrid(string id):cutted_BM(0)
{
   SPftTraits::ReadPftStrategy(); //get list of available Strategies
   CellsInit();

   LDDSeeds = new map<string,long>;
   //generate ZOIBase...
   ZOIBase.assign(SRunPara::RunPara.GetSumCells(),0);
   for (unsigned int i=0;i<ZOIBase.size();i++) ZOIBase[i]=i;
   sort(ZOIBase.begin(),ZOIBase.end(),CompareIndexRel);
}
//-----------------------------------------------------------------------------
/**
  Initiate grid cells.

  \note call only once or delete cell objects before;
  better reset cells (resetGrid()) to start a new environment
*/
void CGrid::CellsInit()
{
   using SRunPara::RunPara;
   int index;int SideCells=RunPara.CellNum;
   CellList = new (CCELL* [SideCells*SideCells]);
//     CellList = new (CWaterCell* [SideCells*SideCells]);

   for (int x=0; x<SideCells; x++){
      for (int y=0; y<SideCells; y++){
         index=x*SideCells+y;
         CCELL* cell = new CCELL(x,y,
//         CWaterCell* cell = new CWaterCell(x,y,
                 CEnvir::AResMuster[index],CEnvir::BResMuster[index]);
         CellList[index]=cell;
     }
  }
}//end CellsInit
//---------------------------------------------------------------------------
/**
  Clears the grid from Plants and resets cells.
*/
void CGrid::resetGrid(){
//cells...
   for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
      CCELL* cell = CellList[i];
      cell->clear();
   }
//plants...
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      delete *iplant;
   }
   PlantList.clear();
}
//---------------------------------------------------------------------------
CGrid::~CGrid()
{
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      CPlant* plant = *iplant;
      delete plant;
   }; PlantList.clear();

   for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
      CCELL* cell = CellList[i];
//      CWaterCell* cell = CellList[i];
      delete cell;
   }
   delete[] CellList;
}//end ~CGrid
//---------------------------------------------------------------------------
/**
  Growth (resource allocation and vegetative growth), seed dispersal and
  mortality of plants.
*/
void CGrid::PlantLoop()
{
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      CPlant* plant = *iplant;
      if (!plant->dead){
        plant->Grow2();
        if (CEnvir::week>plant->Traits->DispWeek) DispersSeeds(plant);
        plant->Kill();
      }
      plant->DecomposeDead();
   }
}
//-----------------------------------------------------------------------------
/**
  lognormal dispersal kernel
  Each Seed is dispersed after an log-normal dispersal kernel with mean and sd
  given by plant traits. The dispersal direction has no prevalence.

  \code
         double mean=plant->Traits->Dist*100;   //m -> cm
         double sd=plant->Traits->Dist*100;     //mean = std (simple assumption)

         double sigma=sqrt(log((sd/mean)*(sd/mean)+1));
         double mu=log(mean)-0.5*sigma;
         dist=exp(CEnvir::RandNumGen.normal(mu,sigma));

         double direction=2*Pi*CEnvir::rand01();
         int x=CEnvir::Round(plant->getCell()->x+cos(direction)*dist*CmToCell);
         int y=CEnvir::Round(plant->getCell()->y+sin(direction)*dist*CmToCell);
  \endcode
*/
void getTargetCell(int& xx,int& yy,const float mean,const float sd,double cellscale)
{
   double sigma=sqrt(log((sd/mean)*(sd/mean)+1));
   double mu=log(mean)-0.5*sigma;
   double dist=exp(CEnvir::RandNumGen.normal(mu,sigma));
   if (cellscale==0)cellscale= SRunPara::RunPara.CellScale();
//   double CellScale=SRunPara::RunPara.CellScale();
   double CmToCell=1.0/cellscale;

   //direction uniformly distributed
   double direction=2*Pi*CEnvir::rand01();//rnumber;
   xx=CEnvir::Round(xx+cos(direction)*dist*CmToCell);
   yy=CEnvir::Round(yy+sin(direction)*dist*CmToCell);
}
//-----------------------------------------------------------------------------
//bool Emmigrates(int& xx, int& yy);
/**
  Function disperses the seeds produced by a plant when seeds are to be
  released (at dispersal time - SclonalTraits::DispWeek).

  Each Seed is dispersed after an log-normal dispersal kernel
  in function getTargetCell().

\date 2010-08-30 periodic boundary conditions are transformed
      to dispers seeds for LDD

\return list of seeds to dispers per LDD
  */
int CGrid::DispersSeeds(CPlant* plant)
{
   int x=plant->getCell()->x, y=plant->getCell()->y, NSeeds;
   double dist, direction;
   int nb_LDDseeds=0;

   NSeeds=plant->GetNSeeds();
   for (int j=0; j<NSeeds; ++j){
         //negative exponential dispersal kernel
         //dist=RandNumGen->exponential(1/(plant->DispDist*100));   //m -> cm

         //lognormal dispersal kernel
         getTargetCell(x,y,
           plant->Traits->Dist*100,        //m -> cm
           plant->Traits->Dist*100);       //mean = std (simple assumption)
         //export LDD-seeds
         if (SRunPara::RunPara.torus){Boundary(x,y);}
         else if (Emmigrates(x,y)) {nb_LDDseeds++;continue;}

         CCELL* cell = CellList[x*SRunPara::RunPara.CellNum+y];
         new CSeed(plant,cell);
      }//for NSeeds
      return nb_LDDseeds;
}//end DispersSeeds
//---------------------------------------------------------------------------
/**
  This function calculates ZOI of all plants on grid.
  Each grid-cell gets a list
  of plants influencing the above- (alive and dead individuals) and
  belowgroud (alive plants only) layers.

  \par revision
  Let ZOI be defined by a list sorted after ascending distance to center instead
  of searching a square defined by maximum radius.
*/
void CGrid::CoverCells()
{
   int xmin, xmax, ymin, ymax;
   int index;
   double dist,Radius;
   int xhelp, yhelp;
   using SRunPara::RunPara;

   double CellScale=RunPara.CellScale();
   double CellArea=CellScale*CellScale;
   //loop for all plants
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
        CPlant* plant = *iplant;

     double Ashoot=plant->Area_shoot()/CellArea;
     plant->Ash_disc=floor(plant->Area_shoot())+1;
     double Aroot=plant->Area_root()/CellArea;
     plant->Art_disc=floor(plant->Area_root())+1;
     double Amax=max(Ashoot,Aroot);
     for (int a=0;a<Amax;a++){
        //get current position: add plant pos with ZOIBase-pos
        xhelp=plant->getCell()->x
              +ZOIBase[a]/RunPara.CellNum-RunPara.CellNum/2;//x;
        yhelp=plant->getCell()->y
              +ZOIBase[a]%RunPara.CellNum-RunPara.CellNum/2;//y;
        /// \todo change to absorbing bound for upscaling
        Boundary(xhelp,yhelp);
        index = xhelp*RunPara.CellNum+yhelp;
        CCELL* cell = CellList[index];

        //Aboveground****************************************************
        if (a<Ashoot){
            //dead plants still shade others
            cell->AbovePlantList.push_back(plant);
            cell->PftNIndA[plant->pft()]++;
        }//for <ashoot //Ende if
        //Belowground*****************************************************
        if (a<Aroot){
            //dead plants do not compete for below ground resource
            if (!plant->dead){
               cell->BelowPlantList.push_back(plant);
               cell->PftNIndB[plant->pft()]++;
            }
        }//if <Aroot //Ende if
     }// for <Amax
   }//end of plant loop
}
//---------------------------------------------------------------------------
/**
  Function calculates the interaction intensity (root density)
  for each plant on grid.
  !!uses CCell information in ZOI around each Plant. Assure that BelowPlantList
  and PftNIndB are evaluated and up to date.
*/
void CGrid::CalcRootInteraction(){
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
        CPlant* plant = *iplant;
   CalcRootInteraction(plant);
   }
}
//---------------------------------------------------------------------------
/**
  Function calculates the interaction intensity (root density)
  for one plant on grid.
  !!uses CCell information in ZOI around a Plant. Assure that BelowPlantList
  and PftNIndB are evaluated and up to date.
*/
void CGrid::CalcRootInteraction(CPlant * plant){
   using SRunPara::RunPara;
   double CellScale=RunPara.CellScale();
   double CellArea=CellScale*CellScale;
//   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
//        CPlant* plant = *iplant;
     double Aroot=plant->Area_root()/CellArea;
     plant->Art_disc=floor(plant->Area_root())+1;
     plant->Aroots_all=0;
     plant->Aroots_type=0;
     for (int a=0;a<Aroot;a++){
        //get current position: add plant pos with ZOIBase-pos
        int xhelp=plant->getCell()->x
              +ZOIBase[a]/RunPara.CellNum-RunPara.CellNum/2;//x;
        int yhelp=plant->getCell()->y
              +ZOIBase[a]%RunPara.CellNum-RunPara.CellNum/2;//y;
        /// \todo change to absorbing bound for upscaling
        Boundary(xhelp,yhelp);
        int index = xhelp*RunPara.CellNum+yhelp;
        CCELL* cell = CellList[index];
        //-----------------------
        plant->Aroots_all  += cell->BelowPlantList.size();
        plant->Aroots_type += cell->PftNIndB[plant->pft()];
     }
//   }
}
//---------------------------------------------------------------------------
/**
  Resets all weekly variables of individual cells and plants (only in PlantList)

  Former called FreeCells()
*/
void CGrid::ResetWeeklyVariables()
{
   //loop for all cells
   for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
      CCELL* cell = CellList[i];
      cell->AbovePlantList.clear();
      cell->BelowPlantList.clear();
      cell->RemoveSeedlings(); //remove seedlings and pft-counter
   }
   //loop for all plants
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      CPlant* plant = *iplant;
      //reset weekly variables
      plant->Auptake=0;plant->Buptake=0;
      plant->Ash_disc=0;plant->Art_disc=0;
//      plant->Allometrics();
   }
}

//---------------------------------------------------------------------------
/**
  distribute local resources according to local competition

*/
void CGrid::DistribResource()
{
   for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){  //loop for all cells
      CCELL* cell = CellList[i];
      cell->GetNPft();

      cell->AboveComp();
      cell->BelowComp();
   } //for all cells
}//end distribResource

//-----------------------------------------------------------------------------
/**
  For each grid cell seeds from seed bank germinate and establish.
  Seedlings that do not establish will die.

  \note this function is \b completely reimplemented by CGridclonal

  \par revision
  I tried to make function faster skipping the seed-pft-collecting
  cumulative lists, but that possibly leads to pseudo-endless loops if
  seedling-number per cell is too high
*/
void CGrid::EstabLottery()
{
   double sum=0;
   int index=0;
   using SRunPara::RunPara;
   map<string,double> PftEstabProb;//=map<string,int>(0);
   map<string,int> PftNSeedling;

      for (int i=0; i<RunPara.GetSumCells(); ++i){  //loop for all cells
         CCELL* cell = CellList[i];
         if ((cell->getCover(1)==0) && (!cell->SeedBankList.empty())){  //germination if cell is uncovered

            sum=cell->Germinate();

            //relative probability for establishment for all Pfts (cumulated)
            typedef map<string,int> mapType;
            for(mapType::const_iterator it = cell->PftNSeedling.begin();
              it != cell->PftNSeedling.end(); ++it)
            { //for all types              (diesen Teil in Germinate verschieben?)
              string  pft =it->first;
              map<string, int>::iterator itr = cell->PftNSeedling.find(pft);
              if (itr != cell->PftNSeedling.end()) {
                PftEstabProb[pft]=
                  (double) itr->second
                  *CClonalGridEnvir::getPftLink(pft)->SeedMass;
                PftNSeedling[pft]=itr->second;//(cell->PftNSeedling[pft]);
              }
            }//for each type

            if (!cell->SeedlingList.empty()){
            //chose seedling that establishes at random
            double rnum=CEnvir::rand01()*sum;  //random double between 0 and sum of seed mass
            for(mapType::const_iterator it = cell->PftNSeedling.begin();
                it != cell->PftNSeedling.end()&&(!cell->occupied); ++it)
            { //for each type germinated
              string pft =it->first;
              if (rnum<PftEstabProb[pft])
              {  //random number < current types' estab Probability?
                //mische die Keimlinge des Gewinnertyps
                random_shuffle(cell->SeedlingList.begin(),
                   partition(cell->SeedlingList.begin(),cell->SeedlingList.end(),
                   bind2nd(ptr_fun(SeedOfType),pft)));
                //Was, wenn keine Seedlings(typ==pft) gefunden werden (sollte nicht passieren)?
                //etabliere jetzt das erste Element der Liste
                CSeed* seed = cell->SeedlingList.front();
                CPlant* plant = new CPlant(seed);
                plant->setCell(cell);
                PlantList.push_back(plant);
                cell->PftNSeedling[pft]--;
                continue; //if established, go to next cell
              }//if rnum<
              else{   //if not: subtrahiere   PftCumEstabProb[pft]
                rnum-= PftEstabProb.find(pft)->second;
              }   //und gehe zum nächsten Typ
            }//for all types in list
            cell->RemoveSeedlings();
            }//if seedlings in cell
         }//seeds in cell
      }//for all cells
}//end CGrid::EstabLottery()
//-----------------------------------------------------------------------------
///\todo warum seed->Survive nicht angewendet?
void CGrid::SeedMortAge()
{
   for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){  //loop for all cells
      CCELL* cell = CellList[i];
      for (seed_iter iter=cell->SeedBankList.begin();
         iter!=cell->SeedBankList.end(); ++iter){
         CSeed* seed = *iter;
         if (seed->Age >= seed->Traits->Dorm)  seed->remove=true;
      }//for seeds in cell
      cell->RemoveSeeds();   //removes and deletes all seeds with remove==true
   }// for all cells
}//end SeedMortAge
//-----------------------------------------------------------------------------
/*! \page disturb Disturbances
  The following modes of disturbances are implemented in the model:
     - \link CGrid::Grazing() Aboveground Grazing\endlink (orig. by F.May)
     - \link CGrid::Trampling() Trampling\endlink           (orig. by F.May)
     - \link CGrid::GrazingBelGr() Belowground Grazing\endlink (2010  by K.Koerner)
     - \link CGrid::Cutting() Cutting\endlink             (02/10 by F.May)

  The function CGrid::Disturb() coordinates sequence and occurence
  of events.

*/

/**
   Calculate the effects of Grazing() and Trampling() according to
   the probabilities \ref SRunPara::GrazProb "GrazProb" and
   \ref SRunPara::DistProb() "DistProb"

   NEW: additionally calculate the effect of belowground herbivory

   NEW: additionally calculate the effect of cutting after May(Jan 2010)
*/
bool CGrid::Disturb()
{
   if (PlantList.size()>0){
      if (CEnvir::rand01()<SRunPara::RunPara.GrazProb){
         Grazing();
      }
      if (CEnvir::rand01()<SRunPara::RunPara.DistProb()){
         Trampling();
      }
      if (CEnvir::rand01()<SRunPara::RunPara.BelGrazProb){
         GrazingBelGr(SRunPara::RunPara.BelGrazMode);
      }
      int week = CEnvir::week;
      if (SRunPara::RunPara.NCut>0){
         switch (SRunPara::RunPara.NCut){
            case 1: if (week==22) Cutting(); break;
            case 2: if ((week==22) || (week==10)) Cutting(); break;
            case 3: if ((week==22) || (week==10) || (week==16)) Cutting(); break;
            default: cerr<<"CGrid::Disturb() - wrong input";exit(3);
         }
      }
      return true;
   }
   else return false;
}//end  Disturb
//-----------------------------------------------------------------------------
/**
  The plants on the whole grid are grazed according to
  their relative grazing susceptibility until the given
  \ref SRunPara::PropRemove "proportion of removal"
  is reached or the grid is completely grazed.
   (Above ground mass that is ungrazable - see Schwinning and Parsons (1999):
   15,3 g/m²  * 1.6641 m² = 25.5 g)

*/
void CGrid::Grazing()
{
   int    SumCells     =SRunPara::RunPara.GetSumCells();
   double CellScale    =SRunPara::RunPara.CellScale();
   double ResidualMass =15300*SumCells*CellScale*CellScale*0.0001;
   double MaxMassRemove, TotalAboveMass, MassRemoved=0;
   double grazprob;

   double Max;//,m;

   TotalAboveMass=GetTotalAboveMass();

  //maximal removal of biomass
   MaxMassRemove=TotalAboveMass*SRunPara::RunPara.PropRemove;
   MaxMassRemove=min(TotalAboveMass-ResidualMass,MaxMassRemove);

   while(MassRemoved<MaxMassRemove){
   //calculate slope for individual grazing probability;
      //sort PlantList descending after mshoot/LMR
      sort(PlantList.begin(),PlantList.end(),ComparePalat);
      //plant with highest grazing susceptibility
      CPlant* plant = *PlantList.begin() ;
      Max = plant->mshoot*plant->Traits->GrazFac();
//      m=1.0/Max;

      random_shuffle(PlantList.begin(),PlantList.end());

      plant_size i=0;
      while((i<PlantList.size())&&(MassRemoved<MaxMassRemove)){
         CPlant* lplant=PlantList[i];
         grazprob= (lplant->mshoot*lplant->Traits->GrazFac())/Max;
         if (CEnvir::rand01()<grazprob) MassRemoved+=lplant->RemoveMass();
         ++i;
      }
   }
}//end CGrid::Grazing()
//-----------------------------------------------------------------------------
/**
  Cutting of all plants on the patch

  \author Felix May (Jan2010)
  \change 28-10-2010 lw: quadriere LMR    #
  \change 18-11-2010 kk: gebe entfernte BM an Klassenvariable
  */
void CGrid::Cutting()
{
   CPlant* pPlant;

//   double mass_cut = SRunPara::RunPara.CutMass;
   double height_cut = SRunPara::RunPara.CutLeave;
   double mass_removed=0;

   for (plant_size i=0; i<PlantList.size();i++){
         pPlant = PlantList[i];
//         if (pPlant->mshoot/(pPlant->Traits->LMR*pPlant->Traits->LMR) > mass_cut){
         if (pPlant->getHeight() > height_cut){
//            double to_leave= mass_cut*(pPlant->Traits->LMR*pPlant->Traits->LMR);
            double to_leave= pPlant->mshoot* height_cut/pPlant->getHeight();
            //doc biomass removed
            mass_removed+= pPlant->mshoot-to_leave+pPlant->mRepro;
            pPlant->mshoot = to_leave;
            pPlant->mRepro = 0.0;
         }
   }
   cutted_BM+= mass_removed;
} //end cutting

//-----------------------------------------------------------------------------
/**
\brief get additional mortality

obsolete
*/

double getMortBelGraz(double fraction, double thresh)
{
  if (thresh==1.0) return 0.0;
//  if (fraction<thresh) return 0.0;
  return  max(0.0,(fraction-thresh)/(1.0-thresh));
}
//-----------------------------------------------------------------------------
/**
  The plants on the whole grid are grazed according to the mode given
  A given percentage of RootMass is grazed.
  \param mode mode of belowground grazing
  \arg \c mode=0 general reduction of belowground biomass for all plants
    simultaneously (standard)
  \arg \c mode=1 stochastic reduction of belowground biomass of single plants
    until a given portion is reached (not implemented yet)
  \arg \c mode=2 as '1' but proportional to root mass
  \arg \c mode=3 as '1' but prop. to root mass and taste
    (SPftTraits::palat)
  \arg \c mode=4 as '1' but prop. to root taste (old:mass and 1/taste )
  \arg \c mode=5 as '1' but prop. to root density
  \arg \c mode=6 as '1' but prop. to type-specific root density
  \warning grazing priorities don't change for mode5 and mode6
    within one time step, as they do after first grazing loop
    for modes 1-4
  The inner loop is broken and a log-entry is done
  if no more roots to graze were found in one loop (to
  prevent endless loops).

  \since belowground herbivory simulations
  \bug möglicher Absturz wenn Traits->palat==0 oder rootmass==0

  Additional mortality is assumed if root grazing of a  plant exceeds
  a threshold (only if mode>0).  -- obsolete

  \date Mar2011
  Spatial Heterogeneity is introduced. Only Plants of the left grid part
  are grazed belowground if Heterogeneity-flag is set. (only if mode>0)
*/
void CGrid::GrazingBelGr(const int mode)
{
  bool HetFlag=SRunPara::RunPara.HetBG;  //!< true for heterogenous belowground herbivory
  if (mode==0){
    for (plant_size i=0;i<PlantList.size();i++){
         CPlant* lplant=PlantList[i];
         lplant->RemoveRootMass(SRunPara::RunPara.BelPropRemove);
    }
  }
  else {//if (mode<=4){
//   map<CPlant*,double> oldMroot;
//    for (plant_size i=0;i<PlantList.size();i++)
//      oldMroot[PlantList[i]]=PlantList[i]->mroot;
//partition of Plants left and right of grid
    plant_iter LeftPlants=
    partition(PlantList.begin(),PlantList.end(),is_left);
    vector<CPlant*> leftPlantList(LeftPlants,PlantList.end());
    //get ranking list of aboveground types after aboveground biomass
    map<string,double> aboveDom;

    vector<CPlant*> PlantsToGraze=PlantList;
///!
    if (HetFlag) PlantsToGraze=leftPlantList; //!should only one half of grid be grazed?
    for (plant_size i=0;i<PlantsToGraze.size();i++)
      if (!PlantsToGraze[i]->dead) aboveDom[PlantsToGraze[i]->pft()]+=PlantsToGraze[i]->mshoot;

    double TotalBelowMass=GetTotalBelowMass();
    //see Grazing(), but no information for belowground grazing
    double ResidualMass=0;
    double MassRemoved=0;
    //maximal removal of biomass
    double MaxMassRemove=TotalBelowMass*SRunPara::RunPara.BelPropRemove;
    MaxMassRemove=min(TotalBelowMass-ResidualMass,MaxMassRemove);
    while(MassRemoved<MaxMassRemove){
      double max_value=0;
      double mass_remove_start=MassRemoved;//remember value started
      for (plant_size i=0;i<PlantsToGraze.size();i++){
          CPlant* lplant=PlantsToGraze[i];
          if (lplant->dead) continue;
          //if (mode==1) max_value=0;
          if (mode==2)
            max_value=max(max_value,lplant->mroot);
          else if (mode==3)
            max_value=max(max_value,lplant->mroot*lplant->Traits->palat);
          else if (mode==4)
//            max_value=max(max_value,lplant->mroot/lplant->Traits->palat);
            max_value=max(max_value,lplant->Traits->palat);
          else if (mode==7)
            max_value=max(max_value,aboveDom.find(lplant->pft())->second);  ///\warning what if find fails?
          else {
            CalcRootInteraction(lplant);
            if (mode==5){
              max_value=max(max_value,lplant->Aroots_all/lplant->Art_disc);
            }else {
              max_value=max(max_value,lplant->Aroots_type/lplant->Art_disc);
            }
          }
      }
//      cout<<"max."<<max_value<<"|";
      //stochastic removal of 50% root mass each plant
      random_shuffle(PlantsToGraze.begin(),PlantsToGraze.end());
      plant_size i=0;
      while((i<PlantsToGraze.size())&&(MassRemoved<MaxMassRemove)){
         CPlant* lplant=PlantsToGraze[i];
         if (lplant->dead) {++i;continue;}
         double grazprob=1;
         switch (mode){
           case 1:grazprob= 1;break;
           case 2:grazprob= (lplant->mroot)/max_value;break;
           case 3:grazprob= (lplant->mroot*lplant->Traits->palat)/max_value;
             break;
//           case 4:grazprob= (lplant->mroot/lplant->Traits->palat)/max_value;
           case 4:grazprob= (lplant->Traits->palat)/max_value;
             break;
           case 5:grazprob= lplant->Aroots_all/lplant->Art_disc/max_value;
             break;
           case 6:grazprob= lplant->Aroots_type/lplant->Art_disc/max_value;
             break;
           case 7:grazprob= aboveDom.find(lplant->pft())->second/max_value;
             break;
           default:grazprob= 0; break;    //fehler
         }
         if (CEnvir::rand01()<grazprob){
           MassRemoved+=lplant->RemoveRootMass();
             //grazing induced additional mortality
           if (CEnvir::rand01()<SRunPara::RunPara.BGThres) lplant->dead=true;
         }
         ++i;
      }
      // .. to prevent endless loops...
      if(mass_remove_start>=MassRemoved){
        cout<<" no more roots found - diff: "
            <<(1-MassRemoved/MaxMassRemove)<<" - ";
        break;
      }
    }//end mass-to-remove reached?

//    //grazing induced additional mortality
//    //compare origional mroot with current
//    for(map<CPlant*,double>::const_iterator it = oldMroot.begin();
//       it != oldMroot.end(); ++it)
//    {
//
//    //depending on loss - calculate mortality
//      double loss=1- (it->first->mroot/it->second);
//      double val=getMortBelGraz(loss,SRunPara::RunPara.BGThres);
//      if (CEnvir::rand01()<val) it->first->dead=true;

//    }
  }//if mode>0
}//end CGrid::GrazingBelGr()
//-----------------------------------------------------------------------------
/**
  Round gaps are created randomly, and all plants therein are killed,
  until a certain \ref SRunPara::AreaEvent "Area" is trampled.
  If a cell is trampled twice it does not influence the number of
  disturbed patches.

  (Radius of disturbance currently is 10cm)

    \par revision
  Let ZOI be defined by a list sorted after ascending distance to center instead
  of searching a square defined by maximum radius.
*/
void CGrid::Trampling()
{
   int xcell, ycell,xhelp, yhelp,index;   //central point
   using CEnvir::nrand;using CEnvir::Round;using SRunPara::RunPara;

   double radius=10.0;                 //radius of disturbance [cm]
   double Apatch=(Pi*radius*radius);   //area of patch [cm²]
   //number of gaps
   int NTrample=floor(RunPara.AreaEvent*RunPara.GridSize*RunPara.GridSize/
                      Apatch);
   //area of patch [cell number]
   Apatch/=SRunPara::RunPara.CellScale()*SRunPara::RunPara.CellScale();

   for (int i=0; i<NTrample; ++i){
      //get random center of disturbance
      xcell=nrand(RunPara.CellNum);
      ycell=nrand(RunPara.CellNum);

      for (int a=0;a<Apatch;a++){
        //get current position: add random center pos with ZOIBase-pos
        xhelp=xcell
              +ZOIBase[a]/RunPara.CellNum-RunPara.CellNum/2;
        yhelp=ycell
              +ZOIBase[a]%RunPara.CellNum-RunPara.CellNum/2;
        /// \todo change to absorbing bound for upscaling
        Boundary(xhelp,yhelp);
        index = xhelp*RunPara.CellNum+yhelp;
        CCELL* cell = CellList[index];
        if (cell->occupied){
           CPlant* plant = (CPlant*) cell->PlantInCell;
           plant->remove=true;
        }//if occ
      }//for all cells in patch
   }//for all patches
}//end CGrid::Trampling()
//-----------------------------------------------------------------------------
void CGrid::RemovePlants()
{
   plant_iter irem = partition(PlantList.begin(),PlantList.end(),
     GetPlantRemove);
   for (plant_iter iplant=irem; iplant<PlantList.end(); ++iplant)
   {
      CPlant* plant = *iplant;
      DeletePlant(plant);
   }
   PlantList.erase(irem,PlantList.end());
}
//-----------------------------------------------------------------------------
/**
remove a plant from gridcell and deletes it

\todo move to CPlant
*/
void CGrid::DeletePlant(CPlant* plant1)
{
   plant1->getCell()->occupied=false;
   plant1->getCell()->PlantInCell = NULL;

   delete plant1;
}
//-----------------------------------------------------------------------------
void CGrid::Winter()
{
   RemovePlants();
   //mass removal in wintertime
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      (*iplant)->WinterLoss();
   }
}
//-----------------------------------------------------------------------------
void CGrid::SeedMortWinter()
{
//   double rnumber;
   for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){  //loop for all cells
      CCELL* cell = CellList[i];
      for (seed_iter iter=cell->SeedBankList.begin(); iter!=cell->SeedBankList.end(); ++iter){
         CSeed* seed = *iter;
         if ((CEnvir::rand01()<mort_seeds)){
            seed->remove=true;
         } //if not seed survive
         else ++seed->Age;
      }//for seeds in cell

      cell->RemoveSeeds();   //removes and deletes all seeds with remove==true
   }// for all cells
}//end CGrid::SeedMortWinter()
//---------------------------------------------------------------------------
/**
  Set a number of randomly distributed Plants (CPlant) of a specific
  trait-combination on the grid.

  \param traits   SPftTraits of the plants to be set
  \param n        number of Individuals to be set
*/
void CGrid::InitPlants(SPftTraits* traits,const int n)
{
   using CEnvir::nrand;using SRunPara::RunPara;
   int  x, y;
   int SideCells=RunPara.CellNum;

   int counter=0;
   while (counter<n){
         x=nrand(SideCells);
         y=nrand(SideCells);

         CCELL* cell = CellList[x*SideCells+y];
         if (!cell->occupied){
            CPlant* plant = new CPlant(traits,cell);
            PlantList.push_back(plant);
            plant->mshoot=plant->Traits->MaxMass/2;
            plant->mroot=plant->Traits->MaxMass/2;
            ++counter;
         }
   }
}//end CGrid::PlantsInit()
//-----------------------------------------------------------------------------
/**
  Set a number of randomly distributed Seeds (CSeed) of a specific
  trait-combination on the grid.

  \param traits   SPftTraits of the seeds to be set
  \param n        number of seeds to be set
  \param estab    seed establishment (CSeed) - default is 1
  \since 2010-09-10 estab rate for seeds can be modified (default is 1.0)
*/
void CGrid::InitSeeds(SPftTraits* traits, int n,double estab)
{
   using CEnvir::nrand;using SRunPara::RunPara;
   int x,y;
   int SideCells=RunPara.CellNum;

   //random initial conditions
     for (int i=0; i<n; ++i){
        x=nrand(SideCells);
        y=nrand(SideCells);
//        CCell* cell = CellList[x*SideCells+y];
//        new CSeed(estab,traits,cell);
        InitSeeds(traits,1,x,y,estab);
     }
}//end CGrid::SeedsInit()
//---------------------------------------------------------------------------
void CGrid::InitSeeds(SPftTraits* traits, int n,int x, int y,double estab)
{
   using CEnvir::nrand;using SRunPara::RunPara;
   int SideCells=RunPara.CellNum;
   if (estab==0) estab=traits->pEstab;
   CCell* cell = CellList[x*SideCells+y];

   //random initial conditions
     for (int i=0; i<n; ++i){
        new CSeed(estab,traits,cell);
     }
//     if (n>1) cout<<n<<" seeds of type "<<traits->name<<" at "<<x<<":"<<y<<endl;
}//end CGrid::SeedsInit()
//---------------------------------------------------------------------------
/**
  Weekly sets cell's resources - above- and belowground variation during the
  year.

  At the moment Ampl and Bampl are set zero,
  so ressources are temporally constant.
*/
void CGrid::SetCellResource()
{
   //set mean year
   int gweek=CEnvir::week;
   using SRunPara::RunPara;

   for (int i=0; i<RunPara.GetSumCells(); ++i){
      CCELL* cell = CellList[i];
      cell->AResConc=max(0.0,
         (-1.0)*RunPara.Aampl*cos(2.0*Pi*gweek/(double)CEnvir::WeeksPerYear)
         +CEnvir::AResMuster[i]);
      cell->BResConc=max(0.0,
         RunPara.Bampl*sin(2.0*Pi*gweek/(double)CEnvir::WeeksPerYear)
         +CEnvir::BResMuster[i]);
   }
}//end SetCellResource
//-----------------------------------------------------------------------------
/**
  \note does not account for possible nearer distance-values if torus is assumed
  (not a case at the moment)

  \return eucledian distance between two pairs of coordinates (xx,yy) and (x,y)

*/
double Distance(const double& xx, const double& yy,
//double CGrid::Distance(const double& xx, const double& yy,
  const double& x, const double& y){
   return sqrt((xx-x)*(xx-x) + (yy-y)*(yy-y));
}
bool CompareIndexRel(int i1, int i2)
{ const int Num=SRunPara::RunPara.CellNum;
  return  Distance(i1/Num,i1%Num  ,Num/2,Num/2)
         <Distance(i2/Num,i2%Num  ,Num/2,Num/2);
}
//---------------------------------------------------------------------------
/**
  \param[in,out] xx  torus correction of x-coordinate
  \param[in,out] yy  torus correction of y-coordinate

  alternative code (by Felix) - I don't really know, which is faster:
  \code
   while ((xx<0)||(xx>=SideCells)){
      if (xx<0) xx=SideCells+xx;
      if (xx>=SideCells) xx=xx-SideCells;
   }
   while ((yy<0)||(yy>=SideCells)){
      if (yy<0) yy=SideCells+yy;
      if (yy>=SideCells) yy=yy-SideCells;
   }
  \endcode
*/
void Boundary(int& xx, int& yy)
//void CGrid::Boundary(int& xx, int& yy)
{
   xx%=SRunPara::RunPara.CellNum;
   if(xx<0)xx+=SRunPara::RunPara.CellNum;
   yy%=SRunPara::RunPara.CellNum;
   if(yy<0)yy+=SRunPara::RunPara.CellNum;
}
//---------------------------------------------------------------------------
bool Emmigrates(int& xx, int& yy)
{
   if(xx<0||xx>=SRunPara::RunPara.CellNum)return true;
   if(yy<0||yy>=SRunPara::RunPara.CellNum)return true;
   return false;
}
//---------------------------------------------------------------------------
///manipulates vector with individual numbers of each PFT
///\param[out] PftData list of numbers of various Pfts on grid
void CGrid::GetPftNInd(vector<int>& PftData)
{
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
        ++PftData[(*iplant)->Traits->TypeID-1];
   }
}
//---------------------------------------------------------------------------
///manipulates vector with seed numbers of each PFT
void CGrid::GetPftNSeed(vector<int>& PftData)
{
   for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i){
      CCELL* cell = CellList[i];
      for (seed_iter iter=cell->SeedBankList.begin(); iter!=cell->SeedBankList.end(); ++iter){
         CSeed* seed = *iter;
         ++PftData[seed->Traits->TypeID-1];
      }
   }
}
//---------------------------------------------------------------------------
/**
  \return sum of all plants' aboveground biomass (shoot and fruit)
*/
double CGrid::GetTotalAboveMass()
{
   double above_mass=0;

   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      CPlant* plant = *iplant;
      above_mass+=plant->mshoot+plant->mRepro;
   }
   return above_mass;
}
//---------------------------------------------------------------------------
/**
  \return sum of all plants' belowground biomass (roots)
*/
double CGrid::GetTotalBelowMass()
{
   double below_mass=0;

   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      CPlant* plant = *iplant;
      below_mass+=plant->mroot;
   }
   return below_mass;
}



//--documentation Felix' peper version-------------------------------------------------------------------------
/**
\page ODDbase ODD-Description of the basic model

The model description follows the ODD protocol (overview,
design concepts, details) for describing individualbased
models (Grimm et al. 2006).

\section Purpose
The model is designed to evaluate the response of plant
functional type (PFT) diversity towards grazing under
different local environmental conditions and differentiated
assumptions about plant-plant competition.

\section variables State variables and scales
The model includes the entities seeds, individual plants
and grid cells (Table 1). Seeds are described by the state
variables position, age, and mass. Plant individuals are
characterized by their position, the mass of three plant
compartments (shoot, root and reproductive mass), the
duration of resource stress exposure, and they are classified
as a certain PFT with specific trait attribute parameters
(Table 3). In a simplified model version only two plant
compartments, vegetative and reproductive mass, are taken
into consideration. Spatially, plants are described by their
‘zone-of-influence’ (ZOI), i.e. a circular area around
their location (Schwinning and Weiner 1998, Weiner et
al. 2001). Within this area the individual can acquire
resources and if the ZOIs of neighbouring plants overlap,
the individuals will only compete for resources in the
overlapping area. For the three compartment model
version we consider two independent ZOIs for a plant’s
shoot and root, representing above- and below-ground
resource uptake and competition. The ZOIs radii are
determined from the biomass of the corresponding plant
compartment.

In order to simplify spatial calculations of resource
competition, ZOIs are projected onto a grid of discrete
cells. Grid cells represent 1 cm2. The state of a grid cell is
defined by two resource availabilities, above and below
ground (in the simplified model version, only one resource
is considered). The size of the modelled arena was
128 by 128 cm. To avoid edge effects, periodic boundary
conditions were used, i.e. the grid essentially was a torus.
A model’s time step corresponds to one week; a vegetation
period consisted of 30 weeks per year, and simulations
were run for 100 years.

\section sced Process overview and scheduling
The processes resource competition, plant growth and
plant mortality are considered within each week of the
vegetation period. Seed dispersal and seedling establishment
are limited to certain weeks of the year (Table 2).
Grazing events occur randomly with a fixed probability
which is constant for all weeks. Two processes, winter
dieback of above-ground biomass and mortality of seeds
are only considered once a year, at the end of the
vegetation period (Fig. A1 in Supplementary material
Appendix 1). Plant's state variables are synchronously
updated within the subroutines for growth, mortality,
grazing and winter dieback, i.e. changes to state variables
are updated only after all model entities have been
processed (Grimm and Railsback 2005).

\section emergence Design concepts: emergence
All features observed at the community level, such as
community composition and diversity, emerged from
individual plant-plant interactions, grazing effects at the
individual scale, and resource availabilities.

\section adapt Design concepts: adaptation
In the submodel representing plant growth, aboveand
below-ground competition, plants adaptively allocate
resources to shoot and root growth in order to balance
the uptake of above- and below-ground resources (see
\ref growth "Submodel: plant growth and mortality").

\section interact Design concepts: interactions
Competitive interactions between plant individuals were
described using the ZOI approach.

\section stoch Design concepts: stochasticity
Seed dispersal and establishment, as well as mortality of
seeds and plants are modelled stochastically to include
demographic noise. Grazing events occur randomly during
the vegetation period and the affected plants are chosen
randomly, but the individual’s probability of being grazed
depends on plant traits (see Submodel: grazing).

\section obs Design concepts: observation
See section 'Design and analysis of simulation experiments'.[not in doc here]

\section init Initialization
Initially, ten seedlings of all 81 PFTs (see section Plant traits
and PFT parameterisation) with their respective seedling
mass were randomly distributed over the grid. Their
germination probability was set to 1.0 to assure equal
initial population sizes of all PFTs. A spatially and
temporally homogenous distribution of resources (both
above- and below-ground) was used in all simulation
experiments.

\section Input
The model does not include any external input of driving
environmental variables.

\section comp Submodel: competition
Following the ZOI approach, plants compete for resources
in a circular area around their central location point. To
relate plant mass to the area covered (Ashoot), we extended
the allometric relation used by (Weiner et al. 2001)

 \f{eqnarray*}{
        A_shoot &=& c_shoot \times (f_leaf \times m_shoot)^{2/3} (1)\\
   \f}

where cshoot is a constant ratio between leaf mass and ZOI
area and mshoot is vegetative shoot mass (Tables 1, 2). The
factor fleaf is introduced to describe different shoot
geometries and is defined as the ratio between photosynthetically
active (leaf) and inactive (stem) tissue (Fig. 1).

Only the former is considered for the calculation of the
ZOI size. These circular areas are projected onto a grid of
discrete cells. Grid cells thus contain the information by
which plants they are covered, so that resource competition
can be calculated cell by cell. The resources within a cell are
shared among plants according to their relative competitive
effects (bi). The resource uptake (Dres) of plant i from a cell
with resource availability (Rescell) covered by n plants is thus
calculated as

Dresi
bi
Xn
j1
bj
Rescell (2)

Calculating bi in different ways allows including different
modes of competition (Weiner et al. 2001). We assume
that the relative competitive ability of a plant is correlated
with its maximum growth rate in the absence of resource
competition. Therefore bi is proportional to maximum
resource utilization per unit area covered (rumax, see
Submodel: plant growth and mortality and Table 2). In
the case of size-symmetric competition, bi simply equals
rumax:

birumax (3a)

In the case of partially size-asymmetric competition bi is a
function of plant mass and shoot geometry:

birumaxmshootf1
leaf : (3b)

The inverse of fleaf is used, because plants with a lower
fraction of leaf tissue are considered to be higher and thus
show a higher competitive ability by overtopping other
plants (Fig. 1). In this way, plants with equal rumax receive
equal amounts of resources from one unit of area
irrespective of their mass or height in the case of sizesymmetric
competition, while larger and higher plants
receive a higher share of resources in proportion to their
shoot geometry traits in the case of partially asymmetric
competition (Schwinning and Weiner 1998, Weiner et al.
2001). The resource uptake of one plant within one week
Figure 1. Illustration of the 'zone-of-influence' (ZOI) approach
including above- and below-ground competition and different
shoot geometries. Above- and below-ground 'zones-of-influence'
are shown as light and dark grey circles, respectively. Stems and
support tissue are represented as grey cylinders. Plant individuals
compete for resources in the areas of overlap only (arrows indicate
the area of above-ground competition). The plant to the left has a
lower ratio of leaf mass to shoot mass (fleaf) and thus a smaller
above-ground ZOI. In return its competitive ability for aboveground
resources (light) is higher as it is able to shade the plant to
the right.

To include differences between intra- and interspecific
competition, individuals of the same PFT are considered
as conspecifics and those of different PFTs as heterospecifics.
The relative competitive ability bi of one plant is
then determined as a decreasing function of the number
of plants belonging to the same PFT (nPFT) and covering
the same cell:

birumax 1
ffinffiffiffiffiffiffiffiffi PFT
p (3c)

Equation 3c is used for size-symmetric competition
instead of Eq. 3a. In the case of size asymmetry, plant
mass and geometry are taken into consideration according
to Eq. 3b. This approach represents a situation where
intraspecific competition is increased relatively to interspecific
competition and therefore implicitly includes niche
differentiation of resource competition at the cell scale,
which has been known as an important factor for species
coexistence (Chesson 2000). In the model analysis,
versions with and without niche differentiation were
compared in order to test if this assumption for competition
at the cell scale translates into a different behaviour at
the community scale.

\section growth Submodel: plant growth and mortality
Plant growth only depends on the resources that the plant
acquired during the current time step (Dres). In the absence
of competition, plants show sigmoidal growth. Therefore
we adapted the growth equation used by Weiner et al.
(2001) to the description of plant geometry used here:

Dmg

Drescshootf leafrumaxm2
shoot
m4=3
max

(4)

where g is a constant conversion rate between resource
units and plant biomass and mmax is the maximum mass
of shoot or root, respectively. In addition, the maximum
amount of resources that is allocated to growth each week
is limited by a maximum resource utilization rate given by
rumax (resource units/cm2/week) multiplied by ZOI area
(cm2). If Eq. 4 yields a negative result, Dm is set to zero
and thus negative growth is prohibited.

Growth of reproductive mass is restricted to the time
between weeks 16 to 20. In this period, a constant fraction of
the resources (5% for all PFTs) is allocated to growth of
reproductive mass (Schippers et al. 2001), and reproductive
mass is limited to 5% of shoot mass in total. The same
resource conversion rate (g) is used for reproductive and
vegetative biomass.

For the two layer model version, Eq. 1.4 are applied to
shoot and root ZOIs independently, with the difference
that for root growth the factor fleaf is always one. We assume
that the minimum uptake of above- and below-ground
resources limits plant growth (Lehsten and Kleyer 2007),
and introduced adaptive shoot/root allocation in a way that
more resources are allocated to the growth of the plant
compartment that harvests the limiting resource (Weiner
2004). For resource partitioning, we adopt the model of
Johnson (1985) and the fraction of resources allocated to
shoot growth is calculated as

ashoot
DresB
DresB DresA
(5)

where DresA is above-ground and DresB is below-ground
resource uptake. To assure comparability between the one
and two layer versions, the ‘value’ of resource units has to be
doubled in the two layer version, as resources are shared
between two plant compartments.

Plants suffer resource stress if their resource uptake (in
any layer) is below a fixed threshold fraction (thrmort) of
their optimal uptake, which is calculated as maximum
resource utilization (rumax) times ZOI area. That means
each week the condition DresBthrmortrumaxAshoot/root
is evaluated and if it is true either for shoot or root the plant
is considered as stress-exposed during this week. Consecutive
weeks of resource stress exposure (wstress) linearly
increase the probability of death

pmortpbase wstress
survmax
(6)

where survmax is the maximum number of weeks a plant
can survive under stress exposure and pbase is the
stress independent background mortality of 0.7% per
week corresponding to an annual mortality rate of 20%
(Schippers et al. 2001).

Dead plants do not grow and reproduce anymore, but
they still can shade others and are therefore still considered
for competition in the one layer model and for at least
above-ground competition in the two layer model. Each
week the mass of all dead plants is reduced by 50% and they
are removed from the grid completely as soon as their total
mass decreases below 10 mg.

\section sprod Submodel: seed production, dispersal and establishment
All plants disperse their seeds in week 21 each year. Seed
number is determined by dividing reproductive mass by the
mass of one seed (Schippers et al. 2001, Lehsten and Kleyer
2007). For each seed, dispersal distance is drawn from a lognormal
and direction from a uniform distribution (Stoyan
and Wagner 2001). To avoid edge effects on the small scale
investigated, periodic boundary conditions are used.

Germination and seedling establishment are limited to
four weeks in autumn directly after dispersal and four weeks
in spring of the next year for all PFTs. In between, a winter
mortality of 50% of seeds is assumed and all seeds which
did not germinate in these two seasons are removed.

Seedling recruitment is separated in two consecutive
processes: (1) seed germination and (2) seedling competition.
Germination is only allowed in grid cells that are not
covered by any plant or not covered in the above-ground
layer, respectively. In uncovered cells, seeds germinate with
a fixed probability (pgerm) and are converted to seedlings. In
each cell only a single plant is allowed to establish. Seedling
competition is modelled as a weighted lottery, using seed
mass as a measure of competitive ability between seedlings
(Chesson and Warner 1981, Schippers et al. 2001). The
seedling that is chosen for establishment is converted into a
plant with a shoot (and root) mass equal to seed mass. All
the other seedlings that germinated within the cell die and
are removed from the grid.

\section graz Submodel: grazing
Grazing is modelled as partial removal of an individual’s
above-ground biomass. The frequency of grazing is specified
by a constant weekly probability (pgraz) of a grazing
event. Grazing is a process that acts selectively towards
trait attributes such as shoot size and tissue properties.
Therefore, for each plant the susceptibility to grazing
(sgraz) is calculated as a function of shoot size, geometry
and PFT-specific palatability (palat).

sgrazmshootf1
leafpalat (7)

The probability for each plant to be grazed within one week
is derived by dividing individual susceptibilities by the
current maximum individual susceptibility of all plants. All
plants are checked for grazing in random order. In case a
plant is grazed, 50% of its shoot mass and its complete
reproductive mass are removed. The random choice of
plants is repeated without replacement, until 50% of the
total (above-ground) biomass on the whole grid has been
removed. When all plants have been checked for grazing
once, but less than 50% of the total above-ground biomass
has been removed, grazing probabilities for all individuals
are calculated once more based on Eq. 7 and the whole
procedure is repeated until 50% of the total above-ground
biomass has been removed or until a residual biomass is
reached which is considered ungrazable. This fraction is set
to 15 g m2 following (Schwinning and Parsons 1999).
This allows a plant individual to be grazed never or several
times during one week with a grazing event.

In addition to stochastic grazing, each year at the end of
the vegetation period 50% of the above-ground mass of all
plant individuals is removed to mimic vegetation dieback in
winter.

\section traits Plant traits and PFT parameterisation
The PFTs that are used in the model differ in their
attributes for nine plant functional traits (Table 2). These
are grouped into four trait syndromes based on well
documented tradeoffs and trait correlations. The traits we
chose comprise a subset of the 'common core list of plant
traits', proposed by Weiher et al. (1999).

The growth form of a plant is characterized by the ratio
between leaf mass and total shoot mass (fleaf). Plants with a
low fleaf use more biomass to build up support tissue (e.g.
stems) instead of leave mass. Therefore this trait implicitly
includes a tradeoff between plant height and leaf area or
between competitive ability for light versus relative growth
rate without competition, respectively (Fig. 1; Eq. 1, 3b).

Maximum plant mass is the second trait that describes
plant geometry and is positively related to actual plant
mass, according to the sigmoidal growth equation (Eq. 4).
In the two-layer version equal maximal masses for shoot
and root are assumed. Individual susceptibility to grazing
is modelled as a function of fleaf and mshoot (Eq. 7) as
higher and larger plants tend to be more affected by
grazing (Diaz et al. 2001).

Maximum plant mass and individual seed mass are
assumed to be positively correlated and combined within a
trait syndrome (Eriksson and Jakobsson 1998). The welldocumented
seed size versus seed number tradeoff emerges
in our model, because PFTs with higher seed mass produce
less seeds from the same amount of reproductive mass
(Leishman 2001, Schippers et al. 2001, Westoby et al.
2002). The disadvantage of a low seed number is balanced
by the higher recruitment success of large seedlings, due to
the weighted lottery of seedling competition and the higher
initial mass of plants that germinated from large seeds
(Jakobsson and Eriksson 2000). A negative correlation
between seed mass and dispersal distance was assumed
here, which is supported at least for wind-dispersed seeds
(Jongejans and Schippers 1999). For simplicity, equal
values are used for mean and standard deviation of the
dispersal kernel assuming a higher variance for the dispersal
distances of smaller seeds.

The response of plants to different resource conditions is
distinguished into categories reflecting the tradeoff between
competitive ability and stress tolerance (Grime 2001).

Maximum growth rate in resource-rich environments is
given as the maximum resource utilization per shoot/root
area (rumax), while stress resistance is specified by the
maximal number of weeks a plant can survive under
resource stress exposure (survmax). Accordingly, PFTs with
high rumax feature a low stress resistance and vice versa.

We distinguished two strategies of response to grazing:
(1) grazing tolerance by fast re-growth of removed biomass
and (2) grazing avoidance by low palatability through
defence structures or secondary compounds (Bullock et al.
2001). In our model, the relationship between leaf mass and
leaf area is given by the parameter cshoot (Eq. 1). This
parameter is functionally analogous to the specific leaf area
(SLA), which was proposed by Westoby et al. (2002) as a
key trait to characterize plant strategies. High SLA is related
to high efficiency of light interception and fast growth,
while leaves with low SLA show higher longevity, structural
strength or high allocation to defensive compounds
(Westoby et al. 2002). Accordingly, a high cshoot value for
grazing tolerance versus a low value for grazing avoidance is
used here. The positive effect of defence compounds is
expressed as a low palatability (palat) and thus, a low
susceptibility towards grazing (Eq. 7).

For each of the four functional traits three attributes
were distinguished. The parameter values used should
represent the range of contrasting plant strategies with
respect to resource response, competitive ability and
grazing response that could potentially occur in grassland
ecosystems. Combining all three attributes of all four
functional traits allowed us to define 34 by 81 PFTs in total
(Table 3).

source: \ref bib "May et al. (2009)"
*/
//-eof--------------------------------------------------------------------------

