#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH2F.h"

int time_diference(){

  TFile *f = new TFile("delphes_test.root","READ");
  TTree *tree = (TTree*) f->Get("Delphes");
  Int_t proton_size, vertex_size;
  Float_t proton_t[20], proton_isPU[20], proton_pz[20], vertex_z[100];
  tree->SetBranchAddress("PPSProton.T",&proton_t);
  tree->SetBranchAddress("PPSProton.IsPU",&proton_isPU);
  tree->SetBranchAddress("PPSProton.Pz",&proton_pz);
  tree->SetBranchAddress("PPSProton_size",&proton_size);
  tree->SetBranchAddress("Vertex.Z",&vertex_z);
  tree->SetBranchAddress("Vertex_size",&vertex_size);

  TH2F *th = new TH2F("th","Proton time",100,-1,1,100,-200,200);
  //TH2F *th2 = new TH2F("th2","Time diff vs z",100,-10,10,-200,200);
  float dif;
  for(int i=1; i<tree->GetEntries();i++){
    if(i%10000==0) cout<<"Event n."<<i<<endl;
    tree->GetEntry(i);
    dif=0.;    
    for(int j=0; j<5;j++){
      if(j>proton_size-1) continue;
      //if(!proton_isPU[j])th->Fill(proton_t[j]);
      if(proton_pz[j]<0 && !proton_isPU[j])  dif-=proton_t[j];
      if(proton_pz[j]>0 && !proton_isPU[j]) dif+=proton_t[j];
    //th->Fill(0.,0.);
      if(abs(dif)>0.0001 && vertex_size==1)th->Fill(dif,vertex_z[0],1.);
    //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
    }
  }

  cout<<th->GetEntries()<<endl;
  th->Draw("COLZ");

  
  return 1;
}