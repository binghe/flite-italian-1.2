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
/*  An Italian Phrasing model                                            */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include "cst_hrg.h"
#include "it_phrasing.h"
#include "cst_cart.h"

cst_utterance *it_phrasing_func_old(cst_utterance *u)
{
    cst_relation *r;
    cst_item *w, *p;
    const cst_val *v;
    cst_cart *phrasing_cart;
    //printf("it_phrasing_func\n");
    r = utt_relation_create(u,"Phrase");
    phrasing_cart = val_cart(feat_val(u->features,"phrasing_cart"));
    for (p=NULL,w=relation_head(utt_relation(u,"Word")); w; w=item_next(w))
      {	
	if (p == NULL)
	  p = relation_append(r,NULL);
	item_add_daughter(p,w);
	v = cart_interpret(w,phrasing_cart);
	if (cst_streq(val_string(v),"BB")) 
	  {  	    
	    item_set_string(p,"name","BB");
	    p=NULL;
	  }
	else if (cst_streq(val_string(v),"B")) 
	  {
	    item_set_string(p,"name","B");
	    p=NULL;
	  }
	//anche mB? Per painte 2c no. Per 
	else if (cst_streq(val_string(v),"mB")) 
	  {
	    item_set_string(p,"name","mB");
	    p=NULL;  
	  }
      }
    return u;
}

cst_utterance *it_phrasing_func(cst_utterance *u)
{
    cst_relation *r;
    cst_item *w, *p;
    const cst_val *v;
    cst_cart *phrasing_cart;
    //printf("it_phrasing_func\n");
    r = utt_relation_create(u,"Phrase");
    phrasing_cart = val_cart(feat_val(u->features,"phrasing_cart"));
    for (p=NULL,w=relation_head(utt_relation(u,"Word")); w; w=item_next(w))
      {	
	if (p == NULL)
	  p = relation_append(r,NULL);
	item_add_daughter(p,w);
	v = cart_interpret(w,phrasing_cart);
	if (cst_streq(val_string(v),"BB")) 
	  {  	    
	    item_set_string(p,"name","BB");
	    item_set_string(w,"pbreak","BB");
	    p=NULL;
	  }
	else if (cst_streq(val_string(v),"B")) 
	  {
	    item_set_string(p,"name","B");
	    item_set_string(w,"pbreak","B");
	    p=NULL;
	  }
	else if (cst_streq(val_string(v),"mB")) 
	  {// solo a livello di parola non di frase
	    //item_set_string(p,"name","B");
	    item_set_string(w,"pbreak","mB");
	    //p=NULL;  
	  }
	else 
	  {
	    item_set_string(w,"pbreak","NB");
	  }
      }
    return u;
}

cst_utterance *it_pause_insertion_func_old(cst_utterance *u)
{
    /* Add initial silences and silence at each phrase break */
    const char *silence;
    const cst_item *w;
    cst_item *p, *s;

    silence = val_string(feat_val(u->features,"silence"));

    /* Insert initial silence */
    s = relation_head(utt_relation(u,"Segment"));
    if (s == NULL)
	s = relation_append(utt_relation(u,"Segment"),NULL);
    else
	s = item_prepend(s,NULL);
    item_set_string(s,"name",silence);

    for (p=relation_head(utt_relation(u,"Phrase")); p; p=item_next(p))
    {
	for (w = item_last_daughter(p); w; w=item_prev(w))
	{
	    s = path_to_item(w,"R:SylStructure.daughtern.daughtern.R:Segment");
	    if (s)
	    {
		s = item_append(s,NULL);
		item_set_string(s,"name",silence);
		break;
	    }
	}
    }

    return u;
}


cst_utterance *it_pause_insertion_func(cst_utterance *u)
{
    /* Add initial silences and silence at each phrase BB B mB */
  
    const char *silence;
    char *str;
    cst_item *w;
    cst_item *s;

    silence = val_string(feat_val(u->features,"silence"));

    /* Insert initial silence */
    s = relation_head(utt_relation(u,"Segment"));
    if (s == NULL)
	s = relation_append(utt_relation(u,"Segment"),NULL);
    else
	s = item_prepend(s,NULL);
    item_set_string(s,"name",silence);
    
    for (w=relation_head(utt_relation(u,"Word")); w; w=item_next(w))
      {      
	if (item_feat_present(w,"pbreak")) {
	  str=item_feat_string(w,"pbreak");
	  if ( cst_streq(str,"BB") || cst_streq(str,"B") || cst_streq(str,"mB") ) {
	    s = path_to_item(w,"R:SylStructure.daughtern.daughtern.R:Segment");
	    if (s)
	      {		
		s = item_append(s,NULL);
		item_set_string(s,"name",silence);
		// break; //??
	      }
	  }
	}
      }
    return u;
}
