/**
 * @brief fftsg.h
 * @author Damien Tardieu
 * @date 11/03/2010
 * @copyright (c) 2010 – UMONS - Numediart
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


#ifndef FFTSG_H
#define FFTSG_H

void rdft(int n, int isgn, double *a, int *ip, double *w);
void cdft(int n, int isgn, double *a, int *ip, double *w);
void makewt(int nw, int *ip, double *w);
void makect(int nc, int *ip, double *c);
void makeipt(int nw, int *ip);

void cftfsub(int n, double *a, int *ip, int nw, double *w);
void cftbsub(int n, double *a, int *ip, int nw, double *w);
void rftfsub(int n, double *a, int nc, double *c);
void rftbsub(int n, double *a, int nc, double *c);
void dctsub(int n, double *a, int nc, double *c);
void dstsub(int n, double *a, int nc, double *c);


void bitrv2(int n, int *ip, double *a);
void bitrv216(double *a);
void bitrv208(double *a);
void cftf1st(int n, double *a, double *w);
void cftrec4(int n, double *a, int nw, double *w);
void cftleaf(int n, int isplt, double *a, int nw, double *w);
void cftfx41(int n, double *a, int nw, double *w);
void cftf161(double *a, double *w);
void cftf162(double *a, double *w);

void cftf081(double *a, double *w);
void cftf082(double *a, double *w);

void cftb040(double *a);
void cftf040(double *a);
void cftx020(double *a);

#ifdef USE_CDFT_THREADS
void cftrec4_th(int n, double *a, int nw, double *w);
#endif /* USE_CDFT_THREADS */

void bitrv2conj(int n, int *ip, double *a);
void bitrv216neg(double *a);
void bitrv208neg(double *a);
void cftb1st(int n, double *a, double *w);
 
int cfttree(int n, int j, int k, double *a, int nw, double *w);
void cftleaf(int n, int isplt, double *a, int nw, double *w);
void cftmdl1(int n, double *a, double *w);

void cftmdl2(int n, double *a, double *w);
#endif
