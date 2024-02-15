import os

os.system(
    "gcc UVacat.c -o /home/runner/practica-eso/laboratorio-eso/Practica1/UVacat/UVacat -O3 -Wall -Werror -Wextra -pedantic -std=c99"
)
os.system(
    "gcc UVagrep.c -o /home/runner/practica-eso/laboratorio-eso/Practica1/UVagrep/UVagrep -O3 -Wall -Werror -Wextra -pedantic"
)
os.system(
    "gcc UVazip.c -o /home/runner/practica-eso/laboratorio-eso/Practica1/UVazip/UVazip -O3 -Wall -Werror -Wextra -pedantic -std=c99"
)
os.system(
    "gcc UVaunzip.c -o /home/runner/practica-eso/laboratorio-eso/Practica1/UVaunzip/UVaunzip -O3 -Wall -Werror -Wextra -pedantic -std=c99"
)

#TESTS
os.system(
    "cd /home/runner/practica-eso/laboratorio-eso/Practica1/UVacat/; ./test-UVacat.sh"
)
os.system(
    "cd /home/runner/practica-eso/laboratorio-eso/Practica1/UVagrep/; ./test-UVagrep.sh"
)
os.system(
    "cd /home/runner/practica-eso/laboratorio-eso/Practica1/UVazip/; ./test-UVazip.sh"
)
os.system(
    "cd /home/runner/practica-eso/laboratorio-eso/Practica1/UVaunzip/; ./test-UVaunzip.sh"
)
