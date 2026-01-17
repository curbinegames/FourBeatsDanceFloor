#pragma once

#include <string>

#include "main.h"

#define FBDF_RESULT_SCORE_GRAPH_COUNT 25

typedef struct FBDF_result_score_graph_s {
	double gr_70;
	double gr_90;
	double gr_96;
	double gr_98;
	double gr_99;
} FBDF_result_score_graph_t;

typedef struct FBDF_result_data_s {
	std::string name;
	std::string artist;
	double level = 0;
	uint score = 0;
	double acc = 0;
	uint crit = 0;
	uint  hit = 0;
	uint save = 0;
	uint drop = 0;
	double gap_ave = 0;
	uint charaNo = 0;
	FBDF_result_score_graph_t score_graph[FBDF_RESULT_SCORE_GRAPH_COUNT];
} FBDF_result_data_t;

extern view_num_t FirstResultView(const FBDF_result_data_t *data);
