#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "TLorentzVector.h"
#include "TLegend.h"
#include <TApplication.h>
#include "TStyle.h"

std::vector<double> plot(std::string name) {
    int mass = 0;
    std::string line;
    std::vector<double> Pt;        
    std::ifstream myfile;
    myfile.open (name);
    if (myfile.is_open()) {
        std::string line;
        bool startw = false;
        while (std::getline(myfile, line)) {
            if (line.find("<event>") != std::string::npos) {
                startw = true;
            }
            std::vector<std::string> splitted;
            std::string val;
            std::stringstream ssin(line);
            bool mm = false;
            while (ssin >> val) {
                if (val == "-4110000" || val == "4110000") {
                    mm = true;
                }
                if (mm && startw) {
                    splitted.push_back(val);
                }
            }
            if (!splitted.empty()) {
                TLorentzVector v;
                v.SetPxPyPzE(std::stod(splitted[6]),std::stod(splitted[7]),std::stod(splitted[8]),std::stod(splitted[9]));
                Pt.push_back(v.Perp());
            }
        }
        myfile.close();
    } 
    return Pt;
}

void threemass_pT() {
    TH1F *h1 = new TH1F("h1","pT;;Events", 100, 0, 2500);
    std::vector<double> Pt1000 = plot("LUX1000.lhe");
    for (double val : Pt1000) {
        h1->Fill(val);
    }
    h1 ->SetStats(0);
    
    TH1F *h2 = new TH1F("h2","pT;;Events", 100, 0, 2500);
    std::vector<double> Pt1500 = plot("LUX1500.lhe");
    for (double val : Pt1500) {
        h2->Fill(val);
    }
    h2 ->SetStats(0);

    TH1F *h3 = new TH1F("h3","pT;;Events", 100, 0, 2500);
    std::vector<double> Pt2000 = plot("LUX2000.lhe");
    for (double val : Pt2000) {
        h3->Fill(val);
    }
    h3 ->SetStats(0);
    
    TH1F *h4 = new TH1F("h4","pT;;Events", 100, 0, 2500);
    std::vector<double> Ptd1000 = plot("NN1000.lhe");
    for (double val : Ptd1000) {
        h4->Fill(val);
    }
    h4 ->SetStats(0);
    
    TH1F *h5 = new TH1F("h5","pT;;Events", 100, 0, 2500);
    std::vector<double> Ptd1500 = plot("NN1500.lhe");
    for (double val : Ptd1500) {
        h5->Fill(val);
    }
    h5 ->SetStats(0);

    TH1F *h6 = new TH1F("h6","pT;;Events", 100, 0, 2500);
    std::vector<double> Ptd2000 = plot("NN2000.lhe");
    for (double val : Ptd2000) {
        h6->Fill(val);
    }
    h6 ->SetStats(0);

    TLegend *leg = new TLegend(0.65, 0.65, 0.85, 0.85);

    leg->AddEntry(h1,"id82000_LUX1000GeV","l");  
    leg->AddEntry(h2,"id82000_LUX1500GeV","l"); 
    leg->AddEntry(h3,"id82000_LUX2000GeV","l"); 
    leg->AddEntry(h4,"id324900_NN1000GeV","l");  
    leg->AddEntry(h5,"id324900_NN1500GeV","l"); 
    leg->AddEntry(h6,"id324900_NN2000GeV","l");

    TCanvas *c1 = new TCanvas("c1", "Canvas Title", 800, 600);
    
    // Set the color of hist
    h1->SetLineColor(kBlue); 
	h2->SetLineColor(kRed); 
	h3->SetLineColor(kGreen);
	h4->SetLineColor(kBlue); 
	h5->SetLineColor(kRed); 
	h6->SetLineColor(kGreen);
	h4->SetLineStyle(2);
	h5->SetLineStyle(2);
	h6->SetLineStyle(2);
	h4->SetLineWidth(2);
	h5->SetLineWidth(2);
	h6->SetLineWidth(2);
	
    h1->Draw();
    h2->Draw("SAME");
    h3->Draw("SAME");
    h4->Draw("SAME");
    h5->Draw("SAME");
    h6->Draw("SAME");
    leg->Draw();   
    c1->SaveAs("PT.png"); 
}
