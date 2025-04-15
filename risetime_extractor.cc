
void risetime_extractor()
{
    ROOT::EnableImplicitMT(); // Tell ROOT you want to go parallel
    ROOT::EnableThreadSafety();
    #include <tuple>
    std::vector<int> attenuations = {36, 40, 46, 50, 56, 60, 66};//{40, 46, 50, 56, 60, 66}
    std::string basePath = "/media/rene/Data/4d-tracking/paper/input-files/FNAL_BoardCH8/";//"/media/rene/Data/LGAD/converted/FNAL_CH8/";
    std::string filePrefix = "converted_runs_";
    std::string fileSuffix = "dB-att_6800mV_converted.root";

    std::map<int, std::string> cuts = {
        {36, ""},
        {40, ""},
        {46, ""},
        {50, ""},
        {56, ""},
        {60, ""},
        {66, ""}
    };
    const int bins=100;
    std::map<int, std::tuple<int, double, double>> options = {
        {36, std::make_tuple(bins, 280E-12, 310E-12)},
        {40, std::make_tuple(bins, 290E-12, 400E-12)},
        {46, std::make_tuple(bins, 290E-12, 450E-12)},
        {50, std::make_tuple(bins, 295E-12, 500E-12)},
        {56, std::make_tuple(bins, 230E-12, 530E-12)},
        {60, std::make_tuple(bins, 200E-12, 620E-12)},
        {66, std::make_tuple(bins, 180E-12, 630E-12)}
    };
    
    std::vector<TArrayD> results;  // Guardar {mu, sigma}
    std::vector<TCanvas*> canvases;

    for (size_t i = 0; i < attenuations.size(); ++i) {
        int att = attenuations[i];
        std::string filename = basePath + filePrefix + std::to_string(att) + fileSuffix;
        TFile* file = new TFile(filename.c_str());
        if (!file || file->IsZombie()) {
            std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
            continue;
        }

        TTree* tree = (TTree*) file->Get("pulse");
        if (!tree) {
            std::cerr << "No se encontró el árbol 'pulse' en el archivo: " << filename << std::endl;
            continue;
        }

        auto [nbins, xmin, xmax] = options[att];
        std::string histName = "h" + std::to_string(att) + "dB";
        TH1F* hist = new TH1F(histName.c_str(), "", nbins, xmin, xmax);
        tree->Project(histName.c_str(), "LP2_80[1]-LP2_20[1]", cuts[att].c_str());

        std::string canvasName = "c" + std::to_string(att) + "dB";
        TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 800, 600);
        canvases.push_back(c);

        hist->SetTitle((std::to_string(att) + "dB Attenuation Config.; Risetime (s); #Measurements").c_str());
        hist->Draw();

        TSpectrum* spectrum = new TSpectrum(10);
        int nPeaks = spectrum->Search(hist, 1, "", 0.03);
        double* xPeaks = spectrum->GetPositionX();
        double* yPeaks = spectrum->GetPositionY();

        if (nPeaks < 2) {
            std::cerr << "Menos de dos peaks detectados para " << att << "dB" << std::endl;
            continue;
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
        double p2_init = hist->GetRMS() / 4;
        double p5_init = hist->GetRMS() / 4;

        TF1* fitFunc = new TF1("fitFunc",
            "[0]*TMath::Gaus(x,[1],[2],false) + [3]*TMath::Gaus(x,[4],[5],false)", xmin, xmax);
        fitFunc->SetParameters(p0_init, p1_init, p2_init, p3_init, p4_init, p5_init);
        //fitFunc->SetParLimits(1, 0, 1);
        //fitFunc->SetParLimits(2, 0, 1);
        //fitFunc->SetParLimits(4, 0, 1);
        //fitFunc->SetParLimits(5, 0, 1);


        hist->Fit(fitFunc, "RQ");

        std::cout << "  Parámetros del fit encontrados para " << att << " dB:" << std::endl;
        for (int p = 0; p < 6; ++p){
            std::cout << "    p" << p << " = " << fitFunc->GetParameter(p) << std::endl;}


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

        std::cout << att << " dB: risetime (μ ± σ)= " << mu << " ± " << sigma << " s " << std::endl;
        std::cout << att << " dB: p1 = " << p1 << " ±  " << p2  << " p4 = " << p4 << "± " << p5 <<" s "<< std::endl;
        // Guardar {mu, sigma}
        TArrayD moments(6);
        moments[0] = mu;
        moments[1] = sigma;
        moments[2] = p1;
        moments[3] = p2;
        moments[4] = p4;
        moments[5] = p5;
        results.push_back(moments);
        c->Update();
    }

    std::cout << "\nResumen final de momentos normalizados:\n";
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << attenuations[i] << " dB: ";
        std::cout << "μ = " << results[i][0]
                  << ", σ = " << results[i][1] << std::endl;
    }
    TFile* outFile = new TFile("risetime_results_errors.root", "RECREATE");
    TTree* resultTree = new TTree("results", "Risetime y su error por atenuación");
    double attenuation, rise_err, rise;
    double p1_val, p2_val, p4_val, p5_val;
    resultTree->Branch("attenuation", &attenuation);
    resultTree->Branch("rise", &rise);
    resultTree->Branch("rise_err", &rise_err);
    resultTree->Branch("p1", &p1_val, "p1/D");
    resultTree->Branch("p2", &p2_val, "p2/D");
    resultTree->Branch("p4", &p4_val, "p4/D");
    resultTree->Branch("p5", &p5_val, "p5/D");
    
    for (size_t i = 0; i < results.size(); ++i) {
        attenuation = attenuations[i];
        rise       = results[i][0];
        rise_err   = results[i][1];
        p1_val      = results[i][2];
        p2_val      = results[i][3];
        p4_val      = results[i][4];
        p5_val      = results[i][5];
        resultTree->Fill();
    }

    resultTree->Write();
    outFile->Close();
    delete outFile;
    std::cout << "\n[INFO] Resultados guardados en 'risetime_results_errors.root'\n";

}
