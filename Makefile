f = main

default:
	gcc $f.c  -Wall -I/usr/include/freetype2 -lm -lGL -lGLU -lglut -lglfw -lfreetype 'glad/glad.c' stb_image.h shader_lib.c
	./a.out
	#make clean

clean:
	rm a.out stb_image.h.gch
