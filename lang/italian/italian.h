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
/*  Generic models for Italian                                           */
/*************************************************************************/

#ifndef _ITALIAN_H_
#define _ITALIAN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "cst_utterance.h"
#include "cst_cart.h"
#include "cst_val.h"
#include "cst_phoneset.h"
#include "cst_lexicon.h"
#include "cst_synth.h"

/* Voices call this to use italian. */
void italian_init(cst_voice *v);

/* Default functions and values that you might need. */
cst_utterance *it_f0_model(cst_utterance *u);
cst_utterance *it_postlex(cst_utterance *u);
extern const cst_phoneset it_phoneset;
extern const cst_cart it_phrasing_cart;
extern const cst_cart it_int_accent_cart;
extern const cst_cart it_int_accent_ds_cart; //ds 
extern const cst_cart it_durz_cart;
extern const cst_cart it_durz_ds_cart; //ds
extern const dur_stat * const it_dur_stats[]; //serve anche per ds (cambia solo il file)
extern const cst_cart painte_CW_NUM_cart;
extern const cst_val PaIntE_file_CB;
extern const cst_cart it_f0z_start_cart; //ds
extern const cst_cart it_f0z_mid_cart; //ds
extern const cst_cart it_f0z_end_cart; //ds
extern const cst_val ita_map;



#ifdef __cplusplus
}; /* extern "C" */
#endif /* __cplusplus */

#endif
