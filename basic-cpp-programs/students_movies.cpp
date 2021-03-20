/*
Program: students_movies.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: Gathers statistical data on number of movies watched by each students
*/

#include <iomanip>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
using namespace std;

int * create_array(int);
void enter_data(int *, int);
float find_average(int *, int);
void show_array(int *, int);

int main()
{
	int *dyn_array;
	int students;
	float avrg;

	do
	{
		cout << "How many students will you enter? ";
		cin >> students;
	}while ( students <= 0 );

	dyn_array = create_array( students );
		//this function creates a dynamic array
		//and returns its pointer
	enter_data(dyn_array, students);
		//use 'pointer' notation in this function to
		//access array elements
		//accept only numbers 0-100 for movie seen
	avrg = find_average(dyn_array, students);
		//use 'pointer' notation in this function to
		//access array elements

	cout << "The array is:" << endl << endl;
	show_array(dyn_array, students);
	    //print out array

	cout << endl;
	cout << "The average is " << avrg << ".\n";
        //print out average
    
	delete [] dyn_array;
	return 0;
}

int * create_array(int f_students){
	int * f_dyn_array = new int [f_students];
	return f_dyn_array;
}

void enter_data(int * f_dyn_array, int f_students){
	int movies_seen;
	int arr[f_students];
	for(int i = 0; i < f_students; i++) {
		cout << "Enter movies seen for student " << i + 1 << ": ";
		cin >> movies_seen;
		while(movies_seen < 0 || movies_seen > 100){
			cout << "Enter movies seen for student " << i + 1 << ": ";
			cin >> movies_seen;
		}
		f_dyn_array[i] = movies_seen;
	}
}

float find_average(int * f_dyn_array, int f_students){
	float total = 0;
	float f_avrg = 0;
	for(int i = 0; i < f_students; i++){
		total += *(f_dyn_array + i);
	}
	f_avrg = total / f_students;
	return f_avrg;
}

void show_array(int * f_dyn_array, int f_students){
	for(int i = 0; i < f_students; i++){
		cout << "Student " << i + 1 << " watched " << *(f_dyn_array + i) << " movie(s)." << endl;
	}
}