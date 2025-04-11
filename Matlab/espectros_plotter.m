onLin = 10.^(ondB./10 -3);
onLinV = sqrt(onLin*50);
offLin = 10.^(offdB./10 -3);
offLinV = sqrt(offLin*50);
respLin = abs(onLin - offLin);
respLinV = abs(onLinV - offLinV);
respdB = 10.*log10(respLin);
%plot(frec,onLinV)
plot(frec,onLinV)
%semilogy(frec,respLinV)
title("Spectrum Analyzer")
xlabel("Frequency [GHz]")
ylabel("Noise Amplitude [V]")
xlim([0 3E9])