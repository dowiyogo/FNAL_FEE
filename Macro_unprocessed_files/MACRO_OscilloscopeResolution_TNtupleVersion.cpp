const int nevents  = 500;
const int nchannel = 4;
const int time_div = 202;

void MACRO_OscilloscopeResolution_TNtupleVersion()
{
    TFile* f     = new TFile("../data/signal-diff-limbandwidth-20ns/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    Float_t channel_branch[nchannel][time_div];
    Float_t time_branch[1][time_div];
    Double_t events[nevents][time_div][3];

    pulse->SetBranchAddress("channel",&channel_branch);
    pulse->SetBranchAddress("time",&time_branch);
    for(int evt = 0 ; evt < nevents ; evt++){
        pulse->GetEntry(evt);
        for(int time_slot = 0 ; time_slot < time_div ; time_slot++){
            events[evt][time_slot][0] = channel_branch[0][time_slot];
            events[evt][time_slot][1] = channel_branch[1][time_slot];
            events[evt][time_slot][2] = time_branch[0][time_slot];
        }
    }

    TH1F* h = new TH1F("h","",100,1E-9,1.5E-9);
    for(int evt = 0 ; evt < nevents ; evt++){
        double t1 = 0;
        double t2 = 0;
        for(int time_slot = 0 ; time_slot < time_div ; time_slot++){
            if(events[evt][time_slot][0]>0) {t1 = events[evt][time_slot][2]; std::cout<<"t1 = "<<t1<<std::endl;;break;}
        }
        for(int time_slot = 0 ; time_slot < time_div ; time_slot++){
            if(events[evt][time_slot][1]>0) {t2 = events[evt][time_slot][2]; std::cout<<"t2 = "<<t2<<std::endl;; break;}
        }
        std::cout<<std::setprecision(15)<<"dtime = "<<(t2-t1)<<std::endl;
        h->Fill(t2-t1);
    }

    h->Draw();
}

//TGraph* g = new TGraph(time_div);
    //for(int point = 0 ; point < time_div ; point++){
    //    std::cout<<" time = "<<events[0][point][2]<<"(s)   "<<" voltage = "<<events[0][point][1]<<"(V)"<<std::endl;
    //    g->SetPoint(point,events[0][point][2],events[0][point][0]);
    //}
    //g->Draw("AP");
