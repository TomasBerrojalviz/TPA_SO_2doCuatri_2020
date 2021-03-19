#!/bin/bash

#Las commons ya estan instaladas.
#ESTE DEPLOY ES PARA LAS PRUEBAS COMPLETAS. DESPUES DE LAS INDIVIDUALES

# ______ FileSystem
# Borro el PuntoMontaje para que solo esten los nuevos datos

if test -d /home/utnso/tp-2020-2c-breakfastClub/afip/; then
    sudo rm -r /home/utnso/tp-2020-2c-breakfastClub/afip
fi
if test -d /home/utnso/tp-2020-2c-breakfastClub/logs/; then
    sudo rm -r /home/utnso/tp-2020-2c-breakfastClub/logs
fi

cd
mkdir -p /home/utnso/tp-2020-2c-breakfastClub/afip
mkdir -p /home/utnso/tp-2020-2c-breakfastClub/logs
cd /home/utnso/tp-2020-2c-breakfastClub/afip

mkdir Metadata

cd Metadata/
echo "BLOCK_SIZE=64
BLOCKS=2048
MAGIC_NUMBER=AFIP" >> Metadata.AFIP

cd


# ______ Shared Library

cd /home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/Debug

echo "BUILDEO UTILS"

make clean
make all

unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-2c-breakfastClub/Utils_Shared/Debug"
export LD_LIBRARY_PATH

sudo cp libUtils_Shared.so /usr/lib

#___________ INICIO DE MODULOS:

#_______ APP
cd
cd /home/utnso/tp-2020-2c-breakfastClub/App/Default

make clean
make all

cd .. 

if test -e app.config; then
    rm app.config
fi

echo "IP_COMANDA=10.108.224.3
PUERTO_COMANDA=5001
PUERTO_ESCUCHA=5004
RETARDO_CICLO_CPU=4
GRADO_DE_MULTIPROCESAMIENTO=2
ALGORITMO_DE_PLANIFICACION=FIFO
ALPHA=0.5
ESTIMACION_INICIAL=5
REPARTIDORES=[7|3,3|5]
FRECUENCIA_DE_DESCANSO=[20,8]
TIEMPO_DE_DESCANSO=[1,2]
PLATOS_DEFAULT=[Milanesa]
IP_APP=10.108.224.4
POSICION_REST_DEFAULT_X=5
POSICION_REST_DEFAULT_Y=5
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/app.log" >> app.config


#______ Cliente
cd
cd /home/utnso/tp-2020-2c-breakfastClub/Cliente/Default

make clean
make all

cd .. 

if test -e cliente.config; then
    rm cliente.config
fi

echo "IP_COMANDA=10.108.224.3
PUERTO_COMANDA=5001
IP_RESTAURANTE=10.108.224.3
PUERTO_RESTAURANTE=5002
IP_SINDICATO=10.108.224.2
PUERTO_SINDICATO=5003
IP_APP=10.108.224.4
PUERTO_APP=5004
POSICION_X=1
POSICION_Y=2
ID_CLIENTE=Cliente1
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/cliente.log" >> cliente.config

#_________     Comanda:

cd
cd /home/utnso/tp-2020-2c-breakfastClub/Comanda/Default

make clean
make all

cd ..

if test -e comanda.config; then
    rm comanda.config
fi

echo "PUERTO_ESCUCHA=5001
TAMANIO_MEMORIA=4096
TAMANO_SWAP=4096
TAMANIO_PAGINA=32
TAMANIO_NOMBRE_PLATO=24
IP_COMANDA=10.108.224.3
ALGORITMO_REEMPLAZO=LRU
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/comanda.log">> comanda.config

#_________     Restaurante:

cd
cd /home/utnso/tp-2020-2c-breakfastClub/Restaurante/Default

make clean
make all

cd ..

if test -e restaurante.config; then
    rm restaurante.config
fi

echo "PUERTO_ESCUCHA=5002
IP_SINDICATO=10.108.224.2
PUERTO_SINDICATO=5003
IP_APP=10.108.224.4
PUERTO_APP=5004
QUANTUM=2
ALGORITMO_PLANIFICACION=RR
NOMBRE_RESTAURANTE=LaParri
RETARDO_CICLO_CPU=5
IP_RESTAURANTE=10.108.224.3
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/LaParri.log">> restaurante.config

#_________     Sindicato:

cd
cd /home/utnso/tp-2020-2c-breakfastClub/Sindicato/Debug

make clean
make all


cd ..

if test -e sindicato.config; then
    rm sindicato.config
fi

echo "PUERTO_ESCUCHA=5003
IP_SINDICATO=10.108.224.2
PUNTO_MONTAJE=/home/utnso/tp-2020-2c-breakfastClub/afip
ARCHIVO_LOG=/home/utnso/tp-2020-2c-breakfastClub/logs/sindicato.log">> sindicato.config


echo "Pruebas generales terminadas"

# _______ TERMINADO

cd
