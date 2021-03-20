/*
Program: top_div_structures.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: Calculate which division in a company had the greatest sales for a quarter using structures
*/

// Libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

//Function Prototypes
struct DIV{
	float sale;
	string name;
};

void populate_div_sales(DIV[], int);
int findHighest(DIV[], int);
void print_result(DIV[], int);

int main() {
	//Initialize variables
	const int size = 4;
	int top_div_index = 0;
	DIV div_info[size];
	//Populate div_regions array
	div_info[0].name = "North East";
	div_info[1].name = "South East";
	div_info[2].name = "North West";
	div_info[3].name = "South West";
	
	//Debug
	cout << "debug print for array div_info" << endl;
	for(int i = 0; i < size; i++) {
		cout << div_info[i].name << endl;
	}
	
	//Populate div_sales array
	populate_div_sales(div_info, size);
	
	//Debug
	cout << "debug print for array div_sales" << endl;
	for(int i  = 0; i < size; i++) {
		cout << "$" << div_info[i].sale << endl;
	}
	
	top_div_index = findHighest(div_info, size);
	
	//Debug
	cout << "debug for top_div_index: " << top_div_index << endl;
	
	//Output
	print_result(div_info, top_div_index);
	
	return 0;
}

//Function Definitions
void populate_div_sales(DIV f_div_info[], int f_size) {
	for(int i = 0; i < f_size; i++) {
		cout << "Enter the sales for " << f_div_info[i].name << " division: $";
		cin >> f_div_info[i].sale;
	
		//Input Validation
		while (f_div_info[i].sale < 0.00) {
			cout << "Please enter an amount for " << f_div_info[i].name << " that is greater than $0.00: $";
			cin >> f_div_info[i].sale;
		}
	}
}

int findHighest(DIV f_div_info[], int f_size) {
	float greatestSalesAmount = 0;
	int save_index;
	
	for(int i = 0; i < f_size; i++) {
		if (f_div_info[i].sale > greatestSalesAmount) {
			greatestSalesAmount = f_div_info[i].sale;
			save_index = i;
		}
	}
	
	return save_index;
}

void print_result(DIV f_div_info[], int f_index){
    cout << fixed << setprecision(2);
	cout << "The " << f_div_info[f_index].name;
	cout << " division of the company had the highest sales of $" << f_div_info[f_index].sale << endl;
}