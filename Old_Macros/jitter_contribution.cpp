void jitter_contribution()
{
    TFile* fin  = new TFile("../input-files/converted_runs_36dB-att_6800mV_converted.root");
    TTree* tree = (TTree*) fin->Get("pulse");

    TH1F* h = new TH1F("h","",300,1.92E-9,2.E-9);

    tree->Project("h","LP2_50[1]-LP2_50[0]","");

    h->Draw("E");

    h->SetTitle("36dB Attenuation Config.; #Delta T (s); #Measurements");
}
