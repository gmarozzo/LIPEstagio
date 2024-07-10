#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2F.h"

int LIPEstagio(){

 TFile *f = new TFile("delphes_test.root","READ");
    TTree *tree = (TTree*) f->Get("Delphes");
    Int_t proton_size, vertex_size;
    Float_t proton_t[20], proton_isPU[20], proton_pz[20], vertex_z[100];
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

    TH1F *th = new TH1F("th","Deviation",30,-30,30);
    TH2F *th2 = new TH2F("th2","Time diff vs z",100,-2,2,100,-200,200);
    float dif;
    float c = 299792458/pow(10,6); //[mm/ns]
    for(int i=1; i<tree->GetEntries();i++){
        if(i%10000==0) cout<<"Event n."<<i<<endl;
        tree->GetEntry(i);
        dif=0.;
	int nback=0;
	int nforw=0;
        for(int j=0; j<5;j++){
        if(j>proton_size-1) continue;
        //if(!proton_isPU[j])th->Fill(proton_t[j]);
        if(proton_pz[j]<0 && proton_isPU[j]==0 && nback==0)  {
	  dif-=proton_t[j];
	  nback++;
	}
        if(proton_pz[j]>0  && proton_isPU[j]==0 && nforw==0) {
	  dif+=proton_t[j];
	  nforw++;
	}
        //th->Fill(0.,0.);
        if(abs(dif)>0.0001 && abs(dif) < 5 && vertex_size==1){
          float vertex_calculado = (c/2) * dif;
          float desvio = vertex_z[0] + vertex_calculado;
	  //cout<<dif<<endl;
          th->Fill(desvio);
	  //th2->Fill(dif,vertex_z[0]);
        }
        //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
        }
    }

    th->Draw("COLZ");
    th->Fit("gaus");

    return 1;
}
