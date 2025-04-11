double bootstrapSigmaError(TTree* tree, int nbins, double xmin, double xmax, double p0_init, 
    double p1_init, double p2_init, double p3_init, double p4_init, double p5_init, double& errP2, double& errP5, int nIterations = 15)
{
    Long64_t nEntries = tree->GetEntries();
    std::vector<double> sigmas;
    std::vector<double> p2s, p5s;

    tree->ResetBranchAddresses(); // IMPORTANTE para evitar errores
    float lp2[3];
    tree->SetBranchAddress("LP2_50", lp2);

    double sqrt2pi = std::sqrt(2 * TMath::Pi());

    for (int it = 0; it < nIterations; ++it) {
        TH1F* hBoot = new TH1F("hBoot", "", nbins, xmin, xmax);
        if (it % 10 == 0){
        std::cout << "    [bootstrap] iteración " << it << " / " << nIterations << std::endl;}
        for (Long64_t i = 0; i < nEntries; ++i) {
            Long64_t idx = gRandom->Integer(nEntries);
            tree->GetEntry(idx);
            double dt = lp2[2] - lp2[0];  // <- acceso por índice
            hBoot->Fill(dt);
        }

        TF1* fitFunc = new TF1("fitFunc",
            "[0]*TMath::Gaus(x,[1],[2],false) + [3]*TMath::Gaus(x,[4],[5],false)", xmin, xmax);
        fitFunc->SetParameters(p0_init, p1_init, p2_init, p3_init, p4_init, p5_init);
        fitFunc->SetParLimits(2, 1e-12, 1e-8);
        fitFunc->SetParLimits(5, 1e-12, 1e-8);
        std::cout << "    [bootstrap] Fit iter " << it << std::endl;
        hBoot->Fit(fitFunc, "RQ0");
        std::cout << "    [bootstrap] Fit terminado iter " << it << std::endl;

        double p0 = fitFunc->GetParameter(0);
        double p1 = fitFunc->GetParameter(1);
        double p2 = fitFunc->GetParameter(2);
        double p3 = fitFunc->GetParameter(3);
        double p4 = fitFunc->GetParameter(4);
        double p5 = fitFunc->GetParameter(5);

        double M1 = sqrt2pi * (p0 * p2 * p1 + p3 * p5 * p4);
        double M2 = sqrt2pi * (p0 * p2 * (p1*p1 + p2*p2) + p3 * p5 * (p4*p4 + p5*p5));
        double N  = sqrt2pi * (p0 * p2 + p3 * p5);

        double mu = M1 / N;
        double x2 = M2 / N;
        double sigma = std::sqrt(x2 - mu * mu);
        p2s.push_back(p2);
        p5s.push_back(p5);

        if (std::isfinite(sigma)) sigmas.push_back(sigma);

        delete hBoot;
        delete fitFunc;
    }
    
    errP2 = TMath::StdDev(p2s.begin(), p2s.end());
    errP5 = TMath::StdDev(p5s.begin(), p5s.end());
    return TMath::StdDev(sigmas.begin(), sigmas.end());
}

void jitter_pulse_generator()
{    
    ROOT::EnableImplicitMT(); // Tell ROOT you want to go parallel
    ROOT::EnableThreadSafety();
    
    TFile* Generator  = new TFile("/media/rene/Data/LGAD/FNAL-pulse-analysis/paper/input-files/converted_runs_NOAtt_PGoutput_converted.root");
    TTree* treeGen = (TTree*)Generator->Get("pulse");

    TCanvas* c = new TCanvas("Canvas", "Canvas", 800, 600);
    const int nbins =150;
    double xmin= 1.91E-9;
    double xmax= 2.06E-9;
    TH1F* hGen = new TH1F("hGen","",nbins,xmin,xmax);

    treeGen->Project("hGen","LP2_50[2]-LP2_50[0]","");

    hGen->Draw("");

    TSpectrum* spectrum = new TSpectrum(10);
    int nPeaks = spectrum->Search(hGen, 2, "", 0.05);
    double* xPeaks = spectrum->GetPositionX();
    double* yPeaks = spectrum->GetPositionY();

    if (nPeaks < 2) {
        std::cerr << "Menos de dos peaks detectados " << std::endl;
    }

    std::vector<std::pair<double, double>> peaks;
    for (int j = 0; j < nPeaks; ++j) {
        peaks.emplace_back(yPeaks[j], xPeaks[j]);
    }
    std::sort(peaks.begin(), peaks.end(), std::greater<>()); // por altura

    double p0_init = peaks[0].first;
    double p1_init = peaks[0].second;
    double p3_init = peaks[1].first;
    double p4_init = peaks[1].second;
    double p2_init = hGen->GetRMS() / 4;
    double p5_init = hGen->GetRMS() / 4;

    TF1* fitFunc = new TF1("fitFunc",
        "[0]*TMath::Gaus(x,[1],[2],false) + [3]*TMath::Gaus(x,[4],[5],false)", 1.91E-9, 2.06E-9);
    fitFunc->SetParameters(p0_init, p1_init, p2_init, p3_init, p4_init, p5_init);
    fitFunc->SetParLimits(2, 1e-12, 1e-8);
    fitFunc->SetParLimits(5, 1e-12, 1e-8);

    hGen->Fit(fitFunc, "R");

    double p0 = fitFunc->GetParameter(0);
    double p1 = fitFunc->GetParameter(1);
    double p2 = fitFunc->GetParameter(2);
    double p3 = fitFunc->GetParameter(3);
    double p4 = fitFunc->GetParameter(4);
    double p5 = fitFunc->GetParameter(5);

    double sqrt2pi = std::sqrt(2 * TMath::Pi());

    double M1 = sqrt2pi * (p0 * p2 * p1 + p3 * p5 * p4);
    double M2 = sqrt2pi * (p0 * p2 * (p1*p1 + p2*p2) + p3 * p5 * (p4*p4 + p5*p5));
    double N  = sqrt2pi * (p0 * p2 + p3 * p5);

    double mu = M1 / N;
    double x2 = M2 / N;
    double sigma = std::sqrt(x2 - mu * mu);
    double errP2 = 0, errP5 = 0;
    double sigma_error = bootstrapSigmaError(treeGen,nbins,xmin,xmax,p0,p1,p2,p3,p4,p5,errP2,errP5,30);
    std::cout << " PG: μ = " << mu << " s, σ = " << sigma << " s ± " << sigma_error << " s" << std::endl;
    std::cout << " PG: p2 = " << p2 << " ±  " << errP2 << " p5 = " << p5 << "± " << errP5 <<" s "<< std::endl;
    //PG: μ = 2.01498e-09 s, σ = 3.85137e-11 s ± 4.07e-13 s    

    hGen->SetTitle("Pulse Generator Config.; #Delta T (s); #Measurements");

    c->Update();
}