
#include <tchar.h>

#include <string>
#include <vector>

#include <sancur.h>

typedef struct FBDF_note_s {
	uint time = 0; /* 押すタイミング 0: none, [ms] */
	uint pos = 0; /* %4 */
	uint btn = 0; /* 1-4 */
	uint len = 99;
	uint mtime = 0; /* ダンスの長さ */
	uint bpm = 120;
} FBDF_note_t;

typedef struct FBDF_map_s {
	std::vector<FBDF_note_t>note;
	std::string artist;
	uint   noteNo  = 0;
	uint   noteN   = 0;
	uint   blockNo = 0;
	double bpm     = 120;
	int    offset  = 2000;
	int    song_offset = 2000; // nps 専用
	uint   Stime   = 0;
	uint   Ntime   = 0;
	uint   Etime   = 0;
	TCHAR  music_file[256]=_T("music.mp3"); // rrs osu tja nps 専用
} FBDF_map_t;

typedef struct FBDF_map_enc_s {
	double now_bpm      = 120;
	uint   now_block    = 4;
	uint   now_shutpos  = 0;
	double now_shuttime = 0;
	uint   measure      = 4; // tja nps 専用
	uint   measure_u    = 4; // tja nps 専用
	double scrool       = 1; // nps 専用
} FBDF_map_enc_t;

int GetNoteBlock(FBDF_map_t *map, char const *buf, FBDF_map_enc_t *option);
int GetNoteLine(FBDF_map_t *map, char *buf, FBDF_map_enc_t *option);
int MapLoadOne(FBDF_map_t *map, const char *nex_music);
