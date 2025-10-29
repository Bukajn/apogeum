import ctypes
import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import filedialog

output_dir = None

def select_files_simple():
    """Wybiera pliki jeden po drugim"""
    root = tk.Tk()
    root.withdraw()
    
    files = {}

    global output_dir
    output_dir = filedialog.askdirectory(
    title="Wybierz folder ZAPISU wyników",
    initialdir=os.path.expanduser("~")
    )
    if not output_dir:
        output_dir = os.getcwd()


    files['barometer'] = filedialog.askopenfilename(
        title="1/3: Wybierz plik BAROMETRU",
        initialdir=output_dir,
        filetypes=[("CSV files", "*.csv")]
    )
    if not files["barometer"]:
        exit()
    
    files['accelerometer'] = filedialog.askopenfilename(
        title="2/3: Wybierz plik AKCELEROMETRU", 
        initialdir=output_dir,
        filetypes=[("CSV files", "*.csv")]
    )
    if not files["accelerometer"]:
        exit()

    files['pos'] = filedialog.askopenfilename(
        title="3/3: Wybierz plik z pozycją",
        initialdir=output_dir, 
        filetypes=[("CSV files", "*.csv")]
    )
    if not files["pos"]:
        exit()
    
    root.destroy()
    return files

def calculate_derivative(df, time_col='t', value_col='z'):
    """
    Oblicza pochodną kolumny względem czasu
    
    Args:
        df: DataFrame z danymi
        time_col: nazwa kolumny z czasem (domyślnie 't')
        value_col: nazwa kolumny do różniczkowania (domyślnie 'az')
    
    Returns:
        DataFrame z dodaną kolumną pochodnej
    """
    
    
    df_result = df.copy()
    
    # Oblicz różnice
    dt = df[time_col].diff()  # różnica czasu
    dy = df[value_col].diff()  # różnica wartości
    
    # Pochodna = dy/dt
    derivative = dy / dt
    
    # Pierwszy punkt - ustaw na 0 lub interpoluj
    derivative.iloc[0] = 0  # lub derivative.iloc[1] dla pierwszego punktu
    
    df_result[value_col] = derivative
    
    return df_result

def calculate_integral(df, time_col='t', value_col='z', initial_value=0):
    """
    Oblicza całkę kolumny względem czasu (metoda trapezów)
    
    Args:
        df: DataFrame z danymi
        time_col: nazwa kolumny z czasem (domyślnie 't')
        value_col: nazwa kolumny do całkowania (domyślnie 'az')
        initial_value: wartość początkowa całki (domyślnie 0)
    
    Returns:
        DataFrame z dodaną kolumną całki
    """
  
    
    df_result = df.copy()
    
    # Metoda trapezów: ∫f(x)dx ≈ Σ(f(i) + f(i+1))/2 * Δx
    integral_values = [initial_value]  # wartość początkowa
    
    for i in range(1, len(df)):
        dt = df[time_col].iloc[i] - df[time_col].iloc[i-1]
        avg_value = (df[value_col].iloc[i] + df[value_col].iloc[i-1]) / 2
        integral_step = avg_value * dt
        integral_values.append(integral_values[-1] + integral_step)
    
    df_result[value_col] = integral_values
    
    return df_result

def pressureToAltitude(df: pd.DataFrame):
    def fun(p):
        p0 = 101325.0  # standardowe ciśnienie na poziomie morza (Pa)
        T = 288.15  # standardowa temperatura na poziomie morza (Kelwiny)
        L = 0.0065  # spadek temp. na 1 metr, K/m
        R = 8.31447  # uniwersalna stała gazowa, J/(mol·K)
        g = 9.80665  # przyspieszenie ziemskie, m/s^2
        M = 0.0289644  # molowa masa powietrza, kg/mol

        # h = (T / L) * (1 - (p / p0) ^ ((R * L) / (g * M)))
        return (T / L) * (1 - pow(p / p0, (R * L) / (g * M)))
    
    df = df.copy()
    df['z'] = df.apply(lambda p: fun(p["pressure"]), axis=1)
    df = df.drop("pressure", axis=1)
    return df

def drawBackground(axe):
    axe.axvspan(apogeum_time, apogeum_time+2, alpha=0.3, color='green')
    axe.axvspan(apogeum_time+2, apogeum_time+4, alpha=0.3, color='orange')
    axe.axvspan(apogeum_time+4, apogeum_time+6, alpha=0.3, color='red')

files = select_files_simple()

# Ścieżka do zbudowanej shared library
base_dir = os.path.dirname(__file__)
# ścieżka do pliku z data
lib_path = os.path.join(base_dir, "libmodule.so") 
lib = ctypes.CDLL(lib_path)
lib.filter.argtypes = [
    ctypes.c_char_p,  # output_file
    ctypes.c_char_p,  # barometer_file
    ctypes.c_char_p   # accelerometer_file
]
_dirname = os.getcwd()

output_file = os.path.join(output_dir, "test.txt")
# Wywołanie funkcji
result = lib.filter(
    output_file.encode("utf-8"), 
    files["barometer"].encode("utf-8"), 
    files["accelerometer"].encode("utf-8")) 

#pobieranie danych z plików
acceleration = pd.read_csv(os.path.join(_dirname,  files["accelerometer"]))[["t","az"]]
acceleration['z'] = acceleration['az']
acceleration.drop('az', axis=1, inplace=True)

pressure = pd.read_csv(os.path.join(_dirname, files["barometer"]))

clean_position = pd.read_csv(os.path.join(_dirname,  files["pos"]))[["t","altitude"]]
clean_position['z'] = clean_position['altitude']
clean_position.drop('altitude', axis=1, inplace=True)
h0 = clean_position['z'].iloc[0]
apogeum_idx = clean_position['z'].idxmax()  # indeks maksymalnej wysokości
apogeum_time = clean_position.loc[apogeum_idx, 't'] 

kalman = pd.read_csv(output_file)
#Wykonanie obliczeń

#Z akcelometru
velocity_by_acceleration = calculate_integral(acceleration)

position_by_acceleration  = calculate_integral(velocity_by_acceleration)
position_by_acceleration["z"] += h0

#Z znanej pozycji
velocity_by_clean_position = calculate_derivative(clean_position)
acceleration_by_clean_position = calculate_derivative(velocity_by_clean_position)

#Z barometru
position_by_pressure = pressureToAltitude(pressure)
velocity_by_pressure = calculate_derivative(position_by_pressure)
acceleration_by_pressure = calculate_derivative(velocity_by_pressure)

#-----------Wykresy----------------------


fig, axes = plt.subplots(1, 3, figsize=(12, 5))

#Wykres wysokości
axe = axes[0]
axe.set_title("Wysokość")
axe.set_xlabel("Czas [s]")
axe.set_ylabel("Wysokość [m]")

axe.plot( clean_position['t'], clean_position['z'], label='Znana pozycja', color="blue")
axe.plot( position_by_acceleration['t'], position_by_acceleration['z'], label='Z akcelometru', color="orange")
axe.plot( position_by_pressure['t'], position_by_pressure['z'], label='Z barometru', color="green")
axe.plot( kalman['t'], kalman['z'], label='Z filtru kalmana', color="red")

drawBackground(axe)

axe.grid()
axe.legend()

#Wykres prędkości 
axe = axes[1]
axe.set_title("Prędkość")
axe.set_xlabel("Czas [s]")
axe.set_ylabel("Prędkość [m/s]")

axe.plot( velocity_by_clean_position['t'], velocity_by_clean_position['z'], label='Z znanej pozycji', color="blue")
axe.plot( velocity_by_acceleration['t'], velocity_by_acceleration['z'], label='Z akcelometru', color="orange")
# axe.plot( velocity_by_pressure['t'], velocity_by_pressure['z'], label='Z barometru', color="green")
axe.plot( kalman['t'], kalman['v'], label='Z filtru kalmana', color="red")

drawBackground(axe)

axe.grid()
axe.legend()

#Wykres przyspieszenia
axe = axes[2]
axe.set_title("Przyspieszenie")
axe.set_xlabel("Czas [s]")
axe.set_ylabel("Przyspieszenie [m/s^2]")

axe.plot( acceleration_by_clean_position['t'], acceleration_by_clean_position['z'], label='Z znanej pozycji', color="blue")
axe.plot( acceleration['t'], acceleration['z'], label='Z akcelometru', color="orange")
# axe.plot( acceleration_by_pressure['t'], acceleration_by_pressure['z'], label='Z barometru', color="green")

drawBackground(axe)

axe.legend()
axe.grid()


info_text = f"""Użyte pliki:
Barometr: {os.path.relpath(files['barometer'])}
Akcelerometr: {os.path.relpath(files['accelerometer'])}
Pozycja: {os.path.relpath(files['pos'])}"""

fig.text(0.02, 0.02, info_text, fontsize=8, verticalalignment='bottom', 
         bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.8))


plt.tight_layout(rect=[0, 0.15, 1, 1])
plt.savefig(os.path.join(output_dir, "result.png"), dpi=600, bbox_inches='tight')
plt.show()