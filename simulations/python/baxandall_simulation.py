#!/usr/bin/env python3
"""
Baxandall Active Tone Control Filter Simulation
----------------------------------------------
This script models and simulates the frequency response of an active Baxandall
tone control stage (using a NE5532 op-amp front-end). It defines the analytical
s-plane transfer functions for the active Bass (low-shelf) and Treble (high-shelf)
filters, computes their magnitude responses across the audible audio spectrum 
(20 Hz to 20 kHz), and plots the shelving curves for various boost/cut settings.
"""

import os
import numpy as np
import matplotlib.pyplot as plt

def shelf_low(s, omega_c, gain_db):
    """
    Analytical transfer function of a Low-Shelving (Bass) filter.
    H(s) = (s + omega_c * 10^(A/40)) / (s + omega_c * 10^(-A/40))
    Gives a gain of gain_db at DC (low frequencies) and 0 dB at high frequencies.
    """
    g = 10 ** (gain_db / 40.0)
    num = s + omega_c * g
    den = s + omega_c / g
    return num / den

def shelf_high(s, omega_c, gain_db):
    """
    Analytical transfer function of a High-Shelving (Treble) filter.
    H(s) = (s * 10^(A/40) + omega_c) / (s * 10^(-A/40) + omega_c)
    Gives a gain of 0 dB at DC (low frequencies) and gain_db at high frequencies.
    """
    g = 10 ** (gain_db / 40.0)
    num = s * g + omega_c
    den = s / g + omega_c
    return num / den

def simulate_baxandall():
    # 1. Frequency Vector Setup (Audible Range: 20 Hz to 20 kHz)
    f = np.logspace(1.3, 4.3, 1000)  # Logarithmic spacing from ~20 Hz to 20 kHz
    s = 2 * np.pi * 1j * f           # Laplace variable s = j * omega
    
    # 2. Filter Cutoff Corner Frequencies
    fc_bass = 200.0   # Bass corner frequency: 200 Hz
    fc_treble = 3000.0 # Treble corner frequency: 3.0 kHz
    omega_cb = 2 * np.pi * fc_bass
    omega_ct = 2 * np.pi * fc_treble
    
    # 3. Define Gain Settings to Simulate (in dB)
    gain_levels = [-15, -10, -5, 0, 5, 10, 15]
    
    # Create the Plot
    plt.figure(figsize=(10, 6))
    
    # Plot Bass curves (solid lines)
    for g in gain_levels:
        H = shelf_low(s, omega_cb, g)
        mag_db = 20 * np.log10(np.abs(H))
        
        # Style lines based on boost, cut, or neutral
        if g == 0:
            line_style = '--'
            color = 'black'
            label = 'Flat (0 dB)'
        elif g > 0:
            line_style = '-'
            color = plt.cm.Blues(0.4 + g / 25.0)
            label = f'Bass Boost +{g} dB' if g == 15 else None
        else:
            line_style = '-'
            color = plt.cm.Oranges(0.4 - g / 25.0)
            label = f'Bass Cut {g} dB' if g == -15 else None
            
        plt.semilogx(f, mag_db, linestyle=line_style, color=color, label=label)
        
    # Plot Treble curves (dot-dashed lines)
    for g in gain_levels:
        if g == 0:
            continue # Already plotted flat curve
            
        H = shelf_high(s, omega_ct, g)
        mag_db = 20 * np.log10(np.abs(H))
        
        if g > 0:
            line_style = '-.'
            color = plt.cm.Purples(0.4 + g / 25.0)
            label = f'Treble Boost +{g} dB' if g == 15 else None
        else:
            line_style = '-.'
            color = plt.cm.Reds(0.4 - g / 25.0)
            label = f'Treble Cut {g} dB' if g == -15 else None
            
        plt.semilogx(f, mag_db, linestyle=line_style, color=color, label=label)
        
    # Formatting the Plot
    plt.title('Baxandall Active Tone Control Response Curves', fontsize=14, fontweight='bold', pad=15)
    plt.xlabel('Frequency (Hz)', fontsize=11)
    plt.ylabel('Gain (dB)', fontsize=11)
    plt.grid(True, which="both", ls="-", alpha=0.4)
    plt.axhline(0, color='black', linewidth=0.8, alpha=0.5)
    plt.xlim([20, 20000])
    plt.ylim([-20, 20])
    
    # Professional legend and layout
    plt.legend(loc='lower left', frameon=True, facecolor='#f8f9fa', edgecolor='#ddd', fontsize=9.5)
    plt.tight_layout()
    
    # Save the output plot to images directory
    images_dir = os.path.join(os.path.dirname(__file__), '..', '..', 'images')
    if not os.path.exists(images_dir):
        os.makedirs(images_dir)
        
    plot_path = os.path.join(images_dir, 'baxandall_response.png')
    plt.savefig(plot_path, dpi=150)
    print(f"[+] Active Baxandall simulation plot saved successfully to: {plot_path}")
    
    # Also save to current directory for preview if needed
    plt.close()

if __name__ == '__main__':
    simulate_baxandall()
