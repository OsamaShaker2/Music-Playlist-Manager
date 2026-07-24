#ifndef MUSIC_PLAYLIST_H
#define MUSIC_PLAYLIST_H
//error state & magic numbers Configration
#define MAX_TITLE_LEN      100
#define MAX_ARTIST_LEN     100
#define SECONDS_PER_MINUTE  60
#define SECONDS_PER_HOUR  3600
#define INITIAL_SONG_ID      1

typedef enum
{
	GENERATE_NEW_ID = 0,
	PRESERVE_EXISTING_ID = 1
} Playlist_IDMode_t;

typedef enum
{
	PLAYLIST_OK = 0,
	PLAYLIST_ERROR_NULL_POINTER,
	PLAYLIST_ERROR_ALLOC_FAILED,
	PLAYLIST_ERROR_FILE_OPEN_FAILED,
	PLAYLIST_ERROR_NOT_FOUND
} Playlist_Status_t;





typedef struct Song
{
	uint8 title[MAX_TITLE_LEN];
	uint8 artist[MAX_ARTIST_LEN];
	uint16 duration;//number of seconds
	uint16 id;
	uint8 is_favorite;
}Song_t;


typedef struct DNode
{
	Song_t song;
	struct DNode* prev; 
	struct DNode* next; 
}DNode_t;

typedef struct Playlist
{
	DNode_t* head;
	DNode_t* tail;
	DNode_t* current;
	
	uint16 count;
	uint16 next_id; 
}Playlist_t;

void Playlist_init(Playlist_t *playlist);

DNode_t* Playlist_CreateNode(const Song_t* song);

Playlist_Status_t  Playlist_AddSong(Playlist_t* playlist, const Song_t* song, uint8 preserve_id);




// Search BY ID
DNode_t* Playlist_SearchByID(const Playlist_t *playlist,uint16 SongID);

// Search BY Name or Artist
DNode_t** Playlist_SearchByText(const Playlist_t *playlist, const char *text, uint16 *out_count);



Playlist_Status_t  Playlist_DeleteNode(Playlist_t *playlist,DNode_t *node);



Playlist_Status_t Playlist_Next(Playlist_t *playlist);

Playlist_Status_t Playlist_Previous(Playlist_t *playlist);



Playlist_Status_t Playlist_ToggleFavoriteByID(Playlist_t *playlist, uint16 SongID);



void Playlist_Destroy(Playlist_t *playlist);

// Binary-based
Playlist_Status_t  Playlist_SaveToBinaryFile(const Playlist_t *playlist, const char *filename);
Playlist_Status_t  Playlist_LoadFromBinaryFile(Playlist_t *playlist, const char *filename);


// Text-based
Playlist_Status_t  Playlist_SaveToTextFile(const Playlist_t *playlist, const char *filename);
Playlist_Status_t  Playlist_LoadFromTextFile(Playlist_t *playlist, const char *filename);


#endif
