void compute_SNR() {
    TFile *f_amp = new TFile("AMP_results_errors.root");
    TFile *f_rms = new TFile("RMS_results_errors.root");

    TTree *t_amp = (TTree*)f_amp->Get("results");
    TTree *t_rms = (TTree*)f_rms->Get("results");

    double att_a, amp, amp_err;
    double att_r, rms, rms_err;
    double p1, p2, p4, p5;

    t_amp->SetBranchAddress("attenuation", &att_a);
    t_amp->SetBranchAddress("amp", &amp);
    t_amp->SetBranchAddress("amp_err", &amp_err);

    t_rms->SetBranchAddress("attenuation", &att_r);
    t_rms->SetBranchAddress("RMS", &rms);
    t_rms->SetBranchAddress("RMS_err", &rms_err);
    t_rms->SetBranchAddress("p1", &p1);
    t_rms->SetBranchAddress("p2", &p2);
    t_rms->SetBranchAddress("p4", &p4);
    t_rms->SetBranchAddress("p5", &p5);

    // Salida
    TFile *f_out = new TFile("SNR_results.root", "RECREATE");
    TTree *t_out = new TTree("results", "SNR y su error por atenuación");

    double attenuation, snr, snr_err;
    t_out->Branch("attenuation", &attenuation, "attenuation/D");
    t_out->Branch("SNR", &snr, "SNR/D");
    t_out->Branch("SNR_err", &snr_err, "SNR_err/D");

    Long64_t n = t_amp->GetEntries();
    if (t_rms->GetEntries() != n) {
        std::cerr << "ERROR: los árboles tienen diferente número de entradas." << std::endl;
        return;
    }
   
    for (Long64_t i = 0; i < n; ++i) {
        t_amp->GetEntry(i);
        t_rms->GetEntry(i);
        
        double rms_sel = 0.0;
            // Obtener el jitter como el mínimo de RMS, p1 y p4
        rms_sel = std::min({rms,p1,p4});    
            // Seleccionar la dispersión correspondiente al valor mínimo
        double rms_err_sel = 0.0;
        if (rms_sel == rms) rms_err_sel = rms_err ;
        else if (rms_sel == p1) rms_err_sel = p2;
        else if (rms_sel == p4) rms_err_sel = p5;
        std::cout << att_a << " dB: rms=" << rms << " p1=" << p1 <<" p4="<<p4 << std::endl;
        std::cout << att_a << " dB: rms_sel = " << rms_sel << " +/- " << rms_err_sel << std::endl;
        if (att_a != att_r) {
            std::cerr << "ERROR: las atenuaciones no coinciden en la entrada " << i << std::endl;
            continue;
        }

        attenuation = att_a;
        snr = amp / rms_sel;

        snr_err = std::sqrt(
            std::pow(amp_err / rms_sel, 2) +
            std::pow((amp * rms_err_sel) / (rms_sel * rms_sel), 2)
        );

        t_out->Fill();
    }

    f_out->Write();
    f_out->Close();

    std::cout << "[INFO] SNR calculado y guardado en SNR_results.root" << std::endl;
}