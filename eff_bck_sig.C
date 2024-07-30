void eff_bck_sig(TString files[], int nfiles){
    TString canvas_name;
    TString canvas_title;

    //create new Canvas
    TCanvas *c1 = new TCanvas("c1", "canvas_name", 800, 600);

    //create subtitles
    TLegend *l1 = new TLegend(0.5,0.6,0.6,0.89);  

    //define a new function, in this case two gaussian
    //TF1 *gausss = new TF1("gausss","gaus",-200,200); //(0)+gaus(3)

    Float_t range[] = {10., 20., 30., 40., 50.};

    Int_t size_range = 5;

    TGraphErrors* graph1;

    TGraphErrors* graph2;

    //array of colors for histograms
    int colors[] = { kAzure-7, kGreen, kRed-3, kOrange, kPink, kBlue};

    float maxrange = 0.;

    for(int i_file = 0; i_file < nfiles; i_file++){

        TString filename = files[i_file] + ".root"; //s[i_file]

        //open the file
        TFile *f = new TFile(filename,"READ");

        //get histogram from the file
        TH1F* hist = (TH1F*)f->Get(files[i_file]);

        TF1* func = hist->GetFunction("gausss");

        Float_t eff[size_range];
        Float_t efferror[size_range];
        Float_t xerror[size_range];

        for (int i_range=0; i_range < size_range; i_range++){
            Int_t min = hist->FindBin(-1*range[i_range]);
            Int_t max = hist->FindBin(range[i_range]);
            Float_t corte = hist->Integral(min, max);
            cout<<"Corte: "<<corte<<"Total: "<<hist->Integral()<<endl;
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
    graph1->GetYaxis()->SetRangeUser(0, 1.1);
    graph2->GetYaxis()->SetRangeUser(0, 1.1);
    graph1->Draw("AP");
    graph2->Draw("P");

    graph1->SetMarkerSize(5);
    graph2->SetMarkerSize(5);
    graph1->SetMarkerStyle(2);
    graph2->SetMarkerStyle(2);
    graph1->SetMarkerColor(kBlack);
    graph2->SetMarkerColor(kRed);



    canvas_title = canvas_name + ".pdf";

    c1->SetTitle(canvas_name);
    
    c1->SaveAs(canvas_title);
}