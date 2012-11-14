//---------------------------------------------------------------------------
#include <string>
#include <fstream>
#include <iostream>

#pragma hdrstop

#include "clonalTraits.h"
#include "environment.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//vector<CDTclonalTraits*> SclonalTraits::clonalTraits(8);
std::vector<SclonalTraits*> SclonalTraits::clonalTraits;//(8,new SclonalTraits());
//-----------------------------------------------------------------------------
/**
initialize clonal traits with default values
*/
SclonalTraits::SclonalTraits()  //clonal plant with specifc traits
  :name("default"),clonal(true),PropSex(0.1),meanSpacerlength(17.5),sdSpacerlength(12.5),
  Resshare(true),mSpacer(70)
{
}
//---------------------------------------------------------------------------
void SclonalTraits::ReadclonalTraits(char* file)
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
//--------------------
void SclonalTraits::print(){
   std::cout<<"\nClonal Type: "<<this->name;
   if (this->clonal) std::cout<<"\tclonal"; else std::cout<<"\tnon-clonal";
   std::cout<<"\n  PropSex: "<<this->PropSex;
   std::cout<<"\n  mSpacer: "<<this->mSpacer;
   std::cout<<"\n  Resshare: "<<this->Resshare;
   std::cout<<"\n  SpacerLength: mean "<<this->meanSpacerlength;
   std::cout<<"sd "<<this->sdSpacerlength<<endl;

} //print clonal traits
//--------------------
//eof
