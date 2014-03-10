#include <iostream>
#include <hash_set.h>
#include <set>
#include <vector>
#include <fstream.h>
#include <stdlib.h>
#include <math.h>

#include <memory.h>
//#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <string>
#include <strings.h>
#include <sys/types.h>


#include "s_hull.h"

/* copyright 2010 Dr David Sinclair
   david@s-hull.org
 
	program to compute Delaunay triangulation of a set of points.
 
 
 The software includes the S-hull programs.
 S-hull is copyrighted as above.
 S-hull is free software and may be obtained from www.s-hull.org.
 It may be freely copied, modified, 
 and redistributed under the following conditions:
 
S-hull is free software and may be obtained from www.s-hull.org.
It may be freely copied, modified, 
and redistributed under the following conditions which might loosely be termed a contribtors beerware license:
1. All copyright notices must remain intact in all files.
2. A copy of this text file must be distributed along with any copies 
   of S-hull that you redistribute; this includes copies that you have 
   modified, or copies of programs or other software products that 
   include S-hull where distributed as source.

3. If you modify S-hull, you must include a notice giving the
   name of the person performing the modification, the date of
   modification, and the reason for such modification.

4. If you are distributing a binary or compiled version of s-hull it
	    is not necessary to include any acknowledgement or reference
	    to s-hull.
5. There is no warranty or other guarantee of fitness for S-hull, it is 
   provided solely "as is".  Bug reports or fixes may be sent to 
   bugs@s-hull.org; the authors may or may not act on them as 
   they desire.
6. By copying or compliing the code for S-hull you explicitly indemnify 
the copyright holder against any liability he may incur as a result of you 
copying the code.

7. If you meet any of the contributors to the code you used from s-hull.org
	    in a pub or a bar, and you think the source code they contributed to is worth it,
	    you can buy them a beer.

	    If your principles run against beer a bacon-double-cheeseburger would do just as nicely
	    or you could email david@s-hull.org and arrange to make a donation of 10 of your local currancy units
	    to support s-hull.org.
	    
 
 */


#include <sys/time.h>

int main(int argc, char *argv[])
{
   if( argc == 1 ){
      cerr << "new delaunay triangulation method test" << endl;
      
   }

   // float rs[1000], cs[1000];
   float goat =  ( 2147483648.0-1) /100.0;

   std::vector<Shx> pts, hull;
   Shx pt;
   srandom(1);

   //for(int v=0; v<20000; v++){
   for(int v=0; v<100000; v++){
     pt.id = v;
     pt.r = ((float) random())/goat - 50;
     pt.c = ((float) random())/goat - 50;

     pts.push_back(pt);
   }

   write_Shx(pts, "pts.mat");


   std::vector<Triad> triads;

   //   double t1 = getpropertime();

   struct timeval tv1, tv2;
   gettimeofday(&tv1, NULL);
   


   //cerr << t1 << endl;

  
   s_hull_del_ray2( pts, triads);



   gettimeofday(&tv2, NULL);
   float tx =  (tv2.tv_sec + tv2.tv_usec / 1000000.0) - ( tv1.tv_sec + tv1.tv_usec / 1000000.0);

   cerr <<  tx << " seconds for triangles" << endl;

   write_Triads(triads, "triangles.mat");

   

   exit(0);
   
}



#include <sys/time.h>
//static inline 
double getpropertime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

