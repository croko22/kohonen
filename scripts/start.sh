#!/bin/bash

# Crear carpetas base si no existen
echo "Verificando y creando carpetas necesarias..."
mkdir -p ./build
mkdir -p ./data/afromnist
mkdir -p ./data/fashionmnist
mkdir -p ./models

chmod +x ./build ./data ./models

cd ./scripts

# Función para manejar la descarga, descompresión y conversión
download_and_prepare() {
  local dataset_name=$1
  local url=$2
  local target_dir="../data/$dataset_name"
  local zip_path="../data/${dataset_name}.zip"

  # Descargar solo si no existe el ZIP
  if [ ! -f "$zip_path" ]; then
    echo "Descargando $dataset_name..."
    curl -L -o "$zip_path" "$url"
  else
    echo "$zip_path ya existe. Omitiendo descarga."
  fi

  # Descomprimir si el ZIP existe
  if [ -f "$zip_path" ]; then
    echo "Descomprimiendo $zip_path..."
    unzip -o "$zip_path" -d "$target_dir"
    echo "Eliminando $zip_path..."
    rm -f "$zip_path"
    fi
  
}

# URLs de los datasets
afromnist_url="https://www.kaggle.com/api/v1/datasets/download/danjwu/afromnist"
fashionmnist_url="https://www.kaggle.com/api/v1/datasets/download/zalando-research/fashionmnist"

# Función para verificar existencia de .csv
should_download() {
  local dataset_name="$1"
  local data_dir="./data/$dataset_name"

  # Si no existe el directorio o no hay archivos .csv, retorna 0 (true)
  if [ ! -d "$data_dir" ] || ! ls "$data_dir"/*.csv &>/dev/null; then
    return 0
  else
    echo "Dataset '$dataset_name' ya existe con archivos .csv. Se omite descarga."
    return 1
  fi
}

# Función ficticia de descarga (debes definirla tú)
download_and_prepare() {
  local name="$1"
  local url="$2"
  echo "Descargando y preparando '$name' desde $url ..."
  # Aquí va tu lógica real (por ejemplo, usar kaggle CLI)
}

# Procesar datasets solo si hace falta
if should_download "afromnist"; then
  download_and_prepare "afromnist" "$afromnist_url"
fi

if should_download "fashionmnist"; then
  download_and_prepare "fashionmnist" "$fashionmnist_url"
fi

#!/bin/bash

BASE_DIR="../data/afromnist"
PY_SCRIPT="./convert_to_npy.py"  # Asegúrate de que este es el nombre y está en el mismo directorio

if [ ! -f "$PY_SCRIPT" ]; then
  echo "Error: No se encuentra el script Python $PY_SCRIPT"
  exit 1
fi

echo "Buscando subcarpetas en $BASE_DIR..."

for dir in "$BASE_DIR"/*/; do
  # Elimina la ruta para obtener solo el nombre de la subcarpeta
  folder_name=$(basename "$dir")

  echo "Procesando carpeta: $folder_name"
  python3 "$PY_SCRIPT" "$BASE_DIR/$folder_name" "$folder_name"
done

echo "Conversión completada para todas las subcarpetas."


echo "Proceso completado."
