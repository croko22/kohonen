import numpy as np
import matplotlib.pyplot as plt

# file_path = "../models/kohonen_model_2D.bin"  
file_path = "../models/kohonen_model_3D.bin"  

with open(file_path, 'r') as f:
    lines = f.readlines()

if lines:
    lines = lines[1:]
else:
    print("El archivo está vacío o no contiene datos.")
    lines = []

patterns = []
for line in lines[:100]:  
    pattern = list(map(float, line.strip().split(',')))
    if len(pattern) == 784:
        patterns.append(np.array(pattern).reshape(28, 28))
    else:
        print("Línea ignorada, tamaño incorrecto:", len(pattern))

print(f"Se cargaron {len(patterns)} patrones.")

N = min(100, len(patterns))  
rows = int(np.sqrt(N))
cols = int(np.ceil(N / rows))

plt.figure(figsize=(10, 10))
for i in range(N):
    plt.subplot(rows, cols, i + 1)
    plt.imshow(patterns[i], cmap='gray', vmin=-1, vmax=1)
    plt.axis('off')

plt.suptitle("Patrones Kohonen desde archivo de texto", fontsize=14)
plt.tight_layout()
plt.show()