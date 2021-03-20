/*
Program: top_div_array.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: Calculate which division in a company had the greatest sales for a quarter using 2 arrays - one for sales and one for division names
*/

// Libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

//Function Prototypes
void populate_div_sales(float f_div_sales[], string f_div_regions[], int f_size);
int findHighest(float f_div_sales[], int f_size);
void print_result(float f_div_sales[], string f_div_regions[], int f_index);

int main() {
	//Initialize variables
	const int size = 4;
	int top_div_index = 0;
	float div_sales[size];
	string div_regions[size];
	
	//Populate div_regions array
	div_regions[0] = "North East";
	div_regions[1] = "South East";
	div_regions[2] = "North West";
	div_regions[3] = "South West";
	
	//Populate div_sales array
	populate_div_sales(div_sales, div_regions, size);
	
	//Debug
	cout << "debug print for array div_sales" << endl;
	for(int i  = 0; i < size; i++) {
		cout << "$" << div_sales[i] << endl;
	}
	
	top_div_index = findHighest(div_sales, size);
	
	//Debug
	cout << "debug for top_div_index: " << top_div_index << endl;
	
	//Output
	print_result(div_sales, div_regions, top_div_index);
	
	return 0;
}

//Function Definitions
void populate_div_sales(float f_div_sales[], string f_div_regions[], int f_size) {
	for(int i = 0; i < f_size; i++) {
		cout << "Enter the sales for " << f_div_regions[i] << " division: $";
		cin >> f_div_sales[i];
	
		//Input Validation
		while (f_div_sales[i] < 0.00) {
			cout << "Please enter an amount for " << f_div_regions[i] << " that is greater than $0.00: $";
			cin >> f_div_sales[i];
		}
	}
}

int findHighest(float f_div_sales[], int f_size) {
	float greatestSalesAmount = 0;
	int save_index;
	
	for(int i = 0; i < f_size; i++) {
		if (f_div_sales[i] > greatestSalesAmount) {
			greatestSalesAmount = f_div_sales[i];
			save_index = i;
		}
	}
	
	return save_index;
}

void print_result(float f_div_sales[], string f_div_regions[], int f_index){
    cout << fixed << setprecision(2);
	cout << "The " << f_div_regions[f_index];
	cout << " division of the company had the highest sales of $" << f_div_sales[f_index] << endl;
}