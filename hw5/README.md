# Задание 1. Знакомство в компании

Реализованно на C: [meet.c](https://github.com/cherninkiy/made-2021-hpc/blob/hw5/hw5/meet/meet.c)

    psize = 2
    rank_0 meets rank_1

    rank_0 meets rank_1

    rank_0 meets rank_1


    psize = 3
    rank_0 meets rank_2
    rank_2 meets rank_1

    rank_0 meets rank_1
    rank_1 meets rank_2

    rank_0 meets rank_1
    rank_1 meets rank_2


    psize = 4
    rank_0 meets rank_1
    rank_1 meets rank_2
    rank_2 meets rank_3

    rank_0 meets rank_3
    rank_3 meets rank_1
    rank_1 meets rank_2

    rank_0 meets rank_3
    rank_3 meets rank_1
    rank_1 meets rank_2


    psize = 5
    rank_0 meets rank_2
    rank_2 meets rank_3
    rank_3 meets rank_4
    rank_4 meets rank_1

    rank_0 meets rank_1
    rank_1 meets rank_2
    rank_2 meets rank_3
    rank_3 meets rank_4

    rank_0 meets rank_1
    rank_1 meets rank_2
    rank_2 meets rank_4
    rank_4 meets rank_3


# Задание 2.

Реализованно на Python: [rule110.py](https://github.com/cherninkiy/made-2021-hpc/blob/hw5/hw5/rule110/rule110.py)

    Number of processors 1 - rule 110 cyclic
    0.024300336837768555
    Number of processors 1 - rule 110 bounded
    0.022181034088134766
    Number of processors 2 - rule 110 cyclic
    4.454538822174072
    Number of processors 2 - rule 110 bounded
    2.216996192932129
    Number of processors 3 - rule 110 cyclic
    10.736018657684326
    Number of processors 3 - rule 110 bounded
    4.524310827255249
    Number of processors 4 - rule 110 cyclic
    18.927810192108154
    Number of processors 4 - rule 110 bounded
    6.730258464813232

Сделано все:
- периодические и непериодические граничные условия
- работает параллельный код на нескольких процессорах
- имплементированы клетки-призраки
- можно менять правило

Полный лог [здесь](https://github.com/cherninkiy/made-2021-hpc/blob/hw5/hw5/rule110/test.log)

В отношении скорости работы:
- пересылка данных сьедает ускорение от распределения задачи по нескольким процессам
- периодические граничные условия зацикливают путь передачи сообщений, скорость падает совсем

Визуализация:

[spaceships](https://github.com/cherninkiy/made-2021-hpc/blob/hw5/hw5/rule110/images/110_200_200_4_cyclic.png)