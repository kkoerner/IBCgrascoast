//---------------------------------------------------------------------------


#pragma hdrstop

#include "OutStructs.h"
#include "environment.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
//-result structs - constructors...----------------------------------------
SPftOut::SPftOut():week(CEnvir::week){//GetT()){
//  const unsigned int length=SRunPara::RunPara.NPft;
//  PFT.assign(length,"")
//  totmass.assign(length,0);
//  Nind.assign(length,0);Nseeds.assign(length,0);
//  rootmass.assign(length,0);shootmass.assign(length,0);
}//end PftOut constructor
//-----------------------------------
SPftOut::~SPftOut(){
//lösche PFT...
    typedef map<string,SPftSingle*> mapType;
    for(mapType::const_iterator it = PFT.begin();
          it != PFT.end(); ++it) delete it->second;
    PFT.clear();
//  totmass.clear();rootmass.clear();shootmass.clear();
//  Nind.clear();Nseeds.clear();
}
//-------------------------------------------------------
SPftOut::SPftSingle::SPftSingle():cover(0),Nind(0),Nseeds(0),
 rootmass(0),shootmass(0),totmass(0){}//end SPftSingle constructor
//-------------------------------------------------------
SGridOut::SGridOut():week(CEnvir::week),//GetT()),
  above_mass(0),below_mass(0),
  aresmean(0),bresmean(0),Nind(0),PftCount(0),shannon(0),
  totmass(0),cutted(0){}// end SGridOut constructor
//-------------------------------------------------------
SClonOut::SClonOut():week(CEnvir::week),
  MclonalPlants(0),MeanGeneration(0),MeanGenetsize(0),
  MPlants(0),NclonalPlants(0),NGenets(0),NPlants(0){}
//eof----------------------------------------------------------------