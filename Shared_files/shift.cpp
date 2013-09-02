#include <fftw3.h>
#include "shift.h"
#include "Exp.h"
#include "angle.h"
#include <complex>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <armadillo>

using namespace arma;


void shift(int N, int hopa, int *hops, double *frames, cx_vec *frames2, vec *w, cx_vec *XaPrevious, vec *Xa_arg, vec *Xa_abs, vec *XaPrevious_arg, vec *PhiPrevious, mat *Q, double *yshift, cx_vec *Xa, cx_vec *Xs, vec *q, cx_vec *qaux, vec *Phi, double *ysaida, double *ysaida2, int Qcolumn, vec *d_phi, vec *d_phi_prime, vec *d_phi_wrapped, vec *omega_true_sobre_fs, vec *I, vec *AUX, fftw_plan p, fftw_plan p2)
{
	//Some declaration
	int L;
	L = N;
	for (int i=1; i<= Qcolumn-1; i++)
	{
		L = L + hops[i-1];
	}
	int Hops = 0;
	double r;
	complex<double> j;
	int n1;
	int n2;
	double n3;
	
	//Some inicialization
	
	for (int i=1; i<=L; i++)
	{
		ysaida[i-1] = 0;
	}
	
	ysaida2 = &ysaida[L-N];
	
	//Starts now
	
	//Windowing
	
	for (int i=1; i<=N; i++)
	{
		frames2[0](i-1) = frames[i-1];
	}
	
	frames2[0] = frames2[0]%w[0]/(sqrt(((double)N/(2*(double)hopa))));
	
	/*Analysis*/
	fftw_execute(p);
	
	/*Processing*/
	for (int i=1; i<=N; i++)
	{
		Xa_arg[0](i-1) = angle(Xa[0](i-1));
	}
	d_phi[0] = Xa_arg[0] - XaPrevious_arg[0];
	d_phi_prime[0] = d_phi[0] - ((2*M_PI * hopa) / N) * I[0];
	AUX[0] = floor((d_phi_prime[0] + M_PI) / (2*M_PI));
	d_phi_wrapped[0] = d_phi_prime[0] - AUX[0] * (2*M_PI);
	omega_true_sobre_fs[0] = (2*M_PI/N) * I[0] + d_phi_wrapped[0] / hopa;
	Phi[0] = PhiPrevious[0] + (hops[Qcolumn-1])*omega_true_sobre_fs[0] ;
	Xa_abs[0] = abs(Xa[0]);
	for (int i=1; i<=N; i++)
	{
        Xs[0](i-1) = ExponencialComplexa(Phi[0](i-1));
	}
	Xs[0] = Xa_abs[0] % Xs[0];
	XaPrevious[0] = Xa[0];
	XaPrevious_arg[0] = Xa_arg[0];
	PhiPrevious[0] = Phi[0];
	
	/*Synthesis*/
	fftw_execute(p2);
	q[0] = real(qaux[0])/N;
	q[0] = q[0] % w[0]/(sqrt((N/(2*hops[Qcolumn-1]))));
	for (int j=1; j<=(Qcolumn-1); j++)
	{
		Q[0].col(j-1) = Q[0].col(j);
	}
	
	Q[0].col(Qcolumn-1) = q[0];
	
	for (int k=1; k <= Qcolumn; k++)
	{
		for (int i=1; i<=N; i++)
		{
			ysaida[i+Hops-1] = ysaida[i+Hops-1] + Q[0](i-1,k-1);
		}
		Hops = Hops + hops[k-1];
	}
	
	//Linear interpolation
	r = ((double)hops[Qcolumn-1])/((double)hopa);

        for (int n=1; n <= hopa; n++)
        {
            n3 = (((double)n-1)*r+1);
            n1 = floor(n3);
            n2 = ceil(n3);
            yshift[n-1] = ysaida2[n1-1] + ((ysaida2[n2-1]-ysaida2[n1-1]))*(n3 - (double)n1);
		}
}