double rms(TH1F* h){
    double result = 0;
    double N = 0;
    for(int bin = 1 ; bin <= h->GetNbinsX() ; bin++){
        result += TMath::Power(h->GetBinCenter(bin),2)*h->GetBinContent(bin);
        N += h->GetBinContent(bin);
    }

    return TMath::Sqrt(result/N);
}

void MACRO_NoiseRMS(int time_slot = 0)
{
    gStyle->SetOptStat(1110);
    TFile* f = new TFile("../data/signal-diff-risetime/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH1F* h = new TH1F("h","h",100,-.04,-.03);
    //pulse->Project("h",Form("channel[0][%i]",time_slot));
    pulse->Project("h",Form("channel[0]"),"time[0]<-15E-9");

    std::cout<<"The RMS   of time slot "<<time_slot<<" is "<<rms(h)<<"(V)"<<std::endl;
    std::cout<<"The sigma of time slot "<<time_slot<<" is "<<h->GetStdDev()<<"(V)"<<std::endl;

    h->Draw();
    h->GetXaxis()->SetTitle("Voltage(V)");
    h->GetYaxis()->SetTitle("Counts");
}