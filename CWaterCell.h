/**
 * CWaterCell.h
 *
 *  \date 27.01.2015
 *  \author KatrinK
 */

#ifndef CWATERCELL_H_
#define CWATERCELL_H_

#include "Cell.h"
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
 virtual ~CWaterCell(){};
    //! competition function for size symmetric below-ground resource competition
   /*! function is overwritten if inherited class with different competitive
     size-asymmetry of niche differentiation is used*/
   virtual void BelowComp();
   virtual double Germinate();

 double GetWaterLevel(){return WaterLevel;};
 void SetWaterLevel(double val){WaterLevel=val;};
   std::string asString();///<return content for file saving
};//end CWaterCell


#endif /* CWATERCELL_H_ */
