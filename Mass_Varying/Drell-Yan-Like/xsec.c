#include <fstream>
#include <vector>

struct Coordinates {
  std::vector<Double_t> x;
  std::vector<Double_t> y;
};

Coordinates xnfo(std::string name,bool cc){

  // Vectors to hold the X and Y coordinates
  std::vector<Double_t> x, y;

  // Open the file
  std::ifstream inFile(name);

  Double_t a, b;
  while (inFile >> a >> b) {
    if (cc){
    	x.push_back(a);
    	y.push_back(b);
    }
    else {
    	x.push_back(a);
    	y.push_back(b*1e3);
    }
  }

  // Close the file
  inFile.close();
  return {x,y};
}

void xsec() {
  // Create a TCanvas object
  TCanvas *c1 = new TCanvas("c1", ";;", 1200,900);
  c1->SetLogy();
  std::string lu="DYL_nn23lo1_mass_xsec-pb.log";
  std::string nn="DYL_nn23lo1_mass_xsec-pb.log";
  std::string df="DYL_nn23lo1_mass_xsec-pb.log";
  std::string atc="ATLASspinhalf_xsec.log";
  Coordinates LUcrdn =xnfo(lu,false);
  Coordinates NNcrdn =xnfo(nn,false);
  Coordinates dfcrdn =xnfo(df,false);
  Coordinates atccrdn=xnfo(atc,true);

  // Create a TGraph object
  TGraph *nngr = new TGraph(NNcrdn.x.size(), &NNcrdn.x[0], &NNcrdn.y[0]);
  TGraph *lugr = new TGraph(LUcrdn.x.size(), &LUcrdn.x[0], &LUcrdn.y[0]);
  TGraph *atgr = new TGraph(atccrdn.x.size(), &atccrdn.x[0], &atccrdn.y[0]);
  TGraph *dfgr = new TGraph(dfcrdn.x.size(), &dfcrdn.x[0], &dfcrdn.y[0]);

  // Set line color and width
  nngr->SetLineColor(kRed);
  nngr->SetLineWidth(2);
  lugr->SetLineColor(kBlue);
  lugr->SetLineWidth(2);
  atgr->SetLineColor(kViolet);
  atgr->SetLineWidth(2);
  dfgr->SetLineColor(kGreen);
  dfgr->SetLineWidth(2);

  // Set the title of the graph
  atgr->SetTitle("xsec vs mass");
  // Set the labels for the x and y axes
  atgr->GetXaxis()->SetTitle("mass (GeV)");
  atgr->GetYaxis()->SetTitle("x-sec (fb)");
  TLegend *leg = new TLegend(0.55, 0.7, 0.85, 0.85);
  leg->AddEntry(atgr,"ATLAS","l");
  //leg->AddEntry(lugr,"LUXqed_plus_PDF4LHC15_nnlo_100","l");
  //leg->AddEntry(nngr,"NNPDF31_nlo_as_0118_luxqed","l");
  leg->AddEntry(dfgr,"nn23lo1","l");

  leg->SetTextSize(0.03);

  // set xlim
  atgr->GetXaxis()->SetLimits(0, 6000);

  // Draw the graph
  atgr->Draw("AL");
  //nngr->Draw("same");  // Draw axes and a line connecting the points
  //lugr->Draw("same");
  dfgr->Draw("same");

  leg->Draw();
  // Save the canvas as a PNG image
  c1->SaveAs("x_sec.png");
}
