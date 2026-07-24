#include "../include/STD_TYPES.h"
#include "../include/Music.h"
#include "../include/UI.h"
int main()
{
	Playlist_t playlist;
    Playlist_init(&playlist);
    UI_Run(&playlist);
	
	
	return 0;
}