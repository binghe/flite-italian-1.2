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
/*  A simple MBROLA diphone voice defintion	                         */
/*                                                                       */
/*************************************************************************/

#include <string.h>
#include "flite.h"
#include "cst_diphone.h"
#include "italian.h"
#include "ifdlex.h"

static cst_utterance *irstifd_ita_lp_postlex(cst_utterance *u);

//NO necessary for MBROLA
//extern cst_diphone_db irstifd_ita_lp_db;

cst_voice *irstifd_ita_lp_diphone = NULL;

cst_voice *register_irstifd_ita_lp(const char *voxdir)
{
    cst_voice *v = new_voice();

    /* Sets up language specific parameters in the irstifd_ita_lp. */
    italian_init(v);

    feat_set_string(v->features,"name","irstifd_ita_lp");
    //painte
    feat_set_float(v->features,"int_f0_target_mean",157.222);// hz2mel(104.792) //110.0);
    feat_set_int(v->features,"pitch_male2female_conversion",1);
    //feat_set_float(v->features,"int_f0_target_stddev",15);//15.0);
    //feat_set_float(v->features,"default_pr_base",60);//110.0);
    //feat_set_float(v->features,"default_pr_top",350);//110.0);

    
    //duration 
    if (!feat_present(v->features, "duration_stretch"))
      feat_set_float(v->features,"duration_stretch",1.0); 
    
    /* Lexicon */
    ifd_lex_init();
    feat_set(v->features,"lexicon",lexicon_val(&ifd_lex));
    //feat_set(v->features,"postlex_func",uttfunc_val(&irstifd_ita_lp_postlex));

    /* Waveform synthesis */
    feat_set(v->features,"wave_synth_func",uttfunc_val(&diphone_synth));
    
    //MBROLA
    feat_set_string(v->features,"join_type","modified_mbrola");
    /*ATTENZIONE (percorso tipo semi-windows):
      La dir del database:
      o la specidici esttamente: d:/mbrola_dbs/it3/it3
      o hai una variabile di sistema di nome: export MBROLA_DBS=d:/mbrola_dbs*/
   
    char *mbrola_db;
    char *mbrola_dbs=getenv("MBROLA_DBS");
    if (!mbrola_dbs)
      mbrola_db="it4/it4"; //se non specifichi nulla dalla dir corrente.
    else
      mbrola_db=strcat(mbrola_dbs,"/it4/it4");
    
    feat_set_string(v->features,"mbrola_db",mbrola_db);   
    //feat_set_string(v->features,"mbrola_db",strcat(getenv("MBROLA_DBS"),"/it3/it3"));
        
    /* LPC 
    feat_set(v->features,"diphone_db",diphone_db_val(&irstifd_ita_lp_db));
    feat_set_int(v->features,"sample_rate",irstifd_ita_lp_db.sts->sample_rate);
    // feat_set_string(v->features,"join_type","simple_join"); 
    feat_set_string(v->features,"join_type","modified_lpc");
    // feat_set_string(v->features,"resynth_type","fixed");
    feat_set_string(v->features,"resynth_type","float");
    */
    irstifd_ita_lp_diphone = v;
    return irstifd_ita_lp_diphone;
}

void unregister_irstifd_ita_lp(cst_voice *vox)
{
    if (vox != irstifd_ita_lp_diphone)
	return;
    delete_voice(vox);
    irstifd_ita_lp_diphone = NULL;
}

static cst_utterance *irstifd_ita_lp_postlex(cst_utterance *u)
{
    /* Post lexical rules */

    return u;
}
