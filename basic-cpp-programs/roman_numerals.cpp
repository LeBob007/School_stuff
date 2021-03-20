//Program: roman_numerals.cpp
//Author: Robert Hu
//Online class: cs102
//Semester: Spring 2018
//Description: converts integers ranging from 1 to 10 into roman numerals

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

int main ()
{
	int number;
	cout << "Enter a number between 1 through 10: ";
	cin >> number;
	
	switch (number)
	{
		case 1:
			cout << "The roman numeral conversion is I." << endl;
			break;
		case 2:
			cout << "The roman numeral conversion is II." << endl;
			break;
		case 3:
			cout << "The roman numeral conversion is III." << endl;
			break;
		case 4:
			cout << "The roman numeral conversion is IV." << endl;
			break;
		case 5:
			cout << "The roman numeral conversion is V." << endl;
			break;
		case 6:
			cout << "The roman numeral conversion is VI." << endl;
			break;
		case 7:
			cout << "The roman numeral conversion is VII." << endl;
			break;
		case 8:
			cout << "The roman numeral conversion is VIII." << endl;
			break;
		case 9:
			cout << "The roman numeral conversion is IX." << endl;
			break;
		case 10:
			cout << "The roman numeral conversion is X." << endl;
			break;
		default:
			cout << "That is not a number between 1 and 10" << endl;
			break;
	
	}
	
	return 0;
	
}