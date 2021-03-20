/*
Program: retrieve_form_OOP.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: Retrieves three form fields and sends the result back to the browser using classes and objects
*/

#include "WebApps.h"
#include "FileApps.h"
#include <iomanip>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cctype>
using namespace std;

WebApps wo;					//make the wo object global
FileApps fo("survey.txt");	//fo for file object
void build_form();
void save_data_line(string);
int * create_int_array();

string prepare_vote(int);
void display_result (string *);

int main()
{		
	const int cnt = wo.get_cnt();
	if (cnt != 0) {
		cout << "debug with cnt != 0<br>" << endl;
			//call function to prepare vote 
		string data_line = prepare_vote(cnt);
			cout << "debug with data_line: " << data_line << "<br>" << endl;
			//save_data_line() appends the vote to the end of survey.txt
		fo.save_data_line(data_line);
			//read_data() opens survey.txt and populates array data_array[]
		fo.read_data ();
			cout << "debug past read_data()<br>" << endl;
			//array data_array[] is now ours (contains all of the votes as strings
		string* data_array = fo.get_data_array();
		cout << "From main() debug with 1th element: " << data_array[0] << 
					"<br>" << endl;		
		// End of the need for FileApps.h under this if statement
		build_form();
		display_result(data_array);
	}else {
		build_form();
		fo.read_data ();
		string* data_array = fo.get_data_array();
		display_result(data_array);
	}
	cout << "<a href=\"http://toolkit.cs.ohlone.edu/~gen209/survey.txt\"> Raw Data</a>" << endl;
	return 0;
}

////////////functions/////////////
void build_form () {
	cout << "<html><head><title>Becoming Adults</title></head>" <<
			"<body bgcolor=\"cadetblue\"><center>\n" <<
			"<form action=\"survey.cgi\" method=\"GET\">\n" <<
			"	<font size=\"8\">Is being an adult as much fun as people say?</font><br>\n" <<
			"		<label for=\"vote\"><font size=\"6\">Yes</font></label>\n" <<
			"		<input name=\"vote\" value=\"Y\" type=\"radio\"></td><br>\n" <<
			"		<label for=\"vote\"><font size=\"6\">No</font></label>\n" <<
			"		<input name=\"vote\" value=\"N\" type=\"radio\"></td><p>\n" <<		
			"	<p><input type=\"submit\" value=\"Submit\">\n" <<
			"</form>\n" <<
			"</body></center>\n" << 
			"</html>\n";
}

string prepare_vote(int f_cnt) {
	//create dynamic array name_value_pairs[] from the wo object
	FIELDS * name_value_pairs = wo.create_array(wo.get_cnt());
	//parse qs into name_value_pairs[] array from the wo object
	wo.parse(wo.get_qs(), name_value_pairs); 
	//param the vote field value into variable 'vote' from the wo object
	string vote = wo.param("vote", name_value_pairs, wo.get_cnt());
	cout << "debug with vote: " << vote << "< br>" << endl;

	return  vote + "|\n";	//the new line character will put each vote 
					//on a new line in the file
					//ex of a concatenated vote: "y|\n"
}

int * create_int_array(){
	int * f_dyn_array = new int [2];
	return f_dyn_array;
}

void display_result (string f_data_array[]) {	
	string vote = "";
	
	//Initialize dynamic array with create_int_array()
	int * vote_tally_array;
	vote_tally_array = create_int_array();
		
	//initialize vote_tally_array to 0’s in a loop
	for (int i = 0; i < 2; i++) {
		vote_tally_array[i] = 0;
	}
	//In another loop, increment vote_tally_array from f_data_array[]
	 	// Use something like this pseudocode::
		// if vote == "Y|" then vote_tally_array[0]++
		// else if vote == "N|" then vote_tally_array[1]++
	for (int i = 0; i < fo.get_survey_txt_cnt(); i++) {
		if (f_data_array[i] == "Y|"){ 
			vote_tally_array[0]++;           
		} 
		else if(f_data_array[i] == "N|"){ 
			vote_tally_array[1]++; 
		} 
	}
	//print the results
	cout << "Number of votes for YES: " << vote_tally_array[0] << endl;
	cout << "Number of votes for NO: " << vote_tally_array[1] << endl;
}		








/*
//main begins
int main()
{
	//Create object
	WebApps wo;
	
	//Debug
	cout << "<br>debug in main with wo.get_cnt(): " << wo.get_cnt() << "<br>" << endl;
	
	FIELDS * name_value_pairs = wo.create_array(wo.get_cnt());
	name_value_pairs[0].name = "success<br>";
	//testing array
	cout << "debug with name_value_pairs[0].name: " << name_value_pairs[0].name << "<br>" << endl;

	wo.parse(wo.get_qs(), name_value_pairs); 
	
	// debug to show content of name_value_pairs
	cout << "debug for name_value_pairs array: " << endl << "<br>";
	for (int index = 0; index < wo.get_cnt(); index++) {
        cout << "name: " << name_value_pairs[index].name << " ";
        cout << "value: " << name_value_pairs[index].value << endl << "<br>";
    } 
 

	//Set Variables
	string first = wo.param("first", name_value_pairs, wo.get_cnt());
	string last = wo.param("last", name_value_pairs, wo.get_cnt());
	string method = wo.param("method", name_value_pairs, wo.get_cnt());
	
	//Output
	cout << endl;
	if (method == "Death_Knight") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Herald of Doom.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/3/30/Death_knight_crest.png/revision/latest?cb=20130813094520\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>Free from the Lich King's control, you wander the lands wielding a weapon imbued with dark magic and attacking your foes with an unholy power.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Demon_Hunter") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", an Unforgiving Predator.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/d/db/Demon_hunter_crest-250x271.png/revision/latest?cb=20151004044357\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>Upholding a dark legacy, you utilize fel and chaotic magics to hunt down the demons of the Burning Legion and incite fear in allies and enemies alike.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Druid") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Savage Shapeshifter.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/5/55/Druid_crest.png/revision/latest?cb=20130813094831\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>As a versatile combatant, you harness the vast powers of nature and unleash its raw energy on your foes whilst transforming into other forms.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Hunter") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", an Inescapable Stalker.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/2/20/Hunter_crest.png/revision/latest?cb=20130813094921\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>Master of your environment, you are nimble, slipping through trees and trapping foes, all while attacking with projectiles with help from a pet.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Mage") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Master of Time and Space.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/b/b8/Mage_crest.png/revision/latest?cb=20130813094952\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>Gifted with keen intellect and unwavering discipline, you demolish foes with arcane incantations, protecting yourself with enchantments.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Monk") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Musing Brawler.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/6/60/Monk_crest.png/revision/latest?cb=20130817144820\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>Harnessing your chi, you fight oppressors with your hands and feets to channel the flow of chi to enhance allies and hinder enemies.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Paladin") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Paragon of Justice.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/0/09/Paladin_crest.png/revision/latest?cb=20130813095032\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>Answering the call of the paladins, you are blessed with the light to vanquish evil, protect the weak, and bring justice to the unjust.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Priest") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", an Invoker of Light and Darkness.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/7/71/Priest_crest.png/revision/latest?cb=20130813095106\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>Devoted to the spiritual with unwavering faith, you left the comfort of your shrine to join and support your allies on the war-torn front.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Rogue") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Prowling Cutpurse.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/e/e2/Rogue_crest.png/revision/latest?cb=20130813095139\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>For you, a lethal assassin and master of stealth, your honor is purchased with gold and your only code is the contract.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Shaman") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", an Adept of the Elements.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/d/df/Shaman_crest.png/revision/latest?cb=20130813095240\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>A spiritual guide and practitioner of the elements, you moderate the chaotic balance between these neutral forces to punish your foes and protect your allies.</h3>" << endl;
		cout << "</center>";
	}
	else if (method == "Warlock") {
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Channeler of Forbidden Powers.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/4/4f/Warlock_crest.png/revision/latest?cb=20130813095321\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>With total domination in your eyes, you wield power over dark and burning magics while forming contracts with demons in exchange for souls.</h3>" << endl;
		cout << "</center>";
	}
	else { //Warrior
		cout << "<center>";
		cout << "<h1>Welcome " << first << " " << last << ", a Lord of War.</h1>" << endl;
		cout << "<img src=\"https://vignette.wikia.nocookie.net/wowwiki/images/e/ee/Warrior_crest.png/revision/latest?cb=20130813095500\" width=\"350\" height=\"350\"> " << endl;
		cout << "<h3>A master of the art of battle, you combine strength, leadership, and a vast knowledge of arms and armor to wreak havoc in glorious combat.</h3>" << endl;
		cout << "</center>";
	}
	
	
    return 0;
}
*/
