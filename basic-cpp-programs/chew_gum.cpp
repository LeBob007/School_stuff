/*
Program: chew_gum.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: print out a concatenated sentence made from a sentence that is repeated a chosen number of times
*/

// Libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

int main()
{
	// Initializing
	string phrase;
	string repeatPhrase;
	int repeat, phraseLength, repeatLength;
	
	// Input
	cout << "Enter a sentence to write on the board:" << endl;
	getline(cin, phrase);
	cout << endl;
	cout << "How many times should it be written? ";
	cin >> repeat;
	cout << endl;
	phraseLength = phrase.length();
	repeatLength = phraseLength * repeat;
	
	// Processing loop
	for (int i = 0; i < repeat; i++)
	{
		repeatPhrase += phrase;
		repeatPhrase += " \n";
	}
	
	// Output
	cout << "The concatenated string is now:" << endl;
	cout << repeatPhrase << endl;
	cout << "You've typed " << repeatLength << " characters" << endl;
}