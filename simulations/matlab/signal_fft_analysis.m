%% Analog Audio Amplifier Signal FFT Analysis
% This script simulates a composite audio test signal (comprising low and
% mid-frequency components) and computes its Fast Fourier Transform (FFT)
% to visualize the frequency-domain spectrum.
%
% This is used to analyze the output spectrum, verify filter thresholds,
% and check for distortion/harmonics in simulations.

clear; clc; close all;

%% 1. Signal Parameters
Fs = 48000;              % Sampling frequency (48 kHz standard audio rate)
t = 0:1/Fs:0.01;         % Time vector (10 ms duration for high-frequency resolution)

% Input signal: 500 Hz (fundamental low/bass tone) + 2000 Hz (mid/treble tone)
signal = sin(2*pi*500*t) + 0.5*sin(2*pi*2000*t);

%% 2. FFT Computation
N = length(signal);      % Signal length
Y = fft(signal);         % Compute the Fast Fourier Transform

% Compute the two-sided spectrum P2 and the single-sided spectrum P1
P2 = abs(Y/N);
P1 = P2(1:N/2+1);
P1(2:end-1) = 2*P1(2:end-1);

% Define the frequency domain f
f = (0:(N/2))*(Fs/N);

%% 3. Plotting the Amplitude Spectrum
figure('Color', [1 1 1]);
plot(f, P1, 'LineWidth', 2, 'Color', [0 0.4470 0.7410]);
grid on;

% Labeling and styling
title('Amplitude Spectrum of Composite Test Signal', 'FontSize', 12, 'FontWeight', 'bold');
xlabel('Frequency (Hz)', 'FontSize', 10);
ylabel('Magnitude |X(f)|', 'FontSize', 10);
xlim([0 4000]);          % Focus on the region containing our test frequencies
ylim([0 1.2]);           % Adjust limits for visual comfort

% Add text tags to the peak frequencies
text(500, 1.05, ' \leftarrow 500 Hz Tone (1.0)', 'FontSize', 9, 'FontWeight', 'bold');
text(2000, 0.55, ' \leftarrow 2 kHz Tone (0.5)', 'FontSize', 9, 'FontWeight', 'bold');
