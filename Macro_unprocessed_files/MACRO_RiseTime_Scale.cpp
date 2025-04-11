const double risetime1[19]     = {4.123,4.106,4.09,4.071,4.067,4.057,4.048,4.038,4.034,3.97,3.99,4.067,4.075,4.079,4,4.08,4.081,4.08,4.08};
const double risetime1_err[19] = {0.036,0.037,0.038,0.038,0.038,0.039,0.04,0.039,0.04,0.039,0.039,0.042,0.043,0.042,0.044,0.044,0.044,0.046,0.047};

const double risetime2[19]     = {4.15,4.14,4.127,4.1,4.1,4.092,4.091,4.075,4.065,3.99,4,4.09,4.098,4.11,4.04,4.12,4.12,4.12,4.123};
const double risetime2_err[19] = {0.035,0.037,0.038,0.038,0.036,0.038,0.038,0.039,0.039,0.039,0.039,0.042,0.042,0.042,0.043,0.045,0.045,0.044,0.045};


const double scale1[19]        = {9,9.5,10,10.6,11,11.6,12,12.6,13,13.6,14,14.6,15,15.6,16,16.6,17,17.6,18};
const double scale2[19]        = {9+0.1,9.5+0.1,10+0.1,10.6+0.1,11+0.1,11.6+0.1,12+0.1,12.6+0.1,13+0.1,13.6+0.1,14+0.1,14.6+0.1,15+0.1,15.6+0.1,16+0.1,16.6+0.1,17+0.1,17.6+0.1,18};
const double scale_err[19]    = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void MACRO_RiseTime_Scale()
{
    TGraphErrors* g1 = new TGraphErrors(19,scale1,risetime1,scale_err,risetime1_err);
    TGraphErrors* g2 = new TGraphErrors(19,scale2,risetime2,scale_err,risetime2_err);

    g1->SetMarkerColor(kGreen);
    g1->SetLineColor(kGreen);
    g2->SetMarkerColor(kBlue);
    g2->SetLineColor(kBlue);

    TMultiGraph* mg = new TMultiGraph();
    mg->Add(g1,"APE");
    mg->Add(g2,"APE");

    mg->Draw("APE");
    mg->GetXaxis()->SetTitle("Scale(mV/div)");
    mg->GetYaxis()->SetTitle("risetime(ns)");

    gPad->SetGridy(1);
    gPad->SetGridx(1);

    TLegend* l = new TLegend(0.35,0.75,0.65,1-gPad->GetTopMargin(),"","NDC");
    l->AddEntry(g1,"25 MHz, 70mVpp","p");
    l->AddEntry(g2,"1 MHz, 70mVpp","p");
    l->Draw();
}