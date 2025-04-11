double sigmoid(double* x, double* par){
    double ind = *x;
    return par[0]/(1+TMath::Exp(-(ind-par[2])*par[1]));
}

void MACRO_SquarePulseSigmoidFit()
{
    TFile* f = new TFile("../data/signal-diff-risetime/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h = new TH2F("h","h",200,-30E-9,30E-9,200,-.04,.04);
    pulse->Project("h","channel[1]:time[0]","i_evt==0");

    //h->Draw("colz");

    TF1* fit_func = new TF1("fit_func",sigmoid,-30E-9,30E-9,3);
    fit_func->SetLineColor(kRed);
    fit_func->SetLineStyle(2);
    fit_func->SetParameters(0.04,0.1,1.E-9);

    h->Fit(fit_func,"RS");

    std::cout<<"Goodness of fit = "<<fit_func->GetChisquare()/fit_func->GetNDF()<<std::endl;
}