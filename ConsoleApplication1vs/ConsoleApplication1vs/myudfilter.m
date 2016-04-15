function myUDfilter(U,D,L,fp,fs)
N=max([U,D]);
fpass=fp;%0.9/(2*N);
fstop=fs;%1.1/(2*N);
f1=(fstop+fpass)/2;
f2=(fstop-fpass)/2;
n=[-L:L].';
h=(1/N)*sinc(2*f1*n).*sinc(2*f2*n);
win=hamming(2*L+1);
hw=h.*win;
NFFT=2^14;
freq=[0:NFFT-1]/NFFT;
subplot(211);
plot(freq,20*log10(abs(fft([h hw],NFFT))),'LineWidth',2);
hold on;
ax=axis();
plot(fpass*[1 1],ax(3:4),'r');
plot(1-fpass*[1 1],ax(3:4),'r');
plot(fstop*[1 1],ax(3:4),'r');
plot(1-fstop*[1 1],ax(3:4),'r');
title 'logscale'
xlabel 'frequency'
ylabel 'magnitude(dB)'
hold off;
grid on;
%ylim([-10010]);
subplot(212);
plot(freq,abs(fft([h hw],NFFT)),'Linewidth',2);
hold on;
ax=axis();
plot(fpass*[1 1],ax(3:4),'r');
plot(1-fpass*[1 1],ax(3:4),'r');
plot(fstop*[1 1],ax(3:4),'r');
plot(1-fstop*[1 1],ax(3:4),'r');
title 'linearscale'
xlabel 'frequency'
ylabel 'magnitude'
hold off;
grid on;
file_name=sprintf('lpf_U%d_D%d_L%d.bin',U,D,L);
fid=fopen(file_name,'wb');
fwrite(fid,[1 1 length(h) 1 0],'int');
fwrite(fid,h,'float');
fclose(fid);
end
