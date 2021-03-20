/*
Program: retrieve_form.cpp
Author: Robert Hu
Online class: cs102
Semester: Spring 2018
Description: Retrieves three form fields and sends the result back to the browser
*/

#include <iomanip>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
using namespace std;

struct FIELDS 
{
	string name;
	string value;
}; 

const int cnt = 3;	//cnt should be set to the number of fields the html form contains

// Prototypes
void parse(string, FIELDS []);
string param(string, FIELDS [], int);

//main begins
int main()
{
	FIELDS name_value_pairs [cnt];

	string qs(getenv("QUERY_STRING")); 
		//string qs("first=fred&last=flint&color=red");
	cout << "Content-type:text/html\n\n"; 
	cout << "debug with qs: " << qs << "<p>" << endl;

	parse(qs, name_value_pairs); 

	// debug to show content of name_value_pairs
	cout << "debug to show content of name_value_pairs array: " << endl << "<br>";
	for (int index = 0; index<cnt; index++) {
        cout << "name: " << name_value_pairs[index].name << " ";
        cout << "value: " << name_value_pairs[index].value << endl << "<br>";
    } 
 

	//Set Variables
	string first = param("first", name_value_pairs, cnt);
	string last = param("last", name_value_pairs, cnt);
	string method = param("method", name_value_pairs, cnt);
	
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

//*******************************************
//******** Functions begin ******************
//*******************************************

//*******************************************
// parse()
// This will separate the name/value pairs found after the ? in 
// the URL and populate the name_value_pairs array of structures
//*******************************************
void parse (string qs, FIELDS f_name_value_pairs [])
{	
	cout << "debug in parse<br>\n" << endl;
	string name, value;
	int start_pos = 0, pos; 
	for (int counter=0; counter < cnt; counter++) {
		pos = qs.find("=", start_pos);
		name = qs.substr(start_pos, pos - start_pos); 
		cout << "name: " << name << "<br>" << endl; 
		start_pos = pos + 1;  
		pos = qs.find("&", start_pos);
		if (pos == string::npos) {
			pos = qs.length();
		}
		value = qs.substr(start_pos, pos - start_pos); 
		cout << "value: " << value << "<br>" << endl; 
		start_pos = pos + 1; 
		f_name_value_pairs[counter].name = name;
		f_name_value_pairs[counter].value = value;
	} 
}

//*******************************************
// param()
// This will find the field value based on the
// field name
//*******************************************
string param(string lookUp, FIELDS f_name_value_pairs[], int f_cnt)
{	
	for (int i = 0; i < f_cnt; i++) {
		if (f_name_value_pairs[i].name == lookUp) {
			return f_name_value_pairs[i].value;
		}
	}
	
	return "";
}

