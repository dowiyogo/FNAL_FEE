double pol1[14]     = {5.4,5.5,5.6,5.7,5.8,5.9,6.0,6.1,6.2,6.3,6.4,6.5,6.6,6.7};
double pol1_err[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
double pol2[13]     = {5.4+0.02,5.5+0.02,5.6+0.02,5.7+0.02,5.8+0.02,5.9+0.02,6.0+0.02,6.1+0.02,6.2+0.02,6.3+0.02,6.4+0.02,6.5+0.02,6.6+0.02};
double pol2_err[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

double noise1[14]     = {0.542,0.608,0.630,0.674,0.743,0.749,0.816,0.873,0.904,0.93,0.996,1.024,1.048,1.08};
double noise1_err[14] = {0.069,0.174,0.08,0.243,0.096,0.138,0.260,0.19,0.19,0.21,0.14,0.19,0.14,0.16};

double noise2[13]     = {0.566,0.612,0.68,0.69,0.742,0.765,0.844,0.878,0.92,0.940,1.03,1.04,1.08};
double noise2_err[13] = {0.24,0.13,0.09,0.098,0.093,0.098,0.114,0.15,0.124,0.13,0.22,0.14,0.13};
void MACRO_Board_NoisePolarization()
{
    TGraphErrors* g1 = new TGraphErrors(14,pol1,noise1,pol1_err,noise1_err);
    TGraphErrors* g2 = new TGraphErrors(13,pol2,noise2,pol2_err,noise2_err);

    g1->SetMarkerColor(kRed);
    g1->SetLineColor(kRed);
    g2->SetMarkerColor(kBlue);
    g2->SetLineColor(kBlue);

    TMultiGraph* mg = new TMultiGraph();
    mg->Add(g1,"APE");
    mg->Add(g2,"APE");

    mg->Draw("APE");

    gPad->SetGridx(1);
    gPad->SetGridy(1);

    mg->SetTitle(";Polarization (V); Noise RMS (mV)");

    TLegend* l = new TLegend(gPad->GetLeftMargin(),0.7,0.4,1-gPad->GetTopMargin(),"","NDC");
    l->AddEntry(g1,"Source OFF","lp");
    l->AddEntry(g2,"5MHz, 250mVpp","lp");

    l->Draw();
}