#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#define DEFAULT_R 0.045
#define DEFAULT_L 110.0
#define DEFAULT_C 80.0
#define DEFAULT_Q 1.0

enum Choice { NONE, CHGR, CHGL, CHGC };

extern void sim(double R, double L, double C, double *results);

double p10(short n)
{
	double r = 1;
	
	while(n > 0)
	{
		r *= 10;
		-- n;
	}
	
	return r;
}

double R = DEFAULT_R;
double L = DEFAULT_L;
double C = DEFAULT_C;
double Q = 1.0;
double *results;

enum Choice CurrChoice = NONE;

void draw_plot()
{
	float old_x = 30.0;
	float old_y = 50.0;
 
	for(unsigned i = 50; i <= 350; i += 50)
		al_draw_line(30.0, (float) i, 630.0, (float) i, al_map_rgb(20, 20, 20), 1);
	
	al_draw_line(30.0, 200.0, 630.0, 200.0, al_map_rgb(255, 255, 255), 1);
	
	for(unsigned i = 30; i <= 630; i += 50)
		al_draw_line((float) i, 50.0, (float) i, 350.0, al_map_rgb(20, 20, 20), 1);
	
	al_draw_line(30.0, 50.0, 30.0, 350.0, al_map_rgb(255, 255, 255), 1);
 
	for(unsigned i = 1; i < 600; ++ i)
	{
		float pos_x = (float) (30 + i);
		float pos_y = 200.0 +  (150.0 * -((float) results[i] * C / Q));
		
		al_draw_line(old_x, old_y, pos_x, pos_y, al_map_rgb(255, 0, 0), 1);
		
		old_x = pos_x;
		old_y = pos_y;
	} 
}

int main()
{
	results = malloc(sizeof(double) * 600);
	sim(R, L, C, results);
	
	al_init();
	al_init_primitives_addon();

	ALLEGRO_DISPLAY *display = al_create_display(660, 500);
	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_clear_to_color(al_map_rgb(0, 0, 0));

	ALLEGRO_FONT *font = al_load_font("misc/font.ttf", 14, 0);
	
	double factor = 1;
	
	while(1)
	{	
		ALLEGRO_EVENT ev;
		ALLEGRO_TIMEOUT timeout;
		al_init_timeout(&timeout, 0.01);
 
		bool get_event = al_wait_for_event_until(event_queue, &ev, &timeout);
 
		if(get_event)
		{
			if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				break;
			}
			else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if(CurrChoice == NONE)
				{
					switch(ev.keyboard.keycode)
					{
						case ALLEGRO_KEY_R:
							CurrChoice = CHGR;
							break;
							
						case ALLEGRO_KEY_L:
							CurrChoice = CHGL;
							break;
							
						case ALLEGRO_KEY_C:
							CurrChoice = CHGC;
							break;
							
						case ALLEGRO_KEY_LEFT:
							factor *= 10;
							break;
						
						case ALLEGRO_KEY_RIGHT:
							factor /= 10;
							break;
						
						default:						
							{}
					}
				}
				else
				{
					switch(ev.keyboard.keycode)
					{
						case ALLEGRO_KEY_LEFT:
							factor *= 10;
							break;
						
						case ALLEGRO_KEY_RIGHT:
							factor /= 10;
							break;
							
						case ALLEGRO_KEY_UP:
							switch(CurrChoice)
							{
								case CHGR:
									R += factor;
									break;
									
								case CHGL:
									L += factor;
									break;
								
								case CHGC:	
									C += factor;
									break;
									
								default:
									{}
							}
							
							sim(R, L, C, results);
							break;
				
						case ALLEGRO_KEY_DOWN:
							switch(CurrChoice)
							{
								case CHGR:
									R -= factor;
									break;
									
								case CHGL:
									L -= factor;
									break;
								
								case CHGC:	
									C -= factor;
									break;
									
								default:
									{}
							}
							
							sim(R, L, C, results);
							break;
								
						case ALLEGRO_KEY_ENTER:
							CurrChoice = NONE;
							break;
						
						default:						
							{}
					}
				}				
			}
		}
     
		al_clear_to_color(al_map_rgb(0,0,0));
		
		al_draw_text(font, al_map_rgb(255,255,255), 350, 10, ALLEGRO_ALIGN_CENTER, "RLC SIMULATOR");
		
		char txtR[32], txtL[32], txtC[32], txtF[32];
		sprintf(txtR, "R = %lf", R);
		sprintf(txtL, "L = %lf", L);
		sprintf(txtC, "C = %lf", C);
		sprintf(txtF, "step = %lf", factor);
		
		al_draw_text(font, ((CurrChoice == CHGR) ? al_map_rgb(255,255,0) : al_map_rgb(255,255,255)), 30, 370, ALLEGRO_ALIGN_LEFT, txtR);
		al_draw_text(font, ((CurrChoice == CHGL) ? al_map_rgb(255,255,0) : al_map_rgb(255,255,255)), 30, 400, ALLEGRO_ALIGN_LEFT, txtL);
		al_draw_text(font, ((CurrChoice == CHGC) ? al_map_rgb(255,255,0) : al_map_rgb(255,255,255)), 30, 430, ALLEGRO_ALIGN_LEFT, txtC);
		al_draw_text(font, al_map_rgb(255,255,255), 30, 460, ALLEGRO_ALIGN_LEFT, txtF);
		
		draw_plot();
		al_flip_display();
	}
	 
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
	free(results);

	return 0;
}
