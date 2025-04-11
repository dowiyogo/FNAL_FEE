double lims[2][2] = {{-1.5E-9,1.5E-9},{-0.5E-9,2.5E-9}};
int nevents = 5000;

double linear(double* x, double* par){
    double ind = *x;
    return par[0] + (ind*par[2] - par[1]);
}

double gethalfrisetimeY(TH2F* h){
    TF1* fit_func_1 = new TF1("fit_func_1",linear,-25E-9,-10E-9,3);
    TF1* fit_func_2 = new TF1("fit_func_2",linear, 10E-9, 25E-9,3);
    fit_func_1->SetParameters(0.03,0,.001);
    fit_func_2->SetParameters(-0.03,0,.001);

    h->Fit(fit_func_1,"RSQN");
    h->Fit(fit_func_2,"RSQN");
    
    if(fit_func_1->GetChisquare()/fit_func_1->GetNDF()>8||fit_func_2->GetChisquare()/fit_func_2->GetNDF()>8){
            std::cout<<"Goodness of fit low  = "<<fit_func_1->GetChisquare()/fit_func_1->GetNDF()<<std::endl;
            std::cout<<"Goodness of fit high = "<<fit_func_2->GetChisquare()/fit_func_2->GetNDF()<<std::endl;
            return 0;
    }

    double low_avge  = (fit_func_1->Eval(-25E-9)+fit_func_1->Eval(-15E-9))/2.;
    double high_avge = (fit_func_2->Eval(25E-9)+fit_func_2->Eval(15E-9))/2.;

    delete fit_func_1;
    delete fit_func_2;

    return (high_avge-low_avge)/2. + low_avge;
}

double getAmplitude(TH2F* h){
    TF1* fit_func_1 = new TF1("fit_func_1",linear,-25E-9,-10E-9,3);
    TF1* fit_func_2 = new TF1("fit_func_2",linear, 10E-9, 25E-9,3);
    fit_func_1->SetParameters(0.03,0,.001);
    fit_func_2->SetParameters(-0.03,0,.001);

    h->Fit(fit_func_1,"RSQN");
    h->Fit(fit_func_2,"RSQN");
    
    if(fit_func_1->GetChisquare()/fit_func_1->GetNDF()>8||fit_func_2->GetChisquare()/fit_func_2->GetNDF()>8){
            std::cout<<"Goodness of fit low  = "<<fit_func_1->GetChisquare()/fit_func_1->GetNDF()<<std::endl;
            std::cout<<"Goodness of fit high = "<<fit_func_2->GetChisquare()/fit_func_2->GetNDF()<<std::endl;
            return 0;
    }

    double low_avge  = (fit_func_1->Eval(-25E-9)+fit_func_1->Eval(-15E-9))/2.;
    double high_avge = (fit_func_2->Eval(25E-9)+fit_func_2->Eval(15E-9))/2.;

    delete fit_func_1;
    delete fit_func_2;

    return (high_avge-low_avge);
}

double getBase(TH2F* h){
    TF1* fit_func_1 = new TF1("fit_func_1",linear,-25E-9,-10E-9,3);
    fit_func_1->SetParameters(0.03,0,.001);
    
    h->Fit(fit_func_1,"RSQN");
    
    if(fit_func_1->GetChisquare()/fit_func_1->GetNDF()>8){
            std::cout<<"Goodness of fit low  = "<<fit_func_1->GetChisquare()/fit_func_1->GetNDF()<<std::endl;
            return 0;
    }

    double low_avge  = (fit_func_1->Eval(-25E-9)+fit_func_1->Eval(-15E-9))/2.;
    
    delete fit_func_1;
    
    return low_avge;
}

void MACRO_RiseTime()
{
    gStyle->SetOptStat(1110);
    TFile* f = new TFile("../data/signal-diff-risetime/converted_run100000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h_c1 = new TH2F("h_c1","h_c1",200,-30E-9,30E-9,200,-.02,.02);
    TH2F* h_c2 = new TH2F("h_c2","h_c2",200,-30E-9,30E-9,200,-.02,.02);

    TF1* fit_func_c1 = new TF1("fit_func_c1",linear,lims[0][0],lims[0][1],3);
    TF1* fit_func_c2 = new TF1("fit_func_c2",linear,lims[1][0],lims[1][1],3);
    
    TH1F* h = new TH1F("h","h",100,4.E-9,5.2E-9);

    for(int evt = 0 ; evt < nevents ; evt++){
        pulse->Project("h_c1","channel[0]:time[0]",Form("i_evt==%i",evt));
        pulse->Project("h_c2","channel[1]:time[0]",Form("i_evt==%i",evt));
        
        fit_func_c1->SetParameters(0,0,1);
        fit_func_c2->SetParameters(0,0,1);
        
        h_c1->Fit(fit_func_c1,"RSQN");
        h_c2->Fit(fit_func_c2,"RSQN");
        
        if(fit_func_c1->GetChisquare()/fit_func_c1->GetNDF()>8||fit_func_c2->GetChisquare()/fit_func_c2->GetNDF()>8){
            std::cout<<"Goodness of fit c1 = "<<fit_func_c1->GetChisquare()/fit_func_c1->GetNDF()<<std::endl;
            std::cout<<"Goodness of fit c2 = "<<fit_func_c2->GetChisquare()/fit_func_c2->GetNDF()<<std::endl;

            h_c1->Reset();
            h_c2->Reset();
            continue;
        }
        
        double amplitude_c1 = getAmplitude(h_c1);
        double base_c1      = getBase(h_c1);
        double amplitude_c2 = getAmplitude(h_c2);
        double base_c2      = getBase(h_c2);

        if(amplitude_c1==0 || base_c1==0|| amplitude_c2==0 || base_c2==0) continue;
        double c1_20pct = fit_func_c1->GetX(0.2*amplitude_c1 + base_c1,-5.E-9,5.E-9,1.E-14,100000);
        double c1_80pct = fit_func_c1->GetX(0.8*amplitude_c1 + base_c1,-5.E-9,5.E-9,1.E-14,100000);        
        double c2_20pct = fit_func_c2->GetX(0.2*amplitude_c2 + base_c2,-5.E-9,5.E-9,1.E-14,100000);
        double c2_80pct = fit_func_c2->GetX(0.8*amplitude_c2 + base_c2,-5.E-9,5.E-9,1.E-14,100000);
        
        double c1_risetime = c1_80pct - c1_20pct;
        double c2_risetime = c2_80pct - c2_20pct;

        h->Fill(c1_risetime);
        h->Fill(c2_risetime);

        h_c1->Reset();
        h_c2->Reset();
    }

    h->Draw();
    h->GetXaxis()->SetTitle("t_{r}(s)");
    
    std::cout<<"Mean = "<<h->GetMean()<<std::endl;
    std::cout<<"StdDev = "<<h->GetStdDev()<<std::endl;

    return;
}
