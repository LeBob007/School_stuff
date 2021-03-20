//Program: area_calculator.cpp
//Author: Robert Hu
//Online class: cs102
//Semester: Spring 2018
//Description: calculate the area of either a circle, rectangle, or triangle

#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

int main ()
{
	double length, width, base, height, radius, cirArea, rectArea, triArea, choice;
	const double pie = 3.14159;
	cout << "Calculate the area of a:" << endl;
	cout << "1. circle" << endl;
	cout << "2. rectangle" << endl;
	cout << "3. triangle" << endl;
	
	cout << "Enter option number: ";
	cin >> choice;
	cout << fixed << setprecision(2);
	
	if (choice == 1)
	{
		cout << "Enter circle radius: ";
		cin >> radius;
		cirArea = pie * pow(radius, 2);
		cout << "The circle's area with radius " << radius << " is: " << cirArea << endl;
	}
	else if (choice == 2)
	{
		cout << "Enter rectangle length: ";
		cin >> length;
		cout << "Enter rectangle width: ";
		cin >> width;
		rectArea = length * width;
		cout << "The rectangle's area with length " << length << " and width " << width << " is: " << rectArea << endl;
	}
	else if (choice == 3)
	{
		cout << "Enter triangle base: ";
		cin >> base;
		cout << "Enter triangle height: ";
		cin >> height;
		triArea = base * height * 0.5;
		cout << "The triangle's area with base " << base << " and height " << height << " is: " << triArea << endl;
	}
	else
	{
		cout << "That choice is not an option" << endl;
	}
	
	return 0;
}