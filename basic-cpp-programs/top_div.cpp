/*
Program: top_div.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: Calculate which division in a company had the greatest sales for a quarter
EXTRA CREDIT: IMPLEMENTS DUPLICATE TOP DIVISIONS
*/

// Libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

float input_div(string name){
	//Declaration
	float input;
	cout << "Enter the sales for " << name << " division: $";
	cin >> input;
	
	//Input Validation
	while (input < 0.00) {
		cout << "Please enter an amount for " << name << " that is greater than $0.00: $";
		cin >> input;
	}
	
	//Output
	return input;
}

void findHighest (float nE, float sE, float nW, float sW) {
	//Declaration
	float highest = nE;
	const int size = 4;
	string names[size] = {"North East", "South East", "North West", "South West"};
	float values[size] = {nE, sE, nW, sW};
	
	//Process Loops
	if (sE > highest) {
		highest = sE;
	}
	if (nW > highest) {
		highest = nW;
	}
	if (sW > highest) {
		highest = sW;
	}
	
	//Output
	cout << fixed << setprecision(2);
	cout << "The ";
	for (int i = 0; i < size; i++) {
		if (values[i] == highest) {
			cout << names[i] << " and ";
		}
	}
	cout << "\b\b\b\b\b division(s) of the company had the highest sales of $" << highest << endl;
}

int main() {
	//Declaration
	float nE, sE, nW, sW;
	
	cout << "Sales of the four division of the company: " << endl << endl;
	
	//Function Call
	nE = input_div( "North East" );
	sE = input_div( "South East" );
	nW = input_div( "North West" );
	sW = input_div( "South West" );
	
	//Output
	cout << endl;
	findHighest(nE, sE, nW, sW);
	
	return 0;
}