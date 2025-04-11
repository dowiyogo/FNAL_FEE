void pulsetree_newbranch()
{
    TFile* fin  = new TFile("../input-files/converted_runs_40dB-att_6800mV.root");
    TFile* fout = new TFile("../output-files/new_converted_runs_40dB-att_6800mV.root","RECREATE");
    gROOT->cd();

    TTree* tree = (TTree*) fin->Get("pulse");
    unsigned int i_evt;
    float segment_time;
    float channel[4][102];
    float time[1][102];
    float timeoffsets[8];    
    tree->SetBranchAddress("i_evt",&i_evt);
    tree->SetBranchAddress("segment_time",&segment_time);
    tree->SetBranchAddress("channel",&channel);
    tree->SetBranchAddress("time",&time);
    tree->SetBranchAddress("timeoffsets",&timeoffsets);

    TTree* tree_new = tree->CloneTree();
    float time_start;
    TBranch* time_start_branch = tree_new->Branch("time_start",&time_start,"time_start/F");

    for(int entry = 0 ; entry < tree_new->GetEntries() ; entry++)
    {   
        std::cout<<"Working on event "<<i_evt<<std::endl;
        tree_new->GetEntry(entry);
        time_start = time[0][0];

        time_start_branch->Fill();
    }

    fout->cd();
    tree_new->Write();
    gROOT->cd();

    fout->Close();
    fin->Close();
}