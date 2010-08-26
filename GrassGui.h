/**\file
\brief definition of GUI-class*/
//---------------------------------------------------------------------------

#ifndef GrassGuiH
#define GrassGuiH

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Chart.hpp>
#include <ExtCtrls.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Series.hpp>
#include <BubbleCh.hpp>
#include "MMColorGrid.h"
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#include "MMLegendView.h"
#include "TimeUtils.h"

///the only information the GUI needs from the model
#include "environment.h"
#include <Dialogs.hpp>

//---------------------------------------------------------------------------
//!Base class - graphical user interface (GUI) and control of the model.
class TForm1 : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
        TButton *BRun;
        TButton *BExit;
   TMMColorGrid *MMColorGrid1;
        TEdit *ETmax;
   TLabel *Label1;
   TLabel *Label4;
   TMMColorGrid *MMColorGrid2;
   TLabel *Label5;
   TPageControl *PageControl1;
   TTabSheet *TabSheet1;
   TRadioGroup *RadioGroup1;
   TLabel *Label2;
   TPageControl *PageControl2;
   TProgressBar *ProgressBar1;
        TEdit *EPgraz;
        TEdit *EAres;
   TLabel *Label7;
   TLabel *Label8;
        TEdit *EBres;
   TMMLegendView *MMLegendView1;
   TMemo *Memo1;
   TTabSheet *TabSheet8;
        TChart *ChPFTs;
   TTabSheet *TabSheet7;
        TChart *ChBiomass;
   TLineSeries *Series73;
   TLineSeries *Series74;
   TLineSeries *Series75;
   TLineSeries *Series76;
        TLabel *LWeek;
        TLabel *Label21;
        TLabel *Label22;
        TLabel *Label23;
        TLabel *Label24;
        TLabel *Label29;
        TLabel *Label30;
        TLabel *Label20;
        TLabel *Label33;
        TLabel *Label34;
        TLabel *Label44;
        TLabel *Label45;
        TTabSheet *Clonality;
        TChart *ChClonality;
        TLineSeries *LineSeries3;
        TLineSeries *LineSeries4;
        TButton *BWeek;
        TButton *BYear;
        TButton *BLoadFile;
        TOpenDialog *ODSimFile;
        TLabel *LSimFile;
        TStaticText *StaticText1;
        TStaticText *StaticText2;
   void __fastcall BLoadFileClick(TObject *Sender);
   void __fastcall BWeekClick(TObject *Sender);
   void __fastcall BYearClick(TObject *Sender);
   void __fastcall BRunClick(TObject *Sender);
   void __fastcall BExitClick(TObject *Sender);

private:	// Anwender-Deklarationen
   CClonalGridEnvir* Envir;   ///<environment in which simulations are run
public:		// Anwender-Deklarationen
   __fastcall TForm1(TComponent* Owner);
   virtual __fastcall ~TForm1();
   void clonalOutput();       ///< generates output and on-gui results for clonal runs
   void PaintGrid();          ///< paint above- and belowground zones
   void PaintTimeSeries();    ///< Time series painted - not used
   void __fastcall GUIInit();
   void __fastcall OneYear(); //!< calls OneWeek and processes in winter
   void __fastcall RunSim();  //!< runs ONE simulation
   void __fastcall InitSim(); //!< inits ONE simulation

   int stoptime;
   int multisim;
   int exitcond;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
