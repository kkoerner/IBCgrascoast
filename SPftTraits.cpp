/*
 * SPftTraits.cpp
 *
 *  Created on: 21.04.2014
 *      Author: KatrinK
 */

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>


#include "SPftTraits.h"

SPftTraits::SPftTraits() :TypeID(999),name("default"),MaxAge(100),
	  AllocSeed(0.05),LMR(0),m0(0),MaxMass(0),SeedMass(0),Dist(0),
	  pEstab(0.5),Gmax(0),memory(0),SLA(0),palat(0),RAR(1),growth(0.25),
	  mThres(0.2),Dorm(1),FlowerWeek(16),DispWeek(20),
	  PropSex(0.1),meanSpacerlength(17.5),sdSpacerlength(12.5),
	  Resshare(true),mSpacer(70),AllocSpacer(0.05),clonal(true)
{
}//end constructor

SPftTraits::~SPftTraits() {
	// TODO Auto-generated destructor stub
}


//std::vector<SPftTraits*> SPftTraits::PftList;//(CRunPara::RunPara.NPft,new SPftTraits);
//std::vector<SclonalTraits*> SclonalTraits::clonalTraits;//(8,new SclonalTraits());
map<string,SPftTraits*> SPftTraits::PftLinkList=map<string,SPftTraits*>();

//------------------------------------------------------------------------------
/**
 * Get - link for specific PFT
 * @param type PFT asked for
 * @return Object pointer to PFT definition
 */
SPftTraits* SPftTraits::getPftLink(string type)
{
  SPftTraits* traits=NULL;
  map<string,SPftTraits*>::iterator pos = PftLinkList.find(type);
  if (pos==(PftLinkList.end())) cerr<<"type not found:"<<type<<endl;
  else traits=pos->second;
  if (traits==NULL) cerr<<"NULL-pointer error\n";
  return traits;
}
//-----------------------------------------------------------------------------
/**
 * Read definition of PFTs used in the simulation
 * @param file file containing PFT definitions
 * @param n default=-1; in case of monoculture runs, nb of PFT to test
 */
void SPftTraits::ReadPFTDef(const string& file, int n) {
	//delete old definitions
	   //delete static pointer vectors
	   for (map<string,SPftTraits*>::iterator i=SPftTraits::PftLinkList.begin();
			   i!=SPftTraits::PftLinkList.end(); ++i) delete i->second;
	   SPftTraits::PftLinkList.clear();
	//Open InitFile,
	ifstream InitFile(file.c_str());
	if (!InitFile.good()) {
		cerr << ("Fehler beim Öffnen InitFile");
		exit(3);
	}
	cout << "InitFile: " << file << endl;
	string line;
	getline(InitFile, line); //skip header line
	//skip first lines if only one Types should be initiated
	if (n > -1)
		for (int x = 0; x < n; x++)
			getline(InitFile, line);

	int dummi1;
//	float dummi0;
	string dummi2; //int PFTtype; string Cltype;
	do {
		//erstelle neue traits
		SPftTraits* traits = new SPftTraits();
		//    SclonalTraits* cltraits=new SclonalTraits();
		// file structure
		// "ID"      "Species" "alSeed"   "LMR"     "maxMass" "mSeed" "Dist"
		// "pEstab"  "Gmax"    "SLA1"     "palat"   "memo"    "RAR"   "dgrow_BG_shoot" "dgrow_BG_root"
		// "PropSex" "meanSpacerLength" "Resshare" "mSpacer"
		//get type definitions from file
		InitFile >> dummi1;
		InitFile >> dummi2;
		InitFile >> traits->MaxAge >> traits->AllocSeed >> traits->LMR
				>> traits->m0 >> traits->MaxMass >> traits->SeedMass
				>> traits->Dist >> traits->pEstab >> traits->Gmax >> traits->SLA
				>> traits->palat >> traits->memory >> traits->RAR
				>> traits->growth >> traits->mThres >> traits->clonal
				>> traits->PropSex >> traits->meanSpacerlength
				>> traits->sdSpacerlength >> traits->Resshare >> // >> cltraits->mSpacer
				traits->AllocSpacer >> traits->mSpacer;
		//namen und IDs
		traits->name = dummi2; //=cltraits->name
		traits->TypeID = dummi1;
		//in Listen einfügen..
		SPftTraits::addPftLink(dummi2, traits);
		//    addClLink(dummi2,cltraits);
//		SPftTraits::PftList.push_back(traits);
	} while (!InitFile.eof());
//	return InitFile;
}//read PFT defs

/**
 * print object's properties to prompt
 *
 */
void SPftTraits::print(){
  std::cout<<"\nBase type:"<<this->name;
  std::cout<<"\n  AllocSeed:"<<this->AllocSeed;
  std::cout<<"\n  Weeks: Flower"<<this->FlowerWeek;
  std::cout<<"\tDisp "<<this->DispWeek;
  std::cout<<"\n  mSeed:"<<this->SeedMass;
  std::cout<<"\n  Dist:"<<this->Dist;
  std::cout<<"\n  Dorm:"<<this->Dorm;
  std::cout<<"\n  Gmax:"<<this->Gmax;
  std::cout<<"\tgrowth:"<<this->growth;
  std::cout<<"\n  MaxAge:"<<this->MaxAge;
  std::cout<<"\n  LMR:"<<this->LMR;
  std::cout<<"\n  MaxMass:"<<this->MaxMass;
  std::cout<<"\tm0:"<<this->m0;
  std::cout<<"\n  SLA:"<<this->SLA;
  std::cout<<"\tpalat:"<<this->palat;
  std::cout<<"\n  pEstab:"<<this->pEstab;
  std::cout<<"\n  RAR:"<<this->RAR;
  std::cout<<"\n  PropSex:"<<this->PropSex;
  std::cout<<"\n  meanSpacerlength:"<<this->meanSpacerlength;
  std::cout<<"\t  sdSpacerlength:"<<this->sdSpacerlength;
  std::cout<<"\n  Resshare:"<<this->Resshare<<endl;

}//print base traits

/**
initialize clonal traits with default values
* /
SclonalTraits::SclonalTraits()  //clonal plant with specifc traits
  :name("default")//,PropSex(0.1),meanSpacerlength(17.5),sdSpacerlength(12.5),
  //Resshare(true),mSpacer(70),AllocSpacer(0.05),clonal(true)
{
}
/* /---------------------------------------------------------------------------
void SclonalTraits::ReadclonalTraits(char* file)    // not needed anymore -> new initialisation
{
   std::string sfile=file;
   std::ifstream clonalFile;
   //open parameter file
   if (sfile=="") sfile=(CClonalGridEnvir::NameClonalPftFile);
   clonalFile.open(sfile.c_str());
   if (!clonalFile.good()) {std::cerr<<("Fehler beim Öffnen clonalFile");exit(3); }
   string line1;
   getline(clonalFile,line1);
   //*******************

    //loop for all clonal types
   for (int type=0; type<8; type++)
   {   int num; string name;SclonalTraits* temp=new SclonalTraits;
      //read plant parameter from inputfile
      clonalFile>>num
//             >>(char*)clonalTraits[type].name
             >>name
             >>temp->PropSex
             >>temp->meanSpacerlength
             >>temp->sdSpacerlength
             >>temp->Resshare
             ;
      temp->name=name;
      clonalTraits.push_back(temp);
   }
   clonalFile.close();
}
*/
//eof
