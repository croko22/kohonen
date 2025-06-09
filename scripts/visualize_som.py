import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import math

def load_som_model(filepath: str):
    """Carga un modelo SOM desde un archivo CSV."""
    print(f"Cargando modelo SOM desde: {filepath}")
    with open(filepath, 'r') as f:
        header = f.readline()
        rows, cols, input_size = map(int, header.strip().split(','))
    weights = np.loadtxt(filepath, delimiter=',', skiprows=1)
    if weights.shape[0] != rows * cols:
        raise ValueError("El número de neuronas en el archivo no coincide con las dimensiones rows*cols.")
    som_grid = weights.reshape((rows, cols, input_size))
    return som_grid, rows, cols

def visualize_umatrix(som_grid: np.ndarray):
    """Visualiza la U-Matrix del SOM para ver los clústeres."""
    rows, cols, _ = som_grid.shape
    u_matrix = np.zeros((rows, cols))
    for i in range(rows):
        for j in range(cols):
            current_neuron = som_grid[i, j]
            total_dist, num_neighbors = 0.0, 0
            for ni, nj in [(i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1)]:
                if 0 <= ni < rows and 0 <= nj < cols:
                    dist = np.linalg.norm(current_neuron - som_grid[ni, nj])
                    total_dist += dist
                    num_neighbors += 1
            u_matrix[i, j] = total_dist / num_neighbors

    fig, ax = plt.subplots(figsize=(8, 8))
    im = ax.imshow(u_matrix, cmap='gray_r', interpolation='nearest')
    ax.set_title('Self-Organizing Map: U-Matrix', fontsize=16)
    plt.colorbar(im, ax=ax, label='Distancia Promedio a Vecinos')
    plt.show()

def visualize_component_planes(som_grid: np.ndarray, feature_names: list = None):
    """Visualiza los planos de componentes para cada dimensión del dato de entrada."""
    rows, cols, input_size = som_grid.shape
    
    # Determinar un tamaño de cuadrícula razonable para los subplots
    grid_w = int(math.ceil(math.sqrt(input_size)))
    grid_h = int(math.ceil(input_size / grid_w))

    fig, axes = plt.subplots(grid_h, grid_w, figsize=(grid_w * 2.5, grid_h * 2.5))
    axes = axes.flatten()
    fig.suptitle('Planos de Componentes', fontsize=16)

    for i in range(input_size):
        plane = som_grid[:, :, i]
        im = axes[i].imshow(plane, cmap='viridis', interpolation='nearest')
        feature_title = feature_names[i] if feature_names and i < len(feature_names) else f'Feature {i+1}'
        axes[i].set_title(feature_title)
        axes[i].axis('off')
        fig.colorbar(im, ax=axes[i], shrink=0.8)

    # Ocultar ejes de subplots no utilizados
    for i in range(input_size, len(axes)):
        axes[i].axis('off')
        
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()

def visualize_hit_map(som_grid: np.ndarray, data: np.ndarray, labels: np.ndarray = None):
    """Calcula y visualiza el mapa de activaciones (hit map)."""
    rows, cols, _ = som_grid.shape
    hit_map = np.zeros((rows, cols), dtype=int)
    
    # (Opcional) Mapa para guardar la etiqueta más común para cada neurona
    if labels is not None:
        label_map = {}

    # Reshape de los datos si es necesario (ej. de (N, 28, 28) a (N, 784))
    data_flat = data.reshape(data.shape[0], -1)

    print("Calculando el mapa de activaciones...")
    for i, sample in enumerate(data_flat):
        # Encontrar la BMU (Best Matching Unit)
        distances = np.linalg.norm(som_grid - sample, axis=2)
        bmu_idx = np.unravel_index(np.argmin(distances, axis=None), distances.shape)
        hit_map[bmu_idx] += 1
        
        if labels is not None:
            if bmu_idx not in label_map:
                label_map[bmu_idx] = []
            label_map[bmu_idx].append(labels[i])
            
    fig, ax = plt.subplots(figsize=(8, 8))
    im = ax.imshow(hit_map, cmap='hot', interpolation='nearest')
    ax.set_title('Mapa de Activaciones (Hit Map)', fontsize=16)
    
    # Añadir el número de activaciones en cada celda
    for i in range(rows):
        for j in range(cols):
            count = hit_map[i, j]
            text_color = 'white' if count < hit_map.max() / 2 else 'black'
            ax.text(j, i, count, ha="center", va="center", color=text_color)
            
    plt.colorbar(im, ax=ax, label='Número de Activaciones (Hits)')
    plt.show()

    # (Opcional) Visualizar el mapa con las etiquetas asignadas
    if labels is not None:
        visualize_label_map(som_grid, label_map)

def visualize_label_map(som_grid: np.ndarray, label_map: dict):
    """Visualiza el SOM con colores según la etiqueta mayoritaria de cada neurona."""
    rows, cols, _ = som_grid.shape
    # Asignar la etiqueta más frecuente a cada neurona que fue activada
    majority_label_map = np.full((rows, cols), -1, dtype=int)
    for bmu_idx, label_list in label_map.items():
        majority_label_map[bmu_idx] = max(set(label_list), key=label_list.count)

    unique_labels = np.unique(majority_label_map)
    # Usar un colormap discreto
    cmap = plt.get_cmap('tab10', len(unique_labels))

    fig, ax = plt.subplots(figsize=(8, 8))
    im = ax.imshow(majority_label_map, cmap=cmap, interpolation='nearest')
    ax.set_title('Mapa de Etiquetas del SOM', fontsize=16)

    # Añadir leyenda de colores
    cbar = plt.colorbar(im, ticks=unique_labels, ax=ax, label='Clase Mayoritaria')
    cbar.ax.set_yticklabels(unique_labels)
    plt.show()


# --- Inicio del Script ---
if __name__ == '__main__':
    # --- 1. CONFIGURACIÓN ---
    # Ruta al modelo SOM guardado
    MODEL_FILE = '../models/kohonen_model.bin' # Asegúrate que esta ruta sea correcta

    # (Opcional) Rutas a tus datos para el Mapa de Activaciones
    # Cambia esto por la ruta a los datos con los que quieres probar el mapa
    DATA_FILE = '../data/train_images.npy'
    LABEL_FILE = '../data/train_labels.npy'

    try:
        # --- 2. CARGAR MODELO ---
        som_grid, rows, cols = load_som_model(MODEL_FILE)
        print(f"Modelo SOM cargado: {rows}x{cols} neuronas, tamaño de entrada {som_grid.shape[2]}")

        # --- 3. ELEGIR VISUALIZACIÓN ---
        
        print("\n--- Visualización 1: U-Matrix ---")
        visualize_umatrix(som_grid)
        
        print("\n--- Visualización 2: Planos de Componentes ---")
        visualize_component_planes(som_grid)

        print("\n--- Visualización 3: Mapa de Activaciones y Etiquetas ---")
        # Esta visualización necesita los datos originales
        try:
            input_data = np.load(DATA_FILE)
            input_labels = np.load(LABEL_FILE)
            visualize_hit_map(som_grid, input_data, labels=input_labels)
        except FileNotFoundError:
            print(f"❌ Archivos de datos no encontrados ('{DATA_FILE}', '{LABEL_FILE}'). Saltando Mapa de Activaciones.")
        except Exception as e:
            print(f"Ocurrió un error al cargar los datos para el Hit Map: {e}")

    except FileNotFoundError:
        print(f"❌ Error: El archivo de modelo '{MODEL_FILE}' no fue encontrado.")
    except Exception as e:
        print(f"Ocurrió un error: {e}")