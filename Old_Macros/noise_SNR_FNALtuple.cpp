void noise_SNR_FNALtuple()
{
    //double noise[7]     = { 1.959 , 3.624 , 3.023 , 3.13 , 3.22 , 3.13 , 3.24 };
    //double noisererr[7] = { 6.30E-03 , 7.22E-03 , 7.68E-03 , 5.53E-03 , 5.23E-03 , 5.08E-03 , 6.63E-03};
    //double SNR[7]       = { 404.9 , 96.47 , 70.39 , 50.06 , 23.71 , 17.22 , 7.56 };
    //double SNRerr[7]    = { 0 , 0 , 0 , 0 , 0 , 0 , 0   };
    //TGraphErrors* g = new TGraphErrors(7,SNR,noise,SNRerr,noisererr);

    double noise[5]     = { 3.023 , 3.13 , 3.22 , 3.13 , 3.24 };
    double noisererr[5] = { 7.68E-03 , 5.53E-03 , 5.23E-03 , 5.08E-03 , 6.63E-03};
    double SNR[5]       = { 70.39 , 50.06 , 23.71 , 17.22 , 7.56 };
    double SNRerr[5]    = { 0 , 0 , 0 , 0 , 0   };
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

    gPad->Print("../plots/noise-SNR-FNALtuple.pdf");
    gPad->Print("../plots/noise-SNR-FNALtuple.png");
}