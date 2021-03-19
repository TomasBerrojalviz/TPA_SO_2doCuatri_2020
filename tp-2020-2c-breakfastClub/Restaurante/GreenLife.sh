#_________ GreenLife

if test -e restaurante.config; then
    rm restaurante.config
fi

echo "PUERTO_ESCUCHA=5006
IP_SINDICATO=10.108.64.2
PUERTO_SINDICATO=5003
IP_APP=10.108.64.4
PUERTO_APP=5004
QUANTUM=10
ALGORITMO_PLANIFICACION=FIFO
NOMBRE_RESTAURANTE=GreenLife
RETARDO_CICLO_CPU=5
IP_RESTAURANTE=10.108.64.3
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/GreenLife.log">> restaurante.config