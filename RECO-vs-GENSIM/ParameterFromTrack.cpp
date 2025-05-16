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
#include "Math/Point3D.h"
using namespace std;
using namespace ROOT::Math;


void ParameterFromTrack(){
    // reading root file
    TFile *f1 = new TFile("FittingParameter.root");
    TTree *t1 = (TTree*)f1->Get("MMFitPar");
    //array of ecal hit here
    vector<double> XYPar(6);
    double d,f,g,ex,ey,ez;
    t1->SetBranchAddress("d",&d);
    t1->SetBranchAddress("f",&f);
    t1->SetBranchAddress("g",&g);
    t1->SetBranchAddress("LastX",&ex);
    t1->SetBranchAddress("LastY",&ey);
    t1->SetBranchAddress("LastZ",&ez);
    t1->SetBranchAddress("a",&XYPar[0]);
    t1->SetBranchAddress("b",&XYPar[1]);
    t1->SetBranchAddress("c",&XYPar[2]);
    t1->SetBranchAddress("Phi0",&XYPar[3]);
    t1->SetBranchAddress("D0",&XYPar[4]);
    t1->SetBranchAddress("XYPar1",&XYPar[5]);
    // select eVent 2
    // Mono Track Fitting parameter
    //const double FirstEcalRaius=1.29;
    const double FirstEcalRaius=129;


    // recoresult
    vector<double> selection={52,66,152,198,270,344,280};
    vector<double> selection2={53,7,153,199,271,345,281};
    vector<string> logname={"cand_xyp0","cand_xyp1","cand_xyp2","cand_rzp0","cand_rzp1","cand_rzp2","cand_eta","cand_phi"};
    vector< vector<double> > recoresult;
    vector<double> dt1= {-0.0336128,0.428558,2.62929e+06,-4.46421,-0.0435684,-0.000179668,-0.15147,0.422273};
    vector<double> dt2= {0.0606431,-0.389629,-3.96893e+06,-6.65232,1.16287,0.000242203,0.997608,-0.371413};
    vector<double> dt3= {0.125214,0.0891485,-17155.7,4.53855,3.28147,-0.000696545,1.84211,0.07065};
    vector<double> dt4= {0.0749172,-1.11897,-3.92689e+06,-5.97318,-0.332537,-0.000467475,-0.411546,-1.12965};
    vector<double> dt5= {-0.0180147,-3.04283,-2.01154e+07,-3.14649,0.0710036,0.000893943,-0.0111024,-3.05115};
    vector<double> dt6= {-0.181414,-0.565742,192826,-3.46939,0.744661,0.000737776,0.777168,-0.599259};
    vector<double> dt7= {0.159872,-0.888408,-4.9218e+06,4.57807,-1.23996,-0.000318752,-1.03014,-0.887174};
    recoresult.push_back(dt1);
    recoresult.push_back(dt2);
    recoresult.push_back(dt3);
    recoresult.push_back(dt4);
    recoresult.push_back(dt5);
    recoresult.push_back(dt6);
    recoresult.push_back(dt7);
    
    // get number of t1 entries
    int nentries = t1->GetEntries();
    vector<double> neweta,newdr;
    for (int i=0;i<nentries;i++){
        t1->GetEntry(i);
        XYZPoint p(ex,ey,ez);

        //const double ThisZ=RZPar[0]+RZPar[1]*FirstEcalRaius+RZPar[2]*pow(FirstEcalRaius,2);
        //const double ThisEta=asinh(ThisZ / FirstEcalRaius);
        neweta.push_back(abs(p.eta()-dt1[7]));
        newdr.push_back(sqrt(pow(p.eta()-dt1[7],2)+pow(p.phi()-dt1[8],2)));
    }
    // find index that has lowest value of neweta
    int minIndex = min_element(neweta.begin(),neweta.end()) - neweta.begin();
    //cout<<"This is image number : "<<minIndex+1<<endl;
/*
    cout << "Enter image number : ";
    int image;
    cin >> image;
    
    
    cout << "Enter passing Event ID : ";
    int event;
    cin >> event;
    */
    cout << endl << endl;

    vector<int> image={54,67,154,200,271,345,282};
    for (int i=0;i<7;i++){
        cout << "ImageNumber : " << image[i] << endl;
        int event=i;
        t1->GetEntry(image[i]-1);
        const double ThisZ=d+f*FirstEcalRaius+g*pow(FirstEcalRaius,2);
        

        const double ThisEta=asinh(ThisZ / FirstEcalRaius);
        double ThisPhi=XYPar[0]- TMath::ASin((pow(FirstEcalRaius,2)-XYPar[0]*XYPar[2])/(2*FirstEcalRaius*(XYPar[2]-XYPar[0])));
        //ThisPhi=ThisPhi+XYPar[3];
        ThisPhi=ThisPhi-XYPar[3];
        while (ThisPhi < -3.14159) ThisPhi += 2*3.14159;
        while (ThisPhi > 3.14159) ThisPhi -= 2*3.14159;
        //cout << "RECO_eta  : " << recoresult[event][6]  << "\tRECO_phi  : " << recoresult[event][7] << endl;
        cout << "cand_xyp0 = " << XYPar[4] << endl;
        cout << "cand_xyp1 = " << XYPar[5] << endl;
        cout << "cand_xyp2 = " << XYPar[2] << endl;
        cout << "cand_rzp0 = " << d << endl;
        cout << "cand_rzp1 = " << f << endl;
        cout << "cand_rzp2 = " << g << endl;
        //cout << "Track_eta : " << ThisEta << "\tTrack_phi : " << ThisPhi << endl;
        XYZPoint p(ex,ey,ez);
        cout << "cand_eta  = " << p.eta() << endl;
        cout << "cand_phi  = " << p.phi() << endl;
        //double DPhi=dt1[8]-p.phi();
        //double DEta=dt1[7]-p.eta();
        double DPhi=recoresult[event][7]-ThisPhi;
        double DEta=recoresult[event][6]-ThisEta;
        while(DPhi < -3.14159) DPhi += 2*3.14159;
        while(DPhi >  3.14159) DPhi -= 2*3.14159;
        double ThisDR = sqrt(pow(DEta,2) + pow(DPhi,2));
        
        // calculate from first DR
        double eDPhi=recoresult[event][7]-p.phi();
        double eDEta=recoresult[event][6]-p.eta();
        while(eDPhi < -3.14159) eDPhi += 2*3.14159;
        while(eDPhi >  3.14159) eDPhi -= 2*3.14159;
        double eThisDR = sqrt(pow(eDEta,2) + pow(eDPhi,2));
        //cout << "DR_FTrack : " << ThisDR << "\tDR_FECAL  : " << eThisDR << endl;
        cout << "cand_phi0 = " << XYPar[3] << endl;
        cout << "cand_DR   = " << eThisDR << endl;
        double t=XYPar[0]- TMath::ASin((pow(FirstEcalRaius,2)-XYPar[0]*XYPar[2])/(2*FirstEcalRaius*(XYPar[2]-XYPar[0])));
        cout << "Term After Pji0 : " << t << endl;
        while(t < -3.14159) t += 2*3.14159;
        while(t >  3.14159) t -= 2*3.14159;
        cout << "Term After Mod : " << t << endl;
        cout << endl << endl;
    }
    
    
    // plot histogram from neweta
    TCanvas *c1 = new TCanvas("c1","c1",600,400);
    TH1F *h1 = new TH1F("h1","DR Difference",100,-0.5,0.5);
    for (int i=0;i<nentries;i++){
        h1->Fill(newdr[i]);
    }
    h1->Draw();
    c1->SaveAs("DRDifference.png");
}