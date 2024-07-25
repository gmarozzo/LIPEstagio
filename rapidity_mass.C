void rapidity_mass(){
    //string fileName = "delphes_PU" + to_string(PU) + "_Res" + to_string(Res) + "ps.root";

    TFile *f = new TFile("Signal.root","READ");
    TTree *tree = (TTree*) f->Get("Delphes");
    Int_t proton_size, vertex_size, particle_size;
    Int_t proton_isPU[20], particle_PID[20];
    Float_t proton_t[20], proton_pz[20], proton_e[20], vertex_z[100], particle_e[20], particle_px[20], particle_py[20], particle_pz[20], particle_z[100];

    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("PPSProton.T", 1);
    tree->SetBranchStatus("PPSProton.IsPU", 1);
    tree->SetBranchStatus("PPSProton.Pz", 1);
    tree->SetBranchStatus("PPSProton_size", 1);
    tree->SetBranchStatus("PPSProton.E", 1);
    tree->SetBranchStatus("Vertex.Z", 1);
    tree->SetBranchStatus("Vertex_size", 1);
    tree->SetBranchStatus("Particle.PID", 1);
    tree->SetBranchStatus("Particle.E", 1);
    tree->SetBranchStatus("Particle.Px", 1);
    tree->SetBranchStatus("Particle.Py", 1);
    tree->SetBranchStatus("Particle.Pz", 1);
    tree->SetBranchStatus("Particle_size", 1);
    tree->SetBranchStatus("Particle.Z", 1);

    tree->SetBranchAddress("PPSProton.IsPU", proton_isPU);
    tree->SetBranchAddress("PPSProton.T", proton_t);
    tree->SetBranchAddress("PPSProton.Pz", proton_pz);
    tree->SetBranchAddress("PPSProton_size", &proton_size);
    tree->SetBranchAddress("PPSProton.E", proton_e);
    tree->SetBranchAddress("Vertex.Z", vertex_z);
    tree->SetBranchAddress("Vertex_size", &vertex_size);
    tree->SetBranchAddress("Particle.PID", particle_PID);
    tree->SetBranchAddress("Particle.E", particle_e);
    tree->SetBranchAddress("Particle.Px", particle_px);
    tree->SetBranchAddress("Particle.Py", particle_py);
    tree->SetBranchAddress("Particle.Pz", particle_pz);
    tree->SetBranchAddress("Particle_size", &particle_size);
    tree->SetBranchAddress("Particle.Z", particle_z);

    Int_t p = 6800;

    Double_t c = 299.792458; //[mm/ns]

    Float_t csi1;
    Float_t csi2;

    Float_t Mpp;
    Float_t Mcen;
    Float_t Rpp;
    Float_t Rcen;

    Float_t dif_M;
    Float_t dif_R;
    Float_t dif;

    // Create a new 2D histogram
    TH2F *h = new TH2F("h", "#eta_{pp} - #eta_{cen} Vs M_{pp} - M_{cen}", 100, -200, 200, 100, -1, 1);

    TH1F *h2 = new TH1F("h2", "Tempo ", 30, -60, 60);

    for(int i = 0; i < tree->GetEntries(); i++){
        if(i%1000==0) cout<<"Event n."<<i<<endl;
        tree->GetEntry(i);

        dif=0.;
        float best_dif=10000.;
        csi1 = 0.;
        csi2 = 0.;
        Mpp = 0.;
        Mcen = 0.;
        Rpp = 0.;
        Rcen = 0.;
        dif_M = 0;
        dif_R = 0;
        Int_t nforw = 0, nback = 0;
        Int_t particle1_index = 0, particle2_index = 0;

        //choose the W bosons (PID = +-24) 
        Int_t n_bosons = 0;
        for(int i_PID = 0; i_PID < particle_size; i_PID++){
            if(particle_PID[i_PID] == 24){
                particle1_index = i_PID;
                n_bosons++;
            }
            else if(particle_PID[i_PID] == -24){
                particle2_index = i_PID;
                n_bosons++;
            }
        }
        if(n_bosons == 2){
            cout<<particle_z[particle1_index]-particle_z[particle2_index]<<endl;
        }
        bool vertex_certo = 0;
        if(abs(particle_z[particle1_index]-particle_z[particle2_index])<0.001){
            vertex_certo = 1;
            cout << particle_z[particle1_index] << "     "<< particle_z[particle2_index] <<endl;
        }
        
        // create two lorentz vector for each W boson
        TLorentzVector p1(particle_px[particle1_index], particle_py[particle1_index], particle_pz[particle1_index], particle_e[particle1_index]);
        TLorentzVector p2(particle_px[particle2_index], particle_py[particle2_index], particle_pz[particle2_index], particle_e[particle2_index]);

        //create a lorentz vector of the sum
        TLorentzVector sum = p1 + p2;

        //calculate the mass and rapidity of the protons and of the bosons
       
        Mcen = sum.M();
       
        Rcen = sum.Rapidity();
        
        //choose the best pair of protons and calculate cs1 and csi2
        for(int j=0; j<5;j++){
            if(j>proton_size-1) continue;
            if(proton_pz[j]<0)  { // && proton_isPU[j] == 0
                csi1 = 1 - abs(proton_e[j])/p;
                nback++;
                //if(proton_isPU[j] ==1){
                   // cout<<"PU proton"<<endl;
                //}

              
                for(int l=0; l<5;l++){
                    if(l>proton_size-1) continue;
                    if(proton_pz[l]>0) { // && proton_isPU[l] == 0
                        csi2 = 1 - proton_e[l]/p;
                        dif = proton_t[j]-proton_t[l];
                        nforw++;
                        if(abs(Mcen-13.6 *1000 * sqrt(csi1 * csi2))<abs(Mcen-Mpp)){
                            Mpp = 13.6 *1000 * sqrt(csi1 * csi2);
                            Rpp = -0.5*log(csi1/csi2);
                        }
                        
                    }
                }
            }
        }        
        
        
        //calculate the difference between the masses and the rapiditys 
        if( nback > 0 && nforw >0 ){
            dif_M = Mpp - Mcen;
            dif_R = Rpp - Rcen;
            if(nback == 1 && nforw == 1 && n_bosons == 2 && vertex_certo){
                //fill the 2D histogram
                h->Fill(dif_M, dif_R);
                h2->Fill(particle_z[particle1_index]  - (c/2) * dif);
            }
        }

             
    }

    TCanvas *c3 = new TCanvas("c3", "Rapidity Vs Mass", 800, 600); 

    h->SetMarkerColor(kOrange+7);
    h->SetMarkerSize(1.0);
    h->SetLineColor(kOrange+7);
    h->Draw("COLZ");
    h->GetXaxis()->SetTitle("M_{pp} - M_{cen} [GeV]");
    h->GetXaxis()->SetTitleSize(0.038);
    h->GetYaxis()->SetTitle("#eta_{pp} - #eta_{cen}");
    h->GetYaxis()->SetTitleSize(0.038);

    float range = h2->GetMaximum() + h2->GetMaximum()/2.45;
    h2->GetYaxis()->SetRangeUser(-1, range);
    h2->GetXaxis()->SetTitle("Zpps-Zvtx [mm]");
    h2->GetXaxis()->SetTitleSize(0.038);
    h2->GetYaxis()->SetTitle("N. events");
    h2->GetYaxis()->SetTitleSize(0.038);
    h2->Draw("COLZ");
    cout<<"Integral " << h2->Integral()<<endl;
    
    //define a new function, in this case two gaussian
    TF1 *gausss = new TF1("gausss","gaus(0)+gaus(3)",-60,60);

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
    h2->Fit("gausss","","",-60,60);

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

    //draw legend
    TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);
    l1->AddEntry(h,"Data");
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw("SAME");

    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    //save canvas as a pdf file
    c3->SaveAs("Rapidity Vs Mass.pdf"); 

}