g++ main.cpp -o Typer \
$(pkg-config --cflags --libs allegro-5 allegro_font-5 allegro_ttf-5 allegro_primitives-5)