/*
Program: retrieve.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: Retrieves data from a url and returns it to the browser
*/

#include <iostream>
#include <cstdlib>
using namespace std;

int main ()
{
        string s(getenv("QUERY_STRING"));
		//string s = "first=fred&last=flint";	//use this for testing in an IDE
							//instead of the QUERY_STRING
        cout << "Content-type:text/html\n\n";
        cout << "<html>" << endl;
        cout << "<head>" << endl;
        cout << "<title>Grabbing Data From the URL</title>" << endl;
        cout << "</head>" << endl;
        cout << "<body>" << endl;
        cout << "<body bgcolor=\"black\">" << endl;
        cout << "<font color=\"orange\">" << endl;
        cout << "s: " << s << "<br>\n";
        cout << "</font></body>" << endl;
        cout << "</html>" << endl;
        return 0;
}