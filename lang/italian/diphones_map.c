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
/*                   Mappatura difoni                                    */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include "cst_hrg.h"
#include "cst_phoneset.h"
#include "diphones_map.h"

cst_utterance *diphones_map(cst_utterance *u)
{
  /*Mappatura difoni non esistenti*/
 
  cst_item *s1,*s2;
  
  cst_val *map_table_intact;
  cst_val *map_table = val_car(val_userdata(feat_val(u->features,"map_table")));

  for (s1=relation_head(utt_relation(u,"Segment"));
       item_next(s1);
       s1 = item_next(s1))
    {
      s2=item_next(s1);
      for (map_table_intact=map_table;val_cdr(map_table_intact);map_table_intact=val_cdr(map_table_intact))
	{
	  if ((cst_streq(val_string(item_feat(s1,"name")), val_string(val_car(val_car(map_table_intact))))) && (cst_streq(val_string(item_feat(s2,"name")), val_string(val_car(val_cdr(val_car(map_table_intact)))))))
	    {
	      //ok mappa:
	      item_set_string(s1,"name",val_string(val_car(val_car(val_cdr(val_cdr(val_car(map_table_intact)))))));
	      item_set_string(s2,"name",val_string(val_car(val_cdr(val_car(val_cdr(val_cdr(val_car(map_table_intact))))))));
	    }
	}
    }
  return u;
}
