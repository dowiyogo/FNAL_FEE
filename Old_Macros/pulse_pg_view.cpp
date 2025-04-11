void pulse_pg_view()
{
    TFile* f  = new TFile("../input-files/converted_runs_NOAtt_PGoutput_converted.root");
    TTree* tree = (TTree*) f->Get("pulse");
    TH2F* h = new TH2F();
    tree->Draw("channel[2]:time>>h","i_evt==110","l");
}
