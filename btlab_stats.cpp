/* 
Program written by Umair Ahmed
Contact me at aumair@umich.edu
This program parses through a file and finds all the login/logous events
It then outputs it all in a csv file called output.csv which can easily be imported into Google Sheets
The logic is as follows:
	1. Read in file name and open input file.
	2. Read every line and determine if it's a logout line. If it is, keep reading until
		a. a login line is found. Then make the event.
		b. a header line or logout line by the same user is found. Don't make any event.
	Once that is done, move the cursor back to the logout line.
	3. Move on to the next line and repeat step 2 until the last line is reached.
	4. Get rid of the duplicate events.
	5. Output all the events into output.csv.
	6. Return the amount of time the process took.

Project can be compiled with 
	g++ -Wall -Werror -pedantic --std=c++17 -g btlab_stats.cpp -o btlab_stats.exe
Project can be run with
	.\btlab_stats.exe [INSERT_FILENAME_HERE]
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <chrono>

using namespace std;

const string BLOCKLIST[] = {"DWM-", "UMFD-", "USER"};

struct LoginEvent {
	tm time_generated;
	int event_id;
	string machine;
	string user;
	int logon_type;
	string duration;
};

tm date_time_to_tm(string& date, string& time);

string time_difference(tm& logout, tm& login);

bool blocklist_contains(string& str);

vector<LoginEvent> remove_duplicates(vector<LoginEvent>& vec);

int main(int argc, char ** argv) {
	auto start = chrono::high_resolution_clock::now();
	
	string filename = "22-03-22 BTLab Event Log raw data.txt"; // default file name
	if (argc == 1) {
		cout << "Note: filename will automatically be M-2UA1241CWW-master.txt" << endl 
			<< "This can be overriden by running ./btlab_stats.exe [filename]" << endl;
	} else {
		filename = argv[1];
	}

	ifstream master_file;
	master_file.open(filename);
	if (!master_file.is_open()) {
		cout << "Error: Failed to open file with name " << filename << endl;
		return 1;
	}

	vector<LoginEvent> event_list;
	string line;
	long debug_line{ 2 };
	getline(master_file, line); // get rid of header line
	while (getline(master_file, line)) { // loop through file
		string date{ "01/25/2022" };
		string time{ "16:41:17" };
		int event_id{ -1 };
		string user{ "USER" };
		istringstream event(line);
		event >> date >> time >> event_id >> user; // read line
		if (user.length() <= 8 && !blocklist_contains(user)) {
			tm logout_time = date_time_to_tm(date, time);
			streampos initial_pos = master_file.tellg(); // save cursor location
			while (getline(master_file, line)) { // keep looping until login line is found or end of file is reached
				istringstream login_event(line);
				string machine{ "M1" };
				int logon_type{ -1 };
				string check_user{ "USER" }; // this is the user read on this line
				int login_num; // will always be 4624, just used to absorb number
				login_event >> date >> time >> login_num >> machine >> check_user >> logon_type;
				if (date == "TimeGenerated" || machine == user) break; // header line or logout event
				if (user == check_user) {
					tm login_time = date_time_to_tm(date, time);
					LoginEvent new_event;
					new_event.time_generated = login_time;
					new_event.event_id = event_id;
					new_event.machine = machine;
					new_event.user = user;
					new_event.logon_type = logon_type;
					new_event.duration = time_difference(logout_time, login_time);
					if (new_event.duration != "DEL") {
						event_list.push_back(new_event);
						int hrs = stoi(new_event.duration.substr(0, new_event.duration.find(':')));
						if (hrs >= 24)
							cout << hrs << "-hour long event at line " << debug_line << endl;
					}
					break;
				}
			}
			master_file.seekg(initial_pos); // go back to cursor location
		}
		debug_line++;
	}
	remove_duplicates(event_list);

	ofstream output("output.csv");
	output << "Time Generated,Event ID,Machine,User,Logon Type,Duration" << endl;
	for (auto& event : event_list) {
		output << put_time(&event.time_generated, "%Y-%m-%d %H:%M:%S") << ","
			<< event.event_id << ","
			<< event.machine << ","
			<< event.user << ","
			<< event.logon_type << ","
			<< event.duration << endl;
	}
	output.close();
	master_file.close();

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "Process completed in " << (double)duration.count()/1000 << " seconds." << endl;

	return 0;
}

tm date_time_to_tm(string& date, string& time) {
	tm when;
	stringstream date_and_time;
	date_and_time << date << " " << time;
	date_and_time >> get_time(&when, "%Y-%m-%d %H:%M:%S");
	return when;
}

string time_difference(tm& logout, tm& login) {
	double seconds = difftime(mktime(&logout), mktime(&login));
	if (seconds < 0)
		return "DEL";
	int hours = seconds / 3600;
	seconds -= hours * 3600;
	int minutes = seconds / 60;
	seconds -= minutes * 60;
	string time_diff;
	if (hours < 10)
		time_diff += "0";
	time_diff += to_string(hours);
	time_diff += ":";
	if (minutes < 10)
		time_diff += "0";
	time_diff += to_string(minutes);
	time_diff += ":";
	if (seconds < 10)
		time_diff += "0";
	time_diff += to_string(seconds);
	return time_diff;
}

bool blocklist_contains(string& str) {
	if (str == "-") return true;
	for (string blocked : BLOCKLIST)
		if (str.find(blocked) != string::npos) return true;
	return false;
}

vector<LoginEvent> remove_duplicates(vector<LoginEvent>& vec) {
	vector<LoginEvent> final_vector;
	for (auto event : vec) {
		bool duplicate = false;
		for (auto& final_event : final_vector)
			if (event.user == final_event.user &&
				event.duration == final_event.duration &&
				event.event_id == final_event.event_id && 
				event.machine == final_event.machine && 
				event.logon_type == final_event.logon_type)
				duplicate = true;
		if (!duplicate)
			final_vector.push_back(event);
	}

	vec = final_vector;
	return vec;
}