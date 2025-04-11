void offset_analysis()
{
    TFile* fin  = new TFile("../input-files/converted_runs_40dB-att_6800mV.root");
    TFile* fout = new TFile("../output-files/offset_analysis.root","RECREATE");
    gROOT->cd();

    TTree* tree = (TTree*) fin->Get("pulse");

    TH1F* hmeans = new TH1F("hmeans","",200,-3.1E-9  ,-2.9E-9  );
    TH1F* h      = new TH1F("h"     ,"",200,-8.1E-9,2.1E-9);

    for(int evt = 0 ; evt<45000 ; evt++)
    {   
        std::cout<<"Event = "<<evt<<std::endl;
        tree->Project("h","time",Form("i_evt==%i",evt));

        hmeans->Fill(h->GetMean());

        h->Reset();
    }

    fout->cd();
    hmeans->Write();
    gROOT->cd();
}