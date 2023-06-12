//بسم الله الرحمن الحيم

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <fstream>
#include <thread>

//
bool running = true;

// window variables
int cooldown_time = 3;
int show_counter = 2; // for debugging
short cooldown_time_debug = 2;
bool always_show = true; // for debugging
bool debugging = false;
int screen_width = 0, screen_heigth = 0;
int window_width = 250, window_height = 60;

SDL_Color BG_color = SDL_Color{255, 255, 255};

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

// Dikr variables
SDL_Texture *Dikr_Texture = nullptr;
SDL_Color Dikr_color = SDL_Color{0, 0, 0};

TTF_Font* Dikr_font = NULL;

int font_size = 60;
short selected_Dikr = 0;

const char* Dikr_font_arr_ar[3] = {
  "/usr/share/fonts/truetype/kacst/KacstPoster.ttf",
  "KacstPoster.ttf",
  "files/KacstPoster.ttf"
// "/usr/share/fonts/truetype/ubuntu/UbuntuMono-RI.ttf"
// "/home/ibrahimo/Downloads/Noto_Sans_Arabic/NotoSansArabic-VariableFont_wdth,wght.ttf"
// "/usr/share/fonts/truetype/kacst/KacstPoster.ttf"
};

std::string Dikr_list_en[3] = {
  "BismiALlah",
  "Sub7ana Allah",
  "Allah Akbar"
};


/*  word in hex format for Arabic
#define _Allah \uFEEA\uFEE0\uFEDF\uFE8D
#define _Ilaha \uFEEA\uFEDFإ
#define _Mohammed \uFEAA\uFEE4\uFEA4\uFEE3

#define _Bismi \uFEE2\uFEB4\uFE91
#define _Sub7an \uFEE6\uFEA4\uFE92\uFEB3
#define _Astaghfiro \uFEAE\uFED4\uFED0\uFE98\uFEB3أ

#define _salla \uFEF0\uFEE0\uFEBB
#define _ala \uFEF0\uFEE0\uFECB
#define _la ﻼ
#define _illa لاإ
*/

std::string Dikr_list_ar[5] = {
  u8"\uFEEA\uFEE0\uFEDF\uFE8D \uFEE2\uFEB4\uFE91", //BismiAllah
  u8"\uFEEA\uFEE0\uFEDF\uFE8D \uFEE6\uFEA4\uFE92\uFEB3", //Sub7ana Allah
  u8"\uFEAA\uFEE4\uFEA4\uFEE3 \uFEF0\uFEE0\uFECB \uFEEA\uFEE0\uFEDF\uFE8D \uFEF0\uFEE0\uFEBB", // salla Allah ala Mohammed
  u8"\uFEEA\uFEE0\uFEDF\uFE8D \uFEAE\uFED4\uFED0\uFE98\uFEB3أ", //astaghfiro Allah
  u8"\uFEEA\uFEE0\uFEDF\uFE8D لاإ \uFEEA\uFEDFإ ﻼ" //la ilaha illa Allah
};

void initialize();
void load_settings();
void load_font();
void make_Dikr_texture();
void pop_Dikr();
void clean_up();
void cooldown();

void periodicly_load_settings();

int main(){

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0){
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }


    if(TTF_Init() == -1)
    {
        printf("Error: %s\n", TTF_GetError());
        return -1;
    }

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    initialize();
    load_font();

    std::thread timer_to_settings (periodicly_load_settings);

    int showed_times = 0;
    while(always_show || show_counter > showed_times){
        pop_Dikr();
        
        showed_times++;
        
        clean_up();

        if(debugging && show_counter > showed_times)
        {
          SDL_Delay(cooldown_time_debug * 1000);
        }
        else if (show_counter > 0)
        {
          cooldown();
        }
    }
    running = false;
    timer_to_settings.join();
    SDL_Quit();
}

void initialize(){
    //get screen resolution
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    screen_heigth = DM.h;
    screen_width = DM.w;

    load_settings();
    srand(time(NULL));
}

void pop_Dikr(){
    window = SDL_CreateWindow("Dikr", screen_width - window_width, screen_heigth * 3 / 10, window_width, window_height, SDL_WINDOW_POPUP_MENU);
    renderer = SDL_CreateRenderer(window, -1, 0);

    if(renderer == nullptr){
        SDL_Log("Error creating SDL_Renderer!");
    }

    make_Dikr_texture();    
    
    bool running = true;
    
    if(NULL == Dikr_Texture || NULL == Dikr_font)
    {
      return;
    }

    SDL_Event w_event;
    while(running)
    {
        while(SDL_PollEvent(&w_event))
        {
            if(w_event.type == SDL_QUIT || w_event.type == SDL_MOUSEBUTTONDOWN)
            {
                running = false;
            }
        }
        //clear window
        SDL_SetRenderDrawColor(renderer, BG_color.r, BG_color.g, BG_color.b, 255);
        SDL_RenderClear(renderer);
        
        //Draw Dikr
        if(nullptr != Dikr_Texture)
        {
          SDL_RenderCopy(renderer, Dikr_Texture, NULL, NULL);
        }


        SDL_RenderPresent(renderer);
    }
}

void clean_up(){
    SDL_DestroyTexture(Dikr_Texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void load_font()
{
  for (int i = 0; i < 3; i++) 
  {
    Dikr_font = TTF_OpenFont(Dikr_font_arr_ar[i] ,font_size);
    if (NULL != Dikr_font)
    {
      return;
    }
  }

}

void cooldown()
{
  int waited_time = 0;
  while(waited_time < cooldown_time){
    SDL_Delay(60000);
    waited_time++;
  }
}

void make_Dikr_texture()
{
  if(NULL == Dikr_font)
  {
    load_font();
  }
  if(NULL == Dikr_font)
  {
    return;
  }

  selected_Dikr = rand() % 5;

  SDL_Surface * Dikr_Surface = TTF_RenderUTF8_Blended(Dikr_font, Dikr_list_ar[selected_Dikr].data(), Dikr_color);
  Dikr_Texture = SDL_CreateTextureFromSurface(renderer, Dikr_Surface);

  SDL_FreeSurface(Dikr_Surface);
}

void load_settings(){
  std::ifstream SettingsFile;
  SettingsFile.open("files/Settings", std::ifstream::in);
  if(SettingsFile.is_open())
  {
    int f_cooldown_time = f_cooldown_time;
    if(SettingsFile >> f_cooldown_time)
    {
      f_cooldown_time = f_cooldown_time;
    }

    int bg_r = BG_color.r, bg_g = BG_color.g, bg_b = BG_color.b;
    if(SettingsFile >> bg_r && SettingsFile >> bg_g && SettingsFile >> bg_b)
    {
      BG_color.r = bg_r;
      BG_color.g = bg_g;
      BG_color.b = bg_b;
    }

    int Dikr_r = Dikr_color.r, Dikr_g = Dikr_color.g, Dikr_b = Dikr_color.b;
    if(SettingsFile >> Dikr_r && SettingsFile >> Dikr_g && SettingsFile >> Dikr_b)
    {
      Dikr_color.r = Dikr_r;
      Dikr_color.g = Dikr_g;
      Dikr_color.b = Dikr_b;
    }

    bool f_always_show = always_show;
    int f_show_counter = show_counter;
    if(SettingsFile >> f_always_show && SettingsFile >> f_show_counter)
    {
      always_show = f_always_show;
      show_counter = f_show_counter;
    }

    SettingsFile.close();
  }
}

void periodicly_load_settings(){
  while(running)
  {
    SDL_Delay(300000);
    load_settings();
  }
}
