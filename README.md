# cubitos

Código elaborado como parte del TFG **Métodos adaptativos para el cálculo de 
homología persistente cúbica** en la facultad de ciencias de la Universidad de 
Zaragoza.

*Alumno*: Jaime Alconchel Gallego
*Directores*: Álvaro Lozano Rojo y Miguel Ángel Marco Buzunáriz

## Finalidad

Este programa calcula la homología persistente de una secuencia de complejos
cúbicos relacionados por expansiones e imprime su código de barras.

## Dependencias

> linbox, givaro, flint, ntl, iml

Debian:
> # apt install liblinbox-dev libgivaro-dev libflint-dev libntl-dev libiml-dev
Arch Linux
> # pacman -S linbox givaro flint ntl iml

## Compilación

> make

## Uso

> ./cubitos <point cloud file> <maxdepth>

