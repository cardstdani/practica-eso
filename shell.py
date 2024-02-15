import os

os.system(
    "gcc UVash.c -o /home/runner/practica-eso/laboratorio-eso/Practica2/UVash -O3 -Wall -Werror -Wextra -pedantic -std=c99"
)

#TESTS
os.system(
    "cd /home/runner/practica-eso/laboratorio-eso/Practica2/; ./test-UVash.sh")
