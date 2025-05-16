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

void FitPara(const vector<double>& Z, const vector<double>& R, vector<double>& RZPar) {
  TGraph RZGraph(Z.size(), &Z[0], &R[0]);
  TF1* _RZFunc = new TF1("RZFunc", "[0]+[1]*x+[2]*pow(x,2)", 0, 200);
  _RZFunc->SetParameters(1, 1, 1);  // Initial parameters
  TFitResultPtr Result = RZGraph.Fit(_RZFunc, "Q S B");
  RZPar.push_back(Result->Parameter(0));
  RZPar.push_back(Result->Parameter(1));
  RZPar.push_back(Result->Parameter(2));
  TCanvas* c2 = new TCanvas("c2", "RZGraph;R(cm);Z(cm)", 1200, 900);

  // Set the title for the graph
  RZGraph.SetTitle("RZGraph");

  // Set the title for the x-axis and y-axis
  RZGraph.GetXaxis()->SetTitle("Z(cm)");
  RZGraph.GetYaxis()->SetTitle("R(cm)");

  RZGraph.Draw("AL*");
  string imname = getUniqueFilename(1, "Fitting_image/Event_", "RZfit.png");
  c2->SaveAs(imname.c_str());
}

void FitParaWTError(const vector<double>& Z, const vector<double>& R, vector<double>& RZPar,vector<double>& error) {
  TGraph RZGraph(Z.size(), &Z[0], &R[0]);
  TF1* _RZFunc = new TF1("RZFunc", "[0]+[1]*x+[2]*pow(x,2)", 0, 200);
  _RZFunc->SetParameters(1, 1, 1);  // Initial parameters
  TFitResultPtr Result = RZGraph.Fit(_RZFunc, "Q S B");

  for (int j=0;j<3;j++){
    RZPar.push_back(Result->Parameter(j));
    error.push_back(_RZFunc->GetParError(j));
  }

  TCanvas* c2 = new TCanvas("c2", "RZGraph;R(cm);Z(cm)", 1200, 900);

  // Set the title for the graph
  RZGraph.SetTitle("RZGraph");

  // Set the title for the x-axis and y-axis
  RZGraph.GetXaxis()->SetTitle("Z(cm)");
  RZGraph.GetYaxis()->SetTitle("R(cm)");

  RZGraph.Draw("AL*");
  string imname = getUniqueFilename(1, "Fitting_image/Event_", "RZfit.png");
  c2->SaveAs(imname.c_str());
}

void FitParaWTErrorALL(const vector< vector<double> >& tracker, const vector< vector<double> >& ecal,const vector< vector<double> >& hcal,vector<double>& RZPar,vector<double>& error, const vector<double>& XYPar, vector< string>& filen) {
  const vector<double> &Z=tracker[2],&R=tracker[3];
  TGraph RZGraph(Z.size(), &R[0], &Z[0]);
  TF1* _RZFunc = new TF1("RZFunc", "[0]+[1]*x+[2]*pow(x,2)", 0, 200);
  _RZFunc->SetParameters(1, 1, 1);  // Initial parameters
  TFitResultPtr Result = RZGraph.Fit(_RZFunc, "Q S B");


  for (int j=0;j<3;j++){
    RZPar.push_back(Result->Parameter(j));
    error.push_back(_RZFunc->GetParError(j));
  }

  // Recreate New Plot with given parameter
  double NewMinR = *min_element(R.begin(), R.end());
  double NewMaxR = *max_element(R.begin(), R.end());
  double StepSize = (NewMaxR - NewMinR) / 200;
  vector<double> NewR, NewZ;
  for (int i=0;i<300;i++){
    NewR.push_back(NewMinR + i*StepSize);
    NewZ.push_back(RZPar[0]+RZPar[1]*NewR[i]+RZPar[2]*pow(NewR[i],2));
  }
  
  TGraph NRZGraph(Z.size(), &Z[0], &R[0]);
  NRZGraph.SetLineWidth(3);


  TGraph NewRZGraph(NewR.size(), &NewZ[0], &NewR[0]);
  NewRZGraph.SetLineColor(kRed);
  //NewRZGraph.SetLineWidth(3);
  NewRZGraph.SetLineStyle(2);  //set to dashed line
 

  TMultiGraph *mulg = new TMultiGraph();
  mulg->Add(&NRZGraph,"AL");
  TCanvas* c2 = new TCanvas("c2", "RZGraph;R(cm);Z(cm)", 1200, 900);
  
  TLegend *leg = new TLegend(0.55, 0.15, 0.875, 0.30);
  leg->AddEntry(&NewRZGraph,"Fitted","l");
  leg->AddEntry(&NRZGraph,"Tracker","l");
  if (ecal[0].size()>0){
    TGraph *ec = new TGraph(ecal[0].size(), &ecal[2][0], &ecal[3][0]);
    ec->SetLineColor(kOrange);
    ec->SetLineWidth(2);
    //ec->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(ec,"ECAL","l");
    mulg->Add(ec);
  }
  if (hcal[0].size()>0){
    TGraph *hc = new TGraph(hcal[0].size(), &hcal[2][0], &hcal[3][0]);
    hc->SetLineColor(kBlue);
    hc->SetLineWidth(2);
    //hc->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(hc,"HCAL","l");
    mulg->Add(hc);
  }

  // recoresult
  vector<int> selection1={54,67,154,200,271,345,282};
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
  // if dr from fitting parameter is less than 0.1, then it is a good fit
  double FirstEcalRaius=129;
  string imname = getUniqueFilename(1, "Fitting_image/Event_", "RZfit.png");
  vector<double> recor,rocez;
  for (int i=0;i<7;i++){
    // create vector of string contain selection[i] from [i-2,i+2]
    vector<string> namestr;
    for (int k=-3;k<4;k++){
      namestr.push_back("Fitting_image/Event_"+to_string(selection1[i]+k)+"RZfit.png");
    }

    // if imname is in the namestr, then add the reco graph
    if (find(namestr.begin(),namestr.end(),imname)!=namestr.end()){
      filen.push_back(imname);
      for (int j=0;j<300;j++){
        recor.push_back(j*StepSize);
        rocez.push_back(recoresult[i][3]+recoresult[i][4]*recor[j]+recoresult[i][5]*pow(recor[j],2));
      }
      TGraph *reco = new TGraph(recor.size(), &rocez[0], &recor[0]);
      reco->SetLineColor(kGreen);
      reco->SetLineWidth(2);
      leg->AddEntry(reco,"Reco","l");
      mulg->Add(reco);
      break;
    }
  }
  // Set the title for the graph
  mulg->SetTitle("RZGraph");
  // Set the title for the x-axis and y-axis
  mulg->GetXaxis()->SetTitle("Z(cm)");
  mulg->GetYaxis()->SetTitle("R(cm)");
  mulg->Draw("A");
  NewRZGraph.Draw("SAME");
  leg->Draw();
  
  c2->SaveAs(imname.c_str());
  delete c2;
}

double average(const vector<double>& v) {
  if (v.empty()) {
    return 0;
  }
  double acc = 0;
  for (int i = 0; i < v.size(); i++) {
    acc += v[i];
  }
  return acc / v.size();
}


void FitCircleALL(const vector< vector<double> >& tracker, const vector< vector<double> >& ecal,const vector< vector<double> >& hcal, vector<double>& XYPar) {
  const vector<double> &X=tracker[0],&Y=tracker[1];
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

  // Set beginnig parameters
  vector<double> Distance_original;
  for (int i = 0; i < X.size(); i++) {
    Distance_original.push_back(-TMath::Sign(1,Y[i])*sqrt(pow(X[i], 2) + pow(Y[i], 2)));
  }
  double AvePt = average(Distance_original);
  _XYFunc->SetParameters(1, 1, AvePt / 0.0114);

  // record result of the fitting
  TFitResultPtr Result = XYGraph.Fit(_XYFunc, "Q S B");
  XYPar.push_back(Result->Parameter(0));
  XYPar.push_back(Result->Parameter(1));
  XYPar.push_back(Result->Parameter(2));
  XYPar.push_back(Phi0);

  // Make multi graph to hold all the graphs
  TMultiGraph *mulg = new TMultiGraph();
  TCanvas* c2 = new TCanvas("c2", "XYGraph;R(cm);Z(cm)", 1200, 900);
  TLegend *leg = new TLegend(0.55, 0.15, 0.875, 0.30);
  
  // make new graph holding traker data
  TGraph NXYGraph(X.size(), &X[0], &Y[0]);
  mulg->Add(&NXYGraph,"AL");
  NXYGraph.SetLineWidth(3);
  leg->AddEntry(&NXYGraph,"Tracker","l");

  if (ecal[0].size()>0){
    TGraph *ec = new TGraph(ecal[0].size(), &ecal[0][0], &ecal[1][0]);
    ec->SetLineColor(kOrange);
    ec->SetLineWidth(2);
    //ec->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(ec,"ECAL","l");
    mulg->Add(ec);
  }
  if (hcal[0].size()>0){
    TGraph *hc = new TGraph(hcal[0].size(), &hcal[0][0], &hcal[1][0]);
    hc->SetLineColor(kBlue);
    hc->SetLineWidth(2);
    //hc->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(hc,"HCAL","l");
    mulg->Add(hc);
  }
  // Set the title for the graph
  mulg->SetTitle("XYGraph");
  // Set the title for the x-axis and y-axis
  mulg->GetXaxis()->SetTitle("Z(cm)");
  mulg->GetYaxis()->SetTitle("R(cm)");
  mulg->Draw("A");
  

  // make new graph from plotting parameters and derotate the graph
  double NewMinR = 0;
  double NewMaxR = 10;
  double StepSize = (NewMaxR - NewMinR) / 200;
  vector<double> nx, ny;
  for (int i=0;i<200;i++){
    double holdx = NewMinR + i*StepSize;
    //double holdy = XYPar[0] + sqrt(pow(XYPar[2], 2) - pow((holdx-XYPar[1]), 2))*TMath::Sign(1,XYPar[2]) - XYPar[2];
    double holdy = Result->Parameter(0) + sqrt(pow(Result->Parameter(2), 2) - pow((holdx-Result->Parameter(1)), 2))*TMath::Sign(1,Result->Parameter(2)) - Result->Parameter(2);
    nx.push_back(holdx * cos(Phi0) - holdy * sin(Phi0));
    ny.push_back(holdx * sin(Phi0) + holdy * cos(Phi0));
    //nx.push_back(holdx);
    //ny.push_back(holdy);
    //cout << "x: " << nx[i] << " y: " << ny[i] << endl;
  }
  TGraph NewXYGraph(nx.size(), &nx[0], &ny[0]);
  NewXYGraph.SetLineColor(kRed);
  // set to dashed line
  NewXYGraph.SetLineStyle(2);
  //NewXYGraph.SetLineWidth(3);
  NewXYGraph.Draw("same");
  leg->AddEntry(&NewXYGraph,"Fitted","l");
  leg->Draw();

  string imname = getUniqueFilename(1, "Fitting_image/Event_", "XYfit.png");
  c2->SaveAs(imname.c_str());
  delete c2;

}

void GraphPlot(const vector<double>& Z, const vector<double>& R) {
  TCanvas* c1 = new TCanvas("c1", ";;", 1200, 900);
  TGraph* tg = new TGraph(R.size(), &Z[0], &R[0]);
  tg->Draw("AL");
  c1->SaveAs("smapleImage.png");
}

void writeTextFile(const vector<double>& X, const vector<double>& V, const string& name) {
  ofstream file(name.c_str(), std::ios::app);
  file << V[0] << "\t" << V[1] << "\t" << V[2] << X[0] << "\t" << X[1] << "\t" << X[2] << endl;
  file.close();
}

void FitCircle(const vector<double>& X, const vector<double>& Y, vector<double>& XYPar) {
  int NumPoints = X.size();
  TGraph XYGraph(NumPoints);
  TF1* _XYFunc = new TF1("XYFunc", "[0] + sqrt(pow([2], 2) - pow((x-[1]), 2))*TMath::Sign(1,[2]) - [2]", 0, 200);

  // calculate the angle of the first point
  XYZPoint p(X[0], Y[0], 0);
  double Phi0 = p.phi();
  //double Phi0 = atan(Y[0] / X[0]);
  //if (Phi0 < 0) {
  //  Phi0 += TMath::Pi();
  //}
  for (int i = 0; i < NumPoints; i++) {
    float NewX = (X[i]) * cos(Phi0) + (Y[i]) * sin(Phi0);
    float NewY = -(X[i]) * sin(Phi0) + (Y[i]) * cos(Phi0);
    XYGraph.SetPoint(i, NewX, NewY);
  }

  // Set brginnig parameters
  vector<double> Distance_original;
  for (int i = 0; i < X.size(); i++) {
    Distance_original.push_back(-sqrt(pow(X[i], 2) + pow(Y[i], 2)));
  }
  double AvePt = average(Distance_original);

  _XYFunc->SetParameters(1, 1, AvePt / 0.0114);
  TFitResultPtr Result = XYGraph.Fit(_XYFunc, "Q S B");
  XYPar.push_back(Result->Parameter(0));
  XYPar.push_back(Result->Parameter(1));
  XYPar.push_back(Result->Parameter(2));
  XYPar.push_back(Phi0);
  TCanvas* c2 = new TCanvas("c2", ";;", 1200, 900);

  // Set the title for the graph
  XYGraph.SetTitle("XYGraph");
  // Set the title for the x-axis and y-axis
  XYGraph.GetXaxis()->SetTitle("X(cm)");
  XYGraph.GetYaxis()->SetTitle("Y(cm)");
  gPad->SetLeftMargin(0.15);

  XYGraph.Draw("AL*");
  string imname = getUniqueFilename(1, "Fitting_image/Event_", "XYfit.png");
  c2->SaveAs(imname.c_str());
  delete c2;
  
}

//--------------------------------------------------------------------------------***--------------------------------------------------------------------------------
// New Set of Functions
void NewFitRZPlane(const vector< vector<double> >& tracker, const vector< vector<double> >& ecal,const vector< vector<double> >& hcal,vector<double>& RZPar,vector<double>& error, const vector<double>& XYPar, vector< string>& filen, TMultiGraph *mulg) {
  const vector<double> &Z=tracker[2],&R=tracker[3];
  TGraph RZGraph(Z.size(), &R[0], &Z[0]);
  TF1* _RZFunc = new TF1("RZFunc", "[0]+[1]*x+[2]*pow(x,2)", 0, 200);
  _RZFunc->SetParameters(1, 1, 1);  // Initial parameters
  TFitResultPtr Result = RZGraph.Fit(_RZFunc, "Q S B");


  for (int j=0;j<3;j++){
    RZPar.push_back(Result->Parameter(j));
    error.push_back(_RZFunc->GetParError(j));
  }

  // Recreate New Plot with given parameter
  double NewMinR = *min_element(R.begin(), R.end());
  double NewMaxR = *max_element(R.begin(), R.end());
  double StepSize = (NewMaxR - NewMinR) / 200;
  vector<double> NewR, NewZ;
  for (int i=0;i<300;i++){
    NewR.push_back(NewMinR + i*StepSize);
    NewZ.push_back(RZPar[0]+RZPar[1]*NewR[i]+RZPar[2]*pow(NewR[i],2));
  }
  
  TGraph NRZGraph(Z.size(), &Z[0], &R[0]);
  NRZGraph.SetLineWidth(3);


  TGraph NewRZGraph(NewR.size(), &NewZ[0], &NewR[0]);
  NewRZGraph.SetLineColor(kRed);
  //NewRZGraph.SetLineWidth(3);
  NewRZGraph.SetLineStyle(2);  //set to dashed line
 

  //TMultiGraph *mulg = new TMultiGraph();
  mulg->Add(&NRZGraph,"AL");
  TCanvas* c2 = new TCanvas("c2", "RZGraph;R(cm);Z(cm)", 1200, 900);
  
  TLegend *leg = new TLegend(0.55, 0.15, 0.875, 0.30);
  leg->AddEntry(&NewRZGraph,"Fitted","l");
  leg->AddEntry(&NRZGraph,"Tracker","l");
  if (ecal[0].size()>0){
    TGraph *ec = new TGraph(ecal[0].size(), &ecal[2][0], &ecal[3][0]);
    ec->SetLineColor(kOrange);
    ec->SetLineWidth(2);
    //ec->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(ec,"ECAL","l");
    mulg->Add(ec);
  }
  if (hcal[0].size()>0){
    TGraph *hc = new TGraph(hcal[0].size(), &hcal[2][0], &hcal[3][0]);
    hc->SetLineColor(kBlue);
    hc->SetLineWidth(2);
    //hc->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(hc,"HCAL","l");
    mulg->Add(hc);
  }

  // recoresult
  vector<int> selection1={54,67,154,200,271,345,282};
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
  // if dr from fitting parameter is less than 0.1, then it is a good fit
  double FirstEcalRaius=129;
  string imname = getUniqueFilename(1, "Fitting_image/Event_", "RZfit.png");
  vector<double> recor,rocez;
  for (int i=0;i<7;i++){
    // create vector of string contain selection[i] from [i-2,i+2]
    vector<string> namestr;
    for (int k=-3;k<4;k++){
      namestr.push_back("Fitting_image/Event_"+to_string(selection1[i]+k)+"RZfit.png");
    }

    // if imname is in the namestr, then add the reco graph
    if (find(namestr.begin(),namestr.end(),imname)!=namestr.end()){
      filen.push_back(imname);
      for (int j=0;j<300;j++){
        recor.push_back(j*StepSize);
        rocez.push_back(recoresult[i][3]+recoresult[i][4]*recor[j]+recoresult[i][5]*pow(recor[j],2));
      }
      TGraph *reco = new TGraph(recor.size(), &rocez[0], &recor[0]);
      reco->SetLineColor(kGreen);
      reco->SetLineWidth(2);
      leg->AddEntry(reco,"Reco","l");
      mulg->Add(reco);
      break;
    }
  }
  // Set the title for the graph
  mulg->SetTitle("RZGraph");
  // Set the title for the x-axis and y-axis
  mulg->GetXaxis()->SetTitle("Z(cm)");
  mulg->GetYaxis()->SetTitle("R(cm)");
  //mulg->Draw("A");
  //NewRZGraph.Draw("SAME");
  //leg->Draw();
  
  //c2->SaveAs(imname.c_str());
  delete c2;
}

void NewFitXYPlane(const vector< vector<double> >& tracker, const vector< vector<double> >& ecal,const vector< vector<double> >& hcal, vector<double>& XYPar, TMultiGraph *mulg) {
  const vector<double> &X=tracker[0],&Y=tracker[1];
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

  // Set beginnig parameters
  vector<double> Distance_original;
  for (int i = 0; i < X.size(); i++) {
    Distance_original.push_back(-TMath::Sign(1,Y[i])*sqrt(pow(X[i], 2) + pow(Y[i], 2)));
  }
  double AvePt = average(Distance_original);
  _XYFunc->SetParameters(1, 1, AvePt / 0.0114);

  // record result of the fitting
  TFitResultPtr Result = XYGraph.Fit(_XYFunc, "Q S B");
  XYPar.push_back(Result->Parameter(0));
  XYPar.push_back(Result->Parameter(1));
  XYPar.push_back(Result->Parameter(2));
  XYPar.push_back(Phi0);

  // Make multi graph to hold all the graphs
  //TMultiGraph *mulg = new TMultiGraph();
  TCanvas* c2 = new TCanvas("c2", "XYGraph;R(cm);Z(cm)", 1200, 900);
  TLegend *leg = new TLegend(0.55, 0.15, 0.875, 0.30);
  
  // make new graph holding traker data
  TGraph NXYGraph(X.size(), &X[0], &Y[0]);
  mulg->Add(&NXYGraph,"AL");
  NXYGraph.SetLineWidth(3);
  leg->AddEntry(&NXYGraph,"Tracker","l");

  if (ecal[0].size()>0){
    TGraph *ec = new TGraph(ecal[0].size(), &ecal[0][0], &ecal[1][0]);
    ec->SetLineColor(kOrange);
    ec->SetLineWidth(2);
    //ec->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(ec,"ECAL","l");
    mulg->Add(ec);
  }
  if (hcal[0].size()>0){
    TGraph *hc = new TGraph(hcal[0].size(), &hcal[0][0], &hcal[1][0]);
    hc->SetLineColor(kBlue);
    hc->SetLineWidth(2);
    //hc->SetLineStyle(2);  //set to dashed line
    leg->AddEntry(hc,"HCAL","l");
    mulg->Add(hc);
  }
  // Set the title for the graph
  mulg->SetTitle("XYGraph");
  // Set the title for the x-axis and y-axis
  mulg->GetXaxis()->SetTitle("Z(cm)");
  mulg->GetYaxis()->SetTitle("R(cm)");
  //mulg->Draw("A");
  

  // make new graph from plotting parameters and derotate the graph
  double NewMinR = 0;
  double NewMaxR = 10;
  double StepSize = (NewMaxR - NewMinR) / 200;
  vector<double> nx, ny;
  for (int i=0;i<200;i++){
    double holdx = NewMinR + i*StepSize;
    //double holdy = XYPar[0] + sqrt(pow(XYPar[2], 2) - pow((holdx-XYPar[1]), 2))*TMath::Sign(1,XYPar[2]) - XYPar[2];
    double holdy = Result->Parameter(0) + sqrt(pow(Result->Parameter(2), 2) - pow((holdx-Result->Parameter(1)), 2))*TMath::Sign(1,Result->Parameter(2)) - Result->Parameter(2);
    nx.push_back(holdx * cos(Phi0) - holdy * sin(Phi0));
    ny.push_back(holdx * sin(Phi0) + holdy * cos(Phi0));
    //nx.push_back(holdx);
    //ny.push_back(holdy);
    //cout << "x: " << nx[i] << " y: " << ny[i] << endl;
  }
  TGraph NewXYGraph(nx.size(), &nx[0], &ny[0]);
  NewXYGraph.SetLineColor(kRed);
  // set to dashed line
  NewXYGraph.SetLineStyle(2);
  //NewXYGraph.SetLineWidth(3);
  //NewXYGraph.Draw("same");
  leg->AddEntry(&NewXYGraph,"Fitted","l");
  //leg->Draw();

  string imname = getUniqueFilename(1, "Fitting_image/Event_", "XYfit.png");
  //c2->SaveAs(imname.c_str());
  //delete c2;

}