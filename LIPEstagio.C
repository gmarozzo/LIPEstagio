void LIPEstagio(int nPU[], int size_nPU, int nRes[], int size_nRes){

    //create new Canvas' name
    string canvas_name; 
    
    //create new histogram's name
    string hist_title;

    //create new Canvas' title
    string canvas_title;

    //defining maximum range for y-axis
    float maxrange = 0.;

    //array of colors for histograms
    int colors[] = {kBlue, kRed, kGreen, kOrange, kMagenta, kCyan};

    //defining light speed
    double c = 299.792458; //[mm/ns]

    int choice;
    if(size_nPU == 1 && size_nRes != 1){
        hist_title = "<#mu> = " + to_string(nPU[0]) + " with different resolutions";
        canvas_name = "PU" + to_string(nPU[0]) + " with different Resolutions ";
        choice = 1;
        canvas_title = "PU" + to_string(nPU[0]) + " with different Resolutions" + ".pdf";
    }
    if(size_nRes == 1 && size_nPU != 1){
        hist_title = "Resolution  = " + to_string(nRes[0]) + " with different <#mu>";
        canvas_name = "Resolution = " + to_string(nRes[0]) + " with different PU ";
        choice = 2;
        canvas_title = "Resolution = " + to_string(nRes[0]) + " with different PU " + ".pdf";
    }
    if(size_nRes == 1 && size_nPU == 1){
        hist_title = "<#mu> = " + to_string(nPU[0]) + " and Res = " + to_string(nRes[0]);
        canvas_name = "PU" + to_string(nPU[0]) + " expected Resolution " + to_string(nRes[0]);
        choice = 3;
        canvas_title = "PU" + to_string(nPU[0]) + " with Resolution " + to_string(nRes[0]) + ".pdf";
    }

    //create new Canvas
    TCanvas *c1 = new TCanvas("c1", canvas_name.c_str(), 800, 600);
    
    //create subtitles
    TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);

    for(int i_nPU = 0; i_nPU < size_nPU; i_nPU++){

        for (int i_nRes = 0; i_nRes < size_nRes; i_nRes++){
            cout << "PU = " << nPU[i_nPU] << " and expected resolution = " << nRes[i_nRes] << endl;
            string fileName = "delphes_PU" + to_string(nPU[i_nPU]) + "_Res" + to_string(nRes[i_nRes]) + "ps.root";
            
            //open the file
            TFile *f = new TFile(fileName.c_str(),"READ");

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
            tree->SetBranchAddress("PPSProton.T",&proton_t);
            tree->SetBranchAddress("PPSProton.IsPU",&proton_isPU);
            tree->SetBranchAddress("PPSProton.Pz",&proton_pz);
            tree->SetBranchAddress("PPSProton_size",&proton_size);
            tree->SetBranchAddress("Vertex.Z",&vertex_z);
            tree->SetBranchAddress("Vertex_size",&vertex_size);

            //create histogram for current file
            TH1F *th = new TH1F("th",hist_title.c_str(),30,-60,60);
            
            float dif;
            double c = 299.792458; //[mm/ns]
            //int index;
            for(int i=1; i<tree->GetEntries();i++){
                if(i%10000==0) cout<<"Event n."<<i<<endl;
                tree->GetEntry(i);
                dif=0.;
                float best_dif=10000.;
                int nforw=0, nback=0;    
                for(int j=0; j<5;j++){
                
                    //choose the best pair of protons
                    if(j>proton_size-1) continue;
                    //if(!proton_isPU[j])th->Fill(proton_t[j]);
                    if(proton_pz[j]<0 && proton_isPU[j]==0)  { 
                        nback++;
                        for(int l=0; l<5;l++){
                            if(l>proton_size-1) continue;
                            if(proton_pz[l]>0 && proton_isPU[l]==0) {
                                dif=proton_t[j]-proton_t[l];
                                nforw++;
                            }
                            //choose the best vertex
                            for(int k = 0; k < vertex_size; k++){
                                if(abs(vertex_z[k] - (c/2) * dif ) < best_dif){
                                    best_dif = vertex_z[k] - (c/2) * dif ;
                                    //cout<<best_dif<<endl;
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
            }

            cout<<th->GetEntries()<<endl;

            //set line color for histogram
            int colorIndex = (i_nPU * size_nRes + i_nRes) % (sizeof(colors) / sizeof(colors[0]));
            th->SetLineColor(colors[colorIndex]);

            //update maximum range for y-axis
            if(th->GetMaximum() + th->GetMaximum()/2.45 > maxrange){
                maxrange = th->GetMaximum() + th->GetMaximum()/2.45;
            }
            
            cout<<"Integral " << th->Integral()<<endl;
    
            
            // draw histograms with proper formatting
            if (i_nPU == 0 && i_nRes == 0) {
                th->Draw();
            } else {
                th->Draw("SAME");
            }
            
            //add histogram entry to legend
            if(choice == 1){
                l1->AddEntry(th, Form("Res %d", nRes[i_nRes]));
            }
            if(choice == 2){
                l1->AddEntry(th, Form("PU %d", nPU[i_nPU]));
            }
            //if(choice == 3){
            //    l1->AddEntry(th, Form("PU %d, Res %d", nPU[i_nPU], nRes[i_nRes]));
            //}

             if(choice == 3){
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
        th->Fit("gausss","","",-60,60);

        //obtain the values
        double chi2 = gausss->GetChisquare();
        double p0 = gausss->GetParameter(0);
        double p1 =gausss->GetParameter(1);
        double p2 = gausss->GetParameter(2);
        double p3 = gausss->GetParameter(3);
        double p4 = gausss->GetParameter(4);
        double p5 = gausss->GetParameter(5);
        double gl = gausss->GetNDF(); //degrees of freedom

        cout << "Fit Results of PU = " << nPU[0] << " and expected resolution of "<< nRes[0] << ":" << endl;
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

        l1->AddEntry(th,"Data");
        l1->AddEntry(gausss,"Total fit");
        l1->AddEntry(gaus2,"Signal");
        l1->AddEntry(gaus3,"Background");
        
        //write information
        TString label = Form("Global resolution %.3f ns",res);
        TLatex* text1 = new TLatex(-50,maxrange*0.93,label.Data());
        text1->SetTextSize(0.04);
        text1->SetTextFont(62);
        text1->Draw();
        TString label2 = Form("Resolution single arm %.3f ns",res/sqrt(2));
        TLatex* text2 = new TLatex(-50,maxrange*0.85,label2.Data());
        text2->SetTextSize(0.04);
        text2->SetTextFont(62);
        text2->Draw();
        }
            
            
            //set y-axis range and labels
            th->GetYaxis()->SetRangeUser(-1, maxrange);
            th->GetXaxis()->SetTitle("Zpps-Zvtx [mm]");
            th->GetXaxis()->SetTitleSize(0.038);
            th->GetYaxis()->SetTitle("N. events");
            th->GetYaxis()->SetTitleSize(0.038);
        }
    }

   

    
    // Draw legend
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw();
    
    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    c1->SaveAs(canvas_title.c_str());
    return 1; 

}