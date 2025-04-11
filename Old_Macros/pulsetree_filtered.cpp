double timestart_cutoff=-8.07E-9;

void pulsetree_filtered()
{
    TFile* fin  = new TFile("../input-files/converted_runs_36dB-att_6800mV.root");
    TFile* fout = new TFile("../output-files/filtered_converted_runs_36dB-att_6800mV.root","RECREATE");
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

    TTree* tree_new = new TTree("pulse","pulse");
    float time_start;
    unsigned int i_evt_2;
    float segment_time_2;
    float channel_2[4][102];
    float time_2[1][102];
    float timeoffsets_2[8];    
    
    //TBranch* time_start_branch = tree_new->Branch("time_start",&time_start,"time_start/F");
    TBranch* i_evt_2_br        = tree_new->Branch("i_evt",&i_evt_2,"i_evt/i");
    TBranch* segment_time_2_br = tree_new->Branch("segment_time",&segment_time_2,"segment_time/F");
    TBranch* channel_2_br      = tree_new->Branch("channel",&channel_2,Form("channel[%i][%i]/F",4,102));
    TBranch* time_2_br         = tree_new->Branch("time",&time_2,Form("time[%i][%i]/F",1,102));
    TBranch* timeoffsets_2_br  = tree_new->Branch("timeoffsets",&timeoffsets_2,Form("timeoffsets[%i]/F",8));
    
    std::cout<<"Entries = "<<tree->GetEntries()<<std::endl;

    for(int entry = 0 ; entry < tree->GetEntries() ; entry++)
    {   
        tree->GetEntry(entry);
        //std::cout<<"Working on event "<<i_evt<<std::endl;
        
        time_start = time[0][0];

        if(time_start>timestart_cutoff)
        {
            //std::cout<<"Conditions fulfilled"<<std::endl;
    
            i_evt_2        = i_evt;
            segment_time_2 = segment_time;
            for(int i = 0 ; i < 4 ; i++){for(int j = 0 ; j < 102 ; j++) channel_2[i][j] = channel[i][j];}
            for(int i = 0 ; i < 1 ; i++){for(int j = 0 ; j < 102 ; j++) time_2[i][j] = time[i][j];}
            for(int i = 0 ; i < 8 ; i++) timeoffsets_2[i] = timeoffsets[i];
            
            i_evt_2_br->Fill();
            segment_time_2_br->Fill();
            channel_2_br->Fill();
            time_2_br->Fill();
            timeoffsets_2_br->Fill();
            tree_new->Fill();
        }
    }


    fout->cd();
    tree_new->Write();
    gROOT->cd();

    fout->Close();
    fin->Close();
}