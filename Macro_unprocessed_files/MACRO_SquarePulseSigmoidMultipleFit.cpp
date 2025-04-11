int nevents = 500;

double sigmoid(double* x, double* par){
    double ind = *x;
    return par[0]/(1+TMath::Exp(-ind*par[1])) + par[2];
}

void MACRO_SquarePulseSigmoidMultipleFit()
{
    TFile* f = new TFile("../data/signal-diff-risetime/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h = new TH2F("h","h",200,-30E-9,30E-9,200,-.04,.04);
    TF1* fit_func = new TF1("fit_func",sigmoid,-30E-9,30E-9,3);
    
    TH1F* h_channel1 = new TH1F("h_channel1","h_channel1",100,-1.E-10,1.E-10);
    for(int evt = 0 ; evt < nevents ; evt++){
        pulse->Project("h","channel[0]:time[0]",Form("i_evt==%i",evt));
        fit_func->SetParameters(0.04,1,-0.04);
        h->Fit(fit_func,"RSNQ");
        
        double channel1_halftr = fit_func->GetX(0,-1.E-9,1.E-9,1.E-13,10000);
        std::cout<<"Goodness of fit = "<<fit_func->GetChisquare()/fit_func->GetNDF()<<"  time = "<<channel1_halftr<<std::endl;
        h_channel1->Fill(channel1_halftr);

        h->Reset();
    }

    h_channel1->Draw();
    
}