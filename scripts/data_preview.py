import numpy as np
import matplotlib.pyplot as plt

def view_dataset_images(image_path: str, label_path: str, num_images: int = 25, class_names: dict = None):
    """
    Carga y muestra una cuadrícula de imágenes aleatorias de un dataset en formato NumPy.

    Args:
        image_path (str): Ruta al archivo .npy que contiene las imágenes.
        label_path (str): Ruta al archivo .npy que contiene las etiquetas.
        num_images (int): El número total de imágenes a mostrar (debería ser un cuadrado perfecto).
        class_names (dict, optional): Un diccionario para mapear etiquetas numéricas a nombres de clase.
    """
    try:
        
        print(f"Cargando imágenes desde: {image_path}")
        images = np.load(image_path)
        print(f"Cargando etiquetas desde: {label_path}")
        labels = np.load(label_path)
        
        
        if images.shape[0] != labels.shape[0]:
            print("Error: El número de imágenes y etiquetas no coincide.")
            return
            
        print(f"El dataset contiene {images.shape[0]} imágenes.")
        print(f"Forma de las imágenes (shape): {images.shape}")
        
    except FileNotFoundError as e:
        print(f"❌ Error: No se pudo encontrar el archivo. Asegúrate de que la ruta sea correcta.\n{e}")
        return
    except Exception as e:
        print(f"Ocurrió un error al cargar los archivos: {e}")
        return

    
    
    grid_size = int(np.sqrt(num_images))
    if grid_size * grid_size != num_images:
        grid_size = int(images.shape[0]**0.5)
        num_images = grid_size**2
        print(f"Advertencia: num_images no es un cuadrado perfecto. Se usará una cuadrícula de {grid_size}x{grid_size}.")

    fig, axes = plt.subplots(grid_size, grid_size, figsize=(10, 10))
    
    fig.suptitle(f'Muestra de Imágenes de {image_path.split("/")[-1]}', fontsize=16)
    
    
    axes = axes.flatten()

    
    
    random_indices = np.random.choice(images.shape[0], size=num_images, replace=False)

    for i, ax in enumerate(axes):
        idx = random_indices[i]
        image = images[idx]
        
        label = labels[idx].flatten()[0] 

        
        if image.ndim == 2 or image.shape[-1] == 1:
            ax.imshow(image.squeeze(), cmap='gray')
        else:
            ax.imshow(image)
        
        
        display_label = class_names.get(label, label) if class_names else label
        
        ax.set_title(f"Etiqueta: {display_label}")
        ax.axis('off')  

    plt.tight_layout(rect=[0, 0.03, 1, 0.95]) 
    plt.show()


if __name__ == '__main__':
    
    
    files = {
        "train_images": "../data/train_images.npy",
        "train_labels": "../data/train_labels.npy",
        "val_images": "../data/val_images.npy",
        "val_labels": "../data/val_labels.npy",
        "test_images": "../data/test_images.npy",
        "test_labels": "../data/test_labels.npy"
    }
    dataset_to_view = "train"

    print(f"--- Visualizando el dataset de '{dataset_to_view}' ---")
    
    image_file_path = files.get(f"{dataset_to_view}_images")
    label_file_path = files.get(f"{dataset_to_view}_labels")

    if image_file_path and label_file_path:
        
        view_dataset_images(
            image_path=image_file_path,
            label_path=label_file_path,
            num_images=25
            
        )
    else:
        print(f"Error: No se encontraron las rutas para '{dataset_to_view}'. Revisa el diccionario 'files'.")