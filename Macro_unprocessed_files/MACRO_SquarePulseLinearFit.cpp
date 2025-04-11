double linear(double* x, double* par){
    double ind = *x;
    return par[0] + (ind*par[2] - par[1]);
}

void MACRO_SquarePulseLinearFit()
{
    gStyle->SetOptFit(0111);
    TFile* f = new TFile("../data/signal-diff-limbandwidth/converted_run1000000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h = new TH2F("h","h",200,-30E-9,30E-9,200,-.02,.02);
    pulse->Project("h","channel[0]:time[0]","i_evt==1000");

    //h->Draw("colz");

    TF1* fit_func = new TF1("fit_func",linear,-2.E-9,2.E-9,3);
    fit_func->SetLineColor(kRed);
    fit_func->SetLineStyle(1);
    fit_func->SetLineWidth(3);
    fit_func->SetParameters(0,0,1);

    h->Fit(fit_func,"RS");

    h->GetXaxis()->SetTitle("Time(s)");
    h->GetYaxis()->SetTitle("Voltage(V)");

    std::cout<<"Goodness of fit = "<<fit_func->GetChisquare()/fit_func->GetNDF()<<std::endl;
}