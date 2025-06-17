#!/bin/bash

echo "¿Qué dataset deseas analizar?"
echo "1) afromnist"
echo "2) fashionmnist"
read -p "Selecciona una opción [1-2]: " dataset_opcion

# Variables para almacenar la elección
dataset=""
afro_subfolder=""

case "$dataset_opcion" in
  1)
    dataset="afromnist"
    echo "Has elegido Afromnist. Selecciona un subconjunto:"
    echo "1) Ethiopic"
    echo "2) NKo"
    echo "3) Osmanya"
    echo "4) Vai"
    read -p "Selecciona una opción [1-4]: " afro_opcion

    case "$afro_opcion" in
      1) afro_subfolder="Ethiopic" ;;
      2) afro_subfolder="NKo" ;;
      3) afro_subfolder="Osmanya" ;;
      4) afro_subfolder="Vai" ;;
      *)
        echo "Opción inválida. Saliendo..."
        exit 1
        ;;
    esac
    ;;
  2)
    dataset="fashionmnist"
    echo "Has elegido FashionMNIST."
    ;;
  *)
    echo "Opción inválida. Saliendo..."
    exit 1
    ;;
esac

# Mostrar resumen
echo "====================================="
echo "Dataset seleccionado: $dataset"
if [ "$dataset" == "afromnist" ]; then
  echo "Subcarpeta seleccionada: $afro_subfolder"
fi
echo "====================================="


# Construye FILE condicionalmente
if [ -n "$afro_subfolder" ]; then
  FILE="./data/$dataset/$afro_subfolder/${afro_subfolder}"
else
  FILE="./data/$dataset/${dataset}"
fi

echo "====================================="
if [ "$dataset" == "afromnist" ]; then
  echo "EJEUCTANDO Y ENTRENANDO KOHONEM CON : $dataset - $afro_subfolder"
else
  echo "EJEUCTANDO Y ENTRENANDO KOHONEM CON : $dataset"
fi 
echo "====================================="

make run-train3d FILE="$FILE"

make run-train-visualizer FILE="$FILE"

# Aquí podrías continuar con tu lógica, por ejemplo:
# python3 convert_to_npy.py "../data/$dataset/$afro_subfolder" "$afro_subfolder"
