#include <fstream>
#include <TSystem.h>
#include <cstdlib> // Required for system()
#include <sstream>
#include "TFile.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"
#include "NewDeffunction.h"
#include "Math/Point3D.h"
using namespace std;
using namespace ROOT::Math;


void SaveTTreeToRoot(TTree* info_tree, const std::string& filename) {
    if (gSystem->AccessPathName(filename.c_str())) {
        // Create a new file
        TFile* file = new TFile(filename.c_str(), "RECREATE");
        // If the file does not exist, write the given TTree into the file
        info_tree->Write();
        file->Close();
    } else {
        TFile* file = new TFile(filename.c_str(), "UPDATE");
        // If the file exists, get the TTree and fill it with new data
        std::string treename = info_tree->GetName();
        TTree* tree = dynamic_cast<TTree*>(file->Get(treename.c_str()));
        if (!tree) {
            // Handle the case where the TTree does not exist in the file
            info_tree->Write("", TObject::kOverwrite);
        } else {
            // Assuming the TTree structure matches, proceed with filling data
            // Get the list of branches in the TTree
            TObjArray *brlist = tree->GetListOfBranches();
            // Get the number of branches in the TTree        
            int colsize=brlist->GetEntries();
            double value[colsize];
            TString* strbranch = new TString();
            // assign address to ttree branch
            for (int i=0; i<colsize; i++) {
                if (std::string(brlist->At(i)->GetName()) == "LastDetector") {
                    tree->SetBranchAddress(brlist->At(i)->GetName(), &strbranch);
                } else {
                    // Otherwise, assign the address of the double
                    tree->SetBranchAddress(brlist->At(i)->GetName(), &value[i]);
                }
            }
            for (int i=0; i<colsize; i++) {
                if (std::string(brlist->At(i)->GetName()) == "LastDetector") {
                    // If the branch is a TBranchElement (which can hold a TString), assign the address of the TString
                    info_tree->SetBranchAddress(brlist->At(i)->GetName(), &strbranch);
                } else {
                    // Otherwise, assign the address of the double
                    info_tree->SetBranchAddress(brlist->At(i)->GetName(), &value[i]);
                }
            }
            cout << "Pass assign address to old tree and updated tree" << endl;
            // filled all leaf in info_tree to tree
            for (int j=0;j<info_tree->GetEntries();j++){
                info_tree->GetEntry(j);
                tree->Fill();
            }
            cout << "Pass filling to old tree" << endl;
            // Write the updated tree into the file
            tree->Write("", TObject::kOverwrite);
            cout << "Successfully updated TTree in " << filename << endl;
        }
        file->Close();
    }
}

void ReadTrackData(string line, vector<double> &PP,string &DetectorType, vector< vector< vector<double> > > &TrackData, string &Detector){
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
    if (DeTc[0]=='E') DetectorType="010";
    else if (DeTc[0]=='H') DetectorType="100";
    
    x = convertToMeters(x, xDim);
    y = convertToMeters(y, yDim);
    z = convertToMeters(z, zDim);
    r = sqrt(pow(x, 2) + pow(y, 2));
    double *coordpar[]={&x,&y,&z,&r};
    
    double Rnow = sqrt(pow(x-PP[0], 2) + pow(y-PP[1], 2)+ pow(z-PP[2],2));
    double incrmnt = 129. / 1000;
    if (Rnow >= incrmnt) {
        PP = {x,y,z};
        if (DetectorType=="001"){
            for (int i=0;i<4;i++) TrackData[0][i].push_back(*coordpar[i]);
        }
        else if (DetectorType=="010"){
            for (int i=0;i<4;i++) TrackData[1][i].push_back(*coordpar[i]);
        }
        else if (DetectorType=="100"){
            for (int i=0;i<4;i++) TrackData[2][i].push_back(*coordpar[i]);
        }   
    } 
    Detector = DeTc;
}

void SaveToTTree(TTree *Tree, vector< vector<double> > &CallectPara, const string condition, string PreviousMMDeTc, string PreviousAMMDeTc){
    // Check if the tree has a branch named "branchName"
    
    vector<string> Bname={"cand_xyp0(d0)","cand_xyp1","cand_xyp2(c)","cand_rzp0(d)","cand_rzp1(f)","cand_rzp2(g)","cand_eta","cand_phi","a","b","Phi0","ECAL_X","ECAL_Y","ECAL_Z","LastDetector"};
    // 0: cand_xyp0(d0), 1: cand_xyp1, 2: cand_xyp2(c), 3: cand_rzp0(d), 4: cand_rzp1(f), 5: cand_rzp2(g), 6: cand_eta, 7: cand_phi, 8: a, 9: b, 10: Phi0, 11: ECAL_X, 12: ECAL_Y, 13: ECAL_Z, 14: LastDetector
    int index;
    vector<double> Value(Bname.size()-1);

    TString* DeTc = new TString();
    if (condition=="Monopole") {
        index=0; 
        *DeTc = PreviousMMDeTc.c_str();
        if (Tree->GetBranch(Bname[14].c_str())) Tree->SetBranchAddress(Bname[14].c_str(), &DeTc);
        else Tree->Branch(Bname[14].c_str(), &DeTc);
    }
    else if (condition=="AntiMonopole") {
        index=1;
        *DeTc = PreviousAMMDeTc.c_str();
        if (Tree->GetBranch(Bname[14].c_str())) Tree->SetBranchAddress(Bname[14].c_str(), &DeTc);
        else Tree->Branch(Bname[14].c_str(), &DeTc);
    }
    else {
        SaveToTTree(Tree, CallectPara, "Monopole" , PreviousMMDeTc, PreviousAMMDeTc);
        SaveToTTree(Tree, CallectPara, "AntiMonopole" , PreviousMMDeTc, PreviousAMMDeTc);
        return;
    }
    
    for (int i=0;i<Bname.size()-1;i++) {
        if (Tree->GetBranch(Bname[i].c_str())) Tree->SetBranchAddress(Bname[i].c_str(), &Value[i]);
        else Tree->Branch(Bname[i].c_str(), &Value[i]);
    }
    Value[0]=sqrt(pow(CallectPara[index][0]-CallectPara[index][2],2)+pow(CallectPara[index][1],2))-abs(CallectPara[index][2]);
    Value[1]=abs(CallectPara[index][3]-atan(CallectPara[index][1]/(CallectPara[index][2]-CallectPara[index][0])));
    Value[2]=CallectPara[index][2];
    for (int i=3;i<6;i++) Value[i]=CallectPara[index][i+1];
    // 0: a, 1: b, 2: c, 3: Phi0, 4: d, 5: f, 6: g, 7: ECAL_X, 8: ECAL_Y, 9: ECAL_Z
    XYZPoint ECALPoint(CallectPara[index][7],CallectPara[index][8],CallectPara[index][9]);
    Value[6]=ECALPoint.Eta();
    Value[7]=ECALPoint.Phi();
    Value[8]=CallectPara[index][0];
    Value[9]=CallectPara[index][1];
    Value[10]=CallectPara[index][3];
    for (int j=11;j<14;j++) Value[j]=CallectPara[index][j-4];   
    Tree->Fill();
}

void FindFittingPar(const char* userArg1) {
    //ROOT::EnableImplicitMT();
    //gErrorIgnoreLevel = kInfo+1;
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
    vector<double> PreviousPoint = {0,0,0};

    bool EndLastEvent = false;

    //-------------------------------------***-------------------------------------
    // Create a TTree
    TTree *MonopoleTree = new TTree("MonopoleTree", "Contains Monopole Fitting Parameters");
    TTree *AntiMonopoleTree = new TTree("AntiMonopoleTree", "Contains AntiMonopole Fitting Parameters");
    TTree *CombinedTree = new TTree("CombinedTree", "Contains both Monopole and AntiMonopole Fitting Parameters");
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
                // plot xy and rz plane
                cout << "MMTrack: " << MonopoleTrackData[0][0].size() << "\tAMMTrack: " << AntiMonopoleTrackData[0][0].size() << endl;
                cout << "Tracker Point : " << MonopoleTrackData[0][0].size()+AntiMonopoleTrackData[0][0].size() << endl;
                cout << "ECAL Point: " << MonopoleTrackData[1][0].size()+AntiMonopoleTrackData[1][0].size() << endl;
                cout << "HCAL Point: " << MonopoleTrackData[2][0].size()+AntiMonopoleTrackData[2][0].size() << endl;
                vector< vector<double> > CallectPara(2); // 0: Monopole, 1: AntiMonopole
                // 0: a, 1: b, 2: c, 3: Phi0, 4: d, 5: f, 6: g, 7: ECAL_X, 8: ECAL_Y, 9: ECAL_Z, 10: LastDetector
                NewFitXYPlane(MonopoleTrackData, AntiMonopoleTrackData, CallectPara, LUMI, EventNumber);
                cout << "Completed XY Plane" << endl;
                NewFitRZPlane(MonopoleTrackData, AntiMonopoleTrackData, CallectPara, LUMI, EventNumber);
                cout << "Completed RZ Plane" << endl;

                //-------------------------------------***-------------------------------------
                // save fitting parameters to TTree
                SaveToTTree(MonopoleTree, CallectPara, "Monopole", PreviousMMDeTc , PreviousAMMDeTc);
                SaveToTTree(AntiMonopoleTree, CallectPara, "AntiMonopole", PreviousMMDeTc , PreviousAMMDeTc);
                SaveToTTree(CombinedTree, CallectPara, "Both", PreviousMMDeTc , PreviousAMMDeTc);  

                cout << "Completed Saving Parameter to TTree" << endl;   
                cout << "-------------------------------------" << "End Event: " << EventNumber <<"-------------------------------------" << "\n\n";                 
                ///if (EventNumber>2) break;
            }
            
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
                    else ReadTrackData(line, PreviousPoint, DetectorType, MonopoleTrackData, PreviousMMDeTc);
                }
            }
            else if (ParticleType == "AntiMono") {
                //-------------------------------------***-------------------------------------
                // check if the track begin
                if (ToData>=3) {
                    if (line.empty()){ToData=0; ParticleType = "NAN"; EndLastEvent = true; PreviousPoint = {0,0,0};}
                    else ReadTrackData(line, PreviousPoint, DetectorType, AntiMonopoleTrackData, PreviousAMMDeTc);
                }
            }
            if (ParticleType != "NAN") ToData++;
        }
    }

    //-------------------------------------***-------------------------------------
    // Save TTree to root file
    SaveTTreeToRoot(MonopoleTree, "ParameterFromFitting.root");
    SaveTTreeToRoot(AntiMonopoleTree, "ParameterFromFitting.root");
    SaveTTreeToRoot(CombinedTree, "ParameterFromFitting.root");
    cout << "-------------------------------------***-------------------------------------\n";
    cout << "Completed Saving TTree to Root File" << endl;
    cout << "-------------------------------------***-------------------------------------\n";
    exit(0);
}

