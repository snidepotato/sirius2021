#include"DiffusionGBM.h"
#include"DiffusionCEV.h"
#include"DiffusionOU.h"
#include"DiffusionCIR.h"
#include"DiffusionLipton.h"

#include<iostream>

using namespace std;
using namespace SiriusFM;

int main()
{
		double m = 0;
		double s = 0;
		double b = 0;
		double t = 0;
		double kappa = 0; 
		double theta = 0;
		double s_0, s_1, s_2 = 0;

		cin >> m;
		cin >> s;
		cin >> b;
		cin >> s_0 >> s_1 >> s_2 >> kappa >> theta;

		DiffusionGBM A(m, s);
		DiffusionCEV B(m, s, b);
		DiffusionLipton C(m, s_0, s_1, s_2);
		DiffusionOU D(kappa, theta, s);
		DiffusionCIR E(kappa, theta, s);
		
		cout << "S_t\n";
		cin >> t;

		cout << A.mu(t) << " " << A.sigma(t) << endl;
		cout << B.mu(t) << " " << B.sigma(t) << endl;
		cout << C.mu(t) << " " << C.sigma(t) << endl;
		cout << D.mu(t) << " " << D.sigma(t) << endl;
		cout << E.mu(t) << " " << E.sigma(t) << endl;

		return 0;

}

		
