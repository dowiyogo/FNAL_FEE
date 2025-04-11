double sigmoid(double* x, double* par){
    double ind = *x;
    return TMath::TanH(ind*par[1])*par[0];
}

void MACRO_SquarePulseTanhFit()
{
    TFile* f = new TFile("../data/signal-diff-risetime/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h = new TH2F("h","h",200,-30E-9,30E-9,200,-.04,.04);
    pulse->Project("h","channel[0]:time","i_evt==1");

    //h->Draw("colz");

    TF1* fit_func = new TF1("fit_func",sigmoid,-30E-9,30E-9,2);
    fit_func->SetParameters(0.04,1);

    h->Fit(fit_func,"RS");

    std::cout<<"Goodness of fit = "<<fit_func->GetChisquare()/fit_func->GetNDF()<<std::endl;
}