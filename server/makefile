src = server.c ./util/util.c ./util/cJSON.c ./util/sqlite3.c
server : $(src)
	gcc -o server $(src) -lpthread -lm -ldl -w `pkg-config --cflags --libs gtk+-2.0`
