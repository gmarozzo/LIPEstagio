void eff_bck_sig(TString files[], int nfiles){
    TString canvas_name;
    TString canvas_title;

    //create new Canvas
    TCanvas *c1 = new TCanvas("c1", "canvas_name", 800, 600);

    c1->Divide(2,1);

    //create subtitles
    TLegend *l1 = new TLegend(0.6,0.7,0.9,0.89);  
    TLegend *l2 = new TLegend(0.6,0.7,0.9,0.89);  

    //define a new function, in this case two gaussian
    //TF1 *gausss = new TF1("gausss","gaus",-200,200); //(0)+gaus(3)

    Float_t range[] = {10., 20., 30., 40., 50.};

    Int_t size_range = 5;

    Float_t efferror[size_range];
    Float_t xerror[size_range];

    TGraphErrors* graph1; //graphic of the first file
    TGraphErrors* graph2; //graphic of the second file
    TGraphErrors* graph3; //graphic of the variable S/sqrt(S+B)

    //array of colors for histograms
    int colors[] = { kAzure-7, kGreen, kRed-3, kPink, kBlue};

    float maxrange = 0.;

    for(int i_file = 0; i_file < nfiles; i_file++){

        TString filename = files[i_file] + ".root"; //s[i_file]

        //open the file
        TFile *f = new TFile(filename,"READ");

        //get histogram from the file
        TH1F* hist = (TH1F*)f->Get(files[i_file]);

        TF1* func = hist->GetFunction("gausss");

        Float_t eff[size_range];

        for (int i_range=0; i_range < size_range; i_range++){
            Int_t min = hist->FindBin(-1*range[i_range]);
            Int_t max = hist->FindBin(range[i_range]);
            Float_t corte = hist->Integral(min, max);
            //cout<<"Corte: "<<corte<<"Total: "<<hist->Integral()<<endl;
            eff[i_range] = corte/hist->Integral();
            efferror[i_range] = 0.;
            xerror[i_range] = 0.;
            
        }
        if(i_file == 0){
            graph1 = new TGraphErrors(size_range, range, eff, xerror, efferror);
            canvas_name = files[i_file];
            //hist->Draw();
            
        }
        else{
            graph2 = new TGraphErrors(size_range, range, eff, xerror, efferror);
            canvas_name += " and " + files[i_file];
            //hist->Draw("SAME");
        } 
    }
    Int_t nS[] = {1, 10};
    Int_t size_nS = 2;
    Int_t nB[] = {1, 10};
    Int_t size_nB = 2;
    Float_t variable[size_range];
    Double_t* S = graph2->GetY();
    Double_t* B = graph1->GetY();
    for (int n_s = 0; n_s < size_nS; n_s++){
        for(int n_b = 0; n_b < size_nB; n_b++){
            if((nS[n_s] == 1  && nB[n_b] == 1) || (nS[n_s] != nB[n_b])){
                for(int x = 0; x < size_range; x++){
                    variable[x] = S[x] * nS[n_s] / sqrt(S[x] * nS[n_s] + B[x] * nB[n_b]);
                }
                c1->cd(2);
                graph3 = new TGraphErrors(size_range, range, variable, xerror, efferror);

                if(n_s == 0 && n_b == 0){
                    graph3->Draw("AP");
                } else{
                    graph3->Draw("P");
                }
                
                //set line color for histogram
                int colorIndex = (n_s * size_nS + n_b) % (sizeof(colors) / sizeof(colors[0]));
                graph3->SetMarkerColor(colors[colorIndex]);

                graph3->GetYaxis()->SetTitle("S #times n_{s} / #sqrt{S #times n_{S} + B #times n_{B}}");
                graph3->GetYaxis()->SetTitleSize(0.035);
                graph3->GetXaxis()->SetTitle("Cut [mm]");
                graph3->GetXaxis()->SetTitleSize(0.038);
                graph3->GetYaxis()->SetRangeUser(0, 1.5);
                graph3->SetTitle("Nhanha");
                graph3->SetMarkerSize(1);
                graph3->SetMarkerStyle(83);
                
                TString leg = Form("n_{S}=%d & n_{B}=%d", nS[n_s], nB[n_b]);
                l2->AddEntry(graph3, leg);
                l2->Draw();
            }
        }
    }
    
    
    c1->cd(1);
    graph1->GetYaxis()->SetTitle("Efficiency");
    graph1->GetYaxis()->SetTitleSize(0.038);
    graph1->GetXaxis()->SetTitle("Cut [mm]");
    graph1->GetXaxis()->SetTitleSize(0.038);
    graph1->GetYaxis()->SetRangeUser(0, 1.5);
    graph1->SetTitle("Efficiency");
    graph1->Draw("AP");
    graph2->Draw("P");

    graph1->SetMarkerSize(1);
    graph1->SetMarkerStyle(83);
    graph1->SetMarkerColor(kAzure+7);

    graph2->SetMarkerSize(1);
    graph2->SetMarkerStyle(83);
    graph2->SetMarkerColor(kRed-4);

    l1->AddEntry(graph1, "Background data");
    l1->AddEntry(graph2, "Signal data");
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0275);
    l1->SetTextFont(42);
    l1->Draw();

    
    
    l2->SetBorderSize(0);
    l2->SetTextSize(0.0275);
    l2->SetTextFont(42);
    



    canvas_title = canvas_name + "eff2.pdf";

    c1->SetTitle(canvas_name);
    
    c1->SaveAs(canvas_title);
}