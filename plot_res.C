void plot_res(TString nPU, TString nRes[], Double_t size_nRes){
    //create new Canvas' name
    TString canvas_name = "Ploting different Resolutions with PU" + nPU; 
    
    //create new histogram's name
    TString hist_title = "Ploting different Resolutions with <#mu> = " + nPU;

    //create new Canvas' title
    TString canvas_title = "Ploting different Resolutions with PU " + nPU + ".pdf";

    //defining maximum range for y-axis
    float maxrange = 0.;

    //defining light speed
    double c = 299.792458; //[mm/ns]

    //defining vector to save single arm resolutions an it's errors
    Double_t res_single_arm[int(size_nRes)];
    Double_t reserr_single_arm[int(size_nRes)];

    //defining vector to save Res values
    Double_t Res[int(size_nRes)];

    //create new Canvas
    TCanvas *c1 = new TCanvas("c1", canvas_name, 800, 600);
    
    //create subtitles
    TLegend *l1 = new TLegend(0.68,0.2,0.9,0.2);

    for (int i_nRes = 0; i_nRes < size_nRes; i_nRes++){
        cout << "PU = " << nPU << " and expected resolution = " << nRes[i_nRes] << endl;
        TString fileName = "delphes_PU" + nPU + "_Res" + nRes[i_nRes] + "ps.root";
            
        //save Res value in vector
        Res[i_nRes] = stoi(string(nRes[i_nRes].Data()));

        //open the file
        TFile *f = new TFile(fileName,"READ");

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
        TH1F *th = new TH1F("th",hist_title,30,-60,60);
           
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
                    if(proton_pz[j]<0 && proton_isPU[j]==0)  { //
                        nback++;
                        for(int l=0; l<5;l++){
                            if(l>proton_size-1) continue;
                            if(proton_pz[l]>0 && proton_isPU[l]==0) { //
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
                    if(vertex_size < 5 && nback > 0 && nforw > 0) {
                        //float desvio = best_dif;
                        //cout<<desvio<<endl;
                        th->Fill(best_dif);
                    }
                    
                //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
                }
        }
            
        cout<< "Entries" << th->GetEntries()<<endl;
        cout<<"Integral " << th->Integral()<<endl;
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
        double p2 = gausss->GetParameter(2);
        double p2err = gausss->GetParError(2);

        //signle arm resolution calculation
        float res =  (p2/(c/2))/sqrt(2)*1000;
        float reserr = (p2err/(c/2))/sqrt(2)*1000;

        //save single arm resolution and it's error
        res_single_arm[i_nRes] = res;
        reserr_single_arm[i_nRes] = reserr;
    }

    // Create a new canvas for the TGraph
    //TCanvas *c2 = new TCanvas("c2", "TGraph Canvas", 800, 600);

    //make graph Calculated Resolution Vs Theoretical resolution
    TGraph* gr = new TGraph(size_nRes, Res, res_single_arm);

    //calculate maximum y range
    for(int i = 0; i < size_nRes; i++){
        if(res_single_arm[i] > maxrange){
            maxrange = res_single_arm[i];
        }
    }
    gr->Draw("*");
    gr->SetTitle("Calculated Resolution Vs Theoretical resolution");

    //set y-axis range and labels
    gr->GetYaxis()->SetRangeUser(-1, maxrange*1.25);
    gr->GetXaxis()->SetTitle("Theoretical resolution [ps]");
    gr->GetXaxis()->SetTitleSize(0.038);
    gr->GetYaxis()->SetTitle("Calculated resolution [ps]");
    gr->GetYaxis()->SetTitleSize(0.038);
    gr->SetMarkerSize(2);
    gr->SetMarkerColor(kOrange);

    // Draw legend
    l1->AddEntry(gr,"Points");
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0375);
    l1->SetTextFont(62);
    l1->Draw();
    
    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    c1->SaveAs(canvas_title);
    return 1; 
}