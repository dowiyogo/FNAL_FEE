void pulse_configs_view()
{
    TFile* f40  = new TFile("../input-files/converted_runs_40dB-att_6800mV_converted.root");
    TFile* f46  = new TFile("../input-files/converted_runs_46dB-att_6800mV_converted.root");
    TFile* f50  = new TFile("../input-files/converted_runs_50dB-att_6800mV_converted.root");
    TFile* f56  = new TFile("../input-files/converted_runs_56dB-att_6800mV_converted.root");
    TFile* f60  = new TFile("../input-files/converted_runs_60dB-att_6800mV_converted.root");
    TFile* f66  = new TFile("../input-files/converted_runs_66dB-att_6800mV_converted.root");

    TTree* tree40 = (TTree*) f40->Get("pulse");
    TTree* tree46 = (TTree*) f46->Get("pulse");
    TTree* tree50 = (TTree*) f50->Get("pulse");
    TTree* tree56 = (TTree*) f56->Get("pulse");
    TTree* tree60 = (TTree*) f60->Get("pulse");
    TTree* tree66 = (TTree*) f66->Get("pulse");

    TH2F* h40 = new TH2F();
    TH2F* h46 = new TH2F();
    TH2F* h50 = new TH2F();
    TH2F* h56 = new TH2F();
    TH2F* h60 = new TH2F();
    TH2F* h66 = new TH2F();
    h40->SetLineColor(kRed);
    h46->SetLineColor(kGreen);
    h50->SetLineColor(kBlack);
    h56->SetLineColor(kBlue);
    h60->SetLineColor(kViolet);
    h66->SetLineColor(kCyan);

    tree40->Draw("channel[1]:time>>h40","i_evt==0","l");
    tree46->Draw("channel[1]:time>>h46","i_evt==0","l same");
    tree50->Draw("channel[1]:time>>h50","i_evt==0","l same");
    tree56->Draw("channel[1]:time>>h56","i_evt==0","l same");
    tree60->Draw("channel[1]:time>>h60","i_evt==0","l same");
    tree66->Draw("channel[1]:time>>h66","i_evt==0","l same");

    TLegend* legend = new TLegend(0.75,gPad->GetBottomMargin(),1-gPad->GetRightMargin(),gPad->GetBottomMargin()+0.2,"","NDC");
    legend->AddEntry(h40,"40dB Config.","l");
    legend->AddEntry(h46,"46dB Config.","l");
    legend->AddEntry(h50,"50dB Config.","l");
    legend->AddEntry(h56,"56dB Config.","l");
    legend->AddEntry(h60,"60dB Config.","l");
    legend->AddEntry(h66,"66dB Config.","l");
    
    legend->Draw();
}
