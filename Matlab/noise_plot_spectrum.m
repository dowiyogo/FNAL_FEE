clear;
BoardOFFOpt = detectImportOptions('BoardOFF.txt');
BoardOFF = readmatrix('BoardOFF.txt',BoardOFFOpt);
BoardONOpt = detectImportOptions('BoardON.txt');
BoardON = readmatrix('BoardON.txt',BoardONOpt);
frec = BoardON(:,1)/1E9;
BaseLin = 10.^(BoardOFF(:,2)./10 -3);
BaseLinV = sqrt(BaseLin*50);
BoardLin = 10.^(BoardON(:,2)./10 -3);
BoardLinV = sqrt(BoardLin*50);
respLin = abs(BoardLin - BaseLin);
respLinV = abs(BoardLinV - BaseLinV);
respdB = 10.*log10(respLin);
%plot(frec,onLinV)
plot(frec,respdB,frec,BoardON(:,2),frec,BoardOFF(:,2))
%semilogy(frec,respLinV)
title("Spectrum Analyzer")
xlabel("Frequency [GHz]")
ylabel("Noise Power [dBm]")
legend('Estimated board noise','Measured total noise','Oscilloscope baseline noise')
xlim([0 1.8])
ylim([-150 -50])
box(axes1,'on');
grid(axes1,'on');
set(axes1,'FontSize',15,'FontWeight','bold');
legend1 = legend(axes1,'show');
set(legend1,'FontSize',18,'FontWeight','normal');