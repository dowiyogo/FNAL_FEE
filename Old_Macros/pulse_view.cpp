void pulse_view()
{
    TFile* fin  = new TFile("../input-files/converted_runs_40dB-att_6800mV_converted.root");
    TTree* tree = (TTree*) fin->Get("pulse");

    TH2F* h = new TH2F("h","",300,-8E-9,2E-9,300,-800,200);

    tree->Project("h","channel[1]:time","");

    h->Draw("colz");

    h->SetTitle("Pulse Overview; time(s); voltage(mV)");
}
