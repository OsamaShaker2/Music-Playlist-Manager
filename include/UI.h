#ifndef UI_H
#define UI_H
#include "Music.h"

void UI_Run(Playlist_t *playlist);

void UI_ShowMenu(void);



uint16 ReadUint(uint16 *value);

void UI_ReadString(char *buffer, uint16 max_len);



void Playlist_Display(const Playlist_t* playlist);

void Playlist_PrintSong(const DNode_t *node);

#endif