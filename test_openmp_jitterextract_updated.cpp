
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TArrayD.h>
#include <TROOT.h>
#include <TSpectrum.h>

#include <iostream>
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>
#include <cmath>
#include <omp.h>

// Estimador bootstrap paralelo con apertura de archivo por hilo
double bootstrapSigmaError(const std::string& filename, int nbins, double xmin, double xmax, double p0_init, 
    double p1_init, double p2_init, double p3_init, double p4_init, double p5_init, double& errP2, double& errP5, int nIterations = 30)
{
    std::cout << "    [bootstrap] Iniciando bootstrap con " << nIterations << " iteraciones..." << std::endl;

    std::vector<double> sigmas(nIterations, -1.0);
    std::vector<double> p2s, p5s;
    double sqrt2pi = std::sqrt(2 * TMath::Pi());

    #pragma omp parallel for
    for (int it = 0; it < nIterations; ++it) {
        TFile* file = new TFile(filename.c_str(), "READ");
        if (!file || file->IsZombie()) {
            std::cerr << "[ERROR][thread " << it << "] No se pudo abrir el archivo: " << filename << std::endl;
            continue;
        }

        TTree* tree = (TTree*)file->Get("pulse");
        if (!tree) {
            std::cerr << "[ERROR][thread " << it << "] No se encontró el árbol 'pulse'." << std::endl;
            file->Close();
            delete file;
            continue;
        }

        float lp2[4];
        tree->SetBranchAddress("LP2_50", lp2);
        Long64_t nEntries = tree->GetEntries();
        Long64_t sampleSize = nEntries / 5;

        std::string histName = "hBoot_" + std::to_string(it);
        TH1F* hBoot = new TH1F(histName.c_str(), "", nbins, xmin, xmax);

        TRandom3 rng(0 + it);
        for (Long64_t i = 0; i < sampleSize; ++i) {
            Long64_t idx = rng.Integer(nEntries);
            if (tree->GetEntry(idx) <= 0) continue;
            double dt = lp2[1] - lp2[0];
            if (dt >= xmin && dt <= xmax) hBoot->Fill(dt);
        }

        if (hBoot->GetEntries() < 10 || hBoot->Integral() == 0) {
            delete hBoot;
            file->Close();
            delete file;
            continue;
        }

        TF1* fitFunc = new TF1(("fitFunc_" + std::to_string(it)).c_str(),
            "[0]*TMath::Gaus(x,[1],[2],false)+[3]*TMath::Gaus(x,[4],[5],false)", xmin, xmax);
        fitFunc->SetParameters(p0_init, p1_init, p2_init, p3_init, p4_init, p5_init);

        if (hBoot->Fit(fitFunc, "Q0") == 0) {
            double p0 = fitFunc->GetParameter(0);
            double p1 = fitFunc->GetParameter(1);
            double p2 = fitFunc->GetParameter(2);
            double p3 = fitFunc->GetParameter(3);
            double p4 = fitFunc->GetParameter(4);
            double p5 = fitFunc->GetParameter(5);

            double M1 = sqrt2pi * (p0 * p2 * p1 + p3 * p5 * p4);
            double M2 = sqrt2pi * (p0 * p2 * (p1*p1 + p2*p2) + p3 * p5 * (p4*p4 + p5*p5));
            double N = sqrt2pi * (p0 * p2 + p3 * p5);

            double mu = M1 / N;
            double x2 = M2 / N;
            double sigma = std::sqrt(x2 - mu * mu);
            p2s.push_back(p2);
            p5s.push_back(p5);
            if (std::isfinite(sigma)) sigmas[it] = sigma;
        }

        delete fitFunc;
        delete hBoot;
        file->Close();
        delete file;
    }

    std::vector<double> valid;
    for (auto s : sigmas) if (s > 0) valid.push_back(s);
    errP2 = TMath::StdDev(p2s.begin(), p2s.end());
    errP5 = TMath::StdDev(p5s.begin(), p5s.end());
    return TMath::StdDev(valid.begin(), valid.end());
}

int main() {
    gROOT->SetBatch(kTRUE);
    ROOT::EnableImplicitMT(); // Tell ROOT you want to go parallel
    ROOT::EnableThreadSafety();
    std::cout << "[INFO] Iniciando análisis...\n";

    std::vector<int> attenuations = {36, 40, 46, 50, 56, 60, 66};
    std::string basePath = "/media/rene/Data/4d-tracking/paper/input-files/FNAL_BoardCH8/";//"/media/rene/Data/LGAD/converted/FNAL_CH8/";
    std::string filePrefix = "converted_runs_";
    std::string fileSuffix = "dB-att_6800mV_converted.root";

    //ACÁ SE PONEN LIMITES PARA GRAFICAR
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

    std::vector<TArrayD> results;
    size_t total = attenuations.size();

    for (size_t i = 0; i < total; ++i) {
        int att = attenuations[i];
        int percent = int((100.0 * (i + 1)) / total);

        std::cout << "[" << std::string(percent / 5, '#') << std::string(20 - percent / 5, '-') << "] "
                << percent << "% - Procesando " << att << " dB...\n";

        std::string filename = basePath + filePrefix + std::to_string(att) + fileSuffix;
        TFile* file = new TFile(filename.c_str());
        if (!file || file->IsZombie()) {
            std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
            continue;
        }
        TTree* tree = (TTree*)file->Get("pulse");
        if (!tree) continue;

        auto [nbins, xmin, xmax] = options[att];
        TH1F* hist = new TH1F("hist", "", nbins, xmin, xmax);
        tree->Project("hist", "LP2_50[1]-LP2_50[0]");

        TF1* fitFunc = new TF1("fitFunc",
            "[0]*TMath::Gaus(x,[1],[2],false)+[3]*TMath::Gaus(x,[4],[5],false)", xmin, xmax);

        TSpectrum spectrum(10);
        spectrum.Search(hist, 2, "nodraw", 0.05);
        double* xPeaks = spectrum.GetPositionX();
        double* yPeaks = spectrum.GetPositionY();

        std::vector<std::pair<double, double>> peaks;
        for (int j = 0; j < spectrum.GetNPeaks(); ++j)
            peaks.emplace_back(yPeaks[j], xPeaks[j]);
        std::sort(peaks.begin(), peaks.end(), std::greater<>());

        double p0_init = peaks[0].first;
        double p1_init = peaks[0].second;
        double p3_init = peaks[1].first;
        double p4_init = peaks[1].second;
        double p2_init = hist->GetRMS() / 4;
        double p5_init = hist->GetRMS() / 4;

        fitFunc->SetParameters(p0_init, p1_init, p2_init, p3_init, p4_init, p5_init);

        std::cout << "  Fitteando " << att << " dB..." << std::endl;
        hist->Fit(fitFunc, "RQ0");
        std::cout << "  Fit terminado para " << att << " dB." << std::endl;
        
        // Mostrar los parámetros del fit
        std::cout << "  Parámetros del fit para " << att << " dB:" << std::endl;
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
            double M2 = sqrt2pi * (p0 * p2 * (p1 * p1 + p2 * p2) + p3 * p5 * (p4 * p4 + p5 * p5));
            double N = sqrt2pi * (p0 * p2 + p3 * p5);

            double mu = M1 / N;
            double x2 = M2 / N;
            double sigma = std::sqrt(x2 - mu * mu);
            double errP2 = 0, errP5 = 0;
        std::cout << "  Llamando a bootstrapSigmaError()..." << std::endl;
        double sigma_error = bootstrapSigmaError(filename, nbins, xmin, xmax,
            fitFunc->GetParameter(0), fitFunc->GetParameter(1), fitFunc->GetParameter(2),
            fitFunc->GetParameter(3), fitFunc->GetParameter(4), fitFunc->GetParameter(5), errP2, errP5, 15);

        std::cout << "  Bootstrap terminado.\n";

        TArrayD arr(6);
        arr[0] = sigma;
        arr[1] = sigma_error;
        arr[2] = p2;
        arr[3] = p5;
        arr[4] = errP2;
        arr[5] = errP5;
        results.push_back(arr);

        std::cout << "  Resultado: σ = " << sigma << " ± " << sigma_error << " s\n\n";

        delete fitFunc;
        delete hist;
        delete file;
    }
    std::cout << "\nResumen final (σ ± error):\n";
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << attenuations[i] << " dB: σ = " << results[i][0]
                << " ± " << results[i][1] << " s\n";
    }
        // Guardar resultados en archivo ROOT
    TFile* outFile = new TFile("jitter_results_errors.root", "RECREATE");
    TTree* resultTree = new TTree("results", "Sigma y su error por atenuación");

    double attenuation, sigma, sigma_err;
    double p2_val, p5_val;
    double err_p2, err_p5;
    resultTree->Branch("attenuation", &attenuation);
    resultTree->Branch("sigma", &sigma);
    resultTree->Branch("sigma_err", &sigma_err);
    resultTree->Branch("p2", &p2_val, "p2/D");
    resultTree->Branch("p5", &p5_val, "p5/D");
    resultTree->Branch("err_p2", &err_p2, "err_p2/D");
    resultTree->Branch("err_p5", &err_p5, "err_p5/D");
    
    for (size_t i = 0; i < results.size(); ++i) {
        attenuation = attenuations[i];
        sigma       = results[i][0];
        sigma_err   = results[i][1];
        p2_val      = results[i][2];
        p5_val      = results[i][3];
        err_p2      = results[i][4];
        err_p5      = results[i][5];
        resultTree->Fill();
    }

    resultTree->Write();
    outFile->Close();
    delete outFile;

    std::cout << "\n[INFO] Resultados guardados en 'jitter_results_errors.root'\n";

    return 0;
}
