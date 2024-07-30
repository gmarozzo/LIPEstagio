void ler() {


  cout << "[Delphes] Reading ROOT file." << endl;
  TFile *f = new TFile("delphes_test.root");

  cout << "[Delphes] Geting TTree from root file." << endl;
  TTree *t = (TTree*) f -> Get( "Delphes" );

  cout << "[Delphes] Declaring TH1D (your plot object)." << endl;
  TH1D *h1 = new TH1D( "T", "PPSProton.T", 100, 50, 150 );
  //TH1D *h2 = new TH1D( "h", "Another one (GeV)", 100, 50, 150 );

  // These are the variables that will receive the data in the TTree
  float T[20];
  int isPU[20];

  // This is how you say to the TTree that these are the variables taking in the information
  t->SetBranchAddress("PPSProton.T",&T);
  t->SetBranchAddress("PPSProton.IsPU",&isPU);

  // Now will make a for loop on the TTree entries
  cout << "[Delphes]  Looping on the TTree." << endl;
    for (int iEnt = 0; iEnt < t->GetEntries(); ++iEnt) {

    // This is how you get an entry from the TTree, i.e. this is how you load the values from a given entry to your variables
   t->GetEntry(iEnt);

    // We now loop on the objects of the event which are encoded in the vectors:
   for (int iObj = 0; iObj < 20; ++iObj) {

      // We want the Pull Up (IsPU) equal to 1
     if ((isPU)[iObj] == 1 ) h1->Fill((T)[iObj] , 1);
    }
    for (int iObj = 0; iObj < 20; ++iObj) {
      

  }
    }
  // Now we wanna draw our object. You can create a TCanvas for this where you can control the width and the height, but for this demonstration it's not necessary
   //Draw TCanvas;
  TCanvas *c1 = new TCanvas("c1", "PPSProton.T with PPSPronton.IsPU = 1", 100, 100);

  h1->Draw();
  
}