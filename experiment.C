void experiment(){
    TFile *f = new TFile("Signal.root","READ");
    TTree *tree = (TTree*) f->Get("Delphes");
    Int_t proton_size, vertex_size, particle_size;
    Int_t proton_isPU[20], particle_PID[20];
    Float_t proton_t[20], proton_pz[20], vertex_z[100], particle_mass[20], particle_rapidity[20];

    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus("PPSProton.T", 1);
    tree->SetBranchStatus("PPSProton.IsPU", 1);
    tree->SetBranchStatus("PPSProton.Pz", 1);
    tree->SetBranchStatus("PPSProton_size", 1);
    tree->SetBranchStatus("Vertex.Z", 1);
    tree->SetBranchStatus("Vertex_size", 1);
    tree->SetBranchStatus("Particle.PID", 1);
    tree->SetBranchStatus("Particle.Mass", 1);
    tree->SetBranchStatus("Particle.Papidity", 1);
    //tree->SetBranchStatus("Particle.E", 1);
    //tree->SetBranchStatus("Particle.Px", 1);
    //tree->SetBranchStatus("Particle.Py", 1);
    //tree->SetBranchStatus("Particle.Pz", 1);
    //tree->SetBranchStatus("Particle_size", 1);

    tree->SetBranchAddress("PPSProton.IsPU", proton_isPU);
    tree->SetBranchAddress("PPSProton.T", proton_t);
    tree->SetBranchAddress("PPSProton.Pz", proton_pz);
    tree->SetBranchAddress("PPSProton_size", &proton_size);
    tree->SetBranchAddress("Vertex.Z", vertex_z);
    tree->SetBranchAddress("Vertex_size", &vertex_size);
    tree->SetBranchAddress("Particle.PID", particle_PID);
    tree->SetBranchAddress("Particle.Mass", particle_mass);
    tree->SetBranchAddress("Particle.Rapidity", particle_rapidity);
    //tree->SetBranchAddress("Particle.E", particle_e);
    //tree->SetBranchAddress("Particle.Px", particle_px);
    //tree->SetBranchAddress("Particle.Py", particle_py);
    //tree->SetBranchAddress("Particle.Pz", particle_pz);
    //tree->SetBranchAddress("Particle_size", &particle_size);

    Int_t p = 6800;

    Double_t c = 299.792458; //[mm/ns]

    Float_t csi1;
    Float_t csi2;

    Float_t Mpp;
    Float_t Mcen;
    Float_t Rpp;
    Float_t Rcen;

    Float_t dif_M[particle_size];
    Float_t dif_R[particle_size];
    //Float_t dif;


    for(int i = 0; i < tree->GetEntries(); i++){
        if(i%10000==0) cout<<"Event n."<<i<<endl;
        tree->GetEntry(i);

        //dif=0.;
        //float best_dif=10000.;
        csi1 = 0.;
        csi2 = 0.;
        Mpp = 0.;
        Mcen = 0.;
        Rpp = 0.;
        Rcen = 0.;
        Int_t nforw = 0, nback = 0;
        Int_t particle1_index = 0, particle2_index = 0;
        //choose masse best pair of protons and calculate cs1 and csi2
        for(int j=0; j<5;j++){
            if(j>proton_size-1) continue;
            if(proton_pz[j]<0)  { 
                csi1 = 1 - abs(proton_pz[j])/p;
                nback++;
                for(int l=0; l<5;l++){
                    if(l>proton_size-1) continue;
                    if(proton_pz[l]>0) {
                        csi2 = 1 - proton_pz[l]/p;
                        //dif = proton_t[j]-proton_t[l];
                        nforw++;
                    }
                }
            }
        }
        //choose the muons (PID = 24) 

        Int_t n_muons = 0;
        for(int i_PID = 0; i_PID < particle_size; i_PID++){
            if (particle1_index != 0 && particle2_index != 0){
                if(particle_PID[i_PID] == 24 && n_muons == 0){
                    particle1_index = i_PID;
                    n_muons++;
                }
                else if(particle_PID[i_PID] == 24 && n_muons == 1){
                    particle2_index = i_PID;
                    n_muons++;
                }
            } 
        }

        Int_t n_muon = 0;
        for(int i_muon; i_muon < particle_size; i_muon++){
            if(particle_PID[i_muon] == 24 && n_muon <2){
                Mcen = particle_mass[i_muon];
                Rcen = particle_rapidity[i_muon];
                n_muon++;
            }
        }
        //calculate the mass and rapidity of the protons and of the muons
        Mpp = 13.6 *1000 * sqrt(csi1 * csi2);
        Rpp = -0.5*log(csi1/csi2);
        
        //calculate the difference between the masses and the rapiditys and save the values in two arrays
        if( nback > 0 && nforw >0 ){
            dif_M[i] = Mpp - Mcen;
            dif_R[i] = Rpp - Rcen;
        }
        
    }

    
    //string canvas_name = "Invariant Mass with PU " + to_string(PU) + " and Res " +to_string(Res);

    TCanvas *c3 = new TCanvas("c3", "Rapidity Vs Mass", 800, 600); //canvas_name.c_str()

    TGraph* grap = new TGraph(particle_size, dif_M, dif_R);

    //defining maximum range for y-axis
    float maxrange = 0.;

    //calculate maximum y range
    for(int i = 0; i < particle_size; i++){
        if(dif_R[i] > maxrange){
            maxrange = dif_R[i];
        }
    }
    grap->SetMarkerStyle(20);
    grap->SetMarkerSize(1.0);
    grap->SetMarkerColor(kOrange+7);
    grap->SetLineColor(kOrange+7);
    grap->Draw("AP*");
    grap->SetTitle("#eta pp - #eta cen Vs Mpp - Mcen");

    //grap->GetYaxis()->SetRangeUser(-1, maxrange*1.25);
    grap->GetXaxis()->SetTitle("Mpp - Mcen [GeV]");
    grap->GetXaxis()->SetTitleSize(0.038);
    grap->GetYaxis()->SetTitle("#eta pp - #eta cen");
    grap->GetYaxis()->SetTitleSize(0.038);

    //draw legend
    TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);
    l1->AddEntry(grap,"Data");
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw("SAME");

    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    //save canvas as a pdf file
    //string canvas_title = "Invariant Mass with PU " + to_string(PU) + " and Res " +to_string(Res) + ".pdf";

    c3->SaveAs("Rapidity Vs Mass.pdf"); //canvas_title.c_str()

}
