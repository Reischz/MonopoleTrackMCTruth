#include "Math/Point3D.h"
using namespace std;
using namespace ROOT::Math;

std::string getUniqueFilename(int counter, const std::string& baseName, const std::string& extension) {
  std::string filename = baseName + std::to_string(counter) + extension;

  if (!gSystem->AccessPathName(filename.c_str())) {
    return getUniqueFilename(++counter, baseName, extension);
  }
  return filename;
}

double convertToMeters(double value, const std::string& dimension) {
  if (dimension == "m") return value * 100;
  else if (dimension == "mm") return value * 0.1;
  else if (dimension == "um") return value * 1e-4;
  else if (dimension == "nm") return value * 1e-7;
  else if (dimension == "fm") return value * 1e-13;
  else return value;  // If the dimension is not recognized, return the value as is
}

double average(const vector<double>& v) {
  if (v.empty()) return 0;
  double acc = 0;
  for (int i = 0; i < v.size(); i++) acc += v[i];
  return acc / v.size();
}

vector<double> ParabolaFitting(vector<double> &R, vector<double> &Z) {
  TF1 *MMTKFunc = new TF1("MMTKFunc", "[0]+[1]*x+[2]*pow(x,2)", 0, 200);
  MMTKFunc->SetParameters(1, 1, 1);  // Initial parameters
  TGraph *MMFGraph = new TGraph(R.size(), &R[0], &Z[0]);
  TFitResultPtr Result = MMFGraph->Fit(MMTKFunc, "Q S B");
  vector<double> fitpar;
  fitpar.push_back(Result->Parameter(0));
  fitpar.push_back(Result->Parameter(1));
  fitpar.push_back(Result->Parameter(2));
  delete MMTKFunc;
  delete MMFGraph;
  return fitpar;
}

vector<double> CircleFitting(vector<double> &X, vector<double> &Y) {
  int NumPoints = X.size();
  TGraph XYGraph(NumPoints);
  TF1* _XYFunc = new TF1("XYFunc", "[0] + sqrt(pow([2], 2) - pow((x-[1]), 2))*TMath::Sign(1,[2]) - [2]", 0, 200);

  // calculate the angle of the first point
  XYZPoint p(X[0], Y[0], 0);
  double Phi0 = p.phi();

  // rotate the points using the angle of the first point
  for (int i = 0; i < NumPoints; i++) {
    float NewX = (X[i]) * cos(Phi0) + (Y[i]) * sin(Phi0);
    float NewY = -(X[i]) * sin(Phi0) + (Y[i]) * cos(Phi0);
    XYGraph.SetPoint(i, NewX, NewY);
  }

  // Set beginning parameters
  vector<double> Distance_original;
  for (int i = 0; i < X.size(); i++) {
    Distance_original.push_back(-TMath::Sign(1,Y[i])*sqrt(pow(X[i], 2) + pow(Y[i], 2)));
  }
  double AvePt = average(Distance_original);
  _XYFunc->SetParameters(1, 1, AvePt / 0.0114);

  // record result of the fitting
  vector<double> XYPar;
  TFitResultPtr Result = XYGraph.Fit(_XYFunc, "Q S B");
  XYPar.push_back(Result->Parameter(0));
  XYPar.push_back(Result->Parameter(1));
  XYPar.push_back(Result->Parameter(2));
  XYPar.push_back(Phi0);
  delete _XYFunc;
  return XYPar;
}

//--------------------------------------------------------------------------------***--------------------------------------------------------------------------------
// New Set of Functions
void NewFitRZPlane(vector< vector< vector<double> > > &MM, vector< vector< vector<double> > > &ATM, vector< vector<double> > &fitpar,int lumi, int event) {
  // Create Tgraph blank for Tracker using new and pointer, and filled with the data from tracker both MM and ATM
  // 0: a, 1: b, 2: c, 3: Phi0, 4: d, 5: f, 6: g, 7: ECAL_X, 8: ECAL_Y, 9: ECAL_Z
  int MMsign = 1, ATMsign = 1;
  if (fitpar[0][3]<0) MMsign=-1;
  if (fitpar[1][3]<0) ATMsign=-1;
  TGraph *MMTKGraph = new TGraph();
  for (int i = 0; i < MM[0][0].size(); i++) MMTKGraph->SetPoint(i, MM[0][2][i], MMsign*MM[0][3][i]);
  TGraph *ATMTKGraph = new TGraph();
  for (int i = 0; i < ATM[0][0].size(); i++) ATMTKGraph->SetPoint(i, ATM[0][2][i], ATMsign*ATM[0][3][i]);


  // Fitting Tracker Data, using the function z = d + fr + gr^2, notin that the function is the revert axis of above graph
  vector<double> MMF=ParabolaFitting(MM[0][3],MM[0][2]);
  vector<double> ATMF=ParabolaFitting(ATM[0][3],ATM[0][2]);
;

  // Using the fitting parameter to make new the RZ graph
  TGraph *MMTKFIT = new TGraph();
  double StepSize=129./200;
  for (int i = 0; i < 300; i++) MMTKFIT->SetPoint(i, MMF[0] + MMF[1] * i * StepSize + MMF[2] * pow(i*StepSize, 2), MMsign*i*StepSize);
  MMTKFIT->SetLineColor(kRed);
  MMTKFIT->SetLineStyle(2);
  TGraph *ATMTKFIT = new TGraph();
  for (int i = 0; i < 300; i++) ATMTKFIT->SetPoint(i, ATMF[0] + ATMF[1] * i * StepSize + ATMF[2] * pow(i*StepSize, 2), ATMsign*i*StepSize);
  ATMTKFIT->SetLineColor(kRed);
  ATMTKFIT->SetLineStyle(2);


  // Create ECAL Graph
  TGraph *MMECGraph = new TGraph();
  for (int i = 0; i < MM[1][2].size(); i++) MMECGraph->SetPoint(i, MM[1][2][i], MM[1][3][i]*MMsign);
  TGraph *ATMECGraph = new TGraph();
  for (int i = 0; i < ATM[1][2].size(); i++) ATMECGraph->SetPoint(i, ATM[1][2][i], ATM[1][3][i]*ATMsign);

  // Create HCAL Graph
  TGraph *MMHCGraph = new TGraph();
  for (int i = 0; i < MM[2][2].size(); i++) MMHCGraph->SetPoint(i, MM[2][2][i], MM[2][3][i]*MMsign);
  TGraph *ATMHCGraph = new TGraph();
  for (int i = 0; i < ATM[2][2].size(); i++) ATMHCGraph->SetPoint(i, ATM[2][2][i], ATM[2][3][i]*ATMsign);



  // Create MultiGraph to hold all the graphs
  TMultiGraph *mulg = new TMultiGraph();
  mulg->Add(MMTKGraph);
  mulg->Add(ATMTKGraph);
  mulg->Add(MMECGraph);
  mulg->Add(ATMECGraph);
  mulg->Add(MMHCGraph);
  mulg->Add(ATMHCGraph);
  mulg->SetTitle("RZGraph");
  mulg->GetXaxis()->SetTitle("Z(cm)");
  mulg->GetYaxis()->SetTitle("R(cm)");



  // Set Line Color and Style and width
  MMECGraph->SetLineColor(kOrange);
  ATMECGraph->SetLineColor(kOrange);
  MMHCGraph->SetLineColor(kBlue);
  ATMHCGraph->SetLineColor(kBlue);
  MMECGraph->SetLineWidth(3);
  ATMECGraph->SetLineWidth(3);
  MMHCGraph->SetLineWidth(3);
  ATMHCGraph->SetLineWidth(3);
  MMTKGraph->SetLineWidth(3);
  ATMTKGraph->SetLineWidth(3);



  // Create Legend describing the graph
  TLegend *leg = new TLegend(0.725, 0.455, 0.875, 0.545);
  leg->AddEntry(MMTKFIT, "Fitted", "l");
  leg->AddEntry(MMTKGraph, "Tracker", "l");
  leg->AddEntry(MMECGraph, "ECAL", "l");
  leg->AddEntry(MMHCGraph, "HCAL", "l");

  // Create Canvas To Draw the Graph
  TCanvas *c1 = new TCanvas("c1", "RZGraph;Z(cm);R(cm)", 2400, 1800);
  mulg->Draw("AL");
  // Set Origin to the center of the graph
  double xMax = mulg->GetXaxis()->GetXmax();
  double xMin = mulg->GetXaxis()->GetXmin();
  double yMax = mulg->GetYaxis()->GetXmax();
  double yMin = mulg->GetYaxis()->GetXmin();
  double maxRangeX = max(abs(xMax),abs(xMin));
  double maxRangeY = max(abs(yMax),abs(yMin));
  mulg->GetXaxis()->SetLimits(-maxRangeX, maxRangeX);
  mulg->SetMinimum(-maxRangeY);
  mulg->SetMaximum(maxRangeY);

  
  
  // Create lines to divide the graph into quadrants
  TLine *line1 = new TLine(0, -maxRangeY, 0, maxRangeY); // vertical line
  TLine *line2 = new TLine(-maxRangeX, 0, maxRangeX, 0); // horizontal line

  // Set line style to dashed
  line1->SetLineStyle(2); // 2 corresponds to dashed line style
  line2->SetLineStyle(2); // 2 corresponds to dashed line style

  // Draw the lines on the graph
  line1->Draw("same");
  line2->Draw("same");
  MMTKFIT->Draw("same");
  ATMTKFIT->Draw("same");
    // Draw specific Event
  vector<double> lumiselection={2,2,2,2,4,4,4};
  vector<double> Eventselection={27,34,77,100,36,73,41};
  
  vector<string> logname={"cand_xyp0","cand_xyp1","cand_xyp2","cand_rzp0","cand_rzp1","cand_rzp2","cand_eta","cand_phi"};
  // 0:cand_xyp0, 1:cand_xyp1, 2:cand_xyp2, 3:cand_rzp0, 4:cand_rzp1, 5:cand_rzp2, 6:cand_eta, 7:cand_phi
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
  for (int k=0;k<lumiselection.size();k++){
    if (lumi==lumiselection[k] && event==Eventselection[k]){
      TGraph *reco = new TGraph();
      for (int i = 0; i < 300; i++) reco->SetPoint(i, recoresult[k][3] + recoresult[k][4]*i*StepSize + recoresult[k][5]*pow(i*StepSize,2), TMath::Sign(1,recoresult[k][7])*i*StepSize);
      reco->SetLineColor(kGreen);
      reco->SetLineWidth(3);
      reco->Draw("same");
      leg->AddEntry(reco, "Reco", "l");
    }
  }
  leg->Draw();

  string imname;
  if (lumi == 0) {
    imname = getUniqueFilename(1, "Fitting_image/Event_", "RZPlane.png");
  }
  else {
    imname = "Fitting_image/Lumi" + to_string(lumi) + "_Event"+ to_string(event)+"_RZPlane.png";
  }
  c1->SaveAs(imname.c_str());
  delete c1;

  
  
  // Save the fitting parameters to fitpar
  for (int j = 0; j < 3; j++) {
    fitpar[0].push_back(MMF[j]);
    fitpar[1].push_back(ATMF[j]);
  }

  for (int k = 0; k < 3; k++) {
    if (MM[1][k].size()<1) fitpar[0].push_back(std::numeric_limits<double>::quiet_NaN());
    else fitpar[0].push_back(MM[1][k][0]);
    if (ATM[1][k].size()<1) fitpar[1].push_back(std::numeric_limits<double>::quiet_NaN());
    else fitpar[1].push_back(ATM[1][k][0]);
  }
}

void NewFitXYPlane(vector< vector< vector<double> > > &MM, vector< vector< vector<double> > > &ATM,vector< vector<double> > &fitpar,int lumi, int event) {
  // Create Tgraph blank for Tracker using new and pointer, and filled with the data from tracker both MM and ATM
  TGraph *MMXYGraph = new TGraph();
  for (int i = 0; i < MM[0][0].size(); i++) MMXYGraph->SetPoint(i, MM[0][0][i], MM[0][1][i]);
  TGraph *ATMXYGraph = new TGraph();
  for (int i = 0; i < ATM[0][0].size(); i++) ATMXYGraph->SetPoint(i, ATM[0][0][i], ATM[0][1][i]);

  // Fitting Tracker Data, using the function z = d + fr + gr^2, noting that the function has rotated axis of above graph
  vector<double> MMF=CircleFitting(MM[0][0],MM[0][1]);
  vector<double> ATMF=CircleFitting(ATM[0][0],ATM[0][1]);

  // Using the fitting parameter to make new the XY graph
  TGraph *MMXYFIT = new TGraph();
  double StepSize=129./200;
  for (int i = 0; i < 300; i++) {
    double x = i*StepSize;
    double y = MMF[0] + sqrt(pow(MMF[2], 2) - pow((i*StepSize - MMF[1]), 2))*TMath::Sign(1,MMF[2]) - MMF[2];
    double DerotateX = x * cos(MMF[3]) - y * sin(MMF[3]);
    double DerotateY = x * sin(MMF[3]) + y * cos(MMF[3]);
    MMXYFIT->SetPoint(i, DerotateX, DerotateY);
  }
  MMXYFIT->SetLineColor(kRed);
  MMXYFIT->SetLineStyle(2);
  TGraph *ATMXYFIT = new TGraph();
  for (int i = 0; i < 300; i++) {
    double x = i*StepSize;
    double y = ATMF[0] + sqrt(pow(ATMF[2], 2) - pow((i*StepSize - ATMF[1]), 2))*TMath::Sign(1,ATMF[2]) - ATMF[2];
    double DerotateX = x * cos(ATMF[3]) - y * sin(ATMF[3]);
    double DerotateY = x * sin(ATMF[3]) + y * cos(ATMF[3]);
    ATMXYFIT->SetPoint(i, DerotateX, DerotateY);
  }
  ATMXYFIT->SetLineColor(kRed);
  ATMXYFIT->SetLineStyle(2);

  // Create ECAL Graph
  TGraph *MMECGraph = new TGraph(MM[1][0].size(), &MM[1][0][0], &MM[1][1][0]);
  TGraph *ATMECGraph = new TGraph(ATM[1][0].size(), &ATM[1][0][0], &ATM[1][1][0]);

  // Create HCAL Graph
  TGraph *MMHCGraph = new TGraph(MM[2][0].size(), &MM[2][0][0], &MM[2][1][0]);
  TGraph *ATMHCGraph = new TGraph(ATM[2][0].size(), &ATM[2][0][0], &ATM[2][1][0]);

  // Create MultiGraph to hold all the graphs
  TMultiGraph *mulg = new TMultiGraph();
  mulg->Add(MMXYGraph);
  mulg->Add(ATMXYGraph);
  mulg->Add(MMECGraph);
  mulg->Add(ATMECGraph);
  mulg->Add(MMHCGraph);
  mulg->Add(ATMHCGraph);
  mulg->SetTitle("XYGraph");
  mulg->GetXaxis()->SetTitle("X(cm)");
  mulg->GetYaxis()->SetTitle("Y(cm)");

  // Set Line Color and Style and width
  MMECGraph->SetLineColor(kOrange);
  ATMECGraph->SetLineColor(kOrange);
  MMHCGraph->SetLineColor(kBlue);
  ATMHCGraph->SetLineColor(kBlue);
  MMECGraph->SetLineWidth(3);
  ATMECGraph->SetLineWidth(3);
  MMHCGraph->SetLineWidth(3);
  ATMHCGraph->SetLineWidth(3);
  MMXYGraph->SetLineWidth(3);
  ATMXYGraph->SetLineWidth(3);

  // Create Legend describing the graph
  TLegend *leg = new TLegend(0.725, 0.455, 0.875, 0.545);
  leg->AddEntry(MMXYFIT, "Fitted", "l");
  leg->AddEntry(MMXYGraph, "Tracker", "l");
  leg->AddEntry(MMECGraph, "ECAL", "l");
  leg->AddEntry(MMHCGraph, "HCAL", "l");

  // Create Canvas To Draw the Graph
  TCanvas *c1 = new TCanvas("c1", "XYGraph;X(cm);Y(cm)", 2400, 1800);
  mulg->Draw("AL");
  // Set Origin to the center of the graph
  double xMax = mulg->GetXaxis()->GetXmax();
  double xMin = mulg->GetXaxis()->GetXmin();
  double yMax = mulg->GetYaxis()->GetXmax();
  double yMin = mulg->GetYaxis()->GetXmin();
  double maxRangeX = max(abs(xMax),abs(xMin));
  double maxRangeY = max(abs(yMax),abs(yMin));
  mulg->GetXaxis()->SetLimits(-maxRangeX, maxRangeX);
  mulg->SetMinimum(-maxRangeY);
  mulg->SetMaximum(maxRangeY);
  
  // Create lines to divide the graph into quadrants
  TLine *line1 = new TLine(0, -maxRangeY, 0, maxRangeY); // vertical line
  TLine *line2 = new TLine(-maxRangeX, 0, maxRangeX, 0); // horizontal line

  // Set line style to dashed
  line1->SetLineStyle(2); // 2 corresponds to dashed line style
  line2->SetLineStyle(2); // 2 corresponds to dashed line style

  // Draw the lines on the graph
  line1->Draw("same");
  line2->Draw("same");
  MMXYFIT->Draw("same");
  ATMXYFIT->Draw("same");

  leg->Draw();
  string imname;
  if (lumi == 0) {
    imname = getUniqueFilename(1, "Fitting_image/Event_", "XYPlane.png");
  }
  else {
    imname = "Fitting_image/Lumi" + to_string(lumi) + "_Event"+ to_string(event)+"_XYPlane.png";
  }
  c1->SaveAs(imname.c_str());
  delete c1;

  // Save the fitting parameters to fitpar
  for (int j = 0; j < 4; j++) {
    fitpar[0].push_back(MMF[j]);
    fitpar[1].push_back(ATMF[j]);
  }
}