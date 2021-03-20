#ifndef WEBAPPS_H
#define WEBAPPS_H

#include <iomanip>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cctype>
using namespace std;

struct FIELDS 
{
	string name;
	string value;
}; 

//Class
class WebApps {
	public:
		WebApps(){
			cout << "Content-type:text/html\n\n";
			set_qs(getenv("QUERY_STRING"));
				//cout << "debug with get_qs: " << get_qs();//test functions
			set_cnt(how_many(get_qs()));
				//cout << "<br>debug with get_cnt: " << get_cnt();//test functions
		}
		//set qs value
		void set_qs(string f_getenv){
			qs = f_getenv;
		}
		//set cnt
		void set_cnt(int f_how_many){
			cnt = f_how_many;
		}
		
		string get_qs(){
			return qs;
		}
		
		int get_cnt(){
			return cnt;
		}
		//counts how many "=" there are and returns the number
		int how_many (string f_qs)
		{
			//initialize variables
			int count = 0, pos, start_pos = 0;
			do {
				pos = f_qs.find("=", start_pos);
				count++;
				start_pos = pos + 1;
			} while (pos != string::npos);
			return count-1;
		}
		//Create the array
		FIELDS * create_array (int f_cnt){
//			FIELDS * array = new FIELDS [f_cnt];
//			return array;
			FIELDS * address;
			address = new FIELDS[f_cnt];
			return (address);
		}
		//Same parse as before
		void parse (string f_qs, FIELDS f_name_value_pairs []){
			cout << "debug in parse<br>\n" << endl;
			string name, value;
			int start_pos = 0, pos; 
			for (int counter=0; counter < get_cnt(); counter++) {
				pos = f_qs.find("=", start_pos);
				name = f_qs.substr(start_pos, pos - start_pos); 
				f_name_value_pairs[counter].name = name;
				cout << "name: " << name << "<br>" << endl; 
				start_pos = pos + 1;  
				pos = f_qs.find("&", start_pos);
				if (pos == string::npos) {
					pos = f_qs.length();
				}
				value = f_qs.substr(start_pos, pos - start_pos);
				f_name_value_pairs[counter].value = value;				
				cout << "value: " << value << "<br>" << endl; 
				start_pos = pos + 1; 
			} 
		}
		//Same param as before
		string param(string lookUp, FIELDS f_name_value_pairs[], int f_cnt){
			for (int i = 0; i < f_cnt; i++) {
				if (f_name_value_pairs[i].name == lookUp) {
					return f_name_value_pairs[i].value;
				}
			}
			return "";
		}
	private:
		string qs;
		int cnt;
};

#endif //WEBAPPS_H