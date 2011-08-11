/**\file
   \brief functions of class CGrid
*/
#pragma package(smart_init)
#pragma hdrstop

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
   CellList = new (CCell* [SideCells*SideCells]);

   for (int x=0; x<SideCells; x++){
      for (int y=0; y<SideCells; y++){
         index=x*SideCells+y;
         CCell* cell = new CCell(x,y,
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
      CCell* cell = CellList[i];
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
      CCell* cell = CellList[i];
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

         CCell* cell = CellList[x*SRunPara::RunPara.CellNum+y];
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
        CCell* cell = CellList[index];

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
        CCell* cell = CellList[index];
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
      CCell* cell = CellList[i];
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
      CCell* cell = CellList[i];
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
   double* PftCumEstabProb= new double[RunPara.NPft];
   int* PftCumNSeedling   = new int[RunPara.NPft];

      for (int i=0; i<RunPara.GetSumCells(); ++i){  //loop for all cells
         CCell* cell = CellList[i];
         if ((cell->getCover(1)==0) && (!cell->SeedBankList.empty())){  //germination if cell is uncovered

            sum=cell->Germinate();

            //relative probability for establishment for all Pfts (cumulated)
            for (int pft=0; pft<RunPara.NPft; ++pft){
               PftCumEstabProb[pft]=(double) cell->PftNSeedling[pft]
                  *SPftTraits::PftList[pft]->SeedMass/sum;
               PftCumNSeedling[pft]=cell->PftNSeedling[pft];
               if (pft>=1){
                  PftCumEstabProb[pft]+=PftCumEstabProb[pft-1];
                  PftCumNSeedling[pft]+=PftCumNSeedling[pft-1];
               }
            }

            if (!cell->SeedlingList.empty()){
               int pft=0;
               double rnum=CEnvir::rand01();
               //chose seedling that establishes at random
               while ((!cell->occupied) && (pft<SRunPara::RunPara.NPft)){
                  if (rnum<PftCumEstabProb[pft]){
                     //Sort after TypeID
                     cell->SortTypeID();
                     sort(cell->SeedlingList.begin(),
                          cell->SeedlingList.end(),
                          CompareTypeID);
                     index=CEnvir::nrand(cell->PftNSeedling[pft])
                       +PftCumNSeedling[pft]
                       -cell->PftNSeedling[pft];
                     CSeed* seed = cell->SeedlingList[index];
                     CPlant* plant = new CPlant(seed);
                     plant->setCell(cell);
                     PlantList.push_back(plant);
                     cell->PftNSeedling[seed->Traits->TypeID-1]--;
                  }//if
                  ++pft;
               }//while

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
      CCell* cell = CellList[i];
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

   double mass_cut = SRunPara::RunPara.CutMass;
   double mass_removed=0;

   for (plant_size i=0; i<PlantList.size();i++){
         pPlant = PlantList[i];
         if (pPlant->mshoot/(pPlant->Traits->LMR*pPlant->Traits->LMR) > mass_cut){
            double to_leave= mass_cut*(pPlant->Traits->LMR*pPlant->Traits->LMR);
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
        CCell* cell = CellList[index];
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
      CCell* cell = CellList[i];
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

         CCell* cell = CellList[x*SideCells+y];
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
        CCell* cell = CellList[x*SideCells+y];
        new CSeed(estab,traits,cell);
     }
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
      CCell* cell = CellList[i];
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
      CCell* cell = CellList[i];
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
//-eof--------------------------------------------------------------------------

