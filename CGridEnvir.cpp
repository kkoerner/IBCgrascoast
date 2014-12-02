/*
 * CGridEnvir.cpp
 *
 *  Created on: 24.04.2014
 *      Author: KatrinK
 */

#include "CGridEnvir.h"
#include <sstream>
#include <iostream>
using namespace std;
//------------------------------------------------------------------------------
/** \page loadsave Loading and Saving environments
Since initalization phase on grid takes a lot of simulation time,
with this option we aim to shorten simulation runs that look at performance
after environmental changes to an existing community. We should be able to
generate a set of (parameter-defined) 'starting points' with repititions. Thus
we are able to compare the community with and without environmental change.

\par Saving.
All state variables of
  - the environment (CClonalGridEnvir::Save()),
  - the grid (CGridclonal::Save()),
  - the plants (CPlant::asString(), CclonalPlant::asString())
  - and cells  (CCell::asString())
are saved in files with one core id code to identify associated files.

\par Loading.
An environment is loaded mainly via constructors of the various
structures and classes. One core-string (\verb'id') defines the set of files
belonging to one task.

\note saving should work already, while loading ability still is in process
\sa CClonalGridEnvir::CClonalGridEnvir(string id), CEnvir::CEnvir(string id),
 CGridclonal::CGridclonal(string id), CGrid::CGrid(string id), ...

\author KK
\date 120905
*/

//------------------------------------------------------------------------------
/**
 * constructor
 */
CGridEnvir::CGridEnvir():CEnvir(),CGrid()
{
   ReadLandscape();
}
/**
   Constructor - load a previously saved environment.

   \note only state variables are restored
   \param id core string for the set of saved files
   \author KK
   \date  120905
   \todo not yet updated after 'Update2.0'
*/
CGridEnvir::CGridEnvir(string id):CGrid(id),CEnvir(id)
{
  //here re-eval clonal PFTFile
  string dummi=(string)"Save/E_"+id+".sav";
  ifstream loadf0(dummi.c_str()); string d;
  getline(loadf0,d);//>>year>>week;

  //fill PftLinkList
SPftTraits::ReadPFTDef(SRunPara::NamePftFile,-1);
  //open file..
 dummi=(string)"Save/G_"+id+".sav";
 ifstream loadf(dummi.c_str());
 getline(loadf,d);
 int x=0,y=0;int xmax=SRunPara::RunPara.CellNum-1;
   //load cells..
   //loop over cell entries
   do{
     loadf>>x>>y;
     getline(loadf,d);getline(loadf,d);
     while(d!="CE"){
       //set seeds of type x
       stringstream mstr(d);
       string type;int num;
       mstr>>type>>num;
        CGrid::InitSeeds(SPftTraits::getPftLink(type),num,x,y,0);
        //or InitClonalSeeds(..) for clonal types
       getline(loadf,d);
     }

   }while(!(x==xmax&&y==xmax));
   //load Plants..
   int num;
 loadf>>d>>d>>d>>num;getline(loadf,d);
 cout<<"lade "<<num<<"plant individuals.."<<endl;
 do {getline(loadf,d);}while(InitInd(d));
  ReadLandscape();
}
//------------------------------------------------------------------------------
/**
 * destructor
 */
CGridEnvir::~CGridEnvir()
{
//  for (unsigned int i=0;i<ClonOutData.size();i++)  delete ClonOutData[i];
}
//------------------------------------------------------------------------------
/**
  Initiate new Run: reset grid and randomly set initial individuals.
*/
void CGridEnvir::InitRun(){
  CEnvir::InitRun();
  resetGrid();

  //set initial plants on grid...
  InitInds("Input\\RSpec28.txt");

//  another input file for seed rain runs..
//   string mfile("");
//   mfile="Input\\InitPFTdat_seedrain.txt";
//   InitInds(mfile);


//  cout<<" sum of types: "<<PftInitList.size()<<endl;
  init=1; //start new
}


//------------------------------------------------------------------------------

/**      new, flexible version of initialisation that permits to read clonal and other traits from one file

  this function reads a file, introduces PFTs and initializes seeds on grid
  after file data.

  (each PFT on file gets 10 seeds randomly set on grid)
  \since 2012-04-18

  \param n position of type initiates for monoculture
  \param file file name of simulation definitions
\todo init pft defs elsewhere
*/
void CGridEnvir::InitInds(string file,int n){
  const int no_init_seeds=10;//10;
//PFT Traits are read in GetSim()
	// initialization
	for (map<string,SPftTraits*>::iterator var = SPftTraits::PftLinkList.begin();
			var != SPftTraits::PftLinkList.end(); ++var) {
		SPftTraits* traits=var->second;
		InitClonalSeeds(traits, no_init_seeds); //cltraits,

		PftInitList[traits->name] += no_init_seeds;
		cout << " init " << no_init_seeds << " seeds of Pft: " << traits->name
				<< endl;
        CEnvir::SeedRainGr.PftSeedRainList[traits->name] = SRunPara::RunPara.SeedInput;

		if (n > -1) {
			SRunPara::RunPara.NPft = PftInitList.size();
			return;
		}
	}
	this->SeedRainGr.GetNPftSeedsize();
	this->SeedRainGr.GetNPftSeedClonal();
}//initialization based on file
//------------------------------------------------------------------------------
/** initialize individuals with size and type
 on a predifened grid
 \param def definition string
 \return did it work? (flag)
 \todo clonal plants are not yet restored correctly
 */
bool CGridEnvir::InitInd(string def){
  stringstream d(def);
  //get cell
  int x,y;  d>>x>>y;
  //frage streamzustand ab ; wenn nicht good, beende Funktion
  if (!d.good())
  return false;
  CCell* cell = this->CellList[x*SRunPara::RunPara.CellNum+y];

  string type;double mshoot, mroot, mrepro; int stress;bool dead;
  d>>type>>mshoot>>mroot>>mrepro>>stress>>dead;

  //for nonclonal CPlant
  CPlant* plant = new CPlant(SPftTraits::getPftLink(type),cell,mshoot,mroot,mrepro,
                  stress,dead);

  int generation,genetnb; double spacerl=0, spacerl2grow=0;
  d>>generation>>genetnb; if (!d.eof())d>>spacerl>>spacerl2grow;
  //set Genet
  CGenet* genet=NULL;
  for (unsigned int i=0; i<GenetList.size();i++){
    if (GenetList[i]->number==genetnb)genet=GenetList[i];
  }
  if (!genet) {
    CGenet::setStaticId(max(CGenet::getStaticId(),genetnb));
    genet=new CGenet();genet->number=genetnb;
    GenetList.push_back(genet);
  }
  plant->setGenet(genet);

  this->PlantList.push_back(plant);
//  cout<<"Init "<<type<<" at "<<x<<":"<<y
//      <<" ("<<mshoot<<", "<<mroot<<", "<<mrepro<<", "<<stress<<", "<<dead<<")\n";
  return true;
}//<init of one ind based on saved data

//------------------------------------------------------------------------------
/**
 This function initiates a number of seeds of the specified type on the grid.

 \param type string naming the type to be set
 \param number number of seeds to set
*/
void CGridEnvir::InitSeeds(string type, int number)
{
   //searching the type
   SPftTraits *pfttraits=SPftTraits::getPftLink(type);//=SclonalTraits::clonalTraits[Cltype];
   //set seeds...
   CGrid::InitClonalSeeds(pfttraits,number,pfttraits->pEstab);//cltraits,
}//InitSeeds


//------------------------------------------------------------------------------
/**
  one run of simulations

  environmental prefenences are stored in SRunPara::RunPara

  \since 2012/08 seed rain added to test peak theory
*/
void CGridEnvir::OneRun(){

        ResetT (); // reset time

   //get initial conditions
   init=1; //for init the second plant (for the invasion experiments)
   //run simulation until YearsMax (SRunPara::RunPara.Tmax)
   do{
      this->NewWeek();
      cout<<" y"<<year;
      OneYear();
		WriteOFiles(); //to be adapted

//save grid
//comment out: not used at moment
//     if (year==5) {
//        stringstream v; v<<"B"<<setw(3)<<setfill('0')<<CEnvir::RunNr;
//        this->Save(v.str());
//      }
      if (endofrun)break;

   }while(year<SRunPara::RunPara.Tmax);//years

   //save grid
   //stringstream v; v<<"B"<<setw(3)<<setfill('0')<<CEnvir::RunNr<<"E"<<CEnvir::SimNr;
   //this->Save(v.str());

}  // end OneSim
//------------------------------------------------------------------------------
/**
 * calculate one year's todos.
 */
void CGridEnvir::OneYear(){
   do{
cout<<year<<" w "<<week<<endl;
	   OneWeek();
	   //detailed documentation
 //    cout<<"writing grid..\n";
             WriteGridComplete(false);//report last year
	   
	     fflush(NULL);//write immediately on console
     //exit conditions
      exitConditions();
      if (endofrun)break;
  }while(++week<=WeeksPerYear);//weeks
} // end OneYear
//------------------------------------------------------------------------------
/**
  calculation of one week's todos

  \since 2010-07-07 no invasion version
*/
void CGridEnvir::OneWeek(){
   ResetWeeklyVariables(); //cell loop, removes data from cells
   SetCellResource();      //variability between weeks

   CoverCells();           //plant loop
   setCover();             //set ACover und BCover lists, as well as type cover
   DistribResource();      //cell loop, resource uptake and competition

   PlantLoop();            //Growth, Dispersal, Mortality

   if (year>1) Disturb();  //grazing and disturbance

   RemovePlants();         //remove trampled plants

   if ((SRunPara::RunPara.SeedRainType > 0) && (week == 21))//seed rain in seed dispersal week
       SeedRain();
   EstabLottery();         //for seeds and ramets

   if (week==20) SeedMortAge(); //necessary to remove non-dormant seeds before autumn
   if (week==WeeksPerYear){     //at end of year ...
      Winter();            //removal of above ground biomass and of dead plants
      SeedMortWinter();    //winter seed mortality
   }

   if (true){//(week==20){        //general output
      GetOutput();   //calculate output variables
   }
   if (true){//(week==30){
      //get cutted biomass
      GetOutputCutted();
      //get grazed biomass
      GetOutputGrazed();
      //clonal output
//      GetClonOutput();   //calculate output variables - now in week 20
   }
}//end CClonalGridEnvir::OneWeek()
//---------------------------------------------------------------------------
/**
  Exit conditions for runs,
  where clonal Plants migrate into a non-clonal community or
  vice versa.
  \since clonal version

  changed for Lina's realistic-pft-experiments
*/
int CGridEnvir::exitConditions()
{
     int currTime=GetT();
     int NPlants=GetNPlants();//+GetNclonalPlants();
     int NClPlants=GetNclonalPlants();

//    if pop growth too high
     if ((NPlants + NClPlants)>5000)//==0)
//    if no more individuals existing
//     if ((NPlants + NClPlants)==0)
     {
        endofrun=true;
        return currTime; //extinction time
     }
     return 0;
}//end CClonalGridEnvir::exitConditions()

//---------------------------------------------------------------------------
/**
  calculate Output-variables and store in intern 'database'

  Juveniles and adults (>6weeks) are counted separately

  base mortality is computed based on pft cover (100% cover increases base mortality of 0.007 up to 0.056)

  changed in Version 100715 - for  type-flexible Output
*/
void CGridEnvir::GetOutput()//PftOut& PftData, SGridOut& GridData)
{
 //  int pft, df;
   string pft_name;
   double  prop_PFT;//mean,
   double aLitter=0, bLitter=0;

   SPftOut*  PftWeek =new SPftOut();

   SGridOut* GridWeek=new SGridOut();
   //calculate sums
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant){
      CPlant* plant = *iplant;
      pft_name=plant->pft();
      //suche pft...
      if (!plant->dead){
//following lines adapted from Internet
// http://stackoverflow.com/questions/936999/what-is-the-default-constructor-for-c-pointer
//CHANGED BY KK
        map<string,SPftOut::SPftSingle*>::const_iterator pos = PftWeek->PFT.find(pft_name);
        SPftOut::SPftSingle* mi;
        if (pos==PftWeek->PFT.end())
           PftWeek->PFT[pft_name] = new SPftOut::SPftSingle();
        mi =PftWeek->PFT.find(pft_name)->second;
        mi->totmass+=plant->GetMass();
        if(plant->Age > 6) ++mi->Nind; else ++mi->Njuv;
        mi->shootmass+=plant->mshoot;
        mi->rootmass+=plant->mroot;
      }//plant alive
      else{
    	  aLitter+=plant->mshoot+plant->mRepro;
    	  bLitter+=plant->mroot;
      }
   }
   GridWeek->above_litter=aLitter;
   GridWeek->below_litter=bLitter;

   //calculate mean values
   typedef map<string, SPftOut::SPftSingle*> mapType;

   for(mapType::const_iterator it = PftWeek->PFT.begin();
          it != PftWeek->PFT.end(); ++it)
    {
      if ((it->second->Nind + it->second->Njuv) >=1){

         //calculate shannon index and proportion of each PFT
         prop_PFT=(double) (it->second->Nind + it->second->Njuv)/PlantList.size();
         GridWeek->shannon+=(-1)*prop_PFT*log(prop_PFT);
      }
		//update plants' mort_base value
		for (plant_iter iplant = PlantList.begin(); iplant < PlantList.end();
				++iplant) {
			CPlant* plant = *iplant;
			if (plant->dead == false) {
				pft_name = plant->pft();
				if (PftWeek->PFT.find(pft_name) == PftWeek->PFT.end()) {
					cerr << "wrong pft: " << pft_name;
					exit(3);
				}
				//based on Abundance.. 1+(NInd(PFT)/maxnIndPFT)
				//0.007*0.5*([1-2]) meanly resulting in 0.007
//				double abundance = PftWeek->PFT.find(pft_name)->second->Nind;
				double cover = PftWeek->PFT.find(pft_name)->second->cover;
//				double m_area_root = plant->Traits->RAR
//						* pow(plant->Traits->MaxMass * 0.25, 2.0 / 3.0);
//				double m_area_shoot = plant->Traits->SLA
//						* pow(
//								plant->Traits->LMR * plant->Traits->MaxMass
//										* 0.25, 2.0 / 3.0);   //mean shoot area;

//				double max_abundance = (double) SRunPara::RunPara.GridSize
//						* SRunPara::RunPara.GridSize /
				//area per medium plant (min of above-and belowground)
//						min(m_area_root,         //mean root area
//								m_area_shoot);
				plant->mort_base = 0.007
//						* pow(1 + (abundance / (double) max_abundance), 2);
						* pow(1 + (cover/1.0), 2);
				//maximum should be <1
				plant->mort_base = min(0.95,plant->mort_base);
				//     cout<<plant->mort_base<<" ";
			}
		}

      // cover mit Funktion find()  füllen -- da sonst evtl adressierungsfehler
      string type=it->first;
      double cover=this->PftCover.find(it->first)->second ;
      it->second->cover=cover;

      GridWeek->totmass+=it->second->totmass;
      GridWeek->above_mass+=it->second->shootmass;
      GridWeek->below_mass+=it->second->rootmass;
      GridWeek->Nind+=it->second->Nind+it->second->Njuv;//all individuals

      //add LDDSeeds to PftWeek
      for (int d=0; d<NDistClass; ++d){
         PftWeek->PFT[it->first]->LDDseeds[d] = (*LDDSeeds)[it->first].NSeeds[d];
         (*LDDSeeds)[it->first].NSeeds[d] = 0;
      }

   }

   //summarize seeds on grid...
   int sumcells=SRunPara::RunPara.GetSumCells();
      for (int i=0; i<sumcells; ++i){
         CCell* cell = CellList[i];
         for (seed_iter iter=cell->SeedBankList.begin();
              iter<cell->SeedBankList.end(); ++iter){
            string pft=(*iter)->pft();
            map<string,SPftOut::SPftSingle*>::const_iterator
              pos = PftWeek->PFT.find(pft);
            if (pos==PftWeek->PFT.end())
            PftWeek->PFT[pft] = new SPftOut::SPftSingle();
//         if (!PftWeek->PFT[pft])
//           PftWeek->PFT[pft] = new SPftOut::SPftSingle();
            ++PftWeek->PFT[pft]->Nseeds;
         }
      }

   double sum_above=0, sum_below=0;
   for (int i=0; i<sumcells; ++i){
      CCell* cell = CellList[i];
      sum_above+=cell->AResConc;
      sum_below+=cell->BResConc;
   }
   GridWeek->aresmean=sum_above/sumcells;
   GridWeek->bresmean=sum_below/sumcells;
   this->GetClonOutput(*GridWeek);

    NCellsAcover=GetCoveredCells();
    GridWeek->bareGround=1.0- (double(NCellsAcover)/
       (SRunPara::RunPara.GetSumCells()));//bare ground
    GridWeek->bareSoil=1.0- (double(this->GetRootedSoilarea())/
       (SRunPara::RunPara.GetSumCells()));//bare ground


   PftOutData.push_back(PftWeek);
   GridWeek->PftCount=PftSurvival(); //get PFT results
   GridOutData.push_back(GridWeek);
   }//end CClonalGridEnvir::GetOutput(
//---------------------------------------------------------------------------
/**
 * get and reset amount of cutted biomass
 *
 * Appends Output struct
 */
void CGridEnvir::GetOutputCutted(){
   SGridOut* GridWeek=GridOutData.back();
   //store cutted biomass and reset value for next mowing
   GridWeek->cutted=this->getCuttedBM();this->resetCuttedBM();

}
//---------------------------------------------------------------------------
/**
 * get and reset amount of grazed biomass
 *
 * Appends Output struct
 */
void CGridEnvir::GetOutputGrazed(){
   SGridOut* GridWeek=GridOutData.back();
   //store cutted biomass and reset value for next mowing
   GridWeek->grazed=this->getGrazedBM();this->resetGrazedBM();

}
//---------------------------------------------------------------------------
/**
 * get clonal variables (grid wide)
 */
void CGridEnvir::GetClonOutput(SGridOut& GridData)//PftOut& PftData, )
{
   GridData.NclonalPlants=GetNclonalPlants();
   GridData.NGenets=GetNMotherPlants();
   GridData.MeanGeneration=GetNGeneration();
   GridData.NPlants=GetNPlants();
   //only for testing in Comtess version
   int Nspac=0;double Mseed=0;
   for (plant_iter iplant=PlantList.begin(); iplant<PlantList.end(); ++iplant)
   {
     CPlant* plant = *iplant;
     //only if its a clonal plant
     if ((!plant->dead)&& (plant->growingSpacerList.size()>0 )) Nspac++;
     if (!plant->dead) Mseed += plant->mRepro ;
   }

   GridData.NSpacer=Nspac;
   GridData.MSeed=Mseed;//addiere mSeed aller Inds
//end only for testing comtess version

}

//------------------------------------------------------------------------------
/**
   Saves Pft survival times and returns number of surviving PFTs

   changed in version 100716 for type-flexible Output
   \return number of surviving PFTs
*/
int CGridEnvir::PftSurvival()
{
    typedef map<string,long> mapType;
    for(mapType::const_iterator it = PftInitList.begin();
          it != PftInitList.end(); ++it)
    {
       if (PftOutData.back()->PFT.find(it->first)
       ==PftOutData.back()->PFT.end()){  //PFT is extinct
       //vergleiche mit letztem Stand
          if(PftSurvTime.find(it->first)->second==0)//wenn vorher noch existent
            PftSurvTime[it->first]=CEnvir::year;//GetT(); //get current time
       }else{                            //PFT still exists
          PftSurvTime[it->first]=0;
       } // [it->first];
   }
   return PftOutData.back()->PFT.size();//count_pft;
}//endPftSurvival
//-Auswertung--------------------------------------------------------------------------
int CGridEnvir::getACover(int x, int y){
  return ACover[x*SRunPara::RunPara.CellNum+y];}
int CGridEnvir::getBCover(int x, int y){
  return BCover[x*SRunPara::RunPara.CellNum+y];}
int CGridEnvir::getGridACover(int i){return CellList[i]->getCover(1);}
int CGridEnvir::getGridBCover(int i){return CellList[i]->getCover(2);}

///returns grid cover of a given type
/**
 * Get grid-wide cover of PFT in question.
 *
 * To get bare ground, ask for type 'bare'.
 * @param type PFT in question
 * @return cover of PFT type
 */
double CGridEnvir::getTypeCover(const string type)const{
  //for each cell
  double number=0;
  const long int sumcells=SRunPara::RunPara.GetSumCells();
  for (long int i=0; i<sumcells; ++i){
         number+=getTypeCover(i,type);

  }
  return number/sumcells;
}

/**
 * portion of type in question at cell 'i'
 * \warning ohne Adressüberprüfung
 * \sa CCell::getCover()
 * @param i cell index in CellList
 * @param type PFT in question
 * @return cover at position i
 */
double CGridEnvir::getTypeCover(const int i, const string type)const{
  return CellList[i]->getCover(type);}

/**
 * Get cover of cells.
 *
  \warning PftCover is very time consuming
*/
void CGridEnvir::setCover(){
  const int sum=SRunPara::RunPara.GetSumCells();
  for(int i=0;i<sum;i++){
    ACover.at(i)=getGridACover(i);
    BCover[i]   =getGridBCover(i);
  }
  //report cover in week 20
  if (week==20){
     typedef map<string, long> mapType;
     for(mapType::const_iterator it = this->PftInitList.begin();
           it != this->PftInitList.end(); ++it)
     {
           this->PftCover[it->first]=getTypeCover(it->first);
     }
    }//end if week=20
}
//-Save.. and Load.. --------------------------------------------------------------------------
/**
 Saves the current state of the grid and parameters.

 -does it have to be in one file?
 -how to store all information

 \note not saved: output file names, WeeksPerYear
\author KK
\date 120830
*/
void CGridEnvir::Save(string ID){
  //open file(s)
  string fname="Save\\E_"+ID+".sav";
  ofstream SaveFile(fname.c_str());
  if (!SaveFile.good()) {cerr<<("Fehler beim Öffnen InitFile");exit(3); }
  cout<<"SaveFile: "<<fname<<endl;

//environmental parameters CEnvir, CClonalGridEnvir
//ifiles, PftInitList, week, year (SimNr, RunNr)
  SaveFile<<CEnvir::year<<"\t"<<CEnvir::week<<endl;
  SaveFile<<SRunPara::NamePftFile<<endl;
//Run Parameter
  SaveFile<<SRunPara::RunPara.asString()<<endl;

  fname="Save\\G_"+ID+".sav";
//CGrid, CClonalGrid
 ((CGrid*)this)->Save(fname);

}//Save
//------------------------------------------------------------------------------
/**
 * annual seed rain
 *
 * \author FM - seed rain option
 */
void CGridEnvir::SeedRain(){

   string PFT_ID, PFTtype, Cltype;
   //size_t posc;
   SPftTraits *pfttraits;
//   SclonalTraits *cltraits;
   double nseeds =0;

   double SeedFracClonal = 0.1;  //ratio between seed input for clonal and non-clonal seeds;

   //for all plants..
   for (map<string, long>::const_iterator it = PftInitList.begin();
          it!= PftInitList.end(); ++it){

      PFT_ID = it->first;

//      cltraits=getClLink(PFT_ID);
      pfttraits=SPftTraits::getPftLink(PFT_ID);

      switch(SRunPara::RunPara.SeedRainType){
         // [SeedInput] == seed NUMBER & equal seed NUMBER for each PFT
         case 1: nseeds = SRunPara::RunPara.SeedInput/(CEnvir::SeedRainGr.NPftSeedSize[0] +
        		    CEnvir::SeedRainGr.NPftSeedSize[1] + CEnvir::SeedRainGr.NPftSeedSize[2]);
            break;
         // [SeedInput] == seed MASS & equal seed NUMBER for each PFT
         case 2: nseeds = SRunPara::RunPara.SeedInput/(CEnvir::SeedRainGr.NPftSeedSize[0]*0.1 +
        		 CEnvir::SeedRainGr.NPftSeedSize[1]*0.3 + CEnvir::SeedRainGr.NPftSeedSize[2]*1.0);
            break;
         // [SeedInput] == seed MASS & equal seed NUMBER for each PFT
         case 3: nseeds = SRunPara::RunPara.SeedInput/(CEnvir::SeedRainGr.NPftSeedSize[0] +
        		 CEnvir::SeedRainGr.NPftSeedSize[1]/3.0 + CEnvir::SeedRainGr.NPftSeedSize[2]/10.0)*0.1/pfttraits->SeedMass;
            break;
         // [SeedInput] == seed MASS& equal seed MASS  for each PFT
         case 4: nseeds = SRunPara::RunPara.SeedInput/(CEnvir::SeedRainGr.NPftSeedSize[0] +
        		 CEnvir::SeedRainGr.NPftSeedSize[1] + CEnvir::SeedRainGr.NPftSeedSize[2]) / pfttraits->SeedMass;
            break;
         // [SeedInput] == seed NUMBER & fixed ratio of seed numbers for clonal and non-clonal PFTS
         case 5: nseeds = SRunPara::RunPara.SeedInput/(CEnvir::SeedRainGr.NPftClonal[0] +
        		 CEnvir::SeedRainGr.NPftClonal[1]*SeedFracClonal);
            break;
//disabled..
//        // [SeedInput] == factor for pft-specific seed rain & number of seeds is PFT specific and specified in PFTTraits file
//         case 99: nseeds = SRunPara::RunPara.SeedInput*pfttraits->SeedRainGr;
//            break;
         case 111: nseeds = SRunPara::RunPara.SeedInput*CEnvir::SeedRainGr.PftSeedRainList[PFT_ID];
            break;
         default : nseeds = 0;
      }

      int nseeds2 = poissonLCG(nseeds);  //random number from poisson distribution
       //int nseeds2 = Round(nseeds);
       CGrid::InitClonalSeeds(pfttraits,nseeds2,pfttraits->pEstab);//,cltraits
   }
}


//eof
