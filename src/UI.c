#include <stdio.h>
#include <stdlib.h>
#include "../include/STD_TYPES.h"
#include "../include/UI.h"
#include "../include/Music.h"
void UI_Run(Playlist_t *playlist)
{	
	uint16 choice;
	while(1)
	{
		UI_ShowMenu();
		if (!ReadUint(&choice))
		{
			printf("Invalid input! Please enter a number.\n");
			continue; 
		}
		switch(choice)
		{
			case 1://Add Song
			{
				Song_t song;
				song.is_favorite = 0; 
				printf("Enter Song name :"); 	
				scanf(" %99[^\n]",song.title);
				printf("Enter Artist name :"); 	
				scanf(" %99[^\n]",song.artist);
				printf("Enter Song Duration :"); 
				if (!ReadUint(&song.duration))//scan valid input
				{
					printf("Invalid input!\n");
					break;
				}
				Playlist_Status_t  x=Playlist_AddSong(playlist,&song,GENERATE_NEW_ID);
				x==PLAYLIST_OK?printf("Song Added successfully! \n"):printf("failed to Add Song! \n") ;
				break;
			}
			case 2:Playlist_Display(playlist);break;//Display Playlist 
			case 3://search for song
			{
				uint16 SongID;
				printf("enter song Id :");
				if (!ReadUint(&SongID))//scan valid input
				{
					printf("Invalid input!\n");
					break;
				}
				DNode_t *SongNode=Playlist_SearchByID(playlist,SongID);
				if(SongNode==NULL)
				{
					printf("song not found in the list :\n");
					break ;
				}
				Playlist_PrintSong(SongNode);
				
			}break;
			case 4://Delete Node
			{
				uint16 SongID;
				printf("Enter Song ID: ");
				if (!ReadUint(&SongID))//scan valid input
				{
					printf("Invalid input!\n");
					break;
				}
				DNode_t *song = Playlist_SearchByID(playlist, SongID);
				if (song == NULL)
				{
					printf("Song not found.\n");
				}
				else
				{
					Playlist_DeleteNode(playlist, song);
					printf("Song deleted successfully.\n");
				}
				
			}break;
			case 5: // Next Song
			{
				if (Playlist_Next(playlist) == PLAYLIST_OK)
					Playlist_PrintSong(playlist->current);
				else
					printf("Playlist is empty!\n");
				break;
			}
			case 6: // Previous Song
			{
				if (Playlist_Previous(playlist) == PLAYLIST_OK)
					Playlist_PrintSong(playlist->current);
				else
					printf("Playlist is empty!\n");
				break;
			}
			/*case 7: // Save Playlist to Binary File
			{
				if (Playlist_SaveToBinaryFile(playlist, "data/playlist.dat") == PLAYLIST_OK)
					printf("Playlist saved successfully!\n");
				else
					printf("Failed to save playlist.\n");
				break;
			}
			case 8: // Load Playlist from Binary File
			{
				if (Playlist_LoadFromBinaryFile(playlist, "data/playlist.dat") == PLAYLIST_OK)
					printf("Playlist loaded successfully!\n");
				else
					printf("Failed to load playlist (file may not exist).\n");
				break;
			}*/
			case 7: // Save Playlist to Text File
			{
				if (Playlist_SaveToTextFile(playlist, "data/playlist.txt") == PLAYLIST_OK)
					printf("Playlist saved successfully!\n");
				else
					printf("Failed to save playlist.\n");
				break;
			}
			case 8: // Load Playlist from Text File	
			{
				if (Playlist_LoadFromTextFile(playlist, "data/playlist.txt") == PLAYLIST_OK)
					printf("Playlist loaded successfully!\n");
				else
					printf("Failed to load playlist (file may not exist).\n");
				break;
			}
			case 9: // Search by Title or Artist
			{
				char text[MAX_TITLE_LEN];
				printf("Enter title or artist (or part of it): ");
				scanf(" %99[^\n]", text);
			
				uint16 count = 0;
				DNode_t **results = Playlist_SearchByText(playlist, text, &count);
			
				if (count == 0)
					printf("No songs found.\n");
				else
				{
					uint16 i;
					for (i = 0; i < count; i++)
					{
						Playlist_PrintSong(results[i]);
						printf("--------------------------\n");
					}
					printf("Found %hu song(s).\n", count);
				}
			
				free(results);
				break;
			}
			case 10: // Toggle Favorite by ID
			{
				uint16 SongID;
				printf("Enter Song ID: ");
				if (!ReadUint(&SongID))
				{
					printf("Invalid input!\n");
					break;
				}
				Playlist_Status_t status = Playlist_ToggleFavoriteByID(playlist, SongID);
				if (status == PLAYLIST_OK)
					printf("Favorite status updated.\n");
				else
					printf("Song not found.\n");
				break;
			}
			case 11://Exit
			{
				Playlist_Destroy(playlist);
				printf("Goodbye!");
				return;
			}
			default:printf("Invalid Choice!\n");break;
			
		}
	}
}

void UI_ShowMenu(void)
{
	printf("=====================================================================\n");
	printf("                     MUSIC PLAYLIST MANAGER \n");
	printf("=====================================================================\n\n\n");
	printf(" 1-Add Song\n");
	printf(" 2-Display Songs \n");
	printf(" 3-Search by ID \n");
	printf(" 4-Delete Song by ID \n");
	printf(" 5-Next Song\n");
	printf(" 6-Previous Song\n");
	printf(" 7-Save Playlist\n");
	printf(" 8-Load Playlist\n");
	printf(" 9-Search by Title/Artist\n");
	printf(" 10-Toggle Favorite (by ID)\n");
	printf(" 11-Exit\n");
	printf(" Choose:");
}




uint16 ReadUint(uint16 *value)
{
	int c;
	if (scanf("%hu", value) != 1)
	{
		while ((c = getchar()) != '\n' && c != EOF);
		return 0; // failed
	}
	while ((c = getchar()) != '\n' && c != EOF);//clear buffer after success from chars
	return 1; // success
}

void UI_ReadString(char *buffer, uint16 max_len)
{
	int c;
	scanf(" %99[^\n]", buffer); 
	while ((c = getchar()) != '\n' && c != EOF); 
}




void Playlist_Display(const Playlist_t* playlist)
{
	if(playlist->head==NULL)
		printf("list is empty\n");
	else
	{
		printf("============================================\n");
		printf("%-10s%-20s%-20s%-10s\n", "ID", "Title", "Artist", "Duration");
		printf("============================================\n");
		DNode_t* ptr=playlist->head;
		while(ptr!=NULL)
		{
			printf("%-10u",ptr->song.id);
			printf("%-20s",ptr->song.title);
			printf("%-20s",ptr->song.artist);
			if (ptr->song.duration / SECONDS_PER_HOUR > 0)
				printf("%hu:%02hu:%02hu   ",ptr->song.duration / SECONDS_PER_HOUR,(ptr->song.duration % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE,ptr->song.duration % SECONDS_PER_MINUTE);
			else
				printf("%02hu:%02hu   ",(ptr->song.duration % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE,ptr->song.duration % SECONDS_PER_MINUTE);
			
			if (ptr->song.is_favorite)
				printf("[FAV] ");    
			
			if(ptr==playlist->current)
				printf("<--current");
			
			ptr=ptr->next;
			printf("\n");

		}
		printf("\n");
		printf("Total Songs: %hu\n", playlist->count);
	}
}

void Playlist_PrintSong(const DNode_t *node)
{
	if (node == NULL)
	{
		printf("No song to display.\n");
		return;
	}
	printf("Song Information:\n");
	printf("ID: %hu\n", node->song.id);
	printf("Title: %s\n", node->song.title);
	printf("Artist: %s\n", node->song.artist);
	if (node->song.duration / SECONDS_PER_HOUR > 0)
		printf("Duration:%hu:%02hu:%02hu   ",node->song.duration / SECONDS_PER_HOUR,(node->song.duration % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE,node->song.duration % SECONDS_PER_MINUTE);
	else
		printf("Duration:%02hu:%02hu   ",	(node->song.duration % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE,	node->song.duration % SECONDS_PER_MINUTE);
	
	if (node->song.is_favorite)
		printf("[FAV]");
	printf("\n");
}