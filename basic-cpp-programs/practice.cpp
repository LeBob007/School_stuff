//Program: movie_theater.cpp
//Author: Robert Hu
//Online class: cs102
//Semester: Spring 2018
//Description: calculates a theater's gross and net box office profit for a night

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

int main ()
{
	string movie_name;
	cout << "What movie are you asking about? ";
	getline(cin, movie_name);
	int adultTickets;
	cout << "How many adult tickets were sold? ";
	cin >> adultTickets;
	if (cin.fail())
	{
		cout << endl << "Number of adult tickets must be a number." << endl;
		exit(0);
	}
	int childTickets;
	cout << "How many child tickets were sold? ";
	cin >> childTickets;
	if (cin.fail())
	{
		cout << endl << "Number of child tickets must be a number." << endl;
		exit(0);
	}
	double grossProfit = (adultTickets * 6.00) + (childTickets * 3.00);
	double netProfit = grossProfit * 0.20;
	
	cout << fixed << setprecision(2);
	cout << "Movie Name:                  \"" << movie_name << "\"" << endl;
	cout << "Adult Tickets Sold:          "<< right << setw(8) << adultTickets << endl;
	cout << "Child Tickets Sold:          "<< right << setw(8) << childTickets << endl;
	cout << "Gross Box Office Profit:     $" << right << setw(10) << grossProfit << endl;
	cout << "Net Box Office Profit:       $" << right << setw(10) << netProfit << endl;
	cout << "Amount Paid to Distributor:  $" << right << setw(10) << grossProfit - netProfit << endl;
	
	return 0;
}