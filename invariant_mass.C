void invariant_mass(int PU, int Res){
    string fileName = "delphes_PU" + to_string(PU) + "_Res" + to_string(Res) + "ps.root";

    TFile *f = new TFile(fileName.c_str(),"READ");
    TTree *tree = (TTree*) f->Get("Delphes");
    Int_t proton_size, vertex_size;
    Int_t proton_isPU[20];
    Float_t proton_t[20], proton_pz[20], vertex_z[100];

    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus("PPSProton.T", 1);
    tree->SetBranchStatus("PPSProton.IsPU", 1);
    tree->SetBranchStatus("PPSProton.Pz", 1);
    tree->SetBranchStatus("PPSProton_size", 1);
    tree->SetBranchStatus("Vertex.Z", 1);
    tree->SetBranchStatus("Vertex_size", 1);

    tree->SetBranchAddress("PPSProton.IsPU",&proton_isPU);
    tree->SetBranchAddress("PPSProton.T",&proton_t);
    tree->SetBranchAddress("PPSProton.Pz",&proton_pz);
    tree->SetBranchAddress("PPSProton_size",&proton_size);
    tree->SetBranchAddress("Vertex.Z",&vertex_z);
    tree->SetBranchAddress("Vertex_size",&vertex_size);

    Int_t p = 6800;

    Double_t c = 299.792458; //[mm/ns]

    Float_t csi1;
    Float_t csi2;

    Float_t M;
    Float_t dif;

    string hist_title = "Invariant Mass with PU " + to_string(PU) + " and Res " + to_string(Res);
    //"Resolution with PU " + to_string(PU) + ", Res " + to_string(Res) + " and M > 1000";

    TH1F *mass = new TH1F("mass", hist_title.c_str(),30,200,2200); // range for the resolution is -60, 60

    for(int i = 0; i < tree->GetEntries(); i++){
        if(i%10000==0) cout<<"Event n."<<i<<endl;
        tree->GetEntry(i);

        dif=0.;
        float best_dif=10000.;
        csi1 = 0.;
        csi2 = 0.;
        M = 0.;
        Int_t nforw=0, nback=0;
        //choose the best pair of protons and calculate cs1 and csi2
        for(int j=0; j<5;j++){
            if(j>proton_size-1) continue;
            if(proton_pz[j]<0)  { 
                csi1 = 1 - abs(proton_pz[j])/p;
                nback++;
                for(int l=0; l<5;l++){
                    if(l>proton_size-1) continue;
                    if(proton_pz[l]>0) {
                        csi2 = 1 - proton_pz[l]/p;
                        dif = proton_t[j]-proton_t[l];
                        nforw++;
                    }
                    //choose the best vertex
                    for(int k = 0; k < vertex_size; k++){
                        if(abs(vertex_z[k] - (c/2) * dif ) < abs(best_dif)){
                            best_dif = vertex_z[k] - (c/2) * dif ;
                        }
                    }
                }
            }
        }
        M = 13.6 *1000 * sqrt(csi1 * csi2);
        if( nback > 0 && nforw >0 ){ //&& M < 1000
            mass->Fill(M); //best_dif-> put in Fill to calculate the resolution
        }
        
    }

    
    string canvas_name = "Invariant Mass with PU " + to_string(PU) + " and Res " +to_string(Res);
    //"Resolution with PU " + to_string(PU) + ", Res " + to_string(Res) + " and M < 1000";

    TCanvas *c3 = new TCanvas("c3", canvas_name.c_str(), 800, 600);

    mass->GetXaxis()->SetTitle("Mass [GeV]"); //Zpps-Zvtx [mm]
    mass->GetXaxis()->SetTitleSize(0.038);
    mass->GetYaxis()->SetTitle("N. events");
    mass->GetYaxis()->SetTitleSize(0.038);
    mass->SetLineColor(kOrange+7);
    mass->Draw();

    //define a new function, in this case two gaussian
    /*TF1 *gausss = new TF1("gausss","gaus(0)+gaus(3)",-60,60);

    //define a initial value for each parameter (0,3 = norm; 1,4 = mean 2,5 = width) (0,1,2 1st gaussian; 3,4,5 2nd gaussian) 
    gausss->SetParameter(0,160);
    gausss->SetParameter(1,0);
    gausss->SetParameter(2,10);
    gausss->SetParameter(3,100);
    gausss->SetParameter(4,0);
    gausss->SetParameter(5,20);

    //define the limits for each parameter
    gausss->SetParLimits(0,1,10000);
    gausss->SetParLimits(1,-50,50);
    gausss->SetParLimits(2,0,100);
    gausss->SetParLimits(3,1,10000);
    gausss->SetParLimits(4,-50,50);
    gausss->SetParLimits(5,0,100);

    //fit the date with the function defined (-60,60 x axis limits)
    mass->Fit("gausss","","",-60,60);

    //obtain the values
    double chi2 = gausss->GetChisquare();
    double p0 = gausss->GetParameter(0);
    double p1 =gausss->GetParameter(1);
    double p2 = gausss->GetParameter(2);
    double p3 = gausss->GetParameter(3);
    double p4 = gausss->GetParameter(4);
    double p5 = gausss->GetParameter(5);
    double gl = gausss->GetNDF(); //degrees of freedom

    cout << "Fit Results:" << endl;
    cout << "Chi2: " << chi2 << endl;
    cout << "Signal norm: " << p0 << " ± " << gausss->GetParError(0) << endl;
    cout << "Signal mean: " << p1 << " ± " << gausss->GetParError(1) << endl;
    cout << "Sigma width: " << p2 << " ± " << gausss->GetParError(2) << endl;
    cout << "N. degrees of freedom: " << gl << endl;
    cout << "Background norm: " << p3 << " ± " << gausss->GetParError(3) << endl;
    cout << "Background mean: " << p4 << " ± " << gausss->GetParError(4) << endl;
    cout << "Background width: " << p5 << " ± " << gausss->GetParError(5) << endl;

    //global resolution calculation
    float res =  p2/(c/2);
    cout << "Global Resolution " << res << " ns" << endl;

    //draw the two gaussians with fit values (po, p1, p2, p3, p4, p5)
    TF1 *gaus2 = new TF1("gaus2","gaus",-60,60);
    TF1 *gaus3 = new TF1("gaus3","gaus",-60,60);
    gaus2->SetParameter(0,p0);
    gaus2->SetParameter(1,p1);
    gaus2->SetParameter(2,p2);
    gaus2->Draw("SAME");
    gaus2->SetLineWidth(3);
    gaus2->SetLineStyle(3);

    gaus3->SetParameter(0,p3);
    gaus3->SetParameter(1,p4);
    gaus3->SetParameter(2,p5);
    gaus2->SetLineColor(kRed);
    gaus3->SetLineColor(kBlue);
    gaus3->Draw("SAME");
    gaus3->SetLineWidth(3);
    gaus3->SetLineStyle(3);

    float range = mass->GetMaximum() + mass->GetMaximum()/2.45;

    //write information
    TString label = Form("Global resolution %.3f ns",res);
    TLatex* text1 = new TLatex(-50,range*0.93,label.Data());
    text1->SetTextSize(0.04);
    text1->SetTextFont(62);
    text1->Draw();
    TString label2 = Form("Resolution single arm %.3f ns",res/sqrt(2));
    TLatex* text2 = new TLatex(-50,range*0.85,label2.Data());
    text2->SetTextSize(0.04);
    text2->SetTextFont(62);
    text2->Draw();*/


    //draw legend
    TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);
    l1->AddEntry(mass,"Data");
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw("SAME");

    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    //save canvas as a pdf file
    string canvas_title = "Invariant Mass with PU " + to_string(PU) + " and Res " +to_string(Res) + ".pdf";
    //"Resolution with PU " + to_string(PU) + ", Res " + to_string(Res) + " and M < 1000" + ".pdf";
    c3->SaveAs(canvas_title.c_str());

}