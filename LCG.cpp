/** \brief LCG.cpp random number generator
  source: http://www.c-plusplus.de/forum/39335-full (posted by Mady 12:54:00 15.12.2002)

  Es gab ein Problem mit der Gleichverteilung der Zufallszahlen. Lösung (in C):

  Das Problem liegt an der Implementation der rand()-Funktion auf Deinem System.
  Vielleicht versuchst Du einen anderen Generator, der gleichmäßiger Verteilt.
  Dieser Generator stammt aus 'Angewandte Kryptographie' und liefert einen
  'zufälligen' Wert zwischen 0 und 1 zurück.

  LCG= Linear Congruential Generator

  \code a testcode would be..
  #include <cstdio>
  #include <cstdlib>
  #include <ctime>

  #include "LCG.h"

  enum { max_test = 1000 };

  int main(void)
  {
    int a[4] = { 0 };
    int i;

    initLCG(time(NULL), 3487234); // 3487234 ist 'zufällig' gewählt

    for (i = 0; i < max_test; ++i)
    {
        a[(int)(combinedLCG() * (sizeof (a) / sizeof (a[0])))]++;
    }

    for (i = 0; i < sizeof (a) / sizeof (a[0]); ++i)
    {
        printf("a[%d]: %d (%0.2f%%)\n", i, a[i], (double)a[i] / (double)max_test * 100.0);
    }

    return 0;
  }

 \endcode
*/

#include "LCG.h"
#include <math.h> //for sqrt and log

static long s1 = 1;
static long s2 = 1;

#define MODMULT(a, b, c, m, s) q = s/a; s = b*(s-a*q)-c*q; if (s < 0) s += m;

double combinedLCG(void)
{
    long q, z;

    MODMULT(53668, 40014, 12211, 2147483563L, s1)
    MODMULT(52774, 40692, 3791, 2147483399L, s2)
    z = s1 - s2;
    if (z < 1)
        z += 2147483562;
    return z * 4.656613e-10;
}

/**
 * \return normally distributed random numbers
 *
 * Follows the algorithm
 * of the 'Marsaglia polar method', an modification of the 'Box-Muller'
 * algorithm.
 * \param mean mean
 * \param sd   standard deviation
 * */
double normcLCG(double mean,double sd){
    double u = combinedLCG() * 2 - 1;
    double v = combinedLCG() * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1) return normcLCG(mean,sd);
    double c = sqrt(-2 * log(r) / r);
    return (u * c)*sd + mean;

}

void initLCG(long init_s1, long init_s2)
{
    s1 = init_s1;
    s2 = init_s2;
}
//eof
