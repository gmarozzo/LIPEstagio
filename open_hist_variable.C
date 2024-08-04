void open_hist_variable(TString files[], int nfiles){
    TString canvas_name;
    TString canvas_title;

    //create new Canvas
    TCanvas *c1 = new TCanvas("c1", "canvas_name", 800, 600);

    //create subtitles
    TLegend *l1 = new TLegend(0.65,0.65,0.75,0.89);  

    //define a new function, in this case two gaussian
    TF1 *gausss = new TF1("gausss","gaus",-200,200); //(0)+gaus(3)

    //array of colors for histograms
    int colors[] = { kAzure-7, kGreen, kRed-3, kOrange, kPink, kBlue};

    float maxrange = 0.;

    for(int i_file = 0; i_file < nfiles; i_file++){

        TString filename = "Variable_Output_Signal_withdecay_" + files[i_file] + "ps.root"; //s[i_file]

        //open the file
        TFile *f = new TFile(filename,"READ");

        //get histogram from the file
        TString hist_name = "Variable_Output_Signal_withdecay_" + files[i_file] + "ps";
        TH1F* combined_hist = (TH1F*)f->Get(hist_name);
        combined_hist->SetTitle("Signal Vs Background");

        int nbins = combined_hist->GetNbinsX();
        int validPoints = 0;

        // Count valid points
        for (int i = 1; i <= nbins; ++i) {
            if (combined_hist->GetBinContent(i) != 0) {
                validPoints++;
            }
        }
        double *x = new double[validPoints];
        double *y = new double[validPoints];
        int index = 0;

        // Fill the arrays with the bin centers and contents
        for (int i = 1; i <= nbins; ++i) {
            if (combined_hist->GetBinContent(i) != 0) {
                x[index] = combined_hist->GetBinCenter(i);
                y[index] = combined_hist->GetBinContent(i);
                index++;
            }
        }

        // Create the TGraph
        TGraph *graph = new TGraph(validPoints, x, y);
        graph->SetLineColor(colors[i_file % 6]);
        graph->SetMarkerColor(colors[i_file % 6]);
        graph->SetMarkerStyle(20);

        if(i_file == 0){
            graph->Draw("AP");
            TString legend = "Variable with Res = " + files[0];
            l1->AddEntry(graph, legend);
            canvas_name = "Comparison between variables with Res = " + files[0];
        } else{
            graph->Draw("P");
            TString legend = "Variable with Res = " + files[i_file];
            l1->AddEntry(graph, legend);
            canvas_name += ", Res = " + files[i_file];
            /*TString leg = "Fit of " + files[i_file];
            l1->AddEntry(func, "Signal fit");
            cout << " " << endl;
            cout << "------------------ Fit of " << files[i_file] << " ------------------" << endl << endl;
            cout << "Norm: " << func->GetParameter(0) << " ± " << func->GetParError(0) << endl;
            cout << "Mean: " << func->GetParameter(1) << " ± " << func->GetParError(1) << endl;
            cout << "Width: " << func->GetParameter(2) << " ± " << func->GetParError(2) << endl;*/
        }
        graph->SetTitle("Comparison of S / #sqrt{S + B} between different resolutions");
        graph->GetYaxis()->SetTitle("S / #sqrt{S + B}");
        graph->GetYaxis()->SetTitleSize(0.035);
        //gPad->SetLogy();
        graph->GetXaxis()->SetTitle("Cut [mm]");
        graph->GetXaxis()->SetTitleSize(0.038);
        //graph->GetYaxis()->SetRangeUser(0, 6); // 4.5
        graph->SetTitle("S / #sqrt{S + B}");

    }

    canvas_title = canvas_name + ".pdf";

    c1->SetTitle(canvas_name);

    // Draw legend
    l1->SetBorderSize(0);
    l1->SetTextSize(0.0275);
    l1->SetTextFont(42);
    l1->Draw();
    
    // don't show entries, etc.
    gStyle->SetOptStat(0); 

    c1->SaveAs(canvas_title);
}