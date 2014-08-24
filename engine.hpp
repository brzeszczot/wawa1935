/*
 * engine.hpp
 *
 *  Created on: Sep 22, 2012
 *      Author: brzeszczot
 */

#ifndef ENGINE_HPP_
#define ENGINE_HPP_

#include <ma.h>
//#include <mastring.h>
//#include <vector>
#include "maxtoa.h"
#include <string>

#define GRAY 0xA0A0A0
#define WHITE 0xffffff
#define BLACK 0x000000
#define GREEN 0x00ff00
#define RED 0xff0000
#define KM 12756.274
#define PI 3.1415
#define PATH "/mnt/sdcard/Download/wawa1935_res/"
#define CONF_FILE_NAME "conf.txt"
#define MAX_DIRS 10
#define MAX_DIR_NAME_LENGTH 64

class Engine
{
	public:
		Engine();
		virtual ~Engine();
		bool dir_read();								// wczytaj liste katalogow i zapisz ich nazwy do ponizszej zmiennej "dir_names"
		bool read_conf_file();							// wczytanie pliku konfiguracyjnego z wybranego katalogu z mapami
		bool env_init();								// inicjalizacja zmiennych
		void draw();									// wyswietlaj mape
		void dont_allow();								// sprawdz czy nie wyszlismy poza bszar calkowity
		MAHandle FileReadImage(char* fullPath);			// wczytaj obraz z podanej sciezki
		void points_to_char(char *ch, MAPoint2d p);		// konwersja z wspolrzednych na string
		void delete_vars();

		char dir_names[MAX_DIRS][MAX_DIR_NAME_LENGTH];	// tablica z katalogami zawierajacymi mapy
		unsigned int how_many_dir;						// ile odczytano powyzszych katalogow
		unsigned int selected_dir;						// ktory katalog wybrano
		MALocation *gps;								// wspolrzedne pobrane bezposrednio z biblioteki MALocation
		MAPoint2d gPointer;								// polozenie obszaru glownego (duza mapa)
		MAPoint2d gPointer2;							// zm. pomocniczna
		bool center_view;								// jesli prawda to automatycznie wycentruj mape na punkt GPS
		char* url_path;
	private:
		MAHandle graph_buffer;							// bufor graficzny
		MAPoint2d pieces_quantity;						// ilosc kawalkow map X i Y (0, 1, 2, ...)
		MAPoint2d pieces;								// pozycja aktualnego kawalka mapy X i Y (0, 1, 2, ...)
		MAPoint2d pieces_last;
		MAPoint2d pieces_size;							// rozmiar kzdego z kawalkow mapy
		MAPoint2d pieces_wsp;							// wspolrzedne kawalka mapy na ktory wskazuje GPS (wzgledem gPointer - czyli duzej matrycy wszystkich kawalkow)
		MAPoint2d wsp;									// wsp GPS na obszarze o wspolrzednych gPointer
		MAPoint2d gPointerSize;							// rozmiar glownej duzej mapy
		MAPoint2d gScreenSize;							// rozmiar ekranu telefonu
		MARect temp_rect;								// zmienna pomocnicza
		bool gps_ok;									// gps_err: true = wspolrzedne sa poprawne
		struct POINT_D									// pomocnicza struktura z danymi o wspolrzednych
		{
			double a_lat;
			double a_lon;
			double b_lat;
			double b_lon;
		};
		struct IMGHND
		{
			MAHandle a;
			MAHandle b;
			MAHandle c;
			MAHandle d;
		};
		IMGHND ma_handles;
		POINT_D map_coord;
		POINT_D map_coord_diff;
};

#endif /* ENGINE_HPP_ */
