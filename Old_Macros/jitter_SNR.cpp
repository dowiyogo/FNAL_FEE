void jitter_SNR()
{
    double jitter[5]     = { 21.57 , 11.09 , 43.37 , 29.36 , 78.51 };
    double jitterrerr[5] = { 0 , 0 , 0 , 0 , 0 };
    double SNR[5]        = { 79.42 , 72.90 , 34.44 , 26.32 , 14.57};
    double SNRerr[5]     = { 0 , 0 , 0 , 0 , 0 };
    TGraphErrors* g = new TGraphErrors(5,SNR,jitter,SNRerr,jitterrerr);

    g->SetMarkerStyle(8);
    g->SetMarkerColor(kBlue);
    g->SetLineColor(kBlue);

    g->Draw("AP");

    g->SetTitle("");
    g->GetXaxis()->SetTitle("SNR");
    g->GetYaxis()->SetTitle("Jitter (ps)");

    g->GetXaxis()->CenterTitle();
    g->GetYaxis()->CenterTitle();

    gPad->Print("../plots/jitter-SNR.pdf");
    gPad->Print("../plots/jitter-SNR.png");
}