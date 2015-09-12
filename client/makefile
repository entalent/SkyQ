src = cmUI.c list.c login.c main.c mainUI_notebook.c new_small_window.c password_check.c regist.c ui.h ./util/util.c ./util/cJSON.c ./util/sqlite3.c
client : $(src)
	gcc -o client $(src) -g -lpthread -lm -ldl -w `pkg-config gtk+-2.0 --cflags --libs gtk+-2.0 gthread-2.0`
