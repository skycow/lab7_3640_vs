% myudfilter(1,2,100,80,1);
% myudfilter(1,2,100,40,2);
% myudfilter(1,2,100,20,3);
% myudfilter(1,2,100,10,4);

% %Read in real part
% fr      = fopen('real.bin','rb');
% ndimr   = fread(fr,1,'int');
% nchanr  = fread(fr,1,'int');
% dimr    = fread(fr,1,'int');
% dimrfs  = fread(fr,1,'int');
% dimr2   = fread(fr,1,'int');
% [xr,cnt]= fread(fr,inf,'float');
% 
% %Read in real part
% fc      = fopen('complex.bin','rb');
% ndimc   = fread(fc,1,'int');
% nchanc  = fread(fc,1,'int');
% dimc    = fread(fc,1,'int');
% dimcfs  = fread(fc,1,'int');
% dimc2   = fread(fc,1,'int');
% [xc,cnt]= fread(fc,inf,'float');

%fx = fopen('freq94_8_bw_4.bin','rb');
fx = fopen('stage1.bin','rb');
[xa,cnt] = fread(fx,inf,'float');
for int = 1:cnt/2
    xr(int) = xa(int*2-1);
    xc(int) = xa(int*2);
end

%Combine real and complex
x=complex(xr,xc);

%Mangitude Plot of singal
figure(1);
N=2^12;
f=[0:N-1]/N-0.5;
plot(f,20*log10(abs(fftshift(fft(x,N)))));

%Spectrogram plot of singal
figure(2);
nfft=2^9;
overlap=round(0.95*nfft);
window=hamming(nfft);
num = floor(length(x)/nfft)*nfft;
X = fft(diag(window)*reshape(x(1:num),nfft,[]));
X = X(fftshift([1:nfft]),:);
freq = [0:nfft-1]/nfft - 0.5;
freq = freq * 8+94.8;
time = [0:size(X,2)-1]*nfft/8000000;
imagesc(freq,time,20*log10(abs(X.')),[-50 -10]);
xlabel('Frequency [MHz]');
ylabel('Time [seconds]');

fclose(fx);
%fclose(fc);
