void jitter_extractor_multiple()
{
    #include <tuple>
    std::vector<int> attenuations = {36, 40, 46, 50, 56, 60, 66};//{40, 46, 50, 56, 60, 66};
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
    
    const int bins=200;
    std::map<int, std::tuple<int, double, double>> options = {
        {36, std::make_tuple(bins, 1.84E-9, 1.99E-9)},
        {40, std::make_tuple(bins, 2.4E-9, 2.6E-9)},
        {46, std::make_tuple(bins, 4.2E-9, 4.4E-9)},
        {50, std::make_tuple(bins, 2.5E-9, 2.75E-9)},
        {56, std::make_tuple(bins, 4.2E-9, 4.6E-9)},
        {60, std::make_tuple(bins, 2.5E-9, 2.95E-9)},
        {66, std::make_tuple(bins, 4E-9, 5E-9)}
    };

    std::vector<TArrayD> momentResults;  // Guardar {mu, sigma}
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
        tree->Project(histName.c_str(), "LP2_50[1]-LP2_50[0]", cuts[att].c_str());

        std::string canvasName = "c" + std::to_string(att) + "dB";
        TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 800, 600);
        canvases.push_back(c);

        hist->SetTitle((std::to_string(att) + "dB Attenuation Config.; #Delta T (s); #Measurements").c_str());
        hist->Draw();

        TSpectrum* spectrum = new TSpectrum(10);
        int nPeaks = spectrum->Search(hist, 1.5, "", 0.1);
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

        // Guardar {mu, sigma}
        TArrayD moments(2);
        moments[0] = mu;
        moments[1] = sigma;
        momentResults.push_back(moments);

        std::cout << att << " dB: μ = " << mu << " s, σ = " << sigma << " s" << std::endl;

        c->Update();
    }

    std::cout << "\nResumen final de momentos normalizados:\n";
    for (size_t i = 0; i < momentResults.size(); ++i) {
        std::cout << attenuations[i] << " dB: ";
        std::cout << "μ = " << momentResults[i][0]
                  << ", σ = " << momentResults[i][1] << std::endl;
    }
}
