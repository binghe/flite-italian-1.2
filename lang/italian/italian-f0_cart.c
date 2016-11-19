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

#include "flite.h"
#include "italian.h"
#include "it_text.h"
#include "it_ffeatures.h"
#include "it_lexical_insertion.h"
#include "it_phrasing.h"
#include "it_intonation.h"
#include "it_f0.h"
#include "diphones_map.h"

void italian_init(cst_voice *v)
{ 
    it_text_init();
    
    /* Phoneset */
    feat_set(v->features,"phoneset",phoneset_val(&it_phoneset));
    feat_set_string(v->features,"silence",it_phoneset.silence);

    /* Text analyser */
    feat_set_string(v->features,"text_whitespace",italian_whitespace);
    feat_set_string(v->features,"text_postpunctuation",italian_punctuation);
    feat_set_string(v->features,"text_prepunctuation",
		    italian_prepunctuation);
    feat_set_string(v->features,"text_singlecharsymbols",
		    italian_singlecharsymbols);

    feat_set(v->features,"tokentowords_func",itemfunc_val(&it_tokentowords));
    /*Lexical_insertion*/
    feat_set(v->features,"lexical_insertion_func",uttfunc_val(&it_lexical_insertion));

    /* Phrasing */
    feat_set(v->features,"phrasing_func",uttfunc_val(&it_phrasing_func)); 
    feat_set(v->features,"phrasing_cart",cart_val(&it_phrasing_cart));
    feat_set(v->features,"pause_insertion_func",uttfunc_val(&it_pause_insertion_func)); 
    /* Intonation */
    //feat_set(v->features,"int_cart_tones",cart_val(&it_int_accent_cart));
    //Se vuoi i tones devi lasciare la func di default:
    //{ "intonation_func", cart_intonation },
    feat_set(v->features,"intonation_func",uttfunc_val(&it_accent_func));  
    feat_set(v->features,"int_cart_accents",cart_val(&it_int_accent_ds_cart));

    /* Duration */
    feat_set(v->features,"dur_cart",cart_val(&it_durz_ds_cart));
    feat_set(v->features,"dur_stats",dur_stats_val((dur_stats *)it_dur_stats));

    /* f0 model */
    //feat_set(v->features,"f0_model_func",uttfunc_val(&it_f0_model));
    //feat_set(v->features,"f0_model_func",uttfunc_val(&flat_prosody));
    
    feat_set(v->features,"f0_model_func",uttfunc_val(&it_f0_model_cart));
    feat_set(v->features,"f0_start_cart",cart_val(&it_f0z_start_cart));
    feat_set(v->features,"f0_mid_cart",cart_val(&it_f0z_mid_cart));
    feat_set(v->features,"f0_end_cart",cart_val(&it_f0z_end_cart));
    
    /*//f0 painte model
    feat_set(v->features,"f0_model_func",uttfunc_val(&main_painte_to_f0));
    feat_set(v->features,"painte_CW_NUM_cart",cart_val(&painte_CW_NUM_cart));
    feat_set(v->features,"PaIntE_file_CB",userdata_val(&PaIntE_file_CB));*/
       
    /* Post lexical rules */
    //feat_set(v->features,"postlex_func",uttfunc_val(&it_postlex));

    feat_set(v->features,"diphones_map_func",uttfunc_val(&diphones_map));
    feat_set(v->features,"map_table", userdata_val(&ita_map));

    it_ff_register(v->ffunctions);
}
