//---------------------------------------------------------------------------

#include "OutStructs.h"
#include "CEnvir.h"
//---------------------------------------------------------------------------

//-result structs - constructors...----------------------------------------
/**
 * constructor
 */
SPftOut::SPftOut():week(CEnvir::week){//GetT()){
}//end PftOut constructor
//-----------------------------------
/**
 * copy constructor
 * @param base results to copy
 */
SPftOut::SPftOut(const SPftOut& base):week(base.week)  {
    typedef map<string,SPftSingle*> mapType;
    for(mapType::const_iterator it = base.PFT.begin();
          it != base.PFT.end(); ++it){
         PFT[it->first] = new SPftOut::SPftSingle(*it->second);
    }
}

/**
 * assignment operator
 * @param base results to copy
 * @return changed object
 */
SPftOut& SPftOut::operator =(const SPftOut& base){
	  if(this!=&base){
//delete old content
		      typedef map<string,SPftSingle*> mapType;
		      for(mapType::const_iterator it = PFT.begin();
		            it != PFT.end(); ++it) delete it->second;
		      PFT.clear();
//fill in new content
		      week=(base.week);
		      for(mapType::const_iterator it = base.PFT.begin();
		            it != base.PFT.end(); ++it){
		           PFT[it->first] = new SPftOut::SPftSingle();
		           *PFT[it->first] = *it->second;
		      }
	  }
	  return *this;
}
//-----------------------------------
/**
 * destructor
 */
SPftOut::~SPftOut(){
//lösche PFT...
    typedef map<string,SPftSingle*> mapType;
    for(mapType::const_iterator it = PFT.begin();
          it != PFT.end(); ++it) delete it->second;
    PFT.clear();
}
//-------------------------------------------------------
/**
 * constructor
 */
SPftOut::SPftSingle::SPftSingle():cover(0),Nind(0),Nseeds(0),
 rootmass(0),shootmass(0),totmass(0){}//end SPftSingle constructor
//-------------------------------------------------------
/**
 * constructor
 */
SGridOut::SGridOut():week(CEnvir::week),//GetT()),
  above_mass(0),below_mass(0),
  aresmean(0),bresmean(0),Nind(0),PftCount(0),shannon(0),
  totmass(0),cutted(0),
  grazed(0),WaterLevel(0),bareGround(0),bareSoil(0),
  above_litter(0),below_litter(0),
  MclonalPlants(0),MeanGeneration(0),MeanGenetsize(0),
  MPlants(0), NclonalPlants(
				0), NGenets(0), NPlants(0) {
} // end SGridOut constructor

//-------------------------------------------------------
//eof----------------------------------------------------------------
