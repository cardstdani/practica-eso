import os
#gcc UVash.c -o UVash -O3 -Wall -Werror -Wextra -pedantic -std=c99 -lreadline -lhistory -flto -floop-unroll-and-jam -floop-interchange -floop-block -floop-nest-optimize -fomit-frame-pointer -fgcse -fgcse-sm -ftree-vectorize -fivopts -fsched-pressure -maccumulate-outgoing-args
#valgrind --leak-check=full --track-origins=yes -s --leak-check=full --show-leak-kinds=all ./UVash
#objdump -d -t -R -M amd64 UVash > MIPAS.asm
os.system(
    "gcc UVash.c -o /home/runner/practica-eso/laboratorio-eso/Practica2/UVash -O3 -Wall -Werror -Wextra -pedantic -std=c99 -flto -floop-unroll-and-jam -floop-interchange -floop-block -floop-nest-optimize -fomit-frame-pointer -fgcse -fgcse-sm -ftree-vectorize -fivopts -fsched-pressure -maccumulate-outgoing-args"
)

#TESTS
os.system(
    "cd /home/runner/practica-eso/laboratorio-eso/Practica2/; ./test-UVash.sh -c"
)
