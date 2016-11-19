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
/*  Lexical insertion per diversa sillabification...                     */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include "cst_hrg.h"
#include "cst_lts_rewrites.h"
#include "cst_lexicon.h"
#include "it_lexical_insertion.h"

extern const cst_lts_rewrites italian_downcase_lts_rewrites;
//char *word_lts_rewrites_word(const char *word, const cst_lts_rewrites *r);
cst_val *ifd_mid_lts_rewrites(const cst_val *itape, const cst_lts_rewrites *r);

extern const cst_lts_rewrites sillabe1_lts_rewrites;
extern const cst_lts_rewrites sillabe2_lts_rewrites;

cst_utterance *it_lexical_insertion(cst_utterance *u)
{
    cst_item *word;
    cst_relation *sylstructure,*seg,*syl;
    cst_lexicon *lex, *ulex = NULL;
    const cst_val *p;
    char *phone_name;
    char *pos;
    char *stress = "0";
    cst_val *phones, *w;
    cst_item *ssword, *sssyl, *segitem, *sylitem, *seg_in_syl;
    int return_pos=FALSE;
    
    lex = val_lexicon(feat_val(u->features,"lexicon"));
    if (feat_present(u->features, "user_lexicon"))
	ulex = val_lexicon(feat_val(u->features, "user_lexicon"));

    syl = utt_relation_create(u,"Syllable");
    sylstructure = utt_relation_create(u,"SylStructure");
    seg = utt_relation_create(u,"Segment");
    
    for (word=relation_head(utt_relation(u,"Word")); 
	 word; word=item_next(word))
    {
	ssword = relation_append(sylstructure,word);
	phones = NULL;

	
	/* FIXME: need to make sure that textanalysis won't split
           tokens with explicit pronunciation (or that it will
           propagate such to words, then we can remove the path here) */
	if (item_feat_present(item_parent(item_as(word, "Token")), "phones"))
	    phones = (cst_val *) item_feat(item_parent(item_as(word, "Token")), "phones");
	else
	{
	  if (ulex) {
	    return_pos=FALSE;
	    //printf("ulex\n");
	    phones = lex_lookup_return_pos(ulex,item_feat_string(word, "name"),0,&return_pos);
	  }
	  if (phones == NULL) {
	    return_pos=FALSE;
	    //printf("NULL\n");
	    phones = lex_lookup_return_pos(lex,item_feat_string(word,"name"),0,&return_pos);
	  }
	}
	
	if (return_pos) {
	  //inserisci il pos come feature tPOS // per i cart
	  pos = val_string(val_car(phones));
	  item_set_string(word,"tPOS",pos);
	  phones = val_cdr(phones);
	  //printf("word %s, pos %s\n",item_feat_string(word, "name"),pos);
	}
	
	//feat_print(stdout,item_feats(word));
	//printf("\n");
	//printf("INPUT_sillabe_prima:");
	//val_print(stdout,phones);
	//printf("\n");

	w = cons_val(string_val("#"),NULL);
	w = val_append(phones,w);
	phones = cons_val(string_val("#"),w);

	//printf("INPUT_sillabe:");
	//val_print(stdout,phones);
	//printf("\n");

	phones=ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(phones, &sillabe1_lts_rewrites), &sillabe2_lts_rewrites);
	
	//printf("OUTPUT_sillabe:");
	//val_print(stdout,phones);
	//printf("\n\n");
  	
	for (sssyl=NULL,sylitem=NULL,p=phones; p; p=val_cdr(p))
	{
	  phone_name = cst_strdup(val_string(val_car(p)));
	  if ((phone_name[0]=='-') || ((phone_name[0]=='#') && (val_cdr(p))))
	    {
	      if (sssyl)
		item_set_string(sssyl,"stress",stress);
	      cst_free(phone_name);
	      p=val_cdr(p);
	      phone_name = cst_strdup(val_string(val_car(p)));
	      sylitem = NULL;
	    }
	  

	    if (sylitem == NULL)
	    {
		sylitem = relation_append(syl,NULL);
		sssyl = item_add_daughter(ssword,sylitem);
		stress = "0";
	    }
	    segitem = relation_append(seg,NULL);
	    phone_name = cst_strdup(val_string(val_car(p)));
	    if (phone_name[strlen(phone_name)-1] == '1')
	    {
		stress = "1";
		//la linea successiva vuol dire che togli l'uon dai foni? si! lo tolgo per me!
		//phone_name[strlen(phone_name)-1] = '\0'; 
	    }
	    item_set_string(segitem,"name",phone_name);
	    seg_in_syl = item_add_daughter(sssyl,segitem);
	   
	    if cst_streq(val_string(val_car(val_cdr(p))),"#")
	    {
	      sylitem = NULL;
	      if (sssyl)
		item_set_string(sssyl,"stress",stress);
	      p=val_cdr(p);
	    }
	    
	    cst_free(phone_name);
	}
	/* FIXME: urgh argh ugly hack */
	if (!item_feat_present(item_parent(item_as(word, "Token")), "phones"))
	    delete_val(phones);
    }

    return u;
}
