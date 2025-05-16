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

void TrackEcalRematch(){
    // reading root file
    TFile *f1 = new TFile("FittingParameter.root");
    TTree *t1 = (TTree*)f1->Get("MMFitPar");
    //array of ecal hit here
    vector<double> XYPar(4);
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
    // select eVent 2
    t1->GetEntry(0);
    vector<double> RZPar;
    RZPar.push_back(d);
    RZPar.push_back(f);
    RZPar.push_back(g);
    // Mono Track Fitting parameter
    //const double FirstEcalRaius=1.29;
    const double FirstEcalRaius=1.29;


    // Ecal Eta
    double Hit_R=sqrt(pow(ex,2)+pow(ey,2));
    //double Hit_RZangle=TMath::ATan(Hit_R/ez);
    //while(Hit_RZangle < 0) Hit_RZangle += 3.14159;
    //double Hit_Eta=-log(tan(Hit_RZangle/2));
    
    // try acos
    //double Hit_RZangle=acos(ez/sqrt(pow(Hit_R,2)+pow(ez,2)));
    //double Hit_Eta=-log(tan(Hit_RZangle/2));
    //cout << "Hit_RZangle : " << Hit_RZangle << endl; 
    XYZPoint p(ex,ey,ez);
    double Hit_Eta=p.eta();


    //Ecal Phi
    double Hit_Phi=TMath::Sign( 1 , ey )*acos(ex/Hit_R);
    while(Hit_Phi < 0) Hit_Phi += 2*3.14159;
    Hit_Phi = p.phi()-XYPar[3];
    // phi range is [-pi,pi]

    // find Z,Eta,and Phi of track
    const double f_g=RZPar[1]/RZPar[2];
    //const double ThisZ=(-f_g+sqrt(pow(f_g,2)-4/RZPar[2]*(RZPar[0]-FirstEcalRaius)))/2;
    const double ThisZ=RZPar[0]+RZPar[1]*FirstEcalRaius+RZPar[2]*pow(FirstEcalRaius,2);
    //const double ThisEta=TMath::ASinH(ThisZ / FirstEcalRaius);
    const double ThisEta=asinh(ThisZ / FirstEcalRaius);
    const double ThisPhi=XYPar[0]- TMath::ASin((pow(FirstEcalRaius,2)-XYPar[0]*XYPar[2])/(2*FirstEcalRaius*(XYPar[2]-XYPar[0])));

    cout <<endl<< "Check ECAL Point" << endl;
    cout << "ECAL_X : " << ex << "\tECAL_Y : " << ey << "\tECAL_Z : " << ez << "\tEcal_R : " << sqrt(pow(ex,2)+pow(ey,2)) << endl;
    cout << "Hit_Phi : " << Hit_Phi << "\tHit_Eta : " << Hit_Eta << endl <<endl;

    cout << "Check Last Track" << endl;
    cout << "d : " << d << "\tf : " << f << "\tg : " << g << endl;
    cout << "ThisZ : " << ThisZ << "\tFirstEcalRaius : " << FirstEcalRaius << endl;
    cout << "ThisPhi : " << ThisPhi << "\tThisEta : " << ThisEta << endl;
    double DPhi=Hit_Phi-ThisPhi;
    double DEta=Hit_Eta-ThisEta;
    while(DPhi < -3.14159) DPhi += 2*3.14159;
    while(DPhi >  3.14159) DPhi -= 2*3.14159;
    double ThisDR = sqrt(pow(DEta,2) + pow(DPhi,2));
    cout << "DPhi : " << DPhi << "\t\tDEta : " << DEta << endl;
    cout << "ThisDR : " << ThisDR << endl;


/*
    // Loop to find right choice for this track
    double MinDR=999;
    int EcalHit_index=0;
    for (int i=0;i<EcalHit.size();I++){
        double DPhi=Hit_Phi-ThisPhi;
        double DEta=Hit_Eta-ThisEta;
        while(DPhi < -3.14159) DPhi += 2*3.14159;
        while(DPhi >  3.14159) DPhi -= 2*3.14159;
        double ThisDR = sqrt(pow(DEta,2) + pow(DPhi,2));
        if(ThisDR < MinDR){
            MinDR = ThisDR;
            EcalHit_index = i;
      }
    }*/
}