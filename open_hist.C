void open_hist(TString files[], int nfiles){
    TString canvas_name;
    TString canvas_title;

    //create new Canvas
    TCanvas *c1 = new TCanvas("c1", "canvas_name", 800, 600);

    //create subtitles
    TLegend *l1 = new TLegend(0.5,0.6,0.6,0.89);  

    //define a new function, in this case two gaussian
    TF1 *gausss = new TF1("gausss","gaus",-200,200); //(0)+gaus(3)

    //array of colors for histograms
    int colors[] = { kAzure-7, kGreen, kRed-3, kOrange, kPink, kBlue};

    float maxrange = 0.;

    for(int i_file = 0; i_file < nfiles; i_file++){

        TString filename = files[i_file] + ".root"; //s[i_file]

        //open the file
        TFile *f = new TFile(filename,"READ");

        //get histogram from the file
        TH1F* combined_hist = (TH1F*)f->Get(files[i_file]);

        if(i_file == 0){
            combined_hist->Draw("HISTO");
            l1->AddEntry(combined_hist, files[i_file]);
            canvas_name = files[i_file];

            //define a initial value for each parameter (0,3 = norm; 1,4 = mean 2,5 = width) (0,1,2 1st gaussian; 3,4,5 2nd gaussian) 
            gausss->SetParameter(0,10);
            gausss->SetParameter(1,0);
            gausss->SetParameter(2,10);
        
            //define the limits for each parameter
            gausss->SetParLimits(0,0,1);
            gausss->SetParLimits(1,-50,50);
            gausss->SetParLimits(2,0,100);

            //fit the date with the function defined (-60,60 x axis limits)
            combined_hist->Fit("gausss","","",-200,200);

            //obtain the values
            double chi2 = gausss->GetChisquare();
            double p0 = gausss->GetParameter(0);
            double p1 =gausss->GetParameter(1);
            double p2 = gausss->GetParameter(2);
            double gl = gausss->GetNDF(); //degrees of freedom

            cout << "----------------- Fit Results of " << files[i_file] << " -----------------" << endl;
            cout << "Chi2: " << chi2 << endl;
            cout << "Norm: " << p0 << " ± " << gausss->GetParError(0) << endl;
            cout << "Mean: " << p1 << " ± " << gausss->GetParError(1) << endl;
            cout << "Width: " << p2 << " ± " << gausss->GetParError(2) << endl;
            cout << "N. degrees of freedom: " << gl << endl;

            //draw the gaussian with fit values (po, p1, p2)
            TF1 *gaus2 = new TF1("gaus2","gaus",-200,200);
            gaus2->SetParameter(0,p0);
            gaus2->SetParameter(1,p1);
            gaus2->SetParameter(2,p2);
            gaus2->Draw("SAME");
            gaus2->SetLineWidth(3);
            gaus2->SetLineStyle(3);

            //define line color
            int color_index = i_file * 2;
            gaus2->SetLineColor(colors[color_index]);

            TString l = "Total fit of " + files[i_file];
            l1->AddEntry(gaus2, l);
        } else{
            combined_hist->Draw("HISTO SAME");
            l1->AddEntry(combined_hist, files[i_file]);
            canvas_name += " and " + files[i_file];
            
            //define a initial value for each parameter (0,3 = norm; 1,4 = mean 2,5 = width) (0,1,2 1st gaussian; 3,4,5 2nd gaussian) 
            gausss->SetParameter(0,10);
            gausss->SetParameter(1,0);
            gausss->SetParameter(2,10);
        
            //define the limits for each parameter
            gausss->SetParLimits(0,0,1);
            gausss->SetParLimits(1,-50,50);
            gausss->SetParLimits(2,0,100);

            //fit the date with the function defined (-60,60 x axis limits)
            combined_hist->Fit("gausss","","",-200,200);

            //obtain the values
            double chi2 = gausss->GetChisquare();
            double p0 = gausss->GetParameter(0);
            double p1 =gausss->GetParameter(1);
            double p2 = gausss->GetParameter(2);
            double gl = gausss->GetNDF(); //degrees of freedom

            cout << "----------------- Fit Results of " << files[i_file] << " -----------------" << endl;
            cout << "Chi2: " << chi2 << endl;
            cout << "Norm: " << p0 << " ± " << gausss->GetParError(0) << endl;
            cout << "Mean: " << p1 << " ± " << gausss->GetParError(1) << endl;
            cout << "Width: " << p2 << " ± " << gausss->GetParError(2) << endl;
            cout << "N. degrees of freedom: " << gl << endl;
            
            //draw the gaussian with fit values (po, p1, p2)
            TF1 *gaus2 = new TF1("gaus2","gaus",-200,200);
            gaus2->SetParameter(0,p0);
            gaus2->SetParameter(1,p1);
            gaus2->SetParameter(2,p2);
            gaus2->Draw("SAME");
            gaus2->SetLineWidth(3);
            gaus2->SetLineStyle(3);

            //define line color
            int color_index = i_file * 2;
            gaus2->SetLineColor(colors[color_index]);

            TString l = "Total fit of " + files[i_file];
            l1->AddEntry(gaus2, l);
        }

        //update maximum range for y-axis
        if(combined_hist->GetMaximum() + combined_hist->GetMaximum()/6.> maxrange){
            maxrange = combined_hist->GetMaximum() + combined_hist->GetMaximum()/6.;
            //cout<< "maxrange "<<maxrange<<endl;
        }

        /*if(i_file == nfiles - 1){
            combined_hist->GetYaxis()->SetRangeUser(0, maxrange);
            combined_hist->GetXaxis()->SetTitle("Z_{reconstructed particle} - #frac{c}{2}#Deltat_{protons} [mm]");
            combined_hist->GetXaxis()->SetTitleSize(0.038);
            combined_hist->GetYaxis()->SetTitle("N. events");
            combined_hist->GetYaxis()->SetTitleSize(0.038);
        }*/
    }

    // Set the y-axis range and labels after drawing all histograms
    if(maxrange > 0){
        gPad->Update();
        TH1* hist = (TH1*)gPad->GetListOfPrimitives()->FindObject(files[0]);
        if (hist) {
            hist->GetYaxis()->SetRangeUser(0, maxrange); // Set range with some padding
            hist->GetXaxis()->SetTitle("Z_{reconstructed particle} - #frac{c}{2}#Deltat_{protons} [mm]");
            hist->GetXaxis()->SetTitleSize(0.038);
            hist->GetYaxis()->SetTitle("N. events");
            hist->GetYaxis()->SetTitleSize(0.038);
        }
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