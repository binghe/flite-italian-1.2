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
/*  An Intonation model                                                   */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include "cst_hrg.h"
#include "it_intonation.h" 
#include "cst_cart.h"

//solo accenti 
cst_utterance *it_accent_func(cst_utterance *u)
{
    cst_cart *accents;
    cst_item *s;
    const cst_val *v;

    accents = val_cart(feat_val(u->features,"int_cart_accents"));
    
    for (s=relation_head(utt_relation(u,"Syllable")); s; s=item_next(s))
    {
	v = cart_interpret(s,accents);
	//printf("determinato: %s\n",val_string(v));
	if (!cst_streq("NONE",val_string(v)))
	  item_set_string(s,"accent",val_string(v));
    }
    return u;
}
