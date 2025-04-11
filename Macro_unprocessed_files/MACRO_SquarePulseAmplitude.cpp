double linear(double* x, double* par){
    double ind = *x;
    return par[0] + (ind*par[1]);
}

void MACRO_SquarePulseAmplitude()
{
    //gStyle->SetOptFit(0111);
    TFile* f = new TFile("../data/signal-diff-limbandwidth/converted_run1000000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h = new TH2F("h","h",200,-30E-9,30E-9,200,-.02,.02);
    pulse->Project("h","channel[1]:time[0]","i_evt==3");

    //h->Draw("colz");

    TF1* fit_func_1 = new TF1("fit_func_1",linear,-25E-9,-10E-9,2);
    TF1* fit_func_2 = new TF1("fit_func_2",linear, 10E-9, 25E-9,2);
    fit_func_1->SetLineColor(kRed);
    fit_func_1->SetLineStyle(1);
    fit_func_1->SetParameters(0.03,0);
    fit_func_2->SetLineColor(kRed);
    fit_func_2->SetLineStyle(1);
    fit_func_2->SetParameters(-0.03,0);

    h->Fit(fit_func_1,"RS");
    h->Fit(fit_func_2,"RS+");

    h->GetXaxis()->SetTitle("Time(s)");
    h->GetYaxis()->SetTitle("Voltage(V)");
    std::cout<<"--------------------------------------------------------------------------------------"<<std::endl;
    std::cout<<"                                        Fit Stats                                     "<<std::endl;
    std::cout<<"--------------------------------------------------------------------------------------"<<std::endl;
    std::cout<<"LOW  : Goodness of fit = "<<fit_func_1->GetChisquare()/fit_func_1->GetNDF()<<std::endl;
    std::cout<<"HIGH : Goodness of fit = "<<fit_func_2->GetChisquare()/fit_func_2->GetNDF()<<std::endl;
    std::cout<<"--------------------------------------------------------------------------------------"<<std::endl;
    std::cout<<"                                        Function values                               "<<std::endl;
    std::cout<<"--------------------------------------------------------------------------------------"<<std::endl;
    std::cout<<"LOW  : Function value and ranges = ["<<fit_func_1->Eval(-25E-9)<<" , "<<fit_func_1->Eval(-10E-9)<<"]"<<std::endl;
    std::cout<<"HIGH : Function value and ranges = ["<<fit_func_2->Eval(10E-9)<<" , "<<fit_func_2->Eval(25E-9)<<"]"<<std::endl;
    std::cout<<"--------------------------------------------------------------------------------------"<<std::endl;
}