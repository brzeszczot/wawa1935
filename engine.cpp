/*
 * engine.cpp
 *
 *  Created on: Sep 22, 2012
 *      Author: brzeszczot
 */

#include "engine.hpp"

Engine::Engine()
{
	selected_dir = 0;

	//dir_read();
}

Engine::~Engine()
{
	delete_vars();
	delete gps;
}

bool Engine::dir_read()
{
	MAHandle list = maFileListStart(PATH, "*", 0);
	if(list<0)	maPanic(1, "maFileListStart error");
	int size;
	how_many_dir = 0;
	while(how_many_dir < MAX_DIRS && (size=maFileListNext(list, dir_names[how_many_dir], sizeof(dir_names[how_many_dir]))) > 0)
	{
		if(size>(int)sizeof(dir_names))	maPanic(1, "To small buffor");
		//printf("%d: %s\n", how_many_dir, dir_names[how_many_dir]);
		// jesli to katalog
		if(dir_names[how_many_dir][size-1]=='/')
			how_many_dir++;
	}
	maFileListClose(list);
	return how_many_dir>0?true:false;
}

bool Engine::read_conf_file()
{
	char buffer[256]={0};
	strcpy(buffer, url_path);
	//strcat(buffer, dir_names[selected_dir]);
	strcat(buffer, CONF_FILE_NAME);
	FILE *file = fopen(buffer, "r");
	if(!file) maPanic(1, "Could not open file!");

	//if(!fgets(img_zero_path, 1024, file)) maPanic(1, "Could not read string!");
	fscanf (file, "%lf\n", &map_coord.a_lat);
	fscanf (file, "%lf\n", &map_coord.a_lon);
	fscanf (file, "%lf\n", &map_coord.b_lat);
	fscanf (file, "%lf\n", &map_coord.b_lon);
	fscanf (file, "%d\n", &pieces_quantity.x);
	fscanf (file, "%d\n", &pieces_quantity.y);
	fclose (file);
}

bool Engine::env_init()
{
	gps = NULL;
	wsp.x = wsp.y = 0;
	ma_handles.a = ma_handles.b = ma_handles.c = ma_handles.d = -1;
	pieces_last.x = pieces_last.y = -1;
	gPointer.x = gPointer.y = wsp.x = 0;
	gPointer2.x = gPointer2.y = wsp.y = 0;
	center_view = true;
	gps_ok = false;

////// tymczasowe wsp gps 52.232856,20.964843 (plocka 17 warszawa :)
//	gps = new MALocation;
//	gps->lat = 52.232856;
//	gps->lon = 20.964843;
//	gps_ok = true;
/////////////////////////////////

	int screenSize = maGetScrSize();
	gScreenSize.x = EXTENT_X(screenSize);
	gScreenSize.y = EXTENT_Y(screenSize);
	graph_buffer = maCreatePlaceholder();
	maCreateDrawableImage(graph_buffer, gScreenSize.x, gScreenSize.y);
	maSetDrawTarget(graph_buffer);

	// pobierz rozmiar obrazka 00.jpg
	char buffer[256]={0};
	strcpy(buffer, url_path);
	//strcat(buffer, dir_names[selected_dir]);
	strcat(buffer, "00.jpg");
	MAHandle image = FileReadImage(buffer);
	MAExtent first_pieces_size = maGetImageSize(image);
	maDestroyObject(image);

	pieces_size.x = EXTENT_X(first_pieces_size);
	pieces_size.y = EXTENT_Y(first_pieces_size);
	gPointerSize.x = pieces_size.x*pieces_quantity.x;
	gPointerSize.y = pieces_size.y*pieces_quantity.y;

/*
	int i,j;
	MAPoint2d piec;
	char szFile[256]={0};
	for(i=0;i<5;i++)
		for(j=0;j<5;j++)
	{
		piec.x=j;
		piec.y=i;
	points_to_char(szFile,piec);

	x[(i*5)+j] = FileReadImage(szFile);
	}
*/

}

MAHandle Engine::FileReadImage(char* fullPath)
{
    MAHandle file = maFileOpen(fullPath, MA_ACCESS_READ);
    if(file < 0)	return NULL;
	/*
    int ret = maFileExists(file);
	if(ret<=0)	printf("File not exist!\n");
    */
    int size = maFileSize(file);
    if (size < 0)
    {
        maFileClose(file);
        //maPanic(1, "File size < 0");
        //printf("size: %s\n",fullPath);
        return NULL;
    }

    MAHandle data = maCreatePlaceholder();

    int result = maCreateData(data, size);
    if (RES_OK != result)
    {
        maFileClose(file);
        maDestroyPlaceholder(data);
        //printf("maCreateData: %s\n",fullPath);
        //maPanic(1, "maCreateData != RES_OK");
        return NULL;
    }

    result = maFileReadToData(file, data, 0, size);
    if (result < 0)
    {
        maFileClose(file);
        maDestroyPlaceholder(data);
        //maPanic(1, "maFileReadToData < 0");
        //printf("maFileReadToData: %s\n",fullPath);
        return NULL;
    }

    MAHandle image = maCreatePlaceholder();
    result = maCreateImageFromData(image, data, 0, maGetDataSize(data));
    if (RES_OK != result)
    {
    	//printf("DUPA %d\n",result);
        maFileClose(file);
        maDestroyPlaceholder(data);
        maDestroyPlaceholder(image);
        //printf("maCIFData: %s, %d\n",fullPath,result);
        //maPanic(1, "maCreateImageFromData != RES_OK");
        return NULL;
    }

    maFileClose(file);
    maDestroyPlaceholder(data);

    return image;
}

void Engine::points_to_char(char *ch, MAPoint2d p)
{
	char intStr[2];
	strcpy (ch, url_path);

	//strcat(ch, dir_names[selected_dir]);
	itoa(p.x, intStr,10);
	strcat (ch, intStr);
	itoa(p.y, intStr,10);
	strcat (ch, intStr);
	strcat (ch, ".jpg");
	//strcat (ch, '\0');
}

void Engine::dont_allow()
{
	// ograniczenia do przemieszczania mapy//
	if(gPointer.x>0)	gPointer.x = 0;
	if(gPointer.y>0)	gPointer.y = 0;
	if(gPointer.x+gPointerSize.x<gScreenSize.x)	gPointer.x = gScreenSize.x - gPointerSize.x;
	if(gPointer.y+gPointerSize.y<gScreenSize.y)	gPointer.y = gScreenSize.y - gPointerSize.y;
}

void Engine::delete_vars()
{
	if(ma_handles.a>0)	maDestroyObject(ma_handles.a);
	if(ma_handles.b>0)	maDestroyObject(ma_handles.b);
	if(ma_handles.c>0)	maDestroyObject(ma_handles.c);
	if(ma_handles.d>0)	maDestroyObject(ma_handles.d);
}

void Engine::draw()
{
	dont_allow();

	if(gps!=NULL)
	{
		// obliczenia wsp polozenia wzgledem mapy
		float ma,mb;
		// roznica miedzy lewym gornym i prawym dolnym rogiem
		map_coord_diff.a_lat = std::max(map_coord.a_lat,map_coord.b_lat)-std::min(map_coord.a_lat,map_coord.b_lat);
		map_coord_diff.a_lon = std::max(map_coord.a_lon,map_coord.b_lon)-std::min(map_coord.a_lon,map_coord.b_lon);
		// obliczenie odleglosci miedzy punktami
		//ma = map_coord_diff.a_lon*cos(map_coord.a_lat*PI/180);;
		//mb = map_coord_diff.a_lat;
		//printf("%f\n",std::sqrt(ma*ma+mb*mb)*PI*KM/360);
		// roznica miedzy aktualnym polozeniem a lewym gornym rogiem mapy
		map_coord_diff.b_lat = std::max(gps->lat,map_coord.a_lat)-std::min(gps->lat,map_coord.a_lat);
		map_coord_diff.b_lon = std::max(gps->lon,map_coord.a_lon)-std::min(gps->lon,map_coord.a_lon);
		// oblicz procent wzgledem plozenia na mapie ze wspolrzednych gps
		ma = (100*map_coord_diff.b_lat)/map_coord_diff.a_lat;
		mb = (100*map_coord_diff.b_lon)/map_coord_diff.a_lon;
		//printf("A: %f, %f, %d, %d,%d, %d\n",ma,mb,gPointer.x, gPointer.y,gScreenSize.x,gScreenSize.y);
		// obliczenie wspolrzednych na opdstawie procent
		wsp.x = (int)(mb*gPointerSize.x)/100;
		wsp.y = (int)(ma*gPointerSize.y)/100;
		// nalozenie wsp punktu gps na przesunieta mape
		ma = (int)std::abs(gPointer.x);
		mb = (int)std::abs(gPointer.y);
		wsp.x -=(int)ma;
		wsp.y -=(int)mb;
		// przesun mape zeby pokazac polozenie
		if(center_view)
		{
			gPointer.x -= (int)(wsp.x-(gScreenSize.x/2));
			gPointer.y -= (int)(wsp.y-(gScreenSize.y/2));
			dont_allow();
		}

		/*
		a:=(Lon2-Lon1)*Cos(Lat1*pi/180);
		  b:=(Lat2-Lat1);
		  Result:=Sqrt(a*a+b*b)*pi*KM/360;//[km]
		*/
		//printf("A: %f, %f, %d, %d,%d, %d\n",ma,mb,gPointer.x, gPointer.y,gScreenSize.x,gScreenSize.y);
		//printf("B: %.4g, %.4g, %.4g, %.4g, %f, %f\n",map_coord_diff.a_lat,map_coord_diff.a_lon,map_coord_diff.b_lat,map_coord_diff.b_lon,ma,mb);
		//printf("C: %i %.8g %.8g %.4g %.4g %.4g\n",gps->state, gps->lat, gps->lon, gps->horzAcc, gps->vertAcc, gps->alt);
		gps_ok = true;
	}

	// jesli pozycja gps wychodzi poza mape
	if((gps->lat < map_coord.a_lat && gps->lat > map_coord.b_lat) && (gps->lon > map_coord.a_lon && gps->lon < map_coord.b_lon));
	else
	{
		wsp.x = wsp.y = 0;
		gps_ok = false;
	}

	/// wyswietlanie grafiki
	maSetDrawTarget(HANDLE_SCREEN);
	maDrawImage(graph_buffer, 0,0);
	maSetColor(BLACK);
	maFillRect(0, 0, gScreenSize.x, gScreenSize.y);

	// obliczenie wspolrzednych obrazka wzgledem glownej plaszczyzny i numerow kawalkow do aktualnego wyswietlenia
	pieces.x = (int)floor((double)std::abs(gPointer.x)/pieces_size.x);
	pieces.y = (int)floor((double)std::abs(gPointer.y)/pieces_size.y);
	pieces_wsp.x = gPointer.x+(pieces.x*pieces_size.x);
	pieces_wsp.y = gPointer.y+(pieces.y*pieces_size.y);

	if(center_view || pieces_last.x!=pieces.x || pieces_last.y!=pieces.y)
	{
		MAPoint2d temp;
		char szFile1[256]={0};
		char szFile2[256]={0};
		char szFile3[256]={0};
		char szFile4[256]={0};

		if(ma_handles.a>0)	maDestroyObject(ma_handles.a);
		points_to_char(szFile1,pieces);
		ma_handles.a = FileReadImage(szFile1);

		if(ma_handles.b>0)	maDestroyObject(ma_handles.b);
		temp = pieces;
		temp.x++;
		points_to_char(szFile2,temp);
		ma_handles.b = FileReadImage(szFile2);

		if(ma_handles.c>0)	maDestroyObject(ma_handles.c);
		temp = pieces;
		temp.y++;
		points_to_char(szFile3,temp);
		ma_handles.c = FileReadImage(szFile3);

		if(ma_handles.d>0)	maDestroyObject(ma_handles.d);
		temp = pieces;
		temp.x++;
		temp.y++;
		points_to_char(szFile4,temp);
		ma_handles.d = FileReadImage(szFile4);

		pieces_last.x = pieces.x;
		pieces_last.y = pieces.y;
	}

	// wyswietlanie zdjec
/*
	temp_rect.left = std::abs(pieces_wsp.x);
	temp_rect.top = std::abs(pieces_wsp.y);
	temp_rect.width = gScreenSize.x;
	temp_rect.height = gScreenSize.y;
	pieces_wsp.x = pieces_wsp.y = 0;
    maDrawImageRegion(ma_handles.a, &temp_rect, &pieces_wsp, 0);
*/


/*
	int i;
	for(i=0;i<25;i++)
		maDrawImage(x[i], i*5, i*5);
*/
	//printf("%d, %d # %d, %d, \n",pieces.x,pieces.y,pieces_wsp.x,pieces_wsp.y);

	maDrawImage(ma_handles.a, pieces_wsp.x, pieces_wsp.y);
	//maDestroyObject(ma_handles.a);

	if(pieces_wsp.x+pieces_size.x<gScreenSize.x)
		maDrawImage(ma_handles.b, pieces_wsp.x+pieces_size.x, pieces_wsp.y);

	if(pieces_wsp.y+pieces_size.y<gScreenSize.y)
		maDrawImage(ma_handles.c, pieces_wsp.x, pieces_wsp.y+pieces_size.y);

	if(pieces_wsp.x+pieces_size.x<gScreenSize.x && pieces_wsp.y+pieces_size.y<gScreenSize.y)
		maDrawImage(ma_handles.d, pieces_wsp.x+pieces_size.x, pieces_wsp.y+pieces_size.y);

	////printf("%d, %d\n",pieces.a.x,pieces.a.y);
	if(gps_ok)	maSetColor(GREEN);
	else		maSetColor(RED);
	maFillRect(wsp.x, wsp.y, 15, 15);
	maSetColor(WHITE);
	maDrawText(5,5,"brzeszczot@gmail.com");
	maUpdateScreen();
	maDrawImage(graph_buffer, 0,0);

//	char szFile[256]={0};
//	maGetSystemProperty("mosync.path.local", szFile, sizeof(szFile));
//	strcat (szFile, "map1.png");
//	printf("path: %s\n",&szFile);
//"/data/data/com.mosync./files/MAStore"   "/data/data/brzeszczot.b_gps/files/map1.png"
//"/mnt/sdcard/CameraUtil/cache.jpg"    szFile
}
