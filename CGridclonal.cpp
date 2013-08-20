/**  \file
   \brief functions of class CclonalGrid
*/
//---------------------------------------------------------------------------
//#pragma hdrstop

#include "CGridclonal.h"
#include "environment.h"
#include <iostream>
#include <functional>
#include <sstream>
//---------------------------------------------------------------------------

// #pragma package(smart_init)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CGridclonal::CGridclonal() : CGrid(){
   SclonalTraits::ReadclonalTraits();
}  // Constructor
//---------------------------------------------------------------------------
/**
   Constructor to load a grid from files.

\warning does not initiate pft definitions.
this had to be done at least vie prior dummi grid
   \autor KK
   \date 120905
*/
CGridclonal::CGridclonal(string id) : CGrid(id){
//   SclonalTraits::ReadclonalTraits();
}  // Constructor
//----------------------------------------------------------
void CGridclonal::resetGrid()
{
   CGrid::resetGrid();
   for(unsigned int i=0;i<GenetList.size();i++) delete GenetList[i];
   GenetList.clear();CGenet::staticID=0;
}
//---------------------------------------------------------------------------
CGridclonal::~CGridclonal(){
   for(unsigned int i=0;i<GenetList.size();i++) delete GenetList[i];
   GenetList.clear();CGenet::staticID=0;
}   // Destructor
//---------------------------------------------------------------------------
/**
  File saving the entire clonal grid for later reload.

  \note not included: LDD-Seed list ; genet info - IDs are included in ramets
  \autor KK
  \date 120831
*/
void CGridclonal::Save(string fname){
//open file..
   ofstream SaveFile(fname.c_str());
  if (!SaveFile.good()) {cerr<<("Fehler beim Öffnen InitFile");exit(3); }
  cout<<"SaveFile: "<<fname<<endl;
//write..

//Cells (without Plants, with seeds)
//  SaveFile<<"\nNumber of Cells\t"<<SRunPara::RunPara.GetSumCells() <<endl;
  for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i)  //loop for all cells
     SaveFile<<CellList[i]->asString();

//Plants
  SaveFile<<"\nNumber of Plants\t"<<this->PlantList.size();
  for (int i=0; i<this->PlantList.size(); ++i)  //loop for all cells
//    SaveFile<<"\nPlant "<<i;//Plant->asString()
     SaveFile<<PlantList[i]->asString();
//genet information
  SaveFile<<"\nNumber of Genets\t"<<this->GetNMotherPlants();

}  // file save of entire grid
//-----------------------------------------------------------------------------
/**
  Set a number of randomly distributed clonal Plants (CclonalPlant)
  of a specific trait-combination on the grid.

  \param traits   CPftTraits of the plants to be set
  \param cltraits SclonalTraits of the plants to be set
  \param n        number of Individuals to be set
*/
void CGridclonal::InitClonalPlants(
  SPftTraits* traits,SclonalTraits* cltraits,const int n)
{
   //using CEnvir::nrand;using SRunPara::RunPara;
   int  x, y;
   int SideCells=SRunPara::RunPara.CellNum;
   CCell* cell;

   for (int h=0; h<n; h++)
   {
      do{
        x=CEnvir::nrand(SideCells);
        y=CEnvir::nrand(SideCells);
        cell = CellList[x*SideCells+y];
      }while (cell->occupied);
      if (!cell->occupied)
      {
         CclonalPlant *myplant = new CclonalPlant(
              traits,cltraits,cell);

         CGenet *Genet= new CGenet();
//         Genet->number=GenetList.size();
         GenetList.push_back(Genet);
         myplant->setGenet(Genet);

         myplant->mshoot=myplant->Traits->MaxMass/2.0;
         myplant->mroot=myplant->Traits->MaxMass/2.0;

         PlantList.push_back(myplant);
      } //if cell not occupied
   } //for all n
}//end CGridclonal::clonalPlantsInit()
//-----------------------------------------------------------------------------
/**
  Set a number of randomly distributed clonal Seeds (CclonalSeed) of a specific
  trait-combination on the grid.

  \param traits   SPftTraits of the seeds to be set
  \param cltraits SclonalTraits of the seeds to be set
  \param n        number of seeds to be set
  \param estab    seed establishment (CSeed) - default is 1
  \since 2010-09-10 estab rate for seeds can be modified (default is 1.0)

*/
void CGridclonal::InitClonalSeeds(
  SPftTraits* traits,SclonalTraits* cltraits,const int n,double estab, int x, int y)
{ //init clonal seeds in random cells  if x or y are <0
//   using CEnvir::nrand;using SRunPara::RunPara;
//   int x,y;
   int SideCells=SRunPara::RunPara.CellNum;
if (x<0 || y<0)
   for (int i=0; i<n; ++i){
        x=CEnvir::nrand(SideCells);
        y=CEnvir::nrand(SideCells);

   }
   CCell* cell = CellList[x*SideCells+y];
   new CclonalSeed(estab,traits,cltraits,cell);
} //end CGridclonal::clonalSeedsInit()
//-----------------------------------------------------------------------------
/**
  The clonal version of PlantLoop additionally to the CGrid-version
  disperses and grows the clonal ramets
*/
void CGridclonal::PlantLoop()
{
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant)
   {
      CPlant* plant = *iplant;
//         cout<<"\nW"<<CEnvir::week<<" "<<plant->xcoord<<";"<<plant->ycoord;

      if (!plant->dead)
      {
         plant->Grow2();
         if ((plant->type() != "CPlant"))//only if its a clonal plant
         {
            //ramet dispersal in every week
            DispersRamets((CclonalPlant*) plant);
            //if the plant has a growing spacer - grow it
            ((CclonalPlant*) plant)->SpacerGrow();
         }
         //seed dispersal (clonal and non-clonal seeds)
         if (CEnvir::week>plant->Traits->DispWeek)
//           (*LDDSeeds)[plant->pft()]+=DispersSeeds(plant);
              addLDDSeeds(plant->pft(),DispersSeeds(plant));
         plant->Kill();
      }//else cout<<"\tplant dead";
      plant->DecomposeDead();

   }
}//plant loop
//-----------------------------------------------------------------------------
int CGridclonal::DispersSeeds(CPlant* plant)
{
//   using CEnvir::Round;using SRunPara::RunPara;
   int px=plant->getCell()->x, py=plant->getCell()->y;
   int NSeeds=0;
   double dist=0, direction=0;
   double rnumber=0;
   int nb_LDDseeds=0;
   int SideCells=SRunPara::RunPara.CellNum;

   NSeeds=plant->GetNSeeds();

   for (int j=0; j<NSeeds; ++j)
   {
        int x=px,y=py;
         //negative exponential dispersal kernel
         //dist=RandNumGen->exponential(1/(plant->DispDist*100));   //m -> cm

         //negative exponential dispersal kernel
         //dist=RandNumGen->exponential(1/(plant->DispDist*100));   //m -> cm

         //lognormal dispersal kernel
         getTargetCell(x,y,
           plant->Traits->Dist*100,        //m -> cm
           plant->Traits->Dist*100);       //mean = std (simple assumption)
         //export LDD-seeds
         if (SRunPara::RunPara.torus){Boundary(x,y);}
         else if (Emmigrates(x,y)) {nb_LDDseeds++;continue;}

         CCell* cell = CellList[x*SideCells+y];
         DispSeeds_help(plant,cell);
   }//for NSeeds
   return nb_LDDseeds;
}//end DispersSeeds
void CGridclonal::DispSeeds_help(CPlant* plant,CCell* cell)
{
         if (plant->type()=="CPlant")  // for non-clonal seeds
         {
            new CSeed(plant,cell);
         }

         if (plant->type()=="CclonalPlant")  //for clonal seeds
         {
            new CclonalSeed((CclonalPlant*) plant,cell);
         }

}    //

//-----------------------------------------------------------------------------
///
/**

*/
void CGridclonal::DispersRamets(CclonalPlant* plant)
{
   double CmToCell=1.0/SRunPara::RunPara.CellScale();
//   using CEnvir::Round;

//   if (plant->type() == "CclonalPlant" ||
//       plant->type() == "CWaterPlant")//only if its a clonal plant
   {
        //dispersal
        for (int j=0; j<plant->GetNRamets(); ++j)
        {
         double dist=0, direction;//, rdist;
         double mean, sd, mu, sigma; //parameters for lognormal dispersal kernel

         //normal distribution for spacer length
         mean=plant->clonalTraits->meanSpacerlength;   //cm
         sd  =plant->clonalTraits->sdSpacerlength;     //mean = std (simple assumption)

         while (dist<=0) dist=CEnvir::normrand(mean,sd);
         //direction uniformly distributed
         direction=2*Pi*CEnvir::rand01();
         int x=CEnvir::Round(plant->getCell()->x+cos(direction)*dist*CmToCell);
         int y=CEnvir::Round(plant->getCell()->y+sin(direction)*dist*CmToCell);

         /// \todo change boundary conditions
         Boundary(x,y);   //periodic boundary condition

         // save dist and direction in the plant
         CclonalPlant *Spacer=newSpacer(x/CmToCell,y/CmToCell,plant);
         Spacer->SpacerlengthToGrow=dist;
         Spacer->Spacerlength=dist;
         Spacer->Spacerdirection=direction;
         plant->growingSpacerList.push_back(Spacer);
      }//end for NRamets
  }  //end for if it a clonal plant
}//end CGridclonal::DispersRamets()
//--------------------------------------------------------------------------
/**
  For each plant ramets establish and
  for each grid cell seeds from seed bank germinate and establish.
  Seedlings that do not establish will die.

  -# ramets establish if goal is reached (RametEstab())
  -# seeds establish from the seed bank during
    germination time (weeks 1-3, 22-25).
  -# seedlings that fail to establish will die

  \par revision
  I tried to make function faster skipping the seed-pft-collecting
  cumulative lists, but that possibly leads to pseudo-endless loops if
  seedling-number per cell is too high

  \note this function \b completely reimplements CGrid::EstabLottery()
  because of different code for clonal and non-clonal seedlings
  \todo find a faster algorithm for choosing the winning seedling

*/
void CGridclonal::EstabLottery()
{
  //Ramet establishment for all Plants
   int PlantListsize=PlantList.size();
   for (int z=0; z<PlantListsize;z++)  //for all Plants (befor Rametestab)
   {
      CPlant* plant=PlantList[z];
      if ((plant->type() != "CPlant")&&(!plant->dead))
      {//only if its a clonal plant
         RametEstab((CclonalPlant*)plant);
      }
   }//for all plants

//for Seeds (for clonal and non-klonal plants)
   map<string,double> PftEstabProb;//=map<string,int>(0);
   map<string,int> PftNSeedling;
   int gweek=CEnvir::week;

   if (((gweek>=1) && (gweek<4)) || ((gweek>21)&&(gweek<=25)))
   { //establishment only between week 1-4 and 21-25
     double sum=0;

     for (int i=0; i<SRunPara::RunPara.GetSumCells(); ++i)
     {  //loop for all cells
        CCell* cell = CellList[i];
        if ((cell->AbovePlantList.empty())
            && (!cell->SeedBankList.empty())
            && (!cell->occupied))
        {  //germination if cell is uncovered
          sum=cell->Germinate();
          if (sum>0){//if seeds germinated...
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
                   bind2nd(mem_fun(&CSeed::SeedOfType),pft)));
//                   bind(SeedOfType,_1,pft)));
                //Was, wenn keine Seedlings(typ==pft) gefunden werden (sollte nicht passieren)?
                //etabliere jetzt das erste Element der Liste
                CSeed* seed = cell->SeedlingList.front();
                EstabLott_help(seed);
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
  }//if between week 1-4 and 21-25
  PftEstabProb.clear();
  PftNSeedling.clear();
}//end CGridclonal::EstabLottery()

/**obsolete with watergrid simulations*/
void CGridclonal::EstabLott_help(CSeed* seed){
//cout<<"estabLott_help - CGridClonal";
                  CPlant* plant;
                  if (seed->type()=="CSeed")
                  {
                     plant = new CPlant(seed);
                  }

                  if (seed->type()=="CclonalSeed")
                  {
                     CclonalPlant* tempPlant = new CclonalPlant((CclonalSeed*)seed);

                     CGenet *Genet= new CGenet();
                     GenetList.push_back(Genet);

                     tempPlant->setGenet(Genet);
                     plant=tempPlant;
                  }
                  PlantList.push_back(plant);


}

//--------------------------------------------------------------------------
/**
  Establishment of ramets. If spacer is readily grown tries to settle on
  current cell.
  If it is already occupied it finds a new goal nearby.
  If the cell is empty, the ramet establishes:
  cell information is set and the ramet is added to the global plant list,
  the genet's ramet list as well as erased
  from the spacer list of the mother plant.
*/
void CGridclonal::RametEstab(CclonalPlant* plant)
{
 //  using CEnvir::rand01;
   int RametListSize=plant->growingSpacerList.size();

   if (RametListSize==0)return;
   for (int f=0; f<(RametListSize);f++)    //loop for all Ramets of one plant
   {
      CclonalPlant* Ramet = plant->growingSpacerList[f];
      if (Ramet->SpacerlengthToGrow<=0){//return;
/// \todo hier boundary-Kontrolle einfügen

        int x=CEnvir::Round(Ramet->xcoord/SRunPara::RunPara.CellScale());
        int y=CEnvir::Round(Ramet->ycoord/SRunPara::RunPara.CellScale());

        //find the number of the cell in the List with x,y
        CCell* cell=CellList[x*SRunPara::RunPara.CellNum+y];

        if ((!cell->occupied))
        {  //establish if cell is not central point of another plant
           Ramet->getGenet()->AllRametList.push_back(Ramet);
           Ramet->setCell(cell);

           PlantList.push_back(Ramet);
//cout<<"\nnew Ramet "<<Ramet->pft()<<": "<<x<<";"<<y<<endl;
           //delete from list but not the element itself
           plant->growingSpacerList.erase(plant->growingSpacerList.begin()+f);
           //establishment success
           if(CEnvir::rand01()<(1.0-SRunPara::RunPara.EstabRamet)) Ramet->dead=true; //tag:SA
        }//if cell ist not occupied
        else //find another random cell in the area around
        {
           if (CEnvir::week<CEnvir::WeeksPerYear)
           {
             int factorx;int factory;
             do{
             factorx=CEnvir::nrand(5)-2;
             factory=CEnvir::nrand(5)-2;
             }while(factorx==0&&factory==0);

             double dist=Distance(factorx,factory);
             double direction=acos(factorx/dist);
             double cellscale=SRunPara::RunPara.CellScale();
             int x=CEnvir::Round((Ramet->xcoord+factorx)/cellscale);
             int y=CEnvir::Round((Ramet->ycoord+factory)/cellscale);

             /// \todo change boundary conditions
             Boundary(x,y);

             //new position, dist and direction
             Ramet->xcoord=x*cellscale; Ramet->ycoord=y*cellscale;
             Ramet->SpacerlengthToGrow=dist;
             Ramet->Spacerlength=dist;
             Ramet->Spacerdirection=direction;
           }
           if (CEnvir::week==CEnvir::WeeksPerYear)
           {
             //delete element - ramet dies unestablished
             delete Ramet;
             plant->growingSpacerList.erase(plant->growingSpacerList.begin()+f); //delete
           }
        }//else
      }//end if pos reached
   }//loop for all Ramets
}//end CGridclonal::RametEstab()
//-----------------------------------------------------------------------------
/**
  Distributes local resources according to local competition
  and shares them between connected ramets of clonal genets.
*/
void CGridclonal::DistribResource()
{
   CGrid::DistribResource();
   Resshare();  // resource sharing betwen connected ramets
} //end CGridclonal::DistribResource()
//----------------------------------------------------------------------------
/**
  Resource sharing between connected ramets on grid.
*/
void CGridclonal::Resshare() // resource sharing
{
      for (unsigned int i=0; i<GenetList.size();i++)
      {
         CGenet* Genet = GenetList[i];
         if (Genet->AllRametList.size()>1)//!Genet->AllRametList.empty())
         {
            CclonalPlant* plant=Genet->AllRametList.front();
            if (plant->type()!="CPlant"
                 &&plant->clonalTraits->Resshare==true)
            {//only betwen connected ramets
               Genet->ResshareA();  //above ground
               Genet->ResshareB();  // below ground
            } //if Resshare true
         }//if genet>0
      }//for...
}//end CGridclonal::Resshare()
//-----------------------------------------------------------------------------
/**
  Delete a plant from the grid and it's references in genet list and grid cell.
*/
void CGridclonal::DeletePlant(CPlant* plant1)
{
   if (plant1->type()!="CPlant")
   {
      CGenet *Genet=((CclonalPlant*)plant1)->getGenet();
      //suche Ramet aus Liste und entferne ihn
      for (unsigned int j=0;j<Genet->AllRametList.size();j++)
      {
        CclonalPlant* Ramet;
        Ramet=Genet->AllRametList[j];
        if (plant1==Ramet)
          Genet->AllRametList.erase(Genet->AllRametList.begin()+j);
      }//for all ramets
   }//if clonalplant
   CGrid::DeletePlant(plant1);
} //end CGridclonal::DeletePlant
//-----------------------------------------------------------------------------
/**
  \return number of clonal plants on grid
*/
int CGridclonal::GetNclonalPlants() //count clonal plants
{
  int NClonalPlants=0;
  for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant)
  {
    CPlant* plant = *iplant;
    //only if its a clonal plant
    if ((plant->type() != "CPlant")&&(!plant->dead)) NClonalPlants++;
  }
  return NClonalPlants;
}//end CGridclonal::GetNclonalPlants()
//-----------------------------------------------------------------------------
/**
  \return number of non-clonal plants on grid
*/
int CGridclonal::GetNPlants() //count non-clonal plants
{
  int NPlants=0;
  for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant)
  {
    CPlant* plant = *iplant;
    //only if its a non-clonal plant
    if ((plant->type() == "CPlant")&&(!plant->dead)) NPlants++;
  }
  return NPlants;
}//end CGridclonal::GetNPlants()
//-----------------------------------------------------------------------------
/**
  \return the number of genets with at least one ramet still alive
*/
int CGridclonal::GetNMotherPlants() //count genets
{
   int NMotherPlants=0;
   if (GenetList.size()>0)
   {
      for (unsigned int i=0; i<GenetList.size();i++)
      {
         CGenet* Genet = GenetList[i];
         if ((Genet->AllRametList.size()>0))
         {
            unsigned int g=0;
            do {g++;} while (
              (Genet->AllRametList[g-1]->dead)&&(g<Genet->AllRametList.size()));
            if (!Genet->AllRametList[g-1]->dead) NMotherPlants++;
         }//for all ramets
      }//for all genets
   }//if there are genets
return NMotherPlants;
}//end CGridclonal::GetNMotherPlants()
//------------------------------------------------------------------------------
/**
  Counts a cell covered if the list of aboveground ZOIs has length >0.

  \note Call the function after updating weekly ZOIs
  in function CGrid::CoverCells()

  \return the number of covered cells on grid
*/
int CGridclonal::GetCoveredCells()//count covered cells
{
   int NCellsAcover=0;
   const int sumcells=SRunPara::RunPara.GetSumCells(); //hopefully faster
   for (int i=0; i<sumcells; ++i)
   {
      if (CellList[i]->AbovePlantList.size()>0) NCellsAcover++;
   }//for all cells
   return NCellsAcover;
}//end CGridclonal::GetCoveredCells()
//------------------------------------------------------------------------------
double CGridclonal::GetNGeneration() //calculate the mean generations per genet
{
   double SumGeneration=0;
   double Sum=0;
   double highestGeneration;
   double MeanGeneration=0;

   if (GenetList.size()>0)
   {
      for (unsigned int i=0; i<GenetList.size();i++)
      {
         CGenet* Genet;
         Genet = GenetList[i];
         if ((Genet->AllRametList.size()>0))
         {
            highestGeneration=0;
            for (unsigned int j=0;j<Genet->AllRametList.size();j++)
            {
               CclonalPlant* Ramet;
               Ramet=Genet->AllRametList[j];
               highestGeneration=max(highestGeneration,double (Ramet->Generation));
            }
            SumGeneration+=highestGeneration;
            Sum++;
         }//if genet has ramets
      }//for all ramets
      if (Sum>0) MeanGeneration=(SumGeneration/Sum);
      //else MeanGeneration=0;
   }
   return MeanGeneration;
}//end CGridclonal::GetNGeneration()
//-eof-----------------------------------------------------------------------------

