void decaimento(TString file){

    //create new Canvas' name
    TString canvas_name = file; 
    
    //create new histogram's name
    TString hist_title = file;

    //create new Canvas' title
    TString canvas_title = file + ".pdf";

    //defining maximum range for y-axis
    float maxrange = 0.;

    //array of colors for histograms
    //int colors[] = {kBlue, kRed, kGreen, kOrange, kMagenta, kCyan};

    //defining light speed
    double c = 299.792458; //[mm/ns]

    //chosing the name to save each histogram and canvas
    /*for(int i = 0; i < n_files; i++){
        canvas_name = canvas_name + files[i];
        canvas_title = canvas_title + files[i];
    }*/

    //create new Canvas
    TCanvas *c1 = new TCanvas("c1", canvas_name, 800, 600);
    
    //create subtitles
    TLegend *l1 = new TLegend(0.68,0.6,0.75,0.89);    

    int n_events = 0;
    
    //create a new file to store the histogram
    TString newfile_name = "Output_" + file + ".root";
    TFile *f2 = new TFile(newfile_name, "RECREATE");

    //for(int i_file = 0; i_file < n_files; i_file++){
        //create histogram for current file
        TString hist_name = "Output_" + file;
        TH1F *th = new TH1F(hist_name,hist_title,50,-200,200);

        TString filename = file + ".root"; //s[i_file]

        //open the file
        TFile *f = new TFile(filename,"READ");

        //get the tree form the file
        TTree *tree = (TTree*) f->Get("Delphes");

            
        Int_t proton_size, electron_size, muon_size , particle_size;
        Int_t proton_isPU[20], muon_charge[20], electron_charge[20], particle_PID[20];
        Float_t proton_t[20], proton_pz[20], proton_e[20], electron_DZ[20], muon_DZ[20], particle_pt[20], particle_eta[20];

        tree->SetBranchStatus("*", 0);
        tree->SetBranchStatus("PPSProton.T", 1);
        tree->SetBranchStatus("PPSProton.IsPU", 1);
        tree->SetBranchStatus("PPSProton.Pz", 1);
        tree->SetBranchStatus("PPSProton_size", 1);
        tree->SetBranchStatus("PPSProton.E", 1);
        tree->SetBranchStatus("Electron.DZ", 1);
        tree->SetBranchStatus("Electron_size", 1);
        tree->SetBranchStatus("Muon.DZ", 1);
        tree->SetBranchStatus("Muon_size", 1);
        tree->SetBranchStatus("Electron.Charge", 1);
        tree->SetBranchStatus("Muon.Charge", 1);
        tree->SetBranchStatus("Particle_size", 1);
        tree->SetBranchStatus("Particle.PID", 1);
        tree->SetBranchStatus("Particle.PT", 1);
        tree->SetBranchStatus("Particle.Eta", 1);
    

        tree->SetBranchAddress("PPSProton.IsPU", proton_isPU);
        tree->SetBranchAddress("PPSProton.T", proton_t);
        tree->SetBranchAddress("PPSProton.Pz", proton_pz);
        tree->SetBranchAddress("PPSProton_size", &proton_size);
        tree->SetBranchAddress("PPSProton.E", proton_e);
        tree->SetBranchAddress("Electron.DZ", electron_DZ);
        tree->SetBranchAddress("Electron_size", &electron_size);
        tree->SetBranchAddress("Muon.DZ", muon_DZ);
        tree->SetBranchAddress("Muon_size", &muon_size);
        tree->SetBranchAddress("Electron.Charge", electron_charge);
        tree->SetBranchAddress("Muon.Charge", muon_charge);
        tree->SetBranchAddress("Particle_size", &particle_size);
        tree->SetBranchAddress("Particle.PID", particle_PID);
        tree->SetBranchAddress("Particle.PT", particle_pt);
        tree->SetBranchAddress("Particle.Eta", particle_eta);
            
        cout<<"Entries "<< tree->GetEntries() << endl;
        float dif;

        cout<<"Chegou até este ponto"<<endl;           

        for(int i=0; i<tree->GetEntries();i++){
            if(i%1000==0) cout<<"Event n."<<i<<endl;
            tree->GetEntry(i);

            dif = 0.;
            Int_t nforw = 0, nback = 0;
            Int_t muon1_index = 999, muon2_index = 999;
            Int_t electron1_index = 999, electron2_index = 999;

            //choose the muons 
            Int_t n_muons = 0;
            for(int i_m = 0; i_m < muon_size; i_m++){
                if(muon_charge[i_m] == 1){
                    muon1_index = i_m;
                    n_muons++;
                }
                else if(muon_charge[i_m] == -1){
                    muon2_index = i_m;
                    n_muons++;
                }
            }

            int n= 0;
            for(int i_PID = 0; i_PID < particle_size; i_PID++){
                if(abs(particle_PID[i_PID])==13){
                    cout<<"particle eta " <<particle_eta[i_PID]<< endl;
                    n++;
                }
            }

            if(n==2){
                n_events++;
            }
        
            //choose the electrons
            Int_t n_electrons = 0;
            for(int i_e = 0; i_e < electron_size; i_e++){
                if(electron_charge[i_e] == 1){
                    electron1_index = i_e;
                    n_electrons++;
                }
                else if(electron_charge[i_e] == -1){
                    electron2_index = i_e;
                    n_electrons++;
                }
            }

        //cout<< "n. muons " << n_muons <<endl;
        bool vertex_certo = 0;
        if(n_muons == 2){
            vertex_certo = 1;
            cout << muon_DZ[muon1_index] << "     "<< muon_DZ[muon2_index] <<endl;
        }
        float dz=0.;
        if(n_muons == 2  && nback == 1 && nforw == 1  && vertex_certo) { //  && vertex_certo
            dz = muon_DZ[muon1_index];
        }
        if(n_electrons == 2  && nback == 1 && nforw == 1) { //  && vertex_certo
           dz = electron_DZ[electron1_index];
        }
        if(n_electrons == 1 && n_muons == 1  && nback == 1 && nforw == 1 ) { //  && vertex_certo
            if(electron1_index != 999){
                dz = electron_DZ[electron1_index];
            }
            else{
                dz = electron_DZ[electron2_index];
            }
                    
        }
        float best_dif=999.;
        for(int j=0; j<15;j++){
                
            //choose the best pair of protons
            if(j>proton_size-1) continue;
                if(proton_pz[j]<0)  { // && proton_isPU[j]==0
                    nback++;
                    for(int l=0; l<15;l++){
                        if(l>proton_size-1) continue;
                        if(proton_pz[l]>0) { // && proton_isPU[l]==0
                            dif=proton_t[j]-proton_t[l];
                            nforw++;
                            if(abs(dz - (c/2) * dif ) < best_dif){
                                best_dif = dz - (c/2) * dif ;
                            }
                        }
                            
                        }
                    }
                }
            if(n_muons + n_electrons == 2){
                th->Fill(best_dif);
            }
            
            //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
        }
            
        f->Close();
        cout<<th->GetEntries()<<endl;

        //set line color for histogram
        //int colorIndex = (i_file * n_files) % (sizeof(colors) / sizeof(colors[0]));
        th->SetLineColor(kAzure+7); //kRed-4
            
        cout<<"Integral " << th->Integral()<<endl;
            
        // draw histograms with proper formatting
        //if (n_files == 0) {
            th->Scale(1/th->Integral());
            th->Draw("");
            l1->AddEntry(th, file);
        /*} else {
            th->Scale(1/th->Integral());
            th->Draw("SAME");
            l1->AddEntry(th, "Data");
        }*/

        //update maximum range for y-axis
        if(th->GetMaximum() + th->GetMaximum()/2.45 > maxrange){
            maxrange = th->GetMaximum() + th->GetMaximum()/2.45;
        }
            
        //set y-axis range and labels
        th->GetYaxis()->SetRangeUser(0, maxrange);
        th->GetXaxis()->SetTitle("Z_{reconstructed particle} - #frac{c}{2}#Deltat_{protons} [mm]");
        th->GetXaxis()->SetTitleSize(0.038);
        th->GetYaxis()->SetTitle("N. events");
        th->GetYaxis()->SetTitleSize(0.038);

        //define a new function, in this case two gaussian
        TF1 *gausss = new TF1("gausss","gaus",-200,200); //(0)+gaus(3)

        //define line color
        gausss->SetLineColor(kAzure-7); //kRed+1
        
        l1->AddEntry(gausss, "Total fit");

        //define a initial value for each parameter (0,3 = norm; 1,4 = mean 2,5 = width) (0,1,2 1st gaussian; 3,4,5 2nd gaussian) 
        gausss->SetParameter(0,10);
        gausss->SetParameter(1,0);
        gausss->SetParameter(2,10);
        //gausss->SetParameter(3,100);
        //gausss->SetParameter(4,0);
        //gausss->SetParameter(5,20);

        //define the limits for each parameter
        gausss->SetParLimits(0,0,1);
        gausss->SetParLimits(1,-50,50);
        gausss->SetParLimits(2,0,100);
        //gausss->SetParLimits(3,1,10000);
        //gausss->SetParLimits(4,-50,50);
        //gausss->SetParLimits(5,0,100);

        //fit the date with the function defined (-60,60 x axis limits)
        th->Fit("gausss","","",-200,200);

        //obtain the values
        double chi2 = gausss->GetChisquare();
        double p0 = gausss->GetParameter(0);
        double p1 =gausss->GetParameter(1);
        double p2 = gausss->GetParameter(2);
        //double p3 = gausss->GetParameter(3);
        //double p4 = gausss->GetParameter(4);
        //double p5 = gausss->GetParameter(5);
        double gl = gausss->GetNDF(); //degrees of freedom

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
        cout << "Global Resolution " << res << " ns" << endl;
        cout << "Arm Resolution " << res/sqrt(2) << " ns" << endl;

    
        f2->Write(hist_name);
        //}
    
    cout << "N. events " << n_events<< endl;
   
    // Draw legend
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0275);
    l1->SetTextFont(42);
    l1->Draw();
    
    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    c1->SaveAs(canvas_title);

    return 1; 

}