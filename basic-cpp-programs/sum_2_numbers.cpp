#include <iostream>
#include <iomanip>
using namespace std;

int main ()
{
	double x = 25.9384;
	double y = 32.6103;
	cout << "Variable x contains: " << x << endl;
	cout << "Variable y contains: " << y << endl;
	cout << fixed << setprecision(2);
	double total = x + y;
	cout << "Total:"  << setw(40) << setfill('#') << total << endl;
	return 0;
}
