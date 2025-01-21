#!/bin/bash

# Filnamn: mdu_benchmark.sh

# Skapa eller tömma filen som kommer att innehålla resultaten
echo "Threads,RealTime,UserTime,SystemTime" > mdu_times.csv

for t in {1..100}
do
    # Använd Bashs inbyggda 'time' kommando och fånga upp tiderna
    { time ./mdu -j $t /pkg/ > /dev/null 2>&1; } 2> temp_time.txt

    # Extrahera real, user och sys tider från utdata
    real_time=$(grep real temp_time.txt | awk '{print $2}')
    user_time=$(grep user temp_time.txt | awk '{print $2}')
    sys_time=$(grep sys temp_time.txt | awk '{print $2}')

    # Konvertera tiderna till sekunder i decimalformat
    real_sec=$(echo $real_time | awk -Fm '{split($2, s, "s"); print ($1 * 60) + s[1]}')
    user_sec=$(echo $user_time | awk -Fm '{split($2, s, "s"); print ($1 * 60) + s[1]}')
    sys_sec=$(echo $sys_time | awk -Fm '{split($2, s, "s"); print ($1 * 60) + s[1]}')

    # Lägg till tiden till resultatfilen
    echo "$t,$real_sec,$user_sec,$sys_sec" >> mdu_times.csv
    echo "Körning med $t tråd(ar) klar"
done

# Ta bort temporär fil
rm temp_time.txt
