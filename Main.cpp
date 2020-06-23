/* Author: Florian Lungu
 *
 * Description:
 * This C++ code reads data from the Golden Cheetah default export file
 * golden_cheetah_export.csv and TrainingPeaks.com export file workouts.csv
 * then writes the data I want to a new csv file so that I can easily copy
 * and paste into my annual cycling log Excel file
 *
 * Version History:
 * 2020-Jun-13 Initial creation
 *
 */

#include "BaseFunctions.h"
#include <fstream>
#include <iostream>
#include <vector>

#include <string>
#include <stdexcept> // runtime_error
#include <sstream>
#include <istream>
#include <cmath>
#include <windows.h>

using namespace std;


namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

int main() {
	// settings from ini file
	string line, line_tp, word, param, val, temp;
	string trainingpeaks_export = "";
	string golden_cheetah_export = "";
	string cycling_log_import = "";
	string trainingpeaks_lastname = "";
	ifstream settings_file;

	// csv files
	const char *bom = "\xef\xbb\xbf";
	ifstream gc_file, tp_file;
	ofstream cl_file;
	vector<string> gc_row, tp_row;
	int rownum = 1,
		rownum_tp = 1,
		tcount = 0,
		this_row,
		last_row;

	// columns and fields
	int column_distance,
		column_moving_time,
		column_elevation_gain,
		column_TSS,
		column_hrTSS,
		column_IF,
		column_hrIF,
		column_np,
		column_temp,
		column_avg_hr ,
		column_max_hr,
		column_find_date,
		column_title,
		column_my_comments,
		parse_start;

	string field_title,
		field_my_comments,
		field_date,
		field_distance,
		field_moving_time,
		field_elevation_gain,
		field_TSS,
		field_IF,
		field_np,
		field_temp,
		field_avg_hr,
		field_max_hr,
		field_date_raw,
		field_mph,
		find_date;

	// get file locations from ini file
	settings_file.open("Cycling Log Import Settings.ini");
	if (settings_file) {
		cout << "Reading from the \'Cycling Log Import Settings.ini\' file" << endl;
		while (getline(settings_file,line)) {
			param = trim(line.substr(0, line.find("=")));
			val = trim(line.substr(line.find("=")+1, line.length()-line.find("=")-1));

			if (param=="trainingpeaks_export") {
				trainingpeaks_export = ReplaceAll(val, "\\", "\\\\");
			} else if (param=="golden_cheetah_export") {
				golden_cheetah_export = ReplaceAll(val, "\\", "\\\\");
			} else if (param=="cycling_log_import") {
				cycling_log_import = ReplaceAll(val, "\\", "\\\\");
			} else if (param=="trainingpeaks_lastname") {
				trainingpeaks_lastname = ReplaceAll(val, "\\", "\\\\");
			}
		}
		settings_file.close();
	} else {
		cout << "There was an error opening \'Cycling Log Import Settings.ini\' file" << endl;
	}

	// open csv files
	gc_file.open(golden_cheetah_export);
	if (!gc_file) {
		cout << "There was an error opening golden_cheetah_export file" << endl;
	}
	tp_file.open(trainingpeaks_export);
	if (!tp_file) {
		cout << "There was an error opening trainingpeaks_export file" << endl;
	}
	cl_file.open(cycling_log_import);
	if (!cl_file) {
		cout << "There was an error opening cycling_log_import file" << endl;
	} else {
		cl_file << bom;
	}

	// parse golden cheetah export file
	if (gc_file && tp_file && cl_file) {
		cout << "Reading from the Golden Cheetah Export file..." << endl;

		while (getline(gc_file,line)) {
			// parse line
			string this_line = line;
			stringstream s(this_line);
			gc_row.clear();

			while (getline(s, word, ',')) {
				// figure out column numbers based on first row
				if (rownum==1) {
					if (word==" Distance") {
						column_distance = tcount;
					} else if (word==" Time Moving") {
						column_moving_time = tcount;
					} else if (word==" Elevation Gain") {
						column_elevation_gain = tcount;
					} else if (word==" TSS") {
						column_TSS = tcount;
					} else if (word==" TRIMP Zonal Points") {
						column_hrTSS = tcount;
					} else if (word==" IF") {
						column_IF = tcount;
					} else if (word==" Heart Rate Intensity") {
						column_hrIF = tcount;
					} else if (word==" NP") {
						column_np = tcount;
					} else if (word==" Average Temp") {
						column_temp = tcount;
					} else if (word==" Average Heart Rate") {
						column_avg_hr = tcount;
					} else if (word==" Max Heartrate") {
						column_max_hr = tcount;
					}
					tcount++;

				// parse each activity into a vector
				} else {
					gc_row.push_back(word);
				}
			}

		    if (rownum>1) {

				// training peaks fields
				field_title = "";
				field_my_comments = "";

				// golden cheetah fields
				field_date = "";
				field_distance = "";
				field_moving_time = "";
				field_elevation_gain = "";
				field_TSS = "";
				field_IF = "";
				field_np = "";
				field_temp = "";
				field_avg_hr = "";
				field_max_hr = "";

				field_date_raw = gc_row[0];
				field_date = "20"+field_date_raw.substr(6,2)+"-"+field_date_raw.substr(0,2)+"-"+field_date_raw.substr(3,2);

				if (stof(gc_row[column_distance]) > 0) {
					field_distance = patch::to_string(round(stof(gc_row[column_distance])*0.621371*10)/10);
					field_moving_time = patch::to_string(int(stof(gc_row[column_moving_time])/60));
					if (field_distance != "0" && field_moving_time != "0") {
						field_mph = patch::to_string(stof(field_distance)/stof(field_moving_time)*60);
					} else {
						field_mph = "";
					}
					field_elevation_gain = patch::to_string(round(stof(gc_row[column_elevation_gain])*3.28084));
					if (gc_row[column_np] != "0") {
						field_np = patch::to_string(ceil(stof(gc_row[column_np])));
						field_IF = patch::to_string(round(stof(gc_row[column_IF])*100)/100);
						field_TSS = patch::to_string(round(stof(gc_row[column_TSS])));
					}
					if (field_IF == "") {
						field_IF = patch::to_string(round(stof(gc_row[column_hrIF])*100)/100);
						field_TSS = patch::to_string(round(stof(gc_row[column_hrTSS])));
					}
					if (stof(gc_row[column_temp]) > -20) {
						field_temp = patch::to_string(round((stof(gc_row[column_temp]) * 9/5 + 32)));
						}
						field_avg_hr = patch::to_string(round(stof(gc_row[column_avg_hr])));
						field_max_hr = gc_row[column_max_hr];
					}

					// loop through trainingpeaks file and find the row with the same date
					tcount = 0;
					this_row = 0;
					last_row = 0;
					rownum_tp = 1;
					tp_file.clear();
					tp_file.seekg(0);
					line_tp = "";
					while (getline(tp_file,line_tp)) {
						this_row++;
						tp_row.clear();
						tp_row = CsvlinePopulate(line_tp,",");

						// figure out column numbers from the first row
						if (rownum_tp==1) {
							for (auto &element : tp_row) {
								if (element=="WorkoutDay") {
									column_find_date = tcount;
								} else if (element=="Title") {
									column_title = tcount;
								} else if (element=="AthleteComments") {
									column_my_comments = tcount;
								}
								tcount++;
							}

						// parse columns for data
						} else {
							find_date = tp_row[column_find_date];
							if (find_date == field_date) {
								if (this_row > last_row) {
									field_title = tp_row[column_title];
									field_my_comments = tp_row[column_my_comments];
									if (field_my_comments.length() > 0) {
										field_my_comments = field_my_comments.substr(1,field_my_comments.length()-2);
										parse_start = field_my_comments.find(trainingpeaks_lastname)+trainingpeaks_lastname.length()+1;
										field_my_comments = field_my_comments.substr(parse_start,field_my_comments.length()-parse_start);
									} else {
										field_my_comments = "";
									}
									// cout << "field_my_comments is " << field_my_comments << endl;
									last_row = this_row;
								}
							}
						}
						rownum_tp++;
					}

					// output row to cycling_log_import file
					cl_file << "\"" << field_date << "\"" << ',';
					cl_file << "\"" << field_title << "\"" << ',' << ',' << ',';
					cl_file << "\"" << field_distance << "\"" << ',';
					cl_file << "\"" << field_moving_time << "\"" << ',';
					cl_file << "\"" << field_elevation_gain << "\"" << ',';
					cl_file << "\"" << field_TSS << "\"" << ',';
					cl_file << "\"" << field_IF << "\"" << ',';
					cl_file << "\"" << field_np << "\"" << ',';
					cl_file << "\"" << field_avg_hr << "\"" << ',';
					cl_file << "\"" << field_max_hr << "\"" << ',';
					cl_file << "\"" << field_temp << "\"" << ',';
					cl_file << "\"" << field_my_comments << "\"" << endl;
		    }
		    rownum++;
		}

		gc_file.close();
		tp_file.close();
		cl_file.close();

		cout << "Successfully processed " << rownum << " activities from Golden Cheetah" << endl;
	}

	cout << "Program completed successfully!!" << endl;
	Sleep(3000);
	return 0;
}



