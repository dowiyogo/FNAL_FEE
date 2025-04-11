void MACRO_SquarePulse()
{
    gStyle->SetOptFit(0111);
    TFile* f = new TFile("../data/signal-diff-risetime/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h = new TH2F("h","h",200,-30E-9,30E-9,200,-.04,.04);
    pulse->Project("h","channel[0]:time[0]","");

    h->Draw("colz");

    h->GetXaxis()->SetTitle("Time(s)");
    h->GetYaxis()->SetTitle("Voltage(V)");
}
