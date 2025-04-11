clear;
BaseOpt = detectImportOptions('Base_Noise_LowRes.txt');
BaseNLow = readmatrix('Base_Noise_LowRes.txt',BaseOpt);
BoardOpt = detectImportOptions('Board_Noise.txt');
BoardN = readmatrix('Board_Noise.txt',BoardOpt);
frecH = BoardN(:,1);
frecL = BaseNLow(:,1);
BaseNHigh(:,1) = BoardN(:,1);
BaseNHigh(:,2) = interp1(BaseNLow(:,1),BaseNLow(:,2),BoardN(:,1));
BoardNLow(:,1) = BaseNLow(:,1);
BoardNLow(:,2) = interp1(BoardN(:,1),BoardN(:,2),BaseNLow(:,1));

BaseLin = 10.^(BaseNLow(:,2)./10 -3);
BaseLinV = sqrt(BaseLin*50);
BoardLin = 10.^(BoardNLow(:,2)./10 -3);
BoardLinV = sqrt(BoardLin*50);
respLin = abs(BoardLin - BaseLin);
respLinV = abs(BoardLinV - BaseLinV);
respdB = 10.*log10(respLin);
%plot(frec,onLinV)
plot(frecL,respdB,frecL,BaseNLow(:,2),frecL,BoardNLow(:,2))
%semilogy(frec,respLinV)
title("Spectrum Analyzer")
xlabel("Frequency [GHz]")
ylabel("Noise Power [dBm]")
xlim([0 2E9])