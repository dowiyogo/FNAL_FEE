void jitter_SNR_FNALtuple()
{
//    double jitter[7]     = { 4.35 , 8.06 , 10.85 , 13.96 , 19.47 , 25.13 , 57.51 };
//    double jitterrerr[7] = { 0 , 0 , 0 , 0 , 0 };
//    double SNR[7]        = { 404.9 , 96.47 , 70.39 , 50.06 , 23.71 , 17.22 , 7.56 };
//    double SNRerr[7]     = { 0 , 0 , 0 , 0 , 0 , 0 , 0   };
//    TGraphErrors* g = new TGraphErrors(7,SNR,jitter,SNRerr,jitterrerr);

    double jitter[5]     = { 10.85 , 13.96 , 19.47 , 25.13 , 57.51 };
    double jitterrerr[5] = { 0 , 0 , 0 , 0 , 0 };
    double SNR[5]        = { 70.39 , 50.06 , 23.71 , 17.22 , 7.56 };
    double SNRerr[5]     = { 0 , 0 , 0 , 0 , 0   };
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

    g->GetYaxis()->SetRangeUser(0,65);

    gPad->Print("../plots/jitter-SNR-FNALtuple.pdf");
    gPad->Print("../plots/jitter-SNR-FNALtuple.png");
}