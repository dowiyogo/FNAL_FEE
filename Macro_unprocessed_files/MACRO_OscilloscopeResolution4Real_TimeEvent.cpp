double lims[2][2] = {{-2.E-9,2.E-9},{-1.E-9,3.E-9}};
int nevents = 5000;
double quality_check = 15;
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
    
    if(fit_func_1->GetChisquare()/fit_func_1->GetNDF()>quality_check||fit_func_2->GetChisquare()/fit_func_2->GetNDF()>quality_check){
            //std::cout<<"Goodness of fit low  = "<<fit_func_1->GetChisquare()/fit_func_1->GetNDF()<<std::endl;
            //std::cout<<"Goodness of fit high = "<<fit_func_2->GetChisquare()/fit_func_2->GetNDF()<<std::endl;
            return 0;
    }

    double low_avge  = (fit_func_1->Eval(-25E-9)+fit_func_1->Eval(-15E-9))/2.;
    double high_avge = (fit_func_2->Eval(25E-9)+fit_func_2->Eval(15E-9))/2.;

    delete fit_func_1;
    delete fit_func_2;

    //std::cout<<"Amplitude = "<<high_avge-low_avge<<"(V)"<<std::endl;

    return (high_avge-low_avge)/2. + low_avge;
}

void MACRO_OscilloscopeResolution4Real_TimeEvent()
{
    gStyle->SetOptStat(1110);
    TFile* ftarget = new TFile("fits.root","RECREATE");
    gROOT->cd();
    TFile* f     = new TFile("../data/signal-diff-limbandwidth/converted_run1000000.root","READ");
    TTree* pulse = (TTree*) f->Get("pulse");

    TH2F* h_c1 = new TH2F("h_c1","h_c1",200,-30E-9,30E-9,200,-.02,.02);
    TH2F* h_c2 = new TH2F("h_c2","h_c2",200,-30E-9,30E-9,200,-.02,.02);

    TF1* fit_func_c1 = new TF1("fit_func_c1",linear,lims[0][0],lims[0][1],3);
    TF1* fit_func_c2 = new TF1("fit_func_c2",linear,lims[1][0],lims[1][1],3);
    fit_func_c1->SetLineColor(kRed);
    fit_func_c2->SetLineColor(kRed);

    TGraph* g = new TGraph();
    g->SetMarkerStyle(22);
    int npoint = 0;

    for(int evt = 0 ; evt < nevents ; evt++){
        std::cout<<"PROCESSING EVENT "<<evt<<std::endl;
        pulse->Project("h_c1","channel[0]:time[0]",Form("i_evt==%i",evt));
        pulse->Project("h_c2","channel[1]:time[0]",Form("i_evt==%i",evt));
        
        fit_func_c1->SetParameters(0,0,1);
        fit_func_c2->SetParameters(0,0,1);
        
        h_c1->Fit(fit_func_c1,"RSQ");
        h_c2->Fit(fit_func_c2,"RSQ");

        if(fit_func_c1->GetChisquare()/fit_func_c1->GetNDF()>quality_check||fit_func_c2->GetChisquare()/fit_func_c2->GetNDF()>quality_check){
            std::cout<<"Event "<<evt<<" does not fulfill central fit quality check."<<std::endl;           
            h_c1->Reset();
            h_c2->Reset();
            //npoint++;
            continue;
        }
        double halfrisetimeY = gethalfrisetimeY(h_c1);
        if(halfrisetimeY==0){
            std::cout<<"Event "<<evt<<" does not fulfill flat fit quality check."<<std::endl;
            h_c1->Reset();
            h_c2->Reset();
            //npoint++;
            continue;
        }

        double c1_halftr = fit_func_c1->GetX(halfrisetimeY,-5.E-9,5.E-9,1.E-14,100000);
        double c2_halftr = fit_func_c2->GetX(halfrisetimeY,-5.E-9,5.E-9,1.E-14,100000);
        
        if(c1_halftr==0||c2_halftr==0){
            h_c1->Reset();
            h_c2->Reset();
            //npoint++;
            continue;
        }

        if(c2_halftr-c1_halftr<1.2E-9){
            ftarget->cd();
            h_c1->Write(Form("fit_evt%i_c1",evt));
            h_c2->Write(Form("fit_evt%i_c2",evt));
            f->cd();
        }

        g->SetPoint(npoint, evt, c2_halftr-c1_halftr);
        npoint++;
        h_c1->Reset();
        h_c2->Reset();
    }

    g->Draw("AP");
    g->GetXaxis()->SetTitle("Event");
    g->GetYaxis()->SetTitle("#Delta Time(s)");
    
    ftarget->cd();
    g->Write("dtime_vs_event");
    return;
}
