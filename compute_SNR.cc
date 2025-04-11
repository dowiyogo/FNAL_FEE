void compute_SNR() {
    TFile *f_amp = new TFile("AMP_results_errors.root");
    TFile *f_rms = new TFile("RMS_results_errors.root");

    TTree *t_amp = (TTree*)f_amp->Get("results");
    TTree *t_rms = (TTree*)f_rms->Get("results");

    double att_a, amp, amp_err;
    double att_r, rms, rms_err;

    t_amp->SetBranchAddress("attenuation", &att_a);
    t_amp->SetBranchAddress("amp", &amp);
    t_amp->SetBranchAddress("amp_err", &amp_err);

    t_rms->SetBranchAddress("attenuation", &att_r);
    t_rms->SetBranchAddress("RMS", &rms);
    t_rms->SetBranchAddress("RMS_err", &rms_err);

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

        if (att_a != att_r) {
            std::cerr << "ERROR: las atenuaciones no coinciden en la entrada " << i << std::endl;
            continue;
        }

        attenuation = att_a;
        snr = amp / rms;

        snr_err = std::sqrt(
            std::pow(amp_err / rms, 2) +
            std::pow((amp * rms_err) / (rms * rms), 2)
        );

        t_out->Fill();
    }

    f_out->Write();
    f_out->Close();

    std::cout << "[INFO] SNR calculado y guardado en SNR_results.root" << std::endl;
}