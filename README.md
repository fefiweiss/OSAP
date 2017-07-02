# OSAP

## Definición

Office Space Allocation Problem consiste en un conjunto de entidades que deben ser ubicadas en un conjunto de habitaciones, cumpliendo restricciones tanto duras como blandas. Se tienen dos objetivos principales, minimizar el espacio mal utilizado y minimizar las restricciones blandas no cumplidas, estos dos se complementan en la función de evaluación de los algoritmos para verificar la calidad de la solución.

## Implementación

Problema Office Space Allocation Problem (OSAP) se implementa mediante un algoritmo genético. Este trabaja solo con soluciones factibles como individuos, cada uno de ellos está representado por un vector de tamaño de la cantidad de entidades y almacena la habitacion en que está dicha entidad. Estos individuos son evauados mediante la función de evaluación o aptitud que es la penlización de espacio mal utilizado y violacion de restricciones blandas, el mejor individuo es aquel que minimiza esta métrica.

Para el algoritmo genético se utiliza elitismo que almacena el mejor individuo de todas las generaciones evaluadas, selección por ranking, cruzamiento en un punto y mutación que cambia de habitacion a una entidad. Las soluciones iniciales son factibles creadas aleatoriamente.

El formato input que deben serguir las instancias son las establecidas en PNe150 y SVe150. Es importante mencionar que la instancia debe tener bien escrito "ENTITIES", "ROOMS" y "CONSTRAINS", y que tanto las habitaciones como entidades tienen id's correlativas desde 0.

## Ejecutar implementación

Para compilar use:
make clean
make 

Para ejecutar instancia de prueba use:
make exe

Linea de ejecución:
./OSAP ruta_instancia/nombre_instancia tamaño_de_poblacion cantidad_de_generaciones probabilidad_cruzamiento probabilidad_mutacion

Ejemplo:
./OSAP game.txt 4 4 0.3 0.15