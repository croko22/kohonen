#!/usr/bin/env python3

import numpy as np
import pandas as pd
import os
import argparse
import glob

def convert_dataset(folder, name):
    folder = os.path.abspath(folder)
    print(f"Buscando archivos en {folder} con nombre base: {name}")

    sets = ['train', 'test']
    for subset in sets:
        x_path = os.path.join(folder, f"{name}_MNIST_X_{subset}.npy")
        y_path = os.path.join(folder, f"{name}_MNIST_y_{subset}.npy")

        if not os.path.exists(x_path) or not os.path.exists(y_path):
            print(f"Archivos faltantes para {subset}:")
            if not os.path.exists(x_path): print(f" - No se encontró {x_path}")
            if not os.path.exists(y_path): print(f" - No se encontró {y_path}")
            continue

        print(f"Procesando {subset}...")
        X = np.load(x_path)
        y = np.load(y_path)

        if len(X) != len(y):
            print(f"Dimensiones no coinciden en {subset}: X tiene {len(X)} muestras y y tiene {len(y)} etiquetas.")
            continue

        # Aplanar imágenes (ej. 28x28 -> 784)
        X_flat = X.reshape(X.shape[0], -1)

        # Crear DataFrame: primera columna es etiqueta
        df = pd.DataFrame(X_flat)
        df.insert(0, "label", y)

        # Guardar CSV
        csv_path = os.path.join(folder, f"{name}_{subset}.csv")
        df.to_csv(csv_path, index=False)
        print(f"Guardado: {csv_path}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Convertir archivos .npy a .csv combinando X e y.")
    parser.add_argument("folder", help="Carpeta que contiene los archivos .npy")
    parser.add_argument("name", help="Nombre base del dataset (ej. afromnist, fashionmnist)")

    args = parser.parse_args()
    convert_dataset(args.folder, args.name)
