file = main

default:
	gcc $(file).c  'glad/glad.c' stb_image.h shader_lib.c -lm -lGL -lglfw -I/usr/local/include/freetype2 -lfreetype
	./a.out
	make clean

clean:
	rm a.out stb_image.h.gch
