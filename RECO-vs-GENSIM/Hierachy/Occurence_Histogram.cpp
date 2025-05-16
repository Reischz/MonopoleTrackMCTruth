// Purpose: Create a histogram of the occurence of the last detector in the data
using namespace std;

std::vector< std::string > readBinOrderFromFile(const char* filename) {
    std::vector<std::string> binOrder;
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            binOrder.push_back(line);
        }
        file.close();
    }
    return binOrder;
}

void Occurence_Histogram(){
    
    // Open the root file
    TFile *file = TFile::Open("ParameterFromFitting.root");
    // Select the tree
    TTree *tree = (TTree*)file->Get("CombinedTree");
    TCanvas *c1 = new TCanvas("c1","demo bin labels",10,10,1600,900);

    c1->SetGrid();
   
    TH1F *h = new TH1F("h","Occurence Histogram of the Last Detector",3,0,3);
    h->SetStats(0);
    h->SetFillColor(38);
    h->SetCanExtend(TH1::kAllAxes);

    TString *ele = new TString(); // initialize all elements to 0
    tree->SetBranchAddress("LastDetector", &ele);

    // Loop over the tree
    vector<string> raw_strvec;
    for (int i = 0; i < tree->GetEntries(); i++){
        tree->GetEntry(i);
        TString *filteredEle = new TString();
        for (int i = 0; i < ele->Length(); i++) {
            if (!isdigit((*ele)[i]) && (*ele)[i] != '_') {
                *filteredEle += (*ele)[i];
            }
        }
        // Fill the vector
        raw_strvec.push_back(filteredEle->Data());
    }
    
    // get name and occurence
    map<string, int> occurence;
    for (int i = 0; i < raw_strvec.size(); i++) {
        occurence[raw_strvec[i]]++;
    }
    // Fill the histogram
    std::vector< std::string > binOrder = readBinOrderFromFile("Hiarchi/UniqueDeTc.log");
    for (int i = 0; i < binOrder.size(); i++) {
        for (auto it = occurence.begin(); it != occurence.end(); it++) {
            if (it->first == binOrder[i]) {
                for (int j = 0; j < it->second; j++) {
                    h->Fill(it->first.c_str(), 1);
                }
            }
        }
    }
    cout << "Complete Filling Bins" << endl;
    // Set the labels
    h->LabelsDeflate();
    h->GetXaxis()->SetLabelSize(0.025); // Set the size of the x-axis labels
    // Create a custom statistics box
    TPaveText *stats = new TPaveText(0.743429,0.878425,0.898,0.929795,"brNDC");
    stats->SetFillColor(18);
    stats->SetTextAlign(12);

    // Add the number of entries to the statistics box
    stats->AddText(Form("Entries: %d", (Int_t)h->GetEntries()));
    // Find the bin with the maximum frequency
    Int_t maxBin = h->GetMaximumBin();
    // Add the most frequent occurrence to the statistics box
    stats->AddText(Form("Most frequent occurrence: %d", (int)h->GetBinContent(maxBin)));

    // Draw the custom statistics box
    h->Draw();
    stats->Draw();

    // Show the histogram
    c1->SetTopMargin(0.122);
    c1->SetBottomMargin(0.2); // Increase the bottom margin
    c1->SaveAs("Occurence_Histogram.png");
    //exit(0);
}
