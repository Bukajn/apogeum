import ctypes
import os

# Ścieżka do zbudowanej shared library
base_dir = os.path.dirname(__file__)

# ścieżka do pliku z data
lib_path = os.path.join(base_dir, "libmodule.so")
lib = ctypes.CDLL(lib_path)

# Wywołanie funkcji
result = lib.add(2, 3)
print("Result:", result)