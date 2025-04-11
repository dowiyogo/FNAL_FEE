Fermilab_Adp_50Ohm = sparameters('Fermilab_Board_Ch1_20dB_At_in_out_Compensated_SOLT.s2p')
Fermi_Freq=Fermilab_Adp_50Ohm.Frequencies;
Ferm_gain = rfparam(Fermilab_Adp_50Ohm,2,1);
cir = circuit('entrada');
add(cir,[1 2],resistor(51));
add(cir,[2 3],capacitor(100e-9));
add(cir,[1 3],resistor(16.9));
setports(cir,[2 1],[3 1]);
cir_S = sparameters(cir,Fermi_Freq);
rfplot(cir_S,2,1)
figure;

rfplot(Fermilab_Adp_50Ohm,2,1);
figure;
Fermilab_Adp = sparameters('Adaptor_Board_V1_46dB.s2p');
rfplot(Fermilab_Adp,2,1)