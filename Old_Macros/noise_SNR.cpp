void noise_SNR()
{
    double noise[5]     = { 2.81 , 2.25 , 2.29 , 2.07 , 1.71 };
    double noisererr[5] = { 0.84 , 0.71 , 0.83 , 0.65 , 0.89 };
    double SNR[5]       = { 79.42 , 72.90 , 34.44 , 26.32 , 14.57};
    double SNRerr[5]    = { 0 , 0 , 0 , 0 , 0 };
    TGraphErrors* g = new TGraphErrors(5,SNR,noise,SNRerr,noisererr);

    g->SetMarkerStyle(8);
    g->SetMarkerColor(kBlue);
    g->SetLineColor(kBlue);

    g->Draw("AP");

    g->SetTitle("");
    g->GetXaxis()->SetTitle("SNR");
    g->GetYaxis()->SetTitle("Noise RMS (mV)");

    g->GetXaxis()->CenterTitle();
    g->GetYaxis()->CenterTitle();

    gPad->Print("../plots/noise-SNR.pdf");
    gPad->Print("../plots/noise-SNR.png");
}