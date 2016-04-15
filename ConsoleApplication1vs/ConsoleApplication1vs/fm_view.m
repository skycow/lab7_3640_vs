% NOTES: This data was sampled at 8 MHz, complex interleaved, float.
% The center frequency is 94.8 MHz


fid=fopen('freq94_8_bw_4.bin','rb');
[x,cnt]=fread(fid,2*100,'float'); % Always skip over the first 100
                                  % samples (x2 for complex samples)

Fc = 94.8; % MHz, center frequency
Fs = 8; % MHz, samples/second

figure(2); clf;
NFFT = 2^9; % FFT size
Nwin = 2^8; % Number of windows
win = hamming(NFFT);
freq = [0:NFFT-1]/NFFT - 0.5;
freq = freq * Fs;
freq = freq + Fc;
time = [0:NFFT:NFFT*Nwin-1]/(Fs*1e6);
lam = 0.925;

S = zeros(NFFT,1);
R = zeros(NFFT,1);
Scnt = 1;
X = ones(Nwin,NFFT);
iX = 1;

h1 = axes('Position',[0.1 0.6 0.8 0.35]);
set(h1,'XAxisLocation','top');
hp1a = plot(freq,10*log10(S)); hold on;
hp1b = plot(freq,10*log10(R),'Color',[0 0.5 0]); hold off;
xlim(freq([1,end]));
ylim([-60 -10]);
ylabel('Magnitude [dB]');
title('Spectral Analysis of FM Data');

h2 = axes('Position',[0.1 0.1 0.8 0.5]);
hp2 = imagesc(freq,time*1000,10*log10(abs(X)),[-50 -10]);
xlabel('Frequency [Hz]');
ylabel('Time [milliseconds]');

num_steps = 4000;
[x,cnt]=fread(fid,2*NFFT,'float');
while(cnt==2*NFFT)
x=complex(x(1:2:end),x(2:2:end));
xf = fftshift(fft(win.*x,NFFT));
X(iX,:) = 20*log10(abs(xf)).';
iX = iX + 1;
if(iX>Nwin) iX = 1; end;
S = lam*S + (1-lam)*abs(xf).^2;
R = max(R,S);
Scnt = Scnt + 1;
if(Scnt > Nwin) Scnt = Nwin; end
set(hp1a,'YData',10*log10(S));
set(hp1b,'YData',10*log10(R));
set(hp2,'CData',X);
drawnow;
[x,cnt]=fread(fid,2*NFFT,'float');
end

return;

[x,cnt]=fread(fid,2*2^20,'float');
x = complex(x(1:2:end),x(2:2:end));
Fc = 94.8; % MHz, center frequency
Fs = 8; % MHz, samples/second
%figure(1); clf;
%plot(abs(x));
figure(2); clf;
NFFT = 2^10; % FFT size
Nwin = 2^10; % Number of windows
X = reshape(x(1:NFFT*Nwin),NFFT,Nwin);
X = fft(X,NFFT);
X = X(fftshift([1:NFFT]),:); % FFT shift
S = mean(abs(X).^2,2);
freq = [0:NFFT-1]/NFFT - 0.5;
freq = freq * Fs;
freq = freq + Fc;
time = [0:NFFT:NFFT*Nwin-1]/(Fs*1e6);

figure(1); clf;
subplot(211);
plot(freq,10*log10(S));
xlim(freq([1,end]));
grid on;
subplot(212);
imagesc(freq,time,10*log10(abs(X.')),[-20 -10]);
grid on;

Ft = 92.9; % MHz of target station

fclose(fid);

