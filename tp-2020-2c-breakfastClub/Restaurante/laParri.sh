#_________ LaParri

if test -e restaurante.config; then
    rm restaurante.config
fi

echo "PUERTO_ESCUCHA=5002
IP_SINDICATO=10.108.64.2
PUERTO_SINDICATO=5003
IP_APP=10.108.64.4
PUERTO_APP=5004
QUANTUM=2
ALGORITMO_PLANIFICACION=RR
NOMBRE_RESTAURANTE=LaParri
RETARDO_CICLO_CPU=5
IP_RESTAURANTE=10.108.64.3
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/LaParri.log">> restaurante.config