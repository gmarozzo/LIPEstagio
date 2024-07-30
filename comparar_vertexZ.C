void comparar_vertexZ(){
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

    TH1F *th = new TH1F("th","Deviation",100,-50,50);
    //TH2F *th2 = new TH2F("th2","Time diff vs z",100,-10,10,-200,200);
    float dif;
    double c = 0.299792458; //[m/ns]
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
        if(abs(dif)>0.0001 && vertex_size==1){
            float vertex_calculado = (c/2) * dif;
            float desvio = vertex_z[0] - vertex_calculado;
            th->Fill(desvio);
        }
        //cout<<"diff: "<<dif<<" vertex: "<<vertex_z[0]<<endl;
        }
    }

    cout<<th->GetEntries()<<endl;
    th->Draw("COLZ");

    return 1;
}