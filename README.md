# STR_Robot_Lego

## Authors
  - Álvaro Palacios Criado
  - Rafael Carlos Díaz Mata
  - Adolfo Fernández Gil
  - Juan Antonio Gálvez Jiménez
  - Álvaro Ruza Bajo

## Work Enviroment

Para comenzar con las prácticas de nuestro robot Lego tenemos que preparar un entorno de trabajo. Para ello entramos en el directorio *Entorno* y decargamos tanto el .dvi para poder instalar nuestra máquina virtual de Windows 7 (en mi caso he usador Virtual Box).

<p align="center">
<image width="460" src="/STR/images/Instalacion_MV.png" alt="Instalación Máquina Virtual" caption="Instalación de la Máquina Virtual">
</p>

El siguiente paso es descargar los archivos .zip, dependiendo del sistema que tengamos en nuestra máquina, en mi caso es el de Win64. Una vez descargado solo instalamos el driver de Lego. El driver se encuentra dentro de *OSEK-LEGO-Win64.zip* -> *NXT_64bit_driver.zip* -> *LegoMindstormsNXTdriver64.msi*.

<p align="center">
<image width="460" src="/STR/images/Driver.png" alt="Instalacion del Driver" caption="Instalacion del Driver">
</p>

Una vez con todo el entrono instalado podemos conectar nuestro Robot Lego y será detectado tanto por nuestra máquina, como por la máquina virtual.

## Programing
  
Para programar nuestro Robot usaremos las estructuras de datos que tenemos en el directorio ED, en pricipio usaremos la estructura de tipo 2. Tenemos que rellenar el fichero .cc y .oil.
  
## Running
  
El primer paso para poder ejecutar nuestro programa en el Robot seguiremos estos pasos:
  1. Abrimos Cygwin, es decir, la terminal de Lego.
  2. Copiamos nuestro directorio de la practica 1 al directorio /nxtOSEX
  3. Nos movemos al directorio /nxtOSEX/"nombre practica".
  4. Una vez en el directorio realizamos un **make clean** y un **make all**
  5. Ejecutamos el script **./rxeflash.sh**
  
  
