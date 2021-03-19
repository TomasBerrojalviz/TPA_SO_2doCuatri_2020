#_________ MilangaPalace

if test -e restaurante.config; then
    rm restaurante.config
fi

echo "PUERTO_ESCUCHA=5005
IP_SINDICATO=10.108.64.2
PUERTO_SINDICATO=5003
IP_APP=10.108.64.4
PUERTO_APP=5004
QUANTUM=10
ALGORITMO_PLANIFICACION=FIFO
NOMBRE_RESTAURANTE=MilangaPalace
RETARDO_CICLO_CPU=5
IP_RESTAURANTE=10.108.64.3
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/MilangaPalace.log">> restaurante.config