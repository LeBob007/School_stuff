/***************************************************************
Created by: Hu, Robert
            ryhu
            9 July 2018
			
Assignment: Lab 1: A Simple Program
            CMPE 012, Computer Systems and Assembly Language
            UC Santa Cruz, Summer 2018
			
Description:This program determines which years are leap years between 1800 and 2500 inclusive
			and prints either "leap" for leap years and the year number for the rest

Usage:      javac Leap.java
            java Leap
			
Notes:      If run with bash, must have path to java commands
***************************************************************/

class Leap {
	public static void main(String[] args) {
		int year;
		
		//Loop to examine all years between 1800 and 2500 inclusive
		for (year = 1800; year < 2501; year++) {
			// Prints the word "leap" for all years divisible by 4 and not 100
			if (year % 4 == 0 && year % 100 != 0) {
				System.out.println("leap");
			}
			
			//Prints the word "leap" for all years divisible by 400
			if (year % 400 == 0) {
				System.out.println("leap");
			}
			
			//Prints out the year all years that are either not divisible
			//by four or are divisible by 100 and not 400
			if (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0)) {
				System.out.println(year);
			}
		}
		
	}
	
	
}