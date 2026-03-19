#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <codecvt>
#include <locale>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

int main()
{
    // -----------------------------
    // Allegro initialisieren
    // -----------------------------
    if(!al_init())
    {
        std::cout << "Allegro konnte nicht gestartet werden\n";
        return -1;
    }

    //Convert lokal festlegen
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;

    al_install_keyboard();

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // -----------------------------
    // Fenster
    // -----------------------------
    const int WIDTH = 1400;
    const int HEIGHT = 900;
    const float REL_FONTSIZE = 12.5; //scheiss auf double
    const int centerX = WIDTH / 2;
    int doneWidth;
    int x;
    int y;

    ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
    if(!display)
    {
        std::cout << "Display konnte nicht erstellt werden\n";
        return -1;
    }

    // -----------------------------
    // Event Queue
    // -----------------------------
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    // -----------------------------
    // Zeit
    // -----------------------------
    double frametick = al_get_time();
    double keytick = frametick;
    double starttick = 0;
    double sps;
    uint spm;
    float progress;
    float errors_perc;
    uint infostring_y;
    uint infostring_x;
    char32_t c;
    uint passed_secs;
    uint passed_mins;
    uint passed_9secs;
    uint passed_50secs;

    // -----------------------------
    // Font laden
    // -----------------------------
    int fontsize = HEIGHT / REL_FONTSIZE;
    if (fontsize > 32)
    {
        fontsize = 32;
    }
    ALLEGRO_FONT* font = al_load_ttf_font("JetBrainsMono-Regular.ttf", fontsize, 0);
    int charWidth = al_get_text_width(font, "M");
    int currentWidth = charWidth; //Nur wegen Monospace nicht im Loop sondern hier



    if(!font)
    {
        std::cout << "Font konnte nicht geladen werden\n";
        return -1;
    }

    // -----------------------------
    // Tipptext import
    // -----------------------------


    std::ifstream file("Text.txt");

    std::string line;
    std::getline(file, line);  // erste Zeile lesen

    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
    std::u32string text = conv.from_bytes(line);

    int text_len = text.length();
    std::cout << "Number of Characters: " << text_len << "\n";
    std::string infostring;


    int cursor = 0;
    uint errors = 0;

    bool running = true;
    bool finished = false;
    uint frame_counter = 0;
    uint last_sec = 0;
    uint fps = 0;




    // -----------------------------
    // Hauptloop
    // -----------------------------
    while(running)
    {
        ALLEGRO_EVENT event;
        

        if(!finished){
            frametick = al_get_time();
        }

        while(al_get_next_event(queue, &event))
        {
            
            //Kann man rausnehmen wegen STR+C
            if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                running = false;
            }

            if(event.type == ALLEGRO_EVENT_KEY_CHAR)
            {
                keytick = al_get_time(); 
                c = event.keyboard.unichar;

                if(cursor < text_len);
                {
                    if(c == text[cursor])
                    {
                        cursor++;
                    }
                    else
                    {
                        errors++;
                    }

                }
            }
        }

        // -----------------------------
        // Zeichnen des Schreibetextes
        // -----------------------------

        al_clear_to_color(al_map_rgb(180,180,180));

        std::u32string done = text.substr(0, cursor);
        std::u32string current = U"";
        std::u32string rest = U"";

        if(cursor < text.size())
        {
            current = text.substr(cursor,1);
            rest = text.substr(cursor+1);
        }

        doneWidth = done.size() * charWidth;
        //int currentWidth = charWidth;      //Nach oben geschrieben für Performance


        x = centerX - cursor * charWidth - charWidth/2;
        y = HEIGHT / 2;

        //Strings zum Zeichnen in UTF-8 wieder packen
        std::string done_utf8 = convert.to_bytes(done);
        std::string current_utf8 = convert.to_bytes(current);
        std::string rest_utf8 = convert.to_bytes(rest);

        // geschriebener Text (grau)
        al_draw_text(font,
                     al_map_rgb(135,135,135),
                     x, y,
                     0,
                     done_utf8.c_str());

        // aktuelles Zeichen (weiß)
        al_draw_text(font,
                     al_map_rgb(0,0,0),
                     x + doneWidth,
                     y,
                     0,
                     current_utf8.c_str());

        // restlicher Text
        al_draw_text(font,
                     al_map_rgb(80,80,80),
                     x + doneWidth + currentWidth,
                     y,
                     0,
                     rest_utf8.c_str());

        

        // -----------------------------
        // Zeichnen und Erstellen des Infostrings
        // -----------------------------
        // Erstellen
        sps = cursor / (frametick - starttick); 
        spm = sps * 60;
        
        passed_secs = frametick-starttick;
        passed_mins = passed_secs / 60;
        
        passed_secs = passed_secs%60;
        passed_9secs = passed_secs%10;
        passed_50secs = passed_secs/10;
        
        std::string elapsed_time = std::to_string(passed_mins) + ":" + std::to_string(passed_50secs) + std::to_string(passed_9secs);

        progress = (cursor * 100.0f) / text_len;
        std::string progressstring = std::to_string(cursor) + "/" + std::to_string(text_len) + " (" + std::to_string(progress).substr(0,4) + "%)";

        errors_perc = (errors*100.0f) / cursor;
        std::string infostring = std::to_string(errors) + " Errors (" + std::to_string(errors_perc).substr(0,5) + "%)  " + std::to_string(fps) \
        + " fps  " + progressstring + " chars   " + std::to_string(spm) + " spm" + "    ET: " + elapsed_time;
        



        // Infostring Zeichnen
        infostring_x = WIDTH - 20;
        infostring_y = 10;
        al_draw_text(font,
                     al_map_rgb(30,30,30),
                     infostring_x,
                     infostring_y,
                     ALLEGRO_ALIGN_RIGHT,
                     infostring.c_str());


        al_flip_display();


        // Dinge, die man wegen Latenz gerne nach dem Draw-Call verschieben kann:
        

        //Check ob finished:
        if (cursor >= text_len)
        {
            finished = true;
        }


        if (cursor == 1) //ob wir schon genau ein Zeichen getippt haben
        {
                starttick = al_get_time();
        }


        //Fps messen durch nen Frame counter
        frame_counter++;
        if (passed_secs == last_sec + 1) //feststellung, ob eine Sekunde seit letztem Check vergangen ist.
        {
            last_sec = passed_secs;
            fps = frame_counter;
            frame_counter = 0;
        }


    }

    // -----------------------------
    // Cleanup
    // -----------------------------
    al_destroy_font(font);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    std::cout << infostring << "\n";

    return 0;
}