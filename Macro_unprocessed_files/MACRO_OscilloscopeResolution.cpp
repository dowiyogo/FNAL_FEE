int nevents = 5000;

double linear(double* x, double* par){
    double ind = *x;
    return par[0] + (ind*par[2] - par[1]);
}

void MACRO_OscilloscopeResolution()
{
    gStyle->SetOptStat(1110);

    TFile* f = new TFile("../data/signal-diff-risetime/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h_c1 = new TH2F("h_c1","h_c1",200,-30E-9,30E-9,200,-.04,.04);
    TH2F* h_c2 = new TH2F("h_c2","h_c2",200,-30E-9,30E-9,200,-.04,.04);

    TF1* fit_func_c1 = new TF1("fit_func_c1",linear,-2.E-9,2.E-9,3);
    TF1* fit_func_c2 = new TF1("fit_func_c2",linear,-2.E-9,2.E-9,3);
    
    TH1F* h = new TH1F("h","h",100,0.5E-9,1.5E-9);
    //TH1F* h_c2 = new TH1F("h_c2","h_c2",100,-1.E-10,1.E-10);

    for(int evt = 0 ; evt < nevents ; evt++){
        pulse->Project("h_c1","channel[0]:time[0]",Form("i_evt==%i",evt));
        pulse->Project("h_c2","channel[1]:time[0]",Form("i_evt==%i",evt));
        
        fit_func_c1->SetParameters(0,0,1);
        fit_func_c2->SetParameters(0,0,1);
        
        h_c1->Fit(fit_func_c1,"RSNQ");
        h_c2->Fit(fit_func_c2,"RSNQ");
        
        double c1_halftr = fit_func_c1->GetX(0,-.5E-9,3.E-9,1.E-13,100000);
        double c2_halftr = fit_func_c2->GetX(0,-.5E-9,3.E-9,1.E-13,100000);
        
        if(fit_func_c1->GetChisquare()/fit_func_c1->GetNDF()>10||fit_func_c2->GetChisquare()/fit_func_c2->GetNDF()>10){
            std::cout<<"Goodness of fit c1 = "<<fit_func_c1->GetChisquare()/fit_func_c1->GetNDF()<<"  t_c1 = "<<c1_halftr<<std::endl;
            std::cout<<"Goodness of fit c2 = "<<fit_func_c2->GetChisquare()/fit_func_c2->GetNDF()<<"  t_c2 = "<<c2_halftr<<std::endl;
            continue;
        }
        
        h->Fill(c2_halftr-c1_halftr);

        h_c1->Reset();
        h_c2->Reset();
    }

    h->Draw();
    h->GetXaxis()->SetTitle("(Time1_{50\%}-Time2_{50\%})(s)");
    
    std::cout<<"Mean = "<<h->GetMean()<<std::endl;
    std::cout<<"StdDev = "<<h->GetStdDev()<<std::endl;
}
