import numpy as np
import pandas as pd
import os

def npy_to_mnist_csv(images_npy_path, labels_npy_path, output_csv_path):
    try:
        
        images = np.load(images_npy_path)
        labels = np.load(labels_npy_path)
        
        images_flat = images.reshape(images.shape[0], -1)

        combined_data = np.hstack((labels.reshape(-1, 1), images_flat))
        
        header = ['label'] + [f'pixel{i+1}' for i in range(images_flat.shape[1])]

        df = pd.DataFrame(combined_data, columns=header)

        df.to_csv(output_csv_path, index=False)
        print(f"Successfully converted '{images_npy_path}' and '{labels_npy_path}' to '{output_csv_path}'")

    except FileNotFoundError:
        print(f"Error: One or more .npy files not found. Please check paths:")
        print(f"  Images: {images_npy_path}")
        print(f"  Labels: {labels_npy_path}")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    train_images_npy = '../data/Ethiopic/Ethiopic_MNIST_X_test.npy'
    train_labels_npy = '../data/Ethiopic/Ethiopic_MNIST_Y_test.npy'
    output_train_csv = '../data/Ethiopic/Ethiopic_mnist_test.csv'

    
    if not os.path.exists(train_images_npy) or not os.path.exists(train_labels_npy):
        print(f"Dummy data not found. Creating dummy MNIST .npy files for demonstration.")
        
        dummy_images = np.random.randint(0, 256, size=(5, 28, 28), dtype=np.uint8)
        dummy_labels = np.random.randint(0, 10, size=(5,), dtype=np.uint8)
        np.save(train_images_npy, dummy_images)
        np.save(train_labels_npy, dummy_labels)
        print(f"Dummy '{train_images_npy}' and '{train_labels_npy}' created.")

    npy_to_mnist_csv(train_images_npy, train_labels_npy, output_train_csv)

    print("\n--- To process test data, uncomment and modify the following lines ---")
    
    
    
    

    
    
    
    
    
    
    
    

    