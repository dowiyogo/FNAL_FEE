    #include <algorithm>  // Para std::min
    #include <vector>
void plot_jitter_vs_SNR() {
    // Abrir archivos ROOT

    TFile* snrFile = new TFile("SNR_results.root");
    TFile* jitterFile = new TFile("jitter_results_errors.root");

    if (!snrFile || !jitterFile || snrFile->IsZombie() || jitterFile->IsZombie()) {
        std::cerr << "[ERROR] No se pudieron abrir los archivos ROOT." << std::endl;
        return;
    }

    TTree* snrTree = (TTree*)snrFile->Get("results");
    TTree* jitterTree = (TTree*)jitterFile->Get("results");

    if (!snrTree || !jitterTree) {
        std::cerr << "[ERROR] No se pudieron obtener los TTree desde los archivos." << std::endl;
        return;
    }

    // Variables para leer desde los árboles
    double att_snr, snr, snr_err;
    double att_jitter, sigma, sigma_err, p2, p5, err_p2, err_p5;

    snrTree->SetBranchAddress("attenuation", &att_snr);
    snrTree->SetBranchAddress("SNR", &snr);
    snrTree->SetBranchAddress("SNR_err", &snr_err);

    jitterTree->SetBranchAddress("attenuation", &att_jitter);
    jitterTree->SetBranchAddress("sigma", &sigma);
    jitterTree->SetBranchAddress("sigma_err", &sigma_err);
    jitterTree->SetBranchAddress("p2", &p2);
    jitterTree->SetBranchAddress("p5", &p5);
    jitterTree->SetBranchAddress("err_p2", &err_p2);
    jitterTree->SetBranchAddress("err_p5", &err_p5);

    std::vector<double> x, y, ex, ey;

    Long64_t nEntries = snrTree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        snrTree->GetEntry(i);
        jitterTree->GetEntry(i);
        double jitter_raw = 0.0;
        // Obtener el jitter como el mínimo de sigma, p2 y p5
        jitter_raw = std::min({sigma, p2, p5});

        double jitter = jitter_raw * 1e12; // convertir a ps

        // Seleccionar el error correspondiente al valor mínimo
        double jitter_err = 0.0;
        if (jitter_raw == sigma) jitter_err = sigma_err * 1e12;
        else if (jitter_raw == p2) jitter_err = err_p2 * 1e12;
        else if (jitter_raw == p5) jitter_err = err_p5 * 1e12;
        
        if (jitter_err < 1e-3 || jitter < 1){ // checkea si hay sobreajuste
        jitter = sigma * 1e12;
        jitter_err = sigma_err*1e12;
        }

        x.push_back(snr);
        ex.push_back(snr_err);
        y.push_back(jitter);
        ey.push_back(jitter_err);
    }

    for (size_t i = 0; i < 7; ++i) {
        std::cout << "snr = " << x[i] << " +/- " << ex[i]  <<" jitter = "<< y[i]<<" +/- "<<ey[i]<<std::endl;
    }

    int nPoints = x.size();
    TGraphErrors* graph = new TGraphErrors(nPoints, &x[0], &y[0], &ex[0], &ey[0]);
    graph->SetTitle(";SNR;Jitter [ps]");
    graph->SetMarkerStyle(20);
    graph->SetMarkerColor(kBlue);

    TF1* fitFunc = new TF1("fitFunc", "[0]/x + [1]", 1, 500);
    fitFunc->SetParameters(1, 1);
    graph->Fit(fitFunc, "R");

    // Obtener parámetros y errores
    double a = fitFunc->GetParameter(0);
    double b = fitFunc->GetParameter(1);
    double a_err = fitFunc->GetParError(0);
    double b_err = fitFunc->GetParError(1);

    // Canvas estilo ATLAS
    TCanvas* c = new TCanvas("c", "Jitter vs SNR", 800, 600);
    c->SetLogx();                     // Escala log en eje X
    c->SetGrid();                     // Grid visible
    c->SetTicks(1, 1);                // Ticks en ambos lados
    c->SetLeftMargin(0.13);
    c->SetRightMargin(0.05);
    c->SetBottomMargin(0.12);

    // Dibujar gráfico y función
    graph->GetXaxis()->SetTitle("SNR");
    graph->GetYaxis()->SetTitle("Jitter [ps]");
    graph->Draw("AP");
    fitFunc->Draw("Same");

    // Estilo del TPaveText
    TPaveText* pave = new TPaveText(0.65, 0.75, 0.88, 0.88, "NDC");
    pave->SetFillColor(0);
    pave->SetTextFont(42);
    pave->SetTextSize(0.03);
    pave->SetBorderSize(1);
    pave->AddText(Form("estimated risetime = %.2f #pm %.2f ps", a, a_err));
    pave->AddText(Form("#delta t = %.2f #pm %.2f ps", b, b_err));
    pave->Draw();


    //c->BuildLegend();
    c->SetGrid();
    c->Update();
}
