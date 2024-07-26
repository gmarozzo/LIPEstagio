void bck_Vs_sig(TString files[], int n_files){

    //create new Canvas' name
    TString canvas_name = "signal vs background"; 
    
    //create new histogram's name
    TString hist_title = "signal vs background";

    //create new Canvas' title
    TString canvas_title = "signal vs background.pdf";

    //defining maximum range for y-axis
    float maxrange = 0.;

    //array of colors for histograms
    int colors[] = {kBlue, kRed, kGreen, kOrange, kMagenta, kCyan};

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
    TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);

    //create histogram for current file
    TH1F *th = new TH1F("th",hist_title,30,-60,60);

    for(int i_file = 0; i_file < n_files; i_file++){
            
            //open the file
            TFile *f = new TFile(files[i_file],"READ");

            //get the tree form the file
            TTree *tree = (TTree*) f->Get("Delphes");

            

            //branch variables
            Int_t proton_size, vertex_size;
            Int_t proton_isPU[20];
            Float_t proton_t[20], proton_pz[20], vertex_z[100];

            //set branch addresses
            tree->SetBranchStatus("*",0);
            tree->SetBranchStatus("PPSProton.T", 1);
            tree->SetBranchStatus("PPSProton.IsPU", 1);
            tree->SetBranchStatus("PPSProton.Pz", 1);
            tree->SetBranchStatus("PPSProton_size", 1);
            tree->SetBranchStatus("Vertex.Z", 1);
            tree->SetBranchStatus("Vertex_size", 1);
            tree->SetBranchAddress("PPSProton.T",proton_t);
            tree->SetBranchAddress("PPSProton.IsPU",proton_isPU);
            tree->SetBranchAddress("PPSProton.Pz",proton_pz);
            tree->SetBranchAddress("PPSProton_size",&proton_size);
            tree->SetBranchAddress("Vertex.Z",vertex_z);
            tree->SetBranchAddress("Vertex_size",&vertex_size);

            
            cout<<"Entries"<< tree->GetEntries() << endl;
            float dif;
            double c = 299.792458; //[mm/ns]
            //int index;
            cout<<"Chegou até este ponto"<<endl;
            for(int i=0; i<tree->GetEntries();i++){
                if(i%1000==0) cout<<"Event n."<<i<<endl;
                tree->GetEntry(i);
                dif=0.;
                float best_dif=10000.;
                int nforw=0, nback=0;    
                for(int j=0; j<15;j++){
                
                    //choose the best pair of protons
                    if(j>proton_size-1) continue;
                    //if(!proton_isPU[j])th->Fill(proton_t[j]);
                    if(proton_pz[j]<0 && proton_isPU[j]==0)  { //
                        nback++;
                        for(int l=0; l<15;l++){
                            if(l>proton_size-1) continue;
                            if(proton_pz[l]>0 && proton_isPU[l]==0) { //
                                dif=proton_t[j]-proton_t[l];
                                nforw++;
                            }
                            //choose the best vertex
                            for(int k = 0; k < vertex_size; k++){
                                cout<<"A"<<endl;
                                if(abs(vertex_z[k] - (c/2) * dif ) < abs(best_dif)){
                                    cout<<best_dif<<endl;
                                    best_dif = vertex_z[k] - (c/2) * dif ;
                                    cout<<best_dif<<endl<<endl;
                                }
                                cout<<"D"<<endl;
                            }
                        }
                    }
                }
                //th->Fill(0.,0.);
                    if(vertex_size < 5 && nback > 0 && nforw > 0) {
                        th->Fill(best_dif);
                    }
                //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
            }

            cout<<th->GetEntries()<<endl;

            //set line color for histogram
            int colorIndex = (i_file * n_files) % (sizeof(colors) / sizeof(colors[0]));
            th->SetLineColor(colors[colorIndex]);

            //update maximum range for y-axis
            if(th->GetMaximum() + th->GetMaximum()/2.45 > maxrange){
                maxrange = th->GetMaximum() + th->GetMaximum()/2.45;
            }
            
            cout<<"Integral " << th->Integral()<<endl;
    
            
            // draw histograms with proper formatting
            if (n_files == 0) {
                th->Scale(1/th->Integral());
                th->DrawCopy();
            } else {
                th->Scale(1/th->Integral());
                th->DrawCopy("SAME");
            }
            

    }
            
    //set y-axis range and labels
    th->GetYaxis()->SetRangeUser(-1, maxrange);
    th->GetXaxis()->SetTitle("Zpps-Zvtx [mm]");
    th->GetXaxis()->SetTitleSize(0.038);
    th->GetYaxis()->SetTitle("N. events");
    th->GetYaxis()->SetTitleSize(0.038);
    
   
    // Draw legend
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw();
    
    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    c1->SaveAs(canvas_title);
    return 1; 

}