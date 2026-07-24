#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>

#include "../include/STD_TYPES.h"
#include "../include/Music.h"

void Playlist_init(Playlist_t *playlist)
{
	if(playlist==NULL)
		return;
	playlist->head= NULL;
	playlist->tail= NULL;
	playlist->current= NULL;
	playlist->count= 0;
	playlist->next_id = INITIAL_SONG_ID;
}

DNode_t* Playlist_CreateNode(const Song_t* song)
{
	if (song == NULL)
		return NULL;
	DNode_t* node=(DNode_t*)malloc(sizeof(DNode_t));
	if(node==NULL)
		return NULL;
	node->song=*song;
	node->next=NULL;
	node->prev=NULL;
	return node;
}


Playlist_Status_t Playlist_AddSong(Playlist_t* playlist, const Song_t* song, uint8 preserve_id)
{
	if (playlist == NULL || song == NULL)
		return PLAYLIST_ERROR_NULL_POINTER;
	DNode_t *node = Playlist_CreateNode(song);
	if (node == NULL)
		return PLAYLIST_ERROR_ALLOC_FAILED;

	if (preserve_id)
	{
		if (node->song.id >= playlist->next_id)
			playlist->next_id = node->song.id + 1;
	}
	else
	{
		node->song.id = playlist->next_id;
		playlist->next_id++;
	}

	if (playlist->head == NULL)
	{
		playlist->head = node;
		playlist->tail = node;
		playlist->current = node;
		playlist->count = 1;
		return PLAYLIST_OK;
	}

	playlist->tail->next = node;
	node->prev = playlist->tail;
	playlist->tail = node;
	playlist->count++;
	return PLAYLIST_OK;
}


DNode_t* Playlist_SearchByID(const Playlist_t *playlist,uint16 SongID)
{
	if (playlist == NULL || playlist->head == NULL)
		return NULL;
	else
	{
		
		DNode_t* ptr=playlist->head;
		while(ptr!=NULL&&ptr->song.id!=SongID)
		{
		 	ptr=ptr->next;
		}
		if(ptr==NULL)
		{
			
			return NULL;
		}
			
		
		return ptr;
	}
}

static const char* CaseInsensitiveStrstr(const char *haystack, const char *needle)
{
	if (*needle == '\0')
		return haystack;

	for (; *haystack != '\0'; haystack++)
	{
		const char *h = haystack;
		const char *n = needle;

		while (*h != '\0' && *n != '\0' &&
		       tolower((unsigned char)*h) == tolower((unsigned char)*n))
		{
			h++;
			n++;
		}

		if (*n == '\0')
			return haystack;
	}
	return NULL;
}

DNode_t** Playlist_SearchByText(const Playlist_t *playlist, const char *text, uint16 *out_count)
{
	if(out_count == NULL)
		return NULL;
	*out_count = 0;
	if (playlist == NULL || playlist->head == NULL || text == NULL)
		return NULL;

	DNode_t **results = (DNode_t**)malloc(sizeof(DNode_t*) * playlist->count);
	if (results == NULL)
		return NULL;

	uint16 matches = 0;
	DNode_t *ptr = playlist->head;
	while (ptr != NULL)
	{
		uint8 found_in_title = (CaseInsensitiveStrstr((const char*)ptr->song.title, text) != NULL);
		uint8 found_in_artist = (CaseInsensitiveStrstr((const char*)ptr->song.artist, text) != NULL);
		if (found_in_title || found_in_artist)
		{
			results[matches] = ptr;
			matches++;
		}
		ptr = ptr->next;
	}

	*out_count = matches;
	return results;
}



Playlist_Status_t Playlist_DeleteNode(Playlist_t *playlist,DNode_t *node)
{
	 if (playlist == NULL || node == NULL)
    {
        return PLAYLIST_ERROR_NOT_FOUND ;
    }

    //Only node in the list
    if (node->prev == NULL && node->next == NULL)
    {
        playlist->head = NULL;
        playlist->tail = NULL;
        playlist->current = NULL;
    }
    

    // Delete first node
    else if (node->prev == NULL)
    {
        playlist->head = node->next;
        playlist->head->prev = NULL;

        if (playlist->current == node)
            playlist->current = playlist->head;
    }

    // Delete last node
    else if (node->next == NULL)
    {
        playlist->tail = node->prev;
        playlist->tail->next = NULL;

        if (playlist->current == node)
            playlist->current = playlist->tail;
    }

    //Delete middle node 
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (playlist->current == node)
            playlist->current = node->next;
    }

    free(node);
    playlist->count--;
	return PLAYLIST_OK;
}



Playlist_Status_t Playlist_Next(Playlist_t *playlist)
{
	if (playlist == NULL || playlist->head == NULL)
		return PLAYLIST_ERROR_NULL_POINTER;
	
	if (playlist->current->next != NULL)
		playlist->current = playlist->current->next;
	else
		playlist->current = playlist->head; // wrap around
	
	return PLAYLIST_OK;
}

Playlist_Status_t Playlist_Previous(Playlist_t *playlist)
{
	if (playlist == NULL || playlist->head == NULL)
		return PLAYLIST_ERROR_NULL_POINTER;
	
	if (playlist->current->prev != NULL)
		playlist->current = playlist->current->prev;
	else
		playlist->current = playlist->tail; // wrap around
	
	return PLAYLIST_OK;
}




Playlist_Status_t Playlist_ToggleFavoriteByID(Playlist_t *playlist, uint16 SongID)
{
	DNode_t *node = Playlist_SearchByID(playlist, SongID);
	if (node == NULL)
		return PLAYLIST_ERROR_NOT_FOUND;

	node->song.is_favorite = !node->song.is_favorite;
	return PLAYLIST_OK;
}




void Playlist_Destroy(Playlist_t *playlist)
{
	if (playlist == NULL)
		return;
    DNode_t *temp = playlist->head;
    DNode_t *ptr;
    while (temp != NULL)
    {
        ptr = temp->next;
        free(temp);
        temp = ptr;
    }
    playlist->head = NULL;
    playlist->tail = NULL;
    playlist->current = NULL;
    playlist->count = 0;
	playlist->next_id = INITIAL_SONG_ID;
}




Playlist_Status_t Playlist_SaveToBinaryFile(const Playlist_t *playlist, const char *filename)
{
	FILE *file = fopen(filename, "wb");
	if (file == NULL)
		return PLAYLIST_ERROR_FILE_OPEN_FAILED;

	DNode_t *ptr = playlist->head;
	while (ptr != NULL)
	{
		//separate itemes to avoidig padding in Song_t struct
		fwrite(ptr->song.title,1,MAX_TITLE_LEN, file);
		fwrite(ptr->song.artist,1,MAX_ARTIST_LEN, file);
		uint8 duration_byte[2]={(ptr->song.duration>>8)&0xff,ptr->song.duration&0xff};//for avoiding Endianess problems
		fwrite(duration_byte, 2, 1, file);
		uint8 id_byte[2]={(ptr->song.id>>8)&0xff,ptr->song.id&0xff};//for avoiding Endianess problems
		fwrite(id_byte, 2, 1, file);
		fwrite(&ptr->song.is_favorite,1,1, file);
		ptr = ptr->next;
	}

	fclose(file);
	return PLAYLIST_OK;
}

Playlist_Status_t Playlist_LoadFromBinaryFile(Playlist_t *playlist, const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL)
		return PLAYLIST_ERROR_FILE_OPEN_FAILED;
	Playlist_Destroy(playlist);
	Playlist_init(playlist);
	Song_t song;
	 while (1)
    {
        // read title
        if (fread(song.title, 1, MAX_TITLE_LEN, file) != MAX_TITLE_LEN)
            break;
        song.title[MAX_TITLE_LEN - 1] = '\0';
        
        // read artist
        if (fread(song.artist, 1, MAX_ARTIST_LEN, file) != MAX_ARTIST_LEN)
            break;
        song.artist[MAX_ARTIST_LEN - 1] = '\0';
        
        // read duration (Big Endian)
        uint8 duration_bytes[2];
        if (fread(duration_bytes, 1, 2, file) != 2)
            break;
        song.duration = ((uint16)duration_bytes[0] << 8) | duration_bytes[1];
        
        // read id (Big Endian)
        uint8 id_bytes[2];
        if (fread(id_bytes, 1, 2, file) != 2)
            break;
        song.id = ((uint16)id_bytes[0] << 8) | id_bytes[1];
        
        // read is_favorite
        if (fread(&song.is_favorite, 1, 1, file) != 1)
            break;
        
        Playlist_AddSong(playlist, &song, PRESERVE_EXISTING_ID);// preserve_id = 1
    }

	fclose(file);
	return PLAYLIST_OK;
}



Playlist_Status_t  Playlist_SaveToTextFile(const Playlist_t *playlist, const char *filename)
{
	FILE *file = fopen(filename, "w");
	if (file == NULL)
		return PLAYLIST_ERROR_FILE_OPEN_FAILED;

	DNode_t *ptr = playlist->head;
	while (ptr != NULL)
	{
		fprintf(file, "%hu,%s,%s,%hu,%hu\n",ptr->song.id,	ptr->song.title,ptr->song.artist,ptr->song.duration, ptr->song.is_favorite);
		ptr = ptr->next;
	}

	fclose(file);
	return PLAYLIST_OK;
}


Playlist_Status_t  Playlist_LoadFromTextFile(Playlist_t *playlist, const char *filename)
{	
	FILE *file = fopen(filename, "r");
	if (file == NULL)
		return PLAYLIST_ERROR_FILE_OPEN_FAILED;
	Playlist_Destroy(playlist);
	Playlist_init(playlist);
	Song_t song;
	while (fscanf(file, "%hu,%99[^,],%99[^,],%hu,%hu\n",&song.id, song.title, song.artist, &song.duration, &song.is_favorite) == 5)
	{
		Playlist_AddSong(playlist, &song, PRESERVE_EXISTING_ID); // preserve_id = 1
	}

	fclose(file);
	return PLAYLIST_OK;
}

