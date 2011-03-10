/**
 * @brief example.cpp
 * @author Xavier Siebert
 * @date 27/10/2009
 * @copyright (c) 2009 – UMONS - Numediart
 * 
 * MediaCycle of University of Mons – Numediart institute is 
 * licensed under the GNU AFFERO GENERAL PUBLIC LICENSE Version 3 
 * licence (the “License”); you may not use this file except in compliance 
 * with the License.
 * 
 * This program is free software: you can redistribute it and/or 
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Each use of this software must be attributed to University of Mons – 
 * Numediart Institute
 * 
 * Any other additional authorizations may be asked to avre@umons.ac.be 
 * <mailto:avre@umons.ac.be>
*/

#include <unistd.h>
#include "gnuplot_i.hpp"

#define SLEEP_LGTH 1
#define NPOINTS    50

int main(int argc, char *argv[])
{
    vector<double> x;
    vector<double> y;
    int i;


    cout << "*** example of gnuplot control through C++ ***" << endl;

    Gnuplot g1 = Gnuplot("lines");

    //
    // Slopes
    // 
    cout << "*** plotting slopes" << endl;
    cout << "y = x" << endl;
    g1.plot_slope(1.0,0.0,"unity slope");
    sleep(SLEEP_LGTH);

    cout << "y = 2*x" << endl;
    g1.plot_slope(2.0,0.0,"y=2x");
    sleep(SLEEP_LGTH);

    cout << "y = -x" << endl;
    g1.plot_slope(-1.0,0.0,"y=-x");
    sleep(SLEEP_LGTH);

    //
    // Equations
    //
    g1.reset_plot();
    cout << endl << endl << "*** various equations" << endl;
    cout << "y = sin(x)" << endl;
    g1.plot_equation("sin(x)","sine");
    sleep(SLEEP_LGTH);

    cout << "y = log(x)" << endl;
    g1.plot_equation("log(x)","logarithm");
    sleep(SLEEP_LGTH);

    cout << "y = sin(x) * cos(2*x)" << endl;
    g1.plot_equation("sin(x)*cos(2*x)","sine product");
    sleep(SLEEP_LGTH);

    //
    // Styles
    //
    g1.reset_plot();
    cout << endl << endl << "*** showing styles" << endl;
    cout << "sine in points" << endl;
    g1.set_style("points");
    g1.plot_equation("sin(x)","sine");
    sleep(SLEEP_LGTH);

    cout << "sine in impulses" << endl;
    g1.set_style("impulses");
    g1.plot_equation("sin(x)","sine");
    sleep(SLEEP_LGTH);

    cout << "sine in steps" << endl;
    g1.set_style("steps");
    g1.plot_equation("sin(x)","sine");
    sleep(SLEEP_LGTH);

    //
    // User defined 1d and 2d point sets
    //
    cout << endl << endl << "*** user-defined lists of doubles" << endl;
    for (i = 0; i < NPOINTS; i++)
        x.push_back((double)i * (double)i);
    g1.reset_plot();
    g1.set_style("impulses");
    g1.plot_x(x,"user-defined doubles");
    sleep(SLEEP_LGTH);


    cout << endl << endl << "*** user-defined lists of points" << endl;
    x.clear();
    for (i = 0; i < NPOINTS; i++)
      {
        x.push_back((double)i);
        y.push_back((double)i * (double)i);
      }
    g1.reset_plot();
    g1.set_style("points");
    g1.plot_xy(x,y,"user-defined points");
    sleep(SLEEP_LGTH);

    //
    // Multiple output screens
    //
    cout << endl << endl;
    cout << "*** multiple output windows" << endl;
    g1.reset_plot();
    g1.set_style("lines");

    Gnuplot g2 = Gnuplot("lines");
    Gnuplot g3 = Gnuplot("lines");

    cout << "window 1: sin(x)" << endl;
    g1.plot_equation("sin(x)","sin(x)");
    sleep(SLEEP_LGTH);

    cout << "window 2: x*sin(x)" << endl;
    g2.plot_equation("x*sin(x)","x*sin(x)");
    sleep(SLEEP_LGTH);

    cout << "window 3: log(x)/x" << endl;
    g3.plot_equation("log(x)/x","log(x)/x");
    sleep(SLEEP_LGTH);

    //
    // Using the GnuplotException class
    //
    try {
        Gnuplot g4 = Gnuplot("lines");
        cout << "window 4: sin(x)/x" << endl;
        g4.plot_equation("sin(x)/x","sin(x)/x");
        sleep(SLEEP_LGTH);

    } catch (GnuplotException ge) {
        cout << ge.what() << endl;
    }

    cout << endl << "*** end of gnuplot example" << endl;

    return 0;
}
