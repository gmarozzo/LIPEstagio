#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2F.h"

int LIPEstagio(){

 TFile *f = new TFile("delphes_test_PU3.root","READ");
    TTree *tree = (TTree*) f->Get("Delphes");
    Int_t proton_size, vertex_size;
    Bool_t proton_isPU[20];
    Float_t proton_t[20], proton_pz[20], vertex_z[100];
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

    TH1F *th = new TH1F("th","Deviation",30,-60,60);
    //TH2F *th2 = new TH2F("th2","Time diff vs z",100,-10,10,-200,200);
    float dif;
    double c = 299.792458; //[mm/ns]
    int index;
    for(int i=1; i<tree->GetEntries();i++){
        if(i%10000==0) cout<<"Event n."<<i<<endl;
        tree->GetEntry(i);
        dif=0.;
        float best_dif=10000.;
        int nforw=0, nback=0;    
        for(int j=0; j<5;j++){
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
            float desvio = best_dif;
            //cout<<desvio<<endl;
            th->Fill(desvio);
        }
        //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
        }
    }

    cout<<th->GetEntries()<<endl;
    th->GetYaxis()->SetRangeUser(-1,1000);
    th->Draw("COLZ");
    

    TF1 *gausss = new TF1("gausss","gaus(0)+gaus(3)",-60,60);
    gausss->SetParameter(0,160);
    gausss->SetParameter(1,0);
    gausss->SetParameter(2,10);
    gausss->SetParameter(3,100);
    gausss->SetParameter(4,0);
    gausss->SetParameter(5,20);
    gausss->SetParLimits(0,1,10000);
    gausss->SetParLimits(1,-50,50);
    gausss->SetParLimits(2,0,100);
    gausss->SetParLimits(3,1,10000);
    gausss->SetParLimits(4,-50,50);
    gausss->SetParLimits(5,0,100);
    th->Fit("gausss","","",-60,60);
    double chi2 = gausss->GetChisquare();
    double p0 = gausss->GetParameter(0);
    double p1 =gausss->GetParameter(1);
    double p2 = gausss->GetParameter(2);
    double p3 = gausss->GetParameter(3);
    double p4 = gausss->GetParameter(4);
    double p5 = gausss->GetParameter(5);
    double gl = gausss->GetNDF();
    cout << "Fit Results:" << endl;
    cout << "Chi2: " << chi2 << endl;
    cout << "signal norm: " << p0 << " ± " << gausss->GetParError(0) << endl;
    cout << "Signal mean: " << p1 << " ± " << gausss->GetParError(1) << endl;
    cout << "Sigma width: " << p2 << " ± " << gausss->GetParError(2) << endl;
    cout << "N. degrees of freedom: " << gl << endl;
    cout << "Bkg norm: " << p3 << " ± " << gausss->GetParError(3) << endl;
    cout << "Bkg mean: " << p4 << " ± " << gausss->GetParError(4) << endl;
    cout << "Bkg width: " << p5 << " ± " << gausss->GetParError(5) << endl;
    float res =  p2/(c/2);
    cout << res <<endl;

    TF1 *gaus2 = new TF1("gaus2","gaus",-60,60);
    TF1 *gaus3 = new TF1("gaus3","gaus",-60,60);
    gaus2->SetParameter(0,p0);
    gaus2->SetParameter(1,p1);
    gaus2->SetParameter(2,p2);
    gaus3->SetParameter(0,p3);
    gaus3->SetParameter(1,p4);
    gaus3->SetParameter(2,p5);
    gaus2->SetLineColor(kRed);
    gaus3->SetLineColor(kBlue);
    gaus2->Draw("SAME");
    gaus2->SetLineWidth(3);
    gaus2->SetLineStyle(3);
    gaus3->Draw("SAME");
    gaus3->SetLineWidth(3);
    gaus3->SetLineStyle(3);

    TLegend *l1 = new TLegend(0.68,0.6,0.9,0.89);
    l1->AddEntry(th,"Data");
    l1->AddEntry(gausss,"Total fit");
    l1->AddEntry(gaus2,"Signal");
    l1->AddEntry(gaus3,"Background");
    l1->SetBorderSize(0);
    
    TString label = Form("Global resolution %.3f ns",res);
    TLatex* text = new TLatex(-50,900,label.Data());
    text->Draw();
    TString label2 = Form("Resolution single arm %.3f ns",res*sqrt(2));
    TLatex* text2 = new TLatex(-50,850,label2.Data());
    text2->Draw();

    l1->Draw("SAME");

    gStyle->SetOptStat(0); // não ver caixinha
    return 1; 
}
