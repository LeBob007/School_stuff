#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

int main()
{
	const int array_size = 6000;
	int die = 0;
	int num_array[array_size];
	int tally_array[7] = {0, 0, 0, 0, 0, 0, 0};
	srand((int) time(0));
	
	for (int i = 0; i < 6000; i++)
	{
		die = (rand() % 6) + 1;
		num_array[i] = die;
	}
	cout << "debug for num_array[]" << endl;
	for (int i = 0; i < 200; i++)
	{
		cout << num_array[i] << " ";
	}
	cout << endl;
	
	for (int i = 0; i < 6000; i++)
	{
		int x = 0;
		x = num_array[i];
		tally_array[x]++;
	}
	cout << "debug for tally_array" << endl;
	for (int i = 1; i <= 6; i++)
	{
		cout << "i: " << i << " " << tally_array[i] << endl;
	}
	cout << endl;
	cout << "Display Result for 6000 Rolls" << endl;
	cout << "Die Face        Occurrences" << endl;
	cout << "===========================" << endl;
	int k = 1;
	while (k <= 6)
	{
		cout << k << right << setw(20) << tally_array[k] << endl;
		k++;
	}
	return 0;
}