#ifndef FILEAPPS_H
#define FILEAPPS_H

#include <iomanip>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cctype>	//for isalnum()
#include <fstream>	//for file streaming

using namespace std;


class FileApps {

	private:
		
		char const* file_name;
		ofstream io_file;
		ifstream input_file;
		string data;
		int survey_txt_cnt;
		bool status;
		string * data_array;	


	public:			//public access specifier
	
		FileApps (char const* f_file_name)
		{	//constructor
			file_name = f_file_name;
		} 
	
		string get_file_name() 
		{
			return file_name;
		}
		
		string * get_data_array() 
		{
			return data_array;
		}
		
		int get_survey_txt_cnt()
		{
			return survey_txt_cnt;
		}
		
		/////////////////////////////////////////////
		// io_file_append()
		// open a file for append
		/////////////////////////////////////////////

		bool io_file_append ()
		{
		   io_file.open( file_name, ios::app);	//open for appending new record at the end	

		   if (io_file.fail())
		         return false;
		   else
		         return true;
		}

		/////////////////////////////////////////////
		// save_data_line()
		/////////////////////////////////////////////

		//bool save_data(ofstream &f_io_file, string f_vote, char const *f_file_name) 
		bool save_data_line( string f_vote) 
		{
			//Data from the form gets appended to survey.txt:
			//	1. open survey.txt for append with function io_file_append()
			//	2. append the record, close the file
			//	3. pico or TextWrangler survey.txt to see if the record was saved correctly

			//cout << "debug in save_data<br>" << endl;
		
			status = io_file_append();

			if (!status)
			{
				cout << "<br>io_file_append could not open " << file_name <<
					"for appending\n<br>";
				cout << "Program terminating\n";
				return 0;
			}
			else	
			{
				//write string data at the end of the file into survey.txt
				io_file << f_vote;
				cout << "FileApps.h: vote line saved: " << f_vote << "<br>" << endl;
			}
			io_file.close();
		}
		
		//*******************************************
		// read_data()
		//*******************************************

		bool read_data() 
		{
			// open the data file for reading
			// call count_records function into cnt as in fstream_io
			// call create_array to create dynamic array data_array as done before
			// get content of survey.txt into data_array - reuse populate_students_list_array 
			// function from fstream_io.cpp 
		
			//cout << "debug in read_data<br>" << endl;
			status = input_file_open(input_file);

			if (!status)
			{
				cout << "<br>" << file_name << " could not be opened for reading\n<br>";
				cout << "Program terminating\n";
				return 0;
			}
			else
			{
				//count how many records
				survey_txt_cnt = count_records (input_file);
				if (survey_txt_cnt == 0) {return 1;}
				cout << "FileApps.h debug in read_data with survey_txt_cnt: " << survey_txt_cnt << "<br>" << endl;
				data_array = create_array();
				cout << "FileApps.h debug after data_array created<br>" << endl;
				
				populate_data_array(input_file);
				cout << "FileApps.h debug with 1th element: " << data_array[0] << 
					"<br>" << endl;
	
			}
			input_file.close();

		}

		///////////////////////////////////////////////////
		////////////// count_records //////////////////////
		///////////////////////////////////////////////////

		int count_records(ifstream &f_input_file) 
		{
			//The following two lines bring the file cursor back to beginning of file - after
			//	having been left at the end after count_records function
			f_input_file.clear();
			f_input_file.seekg(0L, ios::beg);
		
		
			//cout << "debug with counting how many records in survey.txt ... <br>" << endl;	
			int records_count = 0;	//Extra record gets counted at the end of the file
			while (!f_input_file.fail()) 
			{
				getline(f_input_file, data); 
				records_count++;
				//cout << "data in cout_records is :" << data << "<br>" << endl;
			}
			
			return records_count -1;	//there is an extra line in text file
		}
		
		///////////////////////////////////////////////////
		////////////// input_file_open ////////////////////
		///////////////////////////////////////////////////
		bool input_file_open (ifstream& f_input_file)
		{
		   input_file.open(file_name);
		   if (f_input_file.fail())
	         return false;
		   else
	         return true;
		}

		//*******************************************
		// create_array 
		// Creates a dynamic array of type string to
		//   hold survey.txt data 
		//*******************************************
		string * create_array ()
		{
			string *address;
			address = new string[survey_txt_cnt];
			return (address);
		}


		//*******************************************
		// populate_data_array 
		//*******************************************
		
		void populate_data_array (ifstream &f_input_file)
		{
			//The following two lines bring the file cursor back to beginning of file - after
			//	having been left at the end after count_records function
			f_input_file.clear();
			f_input_file.seekg(0L, ios::beg);
			//cout << "debug in populate_data_array<br>" << endl;
		
			for (int index=0; index < survey_txt_cnt; index++)
			{
				getline(f_input_file, data);
				data_array [index] = data;
				
				//cout << "data in populate_data_array (again):" << data << "<br>" << endl;
			}
			//cout << "debug after populating data_array<br>" << endl;
			
			//cout << "debug with first element: " << f_data_array[4] << endl;
		}
		
};
	
		

#endif	//FILEAPPS_H
