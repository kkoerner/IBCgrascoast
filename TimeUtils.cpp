//---------------------------------------------------------------------------


#pragma hdrstop

#include "TimeUtils.h"
#include <vcl.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)


double GetHRFrequency()
{ // Der Funktionswert ist die Auflösung des HR-Timers
LARGE_INTEGER f;
BOOL bf = QueryPerformanceFrequency(&f);
if (bf) return f.QuadPart;
else return -1;
}

double HRFrequency =GetHRFrequency();

double HRTimeInSec()
{ // Der Funktionswert ist der aktuelle Wert des
// HR-Timers in Sekunden
LARGE_INTEGER c;
BOOL bc = QueryPerformanceCounter(&c);
if (bc) return c.QuadPart/HRFrequency;
else return -1;
}
 