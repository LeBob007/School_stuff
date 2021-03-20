//Program: sum_numbers.cpp
//Author: Robert Hu
//Online class: cs102
//Semester: Spring 2018
//Description: calculate the sum of all the numbers from 1 to a designated number

#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

int main()
{
	int number, total = 0;
	
	cout << "Please enter a positive number: ";
	cin >> number;
	cout << endl;
	
	while (number <= 0)
	{
		cout << "ERROR: You need to enter a positive number: ";
		cin >> number;
		cout << endl;
	}
	
	for (int i = 1; i <= number; i++)
	{
		total = total + i;
		cout << i << " + ";
	}
	
	cout << "\b\b" << "= " << total << endl;
	
	cout << endl;
	cout << "------------" << endl;
	cout << "  Done !!!  " << endl;
	cout << "------------" << endl;
}