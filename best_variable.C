void best_variable(){
    //string fileName = "delphes_PU" + to_string(PU) + "_Res" + to_string(Res) + "ps.root";

    TFile *f = new TFile("Background.root","READ");
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

    //create a 2D histogram
    TH2F *h = new TH2F("h", "#eta_{pp} - #eta_{cen} Vs M_{pp} - M_{cen}", 100, -200, 200, 100, -1, 1);

    //create 1D histograms
    TH1F *h2 = new TH1F("h2", "Time Resolution", 30, -500, 500);
    TH1F *mass = new TH1F("mass", "Central System invariant mass", 30, 0, 2700);
    TH1F *rap = new TH1F("rap", "Central system rapidity", 30, -1, 1.5);
    TH1F *difference = new TH1F("difference", "Difference between W bosons' energies", 30, -1500, 1500);
    TH1F *addup = new TH1F("addup", "Sum between W bosons' energies", 30, 0, 3000);
    TH1F *energy_low = new TH1F("energy_low", "Energy of the less energetic W bosons", 30, -100, 1500);
    TH1F *energy_high = new TH1F("energy_high", "Energy of the more energetic W bosons", 30, -100, 1500);

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
        /*if(n_bosons == 2){
            cout<<particle_z[particle1_index]-particle_z[particle2_index]<<endl;
        }*/
        bool vertex_certo = 0;
        if(abs(particle_z[particle1_index]-particle_z[particle2_index])<0.001){
            vertex_certo = 1;
            //cout << particle_z[particle1_index] << "     "<< particle_z[particle2_index] <<endl;
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
            if(proton_pz[j]<0 )  { // && proton_isPU[j] == 0
                csi1 = 1 - abs(proton_e[j])/p;
                nback++;

                for(int l=0; l<5;l++){
                    if(l>proton_size-1) continue;
                    if(proton_pz[l]>0 ) { //&& proton_isPU[l] == 0
                        csi2 = 1 - proton_e[l]/p;
                        dif = proton_t[j]-proton_t[l];
                        nforw++;
                        if(abs(Mcen-13.6 *1000 * sqrt(csi1 * csi2))<abs(Mcen-Mpp)){
                            Mpp = 13.6 *1000 * sqrt(csi1 * csi2);
                            Rpp = -0.5*log(csi1/csi2);
                        }
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
        
            cout<<n_bosons<<endl;
        
        
        //calculate the difference between the masses and the rapiditys 
        if( nback > 0 && nforw >0 ){
            dif_M = Mpp - Mcen;
            dif_R = Rpp - Rcen;
            if(nback == 1 && nforw == 1 && n_bosons == 2 && vertex_certo){//
                //fill the 2D histogram
                h->Fill(dif_M, dif_R);
                //fiil the 1D histogram of the time resolution
                h2->Fill(particle_z[particle1_index]  - (c/2) * dif);
                //fill histogram of the mass
                mass->Fill(Mcen);
                //fill histogram of the rapidity
                rap->Fill(Rcen);
                //fill histogram of the difference between W bosons' energies
                difference->Fill(particle_e[particle1_index] - particle_e[particle2_index]);
                //fill histogram of the addup between W bosons' energies
                addup->Fill(particle_e[particle1_index] + particle_e[particle2_index]);
                if(particle_e[particle1_index] < particle_e[particle2_index]){
                    energy_low->Fill(particle_e[particle1_index]);
                    energy_high->Fill(particle_e[particle2_index]);
                }
                if(particle_e[particle1_index] > particle_e[particle2_index]){
                    energy_high->Fill(particle_e[particle1_index]);
                    energy_low->Fill(particle_e[particle2_index]);
                }
            }
        } 
    }

    TCanvas *c3 = new TCanvas("c3", "Comparison between variables", 800, 600); 

    c3->Divide(4,2);

    /*         Rapidity Vs Mass histogram         */
    c3->cd(1);
    h->SetMarkerSize(1.0);
    h->Draw("COLZ");
    h->GetXaxis()->SetTitle("M_{pp} - M_{cen} [GeV]");
    h->GetXaxis()->SetTitleSize(0.038);
    h->GetYaxis()->SetTitle("#eta_{pp} - #eta_{cen}");
    h->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    /*TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);
    l1->AddEntry(h,"Data");
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw("SAME");*/

    /*         Time resolution histogram         */
    c3->cd(2);
    //defining maximum range for y-axis
    float maxrange = 0.;
    //update maximum range for y-axis
    if(h2->GetMaximum() + h2->GetMaximum()/2.45 > maxrange){
        maxrange = h2->GetMaximum() + h2->GetMaximum()/2.45;
    }

    h2->GetYaxis()->SetRangeUser(-1, maxrange);
    h2->GetXaxis()->SetTitle("Zpps-Zvtx [mm]");
    h2->GetXaxis()->SetTitleSize(0.038);
    h2->GetYaxis()->SetTitle("N. events");
    h2->GetYaxis()->SetTitleSize(0.038);
    h2->Draw("COLZ");
    cout<<"Integral " << h2->Integral()<<endl;
    
    //define a new function, in this case two gaussian
    TF1 *gausss = new TF1("gausss","gaus",-500,500); //(0)+gaus(3)

    //define a initial value for each parameter (0,3 = norm; 1,4 = mean 2,5 = width) (0,1,2 1st gaussian; 3,4,5 2nd gaussian) 
    gausss->SetParameter(0,50);
    gausss->SetParameter(1,0);
    gausss->SetParameter(2,10);
    //gausss->SetParameter(3,20);
    //gausss->SetParameter(4,0);
    //gausss->SetParameter(5,20);

    //define the limits for each parameter
    gausss->SetParLimits(0,1,10000);
    gausss->SetParLimits(1,-50,50);
    gausss->SetParLimits(2,0,100);
    //gausss->SetParLimits(3,1,10000);
    //gausss->SetParLimits(4,-50,50);
    //gausss->SetParLimits(5,0,100);

    //fit the date with the function defined (-60,60 x axis limits)
    h2->Fit("gausss","","",-500,500);

    //obtain the values
    double chi2 = gausss->GetChisquare();
    double p0 = gausss->GetParameter(0);
    double p1 =gausss->GetParameter(1);
    double p2 = gausss->GetParameter(2);
   // double p3 = gausss->GetParameter(3);
    //double p4 = gausss->GetParameter(4);
    //double p5 = gausss->GetParameter(5);
    double gl = gausss->GetNDF(); //degrees of freedom
    double p2err = gausss->GetParError(2);

    cout << "Fit Results:" << endl;
    cout << "Chi2: " << chi2 << endl;
    cout << "Signal norm: " << p0 << " ± " << gausss->GetParError(0) << endl;
    cout << "Signal mean: " << p1 << " ± " << gausss->GetParError(1) << endl;
    cout << "Sigma width: " << p2 << " ± " << gausss->GetParError(2) << endl;
    cout << "N. degrees of freedom: " << gl << endl;
    //cout << "Background norm: " << p3 << " ± " << gausss->GetParError(3) << endl;
    //cout << "Background mean: " << p4 << " ± " << gausss->GetParError(4) << endl;
    //cout << "Background width: " << p5 << " ± " << gausss->GetParError(5) << endl;

    //global resolution calculation
    float res =  p2/(c/2);
    float reserr = p2err/(c/2);
    cout << "Global Resolution " << res << " ns" << endl;

    //draw legend
    TLegend *l2 = new TLegend(0.68,0.6,0.9,0.8);
    l2->AddEntry(h2,"Data");
    l2->AddEntry(gausss,"Total fit");
    l2->SetBorderSize(0);
    l2->SetTextSize(0.03);
    l2->SetTextFont(62);
    l2->Draw("SAME");

    //write information
    TString label = Form("Global resolution %.3f ps",res*1000);
    TLatex* text1 = new TLatex(-50,maxrange*0.93,label.Data());
    text1->SetTextSize(0.03);
    text1->SetTextFont(62);
    text1->Draw();
    TString label2 = Form("Resolution single arm %.3f #pm %.3f ps",res/sqrt(2)*1000 ,reserr/sqrt(2)*1000);
    TLatex* text2 = new TLatex(-50,maxrange*0.85,label2.Data());
    text2->SetTextSize(0.03);
    text2->SetTextFont(62);
    text2->Draw();

    /*         Mass histogram        */
    c3->cd(3);
    mass->SetMarkerColor(kViolet-1);
    mass->SetMarkerSize(1.0);
    mass->SetLineColor(kViolet-1);
    mass->Draw("COLZ");
    mass->GetXaxis()->SetTitle("M_{Cen} [GeV]");
    mass->GetXaxis()->SetTitleSize(0.038);
    mass->GetYaxis()->SetTitle("N. events");
    mass->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    TLegend *l3 = new TLegend(0.68,0.6,0.9,0.89);
    l3->AddEntry(mass,"Data");
    l3->SetBorderSize(0);
    l3->SetTextSize(0.0375);
    l3->SetTextFont(62);
    l3->Draw("SAME");

    /*         Rapidity histogram        */
    c3->cd(4);
    rap->SetMarkerColor(kAzure+5);
    rap->SetMarkerSize(1.0);
    rap->SetLineColor(kAzure+5);
    rap->Draw("COLZ");
    rap->GetXaxis()->SetTitle("#eta_{cen}");
    rap->GetXaxis()->SetTitleSize(0.038);
    rap->GetYaxis()->SetTitle("N. events");
    rap->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    TLegend *l4 = new TLegend(0.68,0.6,0.9,0.89);
    l4->AddEntry(rap,"Data");
    l4->SetBorderSize(0);
    l4->SetTextSize(0.0375);
    l4->SetTextFont(62);
    l4->Draw("SAME");
    
    /*         Difference between W bosons' energies histogram        */
    c3->cd(5);
    difference->SetMarkerColor(kTeal+1);
    difference->SetMarkerSize(1.0);
    difference->SetLineColor(kTeal+7);
    difference->Draw("COLZ");
    difference->GetXaxis()->SetTitle("E_{W+} - E_{W-} [GeV]");
    difference->GetXaxis()->SetTitleSize(0.038);
    difference->GetYaxis()->SetTitle("N. events");
    difference->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    TLegend *l5 = new TLegend(0.68,0.6,0.9,0.89);
    l5->AddEntry(difference,"Data");
    l5->SetBorderSize(0);
    l5->SetTextSize(0.0375);
    l5->SetTextFont(62);
    l5->Draw("SAME");

    /*         Addup between W bosons' energies histogram        */
    c3->cd(6);
    addup->SetMarkerColor(kTeal+5);
    addup->SetMarkerSize(1.0);
    addup->SetLineColor(kTeal+5);
    addup->Draw("COLZ");
    addup->GetXaxis()->SetTitle("E_{W+} + E_{W-} [GeV]");
    addup->GetXaxis()->SetTitleSize(0.038);
    addup->GetYaxis()->SetTitle("N. events");
    addup->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    TLegend *l6 = new TLegend(0.68,0.6,0.9,0.89);
    l6->AddEntry(addup,"Data");
    l6->SetBorderSize(0);
    l6->SetTextSize(0.0375);
    l6->SetTextFont(62);
    l6->Draw("SAME");

    /*         Energy of the less energetic W bosons histogram        */
    c3->cd(7);
    energy_low->SetMarkerColor(kOrange-4);
    energy_low->SetMarkerSize(1.0);
    energy_low->SetLineColor(kOrange-4);
    energy_low->Draw("COLZ");
    energy_low->GetXaxis()->SetTitle("E_{W} less energy [GeV]");
    energy_low->GetXaxis()->SetTitleSize(0.038);
    energy_low->GetYaxis()->SetTitle("N. events");
    energy_low->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    TLegend *l7 = new TLegend(0.68,0.6,0.9,0.89);
    l7->AddEntry(energy_low,"Data");
    l7->SetBorderSize(0);
    l7->SetTextSize(0.0375);
    l7->SetTextFont(62);
    l7->Draw("SAME");

    /*         Energy of the more energetic W bosons histogram        */
    c3->cd(8);
    energy_high->SetMarkerColor(kOrange+7);
    energy_high->SetMarkerSize(1.0);
    energy_high->SetLineColor(kOrange+7);
    energy_high->Draw("COLZ");
    energy_high->GetXaxis()->SetTitle("E_{W more energy} [GeV]");
    energy_high->GetXaxis()->SetTitleSize(0.038);
    energy_high->GetYaxis()->SetTitle("N. events");
    energy_high->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    TLegend *l8 = new TLegend(0.68,0.6,0.9,0.89);
    l8->AddEntry(energy_high,"Data");
    l8->SetBorderSize(0);
    l8->SetTextSize(0.0375);
    l8->SetTextFont(62);
    l8->Draw("SAME");

    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    //save canvas as a pdf file
    c3->SaveAs("Comparison between variables.pdf"); 

}