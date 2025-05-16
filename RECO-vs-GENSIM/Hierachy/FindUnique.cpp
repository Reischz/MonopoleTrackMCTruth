#include <vector>
#include <unordered_set>
#include <iostream>
#include "NewDeffunction.h"
using namespace std;

vector<string> uniqueVector(vector<string> &v) {
  
  std::unordered_set<string> s;
  std::vector<string> result;

    for (const auto &i : v) {
        string ele=i;
        string filteredEle;
        for (int j = 0; j < ele.length(); j++) { // Use length() instead of Length()
            // not include digits and underscore
            // if (!isdigit(ele[j]) && ele[j] != '_') { // Use j instead of i
            //     filteredEle += ele[j]; // Use j instead of i
            // }
            // all characters
            filteredEle += ele[j];
        }
        if (s.insert(filteredEle).second) {
            // cout << "Unique Detector: " << filteredEle << "\n";
            result.push_back(filteredEle);
        }
    }
  return result;
}

void UpDateUniqueVector(vector<string> &SelectedDeTc, vector<string> &UMMDeTc) {
    int previousindex=-1;
    for (int i=0;i<UMMDeTc.size();i++) {
        bool isExist = false;
        for (int j=0;j<SelectedDeTc.size();j++) {
            if (UMMDeTc[i] == SelectedDeTc[j]) {
                previousindex=j;
                isExist = true;
                break;
            }
        }
        if (!isExist) {
            SelectedDeTc.insert(SelectedDeTc.begin() + previousindex+1, UMMDeTc[i]);
            cout << "Inserted: " << UMMDeTc[i] << " at position : " << previousindex+1 << endl;
            previousindex++;
        }
    }
}


void ReadTrackData(string line, vector<double> &PP,string &DetectorType, vector< vector< vector<double> > > &TrackData, vector<string> &Detector){
    stringstream ss;
    int col;
    double x, y, z,r, eneg;
    string xDim, yDim, zDim, DeTc, tmdpar,fcol;
    ss << line;    
    // parse string stream to variable respect to column
    // 1 -1.14563 mm  -3.35303 mm  -0.0106581 fm    1.17549 TeV 2.16298e-06 eV   1.92509 cm    1.92509 cm   -116.541 ps BeamVacuum1 MonopoleTransportation
    ss >> fcol >> x >> xDim >> y >> yDim >> z >> zDim >> eneg >> tmdpar >> tmdpar >> tmdpar >> tmdpar >> tmdpar >> tmdpar >> tmdpar >> tmdpar >> tmdpar >> DeTc;
    ss.str(string()); // emptying ss, no need
    // verify detector
    
    Detector.push_back(DeTc);
}

void FindUnique(const char* userArg1) {
  cout << "Log name: " << userArg1 << endl;
  // Check if the file exists
  if (gSystem->AccessPathName(userArg1)) {
      cout << "File does not exist" << endl;
      exit(0);
  } else {
      cout << "File exists" << endl;
  }
  string lumistring=(userArg1);
  int LUMI;
  try{
      lumistring=lumistring.substr(16, 5);
      lumistring.erase(std::remove_if(lumistring.begin(), lumistring.end(), [](char c) { return !std::isdigit(c); }), lumistring.end());
      LUMI=stoi(lumistring);
  }
  catch (...) {
      LUMI=0;
  }

  ifstream inFile(userArg1);
  int EventNumber = 0;
  string ParticleType="NAN";
  string DetectorType="NAN"; // 001: Tracker, 010: ECAL, 100: HCAL
  vector< vector< vector<double> > > MonopoleTrackData(3); // 0: Tracker, 1: ECAL, 2: HCAL
  vector< vector< vector<double> > > AntiMonopoleTrackData(3); // 0: Tracker, 1: ECAL, 2: HCAL
  int ToData = 0;
  string line, PreviousMMDeTc, PreviousAMMDeTc;
  vector<string> MMDeTc, AMMDeTc;
  vector<double> PreviousPoint = {0,0,0};

  bool EndLastEvent = false;

  //-------------------------------------***-------------------------------------
  vector<string> SelectedDeTc;
  // read log file ,if it exists put its content to the vector
    ifstream logFile("UniqueDeTc.log");
    if (logFile.is_open()) {
        cout << "Log file exists" << endl;
        string line;
        while (getline(logFile, line)) {
            SelectedDeTc.push_back(line);
            cout << line << " : Passed" << endl;
        }
    }
    logFile.close();

  for (int i=0;i<3;i++){
      MonopoleTrackData[i].resize(4); // 0: x, 1: y, 2: z, 3: r
      AntiMonopoleTrackData[i].resize(4); // 0: x, 1: y, 2: z, 3: r
  }
  while (getline(inFile, line)) {
      //-------------------------------------***-------------------------------------
      // count and check if event begined?
      if (((line.find("Begin") != string::npos) && (line.find("Event") != string::npos)) || (EndLastEvent) ) {
          //-------------------------------------***-------------------------------------
          // analyze previous track
          cout << "\nBegin Event: " << EventNumber << "\n";
          if (EventNumber > 0 ) {
              // create unique paturns
                vector<string> UMMDeTc = uniqueVector(MMDeTc);
                vector<string> UAMMDeTc = uniqueVector(AMMDeTc);
                // update new vector to existing unique vector
                cout << "Began testing New function" << "\n";
                // show the unique detector
                cout << "Size of SelectedDeTc : " << SelectedDeTc.size() << "\n";
                UpDateUniqueVector(SelectedDeTc, UMMDeTc);
                UpDateUniqueVector(SelectedDeTc, UAMMDeTc);
                // int MMSize = UMMDeTc.size();
                // int AMMSize = UAMMDeTc.size();
                // int SelectedSize = SelectedDeTc.size();
                // if (MMSize > SelectedSize | AMMSize > SelectedSize ) {
                //     if (MMSize > AMMSize) {
                //         SelectedDeTc = UMMDeTc;
                //         cout << "Set Most Number of Dectector to "<< EventNumber << " Event's Monopole" << "\n";
                //     }
                //     else {
                //         SelectedDeTc = UAMMDeTc;
                //         cout << "Set Most Number of Dectector to "<< EventNumber << " Event's AntiMonopole" << "\n";
                //     }
                // }
                cout << "-------------------------------------End Event " << EventNumber << " -------------------------------------\n";
          }
          
        // if (EventNumber == 20) break;

          if (EndLastEvent) {
              cout << "-------------------------------------The file's Last Event has Ended-------------------------------------" << "\n\n";                 
              cout << "Continue to Wrtite the information" << endl << endl;
              break;
          }
          EventNumber++;
          //-------------------------------------***-------------------------------------
          // clear variable values
          EndLastEvent = false;
          ParticleType = "NAN";
          ToData=0;
          PreviousPoint = {0,0,0};
          MMDeTc.clear();
          AMMDeTc.clear();
          for (int i=0;i<3;i++){
              MonopoleTrackData[i].clear();
              MonopoleTrackData[i].resize(4); // 0: x, 1: y, 2: z, 3: r
              AntiMonopoleTrackData[i].clear();
              AntiMonopoleTrackData[i].resize(4); // 0: x, 1: y, 2: z, 3: r
          }
      } 
      else {
          //-------------------------------------***-------------------------------------
          // check particle type
          if (line.find("Particle = Monopole") != string::npos) {ParticleType = "Monopole"; DetectorType = "001";}
          else if (line.find("Particle = AntiMono") != string::npos) {ParticleType = "AntiMono"; DetectorType = "001";}
          if (ParticleType == "Monopole") {
              //-------------------------------------***-------------------------------------
              // check if the track begin  
              if (ToData>=3) {
                  if (line.empty()){ToData=0; ParticleType = "NAN"; PreviousPoint = {0,0,0};}
                  else ReadTrackData(line, PreviousPoint, DetectorType, MonopoleTrackData, MMDeTc);
              }
          }
          else if (ParticleType == "AntiMono") {
              //-------------------------------------***-------------------------------------
              // check if the track begin
              if (ToData>=3) {
                  if (line.empty()){ToData=0; ParticleType = "NAN"; EndLastEvent = true; PreviousPoint = {0,0,0};}
                  else ReadTrackData(line, PreviousPoint, DetectorType, AntiMonopoleTrackData, AMMDeTc);
              }
          }
          if (ParticleType != "NAN") ToData++;
      }
  }

  //-------------------------------------***-------------------------------------
    // write the information to the log file
    ofstream outFile("UniqueDeTc.log");
    if (!outFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    else {
        for (int i=0;i<SelectedDeTc.size();i++) outFile << SelectedDeTc[i] << endl;
        outFile.close();
    }
  cout << "-------------------------------------***-------------------------------------\n";
  cout << "Completed Saving Ordor to Log File" << endl;
  cout << "-------------------------------------***-------------------------------------\n";
  exit(0);
}
