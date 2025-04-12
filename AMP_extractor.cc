void AMP_extractor()
{
    #include <tuple>
    std::vector<int> attenuations = {36, 40, 46, 50, 56, 60, 66};
    std::string basePath = "/media/rene/Data/LGAD/FNAL-pulse-analysis/paper/input-files/";
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
    const int bins=50;
    std::map<int, std::tuple<int, double, double>> options = {
        {36, std::make_tuple(bins, 791, 799)},
        {40, std::make_tuple(bins, 320, 390)},
        {46, std::make_tuple(bins, 198, 228)},
        {50, std::make_tuple(bins, 140, 175)},
        {56, std::make_tuple(bins, 60, 100)},
        {60, std::make_tuple(bins, 40, 70)},
        {66, std::make_tuple(bins, 10, 50)}
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
        tree->Project(histName.c_str(), "amp[1]", cuts[att].c_str());

        std::string canvasName = "c" + std::to_string(att) + "dB";
        TCanvas* c = new TCanvas(canvasName.c_str(), canvasName.c_str(), 800, 600);
        canvases.push_back(c);

        hist->SetTitle((std::to_string(att) + "dB Attenuation Config.; RMS (mV); #Measurements").c_str());
        hist->Draw();

        double p0_init = hist->GetMaximumBin();
        double p1_init = hist->GetMean();
        double p2_init = hist->GetRMS() / 2;

        TF1* fitFunc = new TF1("fitFunc",
            "[0]*TMath::Gaus(x,[1],[2],false)", xmin, xmax);
        fitFunc->SetParameters(p0_init, p1_init, p2_init);

        hist->Fit(fitFunc, "RQ");

        std::cout << "  Parámetros del fit encontrados para " << att << " dB:" << std::endl;
        for (int p = 0; p < 3; ++p){
            std::cout << "    p" << p << " = " << fitFunc->GetParameter(p) << std::endl;}


        double p0 = fitFunc->GetParameter(0);
        double mu = fitFunc->GetParameter(1); //p1
        double sigma = fitFunc->GetParameter(2); //p2

        TArrayD arr(2);
        arr[0] = mu;
        arr[1] = sigma;
        results.push_back(arr);

        std::cout << att << " dB: μ = " << mu << " mV, σ = " << sigma << " mV" << std::endl;


        c->Update();
    }

    std::cout << "\nResumen final de momentos normalizados:\n";
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << attenuations[i] << " dB: ";
        std::cout << "μ = " << results[i][0]
                  << ", σ = " << results[i][1] << std::endl;
    }
    TFile* outFile = new TFile("AMP_results_errors.root", "RECREATE");
    TTree* resultTree = new TTree("results", "Amplitud y su error por atenuación");
    double attenuation, amp_err, amp;
    resultTree->Branch("attenuation", &attenuation);
    resultTree->Branch("amp", &amp);
    resultTree->Branch("amp_err", &amp_err);
    
    for (size_t i = 0; i < results.size(); ++i) {
        attenuation = attenuations[i];
        amp       = results[i][0];
        amp_err   = results[i][1];
        resultTree->Fill();
    }

    resultTree->Write();
    outFile->Close();
    delete outFile;
    std::cout << "\n[INFO] Resultados guardados en 'AMP_results_errors.root'\n";

}
