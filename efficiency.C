void efficiency(int PU, int Res){
    string fileName = "delphes_PU" + to_string(PU) + "_Res" + to_string(Res) + "ps.root";

    TFile *f = new TFile(fileName.c_str(),"READ");
    TTree *tree = (TTree*) f->Get("Delphes");
    Int_t proton_size, genproton_size;
    Int_t proton_isPU[20];
    Float_t proton_pz[20], genproton_pz[20];

    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus("GenProton.Pz", 1);
    tree->SetBranchStatus("GenProton_size", 1);
    tree->SetBranchStatus("PPSProton.IsPU", 1);
    tree->SetBranchStatus("PPSProton.Pz", 1);
    tree->SetBranchStatus("PPSProton_size", 1);

    tree->SetBranchAddress("GenProton.Pz",&genproton_pz);
    tree->SetBranchAddress("GenProton_size",&genproton_size);
    tree->SetBranchAddress("PPSProton.IsPU",&proton_isPU);
    tree->SetBranchAddress("PPSProton.Pz",&proton_pz);
    tree->SetBranchAddress("PPSProton_size",&proton_size);
    
    TH1F *gp = new TH1F("gp","Generated protons",30,5800,7200);
    TH1F *rp = new TH1F("rp","Reconstructed protons",30,5800,7200);

    for(int i = 0; i < tree->GetEntries(); i++){
        if(i%10000==0) cout<<"Event n."<<i<<endl;
        tree->GetEntry(i);

        for(int j = 0; j< genproton_size; j++){
            if(abs(genproton_pz[j]) > 6000){
                gp->Fill(genproton_pz[j]);
            }
        }
         for(int l = 0; l < proton_size; l++){
            //if(proton_isPU[l] == 0){
                rp->Fill(proton_pz[l]);
           // }
        }
        
    }

    //create new historpam for the division
    TH1F *div = (TH1F*)rp->Clone("Div");
    div->SetTitle("Efficiency");

    //divide the reconstructed protons for the generated protons
    div->Divide(gp);

    //create a canvas to draw on
    TCanvas *c1 = new TCanvas("c1", "Efficiency", 800, 600);

    //draw the historpam
    div->SetLineColor(kOrange+7);
    div->Draw();

    //set labels
    div->GetXaxis()->SetTitle("Pz of generated protons");
    div->GetXaxis()->SetTitleSize(0.038);
    div->GetYaxis()->SetTitle("Division");
    div->GetYaxis()->SetTitleSize(0.038);

    // Create and draw legend
    TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);
    l1->AddEntry(div, "Efficiency");
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw("SAME");

    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    string canvas_title = "Efficiency with Res " + to_string(Res) + " ps and PU " + to_string(PU) + ".pdf";
    c1->SaveAs(canvas_title.c_str());
}