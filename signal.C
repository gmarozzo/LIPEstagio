void signal(TString file){

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

    //create a new file to store the histogram
    TString newfile_name = "Output_" + file + ".root";
    //TFile *f2 = new TFile(newfile_name, "RECREATE");

    //for(int i_file = 0; i_file < n_files; i_file++){
        //create histogram for current file
        TString hist_name = "Output_" + file;
        TH1F *th = new TH1F(hist_name,hist_title,30,-200,200);

        TString filename = file + ".root"; //s[i_file]

        //open the file
        TFile *f = new TFile(filename,"READ");

        //get the tree form the file
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
            
        cout<<"Entries"<< tree->GetEntries() << endl;
        float dif;
        cout<<"Chegou até este ponto"<<endl;

        for(int i=0; i<tree->GetEntries();i++){
            if(i%1000==0) cout<<"Event n."<<i<<endl;
            tree->GetEntry(i);

            dif = 0.;
            Int_t nforw = 0, nback = 0;
            Int_t particle1_index = 0, particle2_index = 0;

            //choose the W bosons (PID = +-24)  muons 13
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

            for(int j=0; j<15;j++){
                
                //choose the best pair of protons
                if(j>proton_size-1) continue;
                //if(!proton_isPU[j])th->Fill(proton_t[j]);
                if(proton_pz[j]<0)  { // && proton_isPU[j]==1
                    nback++;
                    for(int l=0; l<15;l++){
                        if(l>proton_size-1) continue;
                        if(proton_pz[l]>0) { // && proton_isPU[l]==1
                            dif=proton_t[j]-proton_t[l];
                            nforw++;
                        }
                            
                    }
                }
            }
                
            if(n_bosons == 2 && nback == 1 && nforw == 1  && vertex_certo) {
                th->Fill(particle_z[particle1_index]  - (c/2) * dif);
            }
            //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
        }

        //f->Close();
        cout<<th->GetEntries()<<endl;

        //set line color for histogram
        //int colorIndex = (i_file * n_files) % (sizeof(colors) / sizeof(colors[0]));
        th->SetLineColor(kRed-4);

        cout<<"Integral " << th->Integral()<<endl;

        // draw histograms with proper formatting
        //if (n_files == 0) {
            th->Scale(1/th->Integral());
            th->Draw("");
            l1->AddEntry(th, file);
        /* } else {
            th->Scale(1/th->Integral());
            th->Draw("SAME");
            l1->AddEntry(th, files[i_file]);
        }*/

        //update maximum range for y-axis
        if(th->GetMaximum() + th->GetMaximum()/2.45 > maxrange){
            maxrange = th->GetMaximum() + th->GetMaximum()/2.45;
        }
            
        //set y-axis range and labels
        th->GetYaxis()->SetRangeUser(0, maxrange);
        th->GetXaxis()->SetTitle("Z_{W} - #frac{c}{2}#Deltat_{protons} [mm]");
        th->GetXaxis()->SetTitleSize(0.038);
        th->GetYaxis()->SetTitle("N. events");
        th->GetYaxis()->SetTitleSize(0.038);

        //define a new function, in this case two gaussian
        TF1 *gausss = new TF1("gausss","gaus",-200,200); //(0)+gaus(3)

        //define line color
        gausss->SetLineColor(kRed+1);

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
    
        //f2->Write(hist_name);
        //f2->Close();
        cout<<"nhanha"<<endl;
    //}
    
   
    // Draw legend
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0275);
    l1->SetTextFont(42);
    l1->Draw();
    
    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    //c1->SaveAs(canvas_title);
    cout<<"GHGH"<<endl;
    return 1; 

}