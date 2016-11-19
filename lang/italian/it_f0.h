/*************************************************************************/
/*                                                                       */
/*                        ITALIAN FLITE                                  */
/*                                                                       */
/* Italian Flite is subject to the Mozilla Public License Version 1.1    */
/* (the "License"); you may not use this file except in compliance with  */
/* the License. You may obtain a copy of the License at                  */
/* http://www.mozilla.org/MPL/                                           */
/*                                                                       */
/* Note: Italian Flite is based on Flite which is a free software        */
/* distributed under a different licence. See the LICENSE file for more  */
/* information.                                                          */
/*                                                                       */
/* Software distributed under the License is distributed on an "AS IS"   */
/* basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.       */
/* See the License for the specific language governing rights and        */
/* limitations under the License.                                        */
/*                                                                       */
/* The Original Code is composed by all the Italian language specific    */
/* parts (tokenization, LTS rules, lexicon, part of speech tagger,       */
/* duration and intonation modules), the PaIntE implementation, the link */
/* with the MBROLA synthesis algorithm and the connection with the LAME  */
/* MP3 encoder.                                                          */
/*                                                                       */
/* Contributor(s):                                                       */
/* Fabio Tesser <tesser@pd.istc.cnr.it>                                  */
/* Piero Cosi   <cosi@pd.istc.cnr.it>                                    */
/*                                                                       */
/*************************************************************************/
/*            Author:  Fabio Tesser <tesser@pd.istc.cnr.it>              */
/*               Date:  October 2004                                     */
/*************************************************************************/
/*                                                                       */
/*  IT F0 model                                                          */
/*                                                                       */
/*************************************************************************/

#ifndef _IT_F0_H__
#define _IT_F0_H__

#include "cst_utterance.h"

cst_utterance *it_f0_model(cst_utterance *u);
cst_utterance *it_f0_model_cart(cst_utterance *u);

//Painte
void set_painte(cst_item *item, const float *params, int pr_norm);
float *get_painte (cst_item *painte, int pr_norm, float *p);
void set_painte_utt(cst_utterance *u, cst_item *s, const float *params, int pr_norm);
cst_utterance *painte_pr_normdenorm_events(cst_utterance *u, int norm);
//max numero di anchorpoint 10? forse anche troppi. 5
#define MAX_ANCHORPOINTS 10
#define eps 0.2  
//const float eps = 0.2;  
typedef struct TimeMap_struct TimeMap;
struct TimeMap_struct {
  int num_rows;
  float a_0[MAX_ANCHORPOINTS];
  float a_1[MAX_ANCHORPOINTS];
};
//PaintE:
#define overlap 3.6 //3.6  //gamma? io 3?
#define size_painte_vec 6
void define_window_from_syl(cst_item *s, float *win_start, float *win_end);
void add_pillar(TimeMap *tm, float val1, float val2);
TimeMap *construct_timemap_from_syl(cst_item *s, TimeMap *tm) ;
void invert(TimeMap *tm);
float map(TimeMap *tm, const float val);
float invmap(TimeMap *tm, const float val);
float param_range(float val, float valmin, float valmax);
//-------------------//
cst_utterance *painte_to_f0(cst_utterance *u);
void insert_f0_in_segment_in_a_sillable(cst_item *syl,const float *painte, TimeMap *tm, float win_start, char *silence);
cst_utterance *iterpolation_and_smoothing(cst_utterance *u);
cst_utterance *painte_CW_NUM_syl(cst_utterance *u); 
cst_val *list_elem_n(cst_val *list, int n);
float *get_param_from_val(cst_val *list, float *p);
cst_utterance *set_painte_CW_NUM_syl(cst_utterance *u);
cst_utterance *main_painte_to_f0(cst_utterance *u);
static float segment_start(cst_item *seg);
static float segment_mid(cst_item *seg);
void print_target_relation(cst_utterance *u);
//
cst_utterance *mel2frq_target(cst_utterance *u);
static float male2female(float fm);

#endif
