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
/*  IFD Lexicon definition                                               */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include "flite.h"
#include "cst_lts_rewrites.h"
#include "cst_regex.h"

extern const cst_regex *apostrofo; //viene preso dalla libreria italian it_text
extern const cst_regex *apice_finale; //viene preso dalla libreria italian it_text

extern const unsigned char ifd_lex_phones[];
extern const lexicon_entry ifd_lex_entry[];
extern const int ifd_num_entries;
extern const char * const ifd_phone_table[44];
//ITALIAN IFD lts_rewrites
extern const cst_lts_rewrites italian_downcase_lts_rewrites;
//
extern const cst_lts_rewrites italian_lts_rewrites;
extern const cst_lts_rewrites italian_stress1a_lts_rewrites;     
extern const cst_lts_rewrites italian_stress1b_lts_rewrites;
extern const cst_lts_rewrites italian_stress1c_lts_rewrites;
extern const cst_lts_rewrites italian_stress4a_lts_rewrites;
extern const cst_lts_rewrites italian_stress4b_lts_rewrites;
extern const cst_lts_rewrites italian_stress4c_lts_rewrites;
extern const cst_lts_rewrites italian_stress4d_lts_rewrites;
extern const cst_lts_rewrites italian_stress5a_lts_rewrites;
extern const cst_lts_rewrites italian_stress5b_lts_rewrites;
extern const cst_lts_rewrites italian_stress6a_lts_rewrites;
extern const cst_lts_rewrites italian_stress7a_lts_rewrites;
extern const cst_lts_rewrites italian_stress7b_lts_rewrites;
extern const cst_lts_rewrites italian_stress8a_lts_rewrites;
extern const cst_lts_rewrites italian_stress8b_lts_rewrites;
extern const cst_lts_rewrites italian_stress8c_lts_rewrites;
extern const cst_lts_rewrites italian_stress9a_lts_rewrites;
extern const cst_lts_rewrites italian_stress9b_lts_rewrites;
extern const cst_lts_rewrites italian_stress10a_lts_rewrites;
extern const cst_lts_rewrites italian_stress11a_lts_rewrites;
extern const cst_lts_rewrites italian_stress12a_lts_rewrites;
extern const cst_lts_rewrites italian_stress13a_lts_rewrites;
extern const cst_lts_rewrites italian_stress14a_lts_rewrites;
extern const cst_lts_rewrites italian_stress15a_lts_rewrites;
extern const cst_lts_rewrites italian_stress15b_lts_rewrites;
extern const cst_lts_rewrites italian_stress15c_lts_rewrites;
extern const cst_lts_rewrites italian_stress16a_lts_rewrites;
extern const cst_lts_rewrites italian_stress16b_lts_rewrites;
extern const cst_lts_rewrites italian_stress17a_lts_rewrites;
extern const cst_lts_rewrites italian_stress17b_lts_rewrites;
extern const cst_lts_rewrites italian_stress17c_lts_rewrites;
extern const cst_lts_rewrites italian_grafo1_lts_rewrites;
extern const cst_lts_rewrites italian_grafo2_lts_rewrites;
extern const cst_lts_rewrites italian_grafo3_lts_rewrites;
extern const cst_lts_rewrites italian_grafo4_lts_rewrites;
extern const cst_lts_rewrites italian_grafo5_lts_rewrites;
extern const cst_lts_rewrites italian_grafo6_lts_rewrites;
extern const cst_lts_rewrites italian_grafo7_lts_rewrites;
extern const cst_lts_rewrites italian_grafo8_lts_rewrites;
extern const cst_lts_rewrites italian_grafo9_lts_rewrites;
extern const cst_lts_rewrites italian_grafo10_lts_rewrites;


cst_val *ifd_mid_lts_rewrites(const cst_val *itape, const cst_lts_rewrites *r);
cst_val *ifd_lts_rewrites_word(const char *word, const cst_lts_rewrites *r);
char *word_lts_rewrites_word(const char *word, const cst_lts_rewrites *r);


//FINE ITALIAN IFD lts_rewrites

cst_val *italian_lts_function(const struct lexicon_struct *l, const char *word, const char *pos);

//static int ifd_is_vowel(const char *p);
//static int ifd_is_silence(const char *p);
//static int ifd_has_vowel_in_list(const cst_val *v);
//static int ifd_has_vowel_in_syl(const cst_item *i);
//static int ifd_sonority(const char *p);

/*static int ifd_is_silence(const char *p)
{
    if (cst_streq(p,"#"))
	return TRUE;
    else
	return FALSE;
}*/

/*static int ifd_has_vowel_in_list(const cst_val *v)
{
    const cst_val *t;

    for (t=v; t; t=val_cdr(t))
	if (ifd_is_vowel(val_string(val_car(v))))
	    return TRUE;
    return FALSE;
}*/

/*static int ifd_has_vowel_in_syl(const cst_item *i)
{
    const cst_item *n;

    for (n=i; n; n=item_prev(n))
	if (ifd_is_vowel(item_feat_string(n,"name")))
	    return TRUE;
    return FALSE;
}*/

/*static int ifd_is_vowel(const char *p)
{
  // this happens to work for US English phoneset 
    if (strchr("aeiou",p[0]) == NULL)
	return FALSE;
    else
	return TRUE;
}*/

/*static int ifd_sonority(const char *p)
{
// A bunch of hacks for US English phoneset 
    if (ifd_is_vowel(p) || (ifd_is_silence(p)))
	return 5;
    else if (strchr("wylr",p[0]) != NULL)
    return 4;  //glides/liquids 
    else if (strchr("nm",p[0]) != NULL)
    return 3;  // nasals 
    else if (strchr("bdgjlmnnnrvwyz",p[0]) != NULL)
    return 2;  // voiced obstruents 
    else
	return 1;
}*/

cst_lexicon ifd_lex;


cst_val *italian_lts_function(const struct lexicon_struct *l, const char *word, const char *pos)
{ 
  cst_val *phones=0;
  //applico in serie tutte le lts rules. In  realtà  ci sarebbe una applicazione condizionale se è o no una function word. 
  //corretto in se c'è o no lapostrofo a fine frase.
  
  if (cst_regex_match(apostrofo,word)) {
    //esegui due trascrizioni e poi uniscile. 
    //    italian_lts_function(l,
    char *p=cst_strrchr(word,'\''); 
    char *aaa = cst_strdup(word); //aaa=dell' 
    char *bbb = cst_strdup(word); //bbb=amore
    //strcpy(&aaa[strlen(word)-strlen(p)],p+1); //parola unita senza apo
    aaa[strlen(word)-strlen(p)+1] = '\0';    //before con apo
    strcpy(&bbb[0],&bbb[strlen(word)-strlen(p)+1]); //after senza apo
    phones=val_append(lex_lookup(l,aaa,NULL),lex_lookup(l,bbb,pos));
    //    lex_lookup(l,word,feats)
    //printf("before: %s after:%s\n",aaa,bbb);
    cst_free(aaa);
    cst_free(bbb);
  }
  else if (cst_regex_match(apice_finale,word)) {
    //if ( cst_streq( (char *)word+(strlen(word)-1) , "'" ) ) { //cerca modo + elegante per l'ultimo carattere...
    //printf("apostrofo fine parola\n");
    phones=lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_lts_rewrites_word(word, &italian_lts_rewrites), &italian_grafo1_lts_rewrites), &italian_grafo2_lts_rewrites), &italian_grafo3_lts_rewrites), &italian_grafo4_lts_rewrites), &italian_grafo5_lts_rewrites), &italian_grafo6_lts_rewrites), &italian_grafo7_lts_rewrites), &italian_grafo8_lts_rewrites), &italian_grafo9_lts_rewrites), &italian_grafo10_lts_rewrites); //function word 
  }
  else {
    //printf("no apot\n");
    phones=lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_mid_lts_rewrites(ifd_lts_rewrites_word(word, &italian_lts_rewrites), &italian_stress1a_lts_rewrites), &italian_stress1b_lts_rewrites), &italian_stress1c_lts_rewrites), &italian_stress4a_lts_rewrites), &italian_stress4b_lts_rewrites), &italian_stress4c_lts_rewrites), &italian_stress4d_lts_rewrites), &italian_stress5a_lts_rewrites), &italian_stress5b_lts_rewrites), &italian_stress6a_lts_rewrites), &italian_stress7a_lts_rewrites), &italian_stress7b_lts_rewrites), &italian_stress8a_lts_rewrites), &italian_stress8b_lts_rewrites), &italian_stress8c_lts_rewrites), &italian_stress9a_lts_rewrites), &italian_stress9b_lts_rewrites), &italian_stress10a_lts_rewrites), &italian_stress11a_lts_rewrites), &italian_stress12a_lts_rewrites), &italian_stress13a_lts_rewrites), &italian_stress14a_lts_rewrites), &italian_stress15a_lts_rewrites), &italian_stress15b_lts_rewrites), &italian_stress16a_lts_rewrites), &italian_stress16b_lts_rewrites), &italian_stress17a_lts_rewrites), &italian_stress17b_lts_rewrites), &italian_stress17c_lts_rewrites), &italian_grafo1_lts_rewrites), &italian_grafo2_lts_rewrites), &italian_grafo3_lts_rewrites), &italian_grafo4_lts_rewrites), &italian_grafo5_lts_rewrites), &italian_grafo6_lts_rewrites), &italian_grafo7_lts_rewrites), &italian_grafo8_lts_rewrites), &italian_grafo9_lts_rewrites), &italian_grafo10_lts_rewrites); //content
  }
  
  //printf("TRASCRIZIONE: ");
  //val_print(stdout,phones);
  //printf("\n");
  return phones;
}

//da qui


char *word_lts_rewrites_word(const char *word, const cst_lts_rewrites *r)
{
  //ritorna una parola 
  //printf("RULE:%s\n",r->name);
  // printf("INPUT word:%s\n", word);
  
  
    cst_val *w, *p;
    char x[2];
    int i;
    
    char *out_word = NULL;
    out_word = cst_alloc(char,strlen(word)+1);

    x[1] = '\0';
    w = cons_val(string_val("#"),NULL);
    
    for (i=0; word[i]; i++)
    {
	x[0] = word[i];
	w = cons_val(string_val(x),w);
    }
    w = cons_val(string_val("#"),w);

    w = val_reverse(w);

    p = lts_rewrites(w,r);
    
    for (i=0; p; i++)
      {
	out_word=strcat(out_word,val_string(val_car(p)));
	p=val_cdr(p);
      }
    
    delete_val(w);
    delete_val(p);
    
    // printf("Output word:%s\n", out_word);
    
    return out_word;
}


cst_val *ifd_lts_rewrites_word(const char *word, const cst_lts_rewrites *r)
    {
  cst_val *p;
  
  //printf("RULE:%s\n",r->name);
  //printf("INPUT word:%s\n", word);
  
  p = cons_val(string_val("#"),NULL);
  p = val_append(lts_rewrites_word(word, r),p);
  p = cons_val(string_val("#"),p);
  
  //printf("OUTPUT:");
  // val_print(stdout,p);
  // printf("\n\n");
  
  return p;
  }

cst_val *ifd_mid_lts_rewrites(const cst_val *itape, const cst_lts_rewrites *r)
{
  cst_val *p;
  
  /*printf("REGOLA:%s\n",r->name);
  printf("INPUT:");
  val_print(stdout,itape);
  printf("\n");*/
  
  p = cons_val(string_val("#"),NULL);
  p = val_append(lts_rewrites(itape, r),p);
  p = cons_val(string_val("#"),p);
  
  /*printf("OUTPUT:");
  val_print(stdout,p);
  printf("\n\n");*/
  
  return p;
}


//fino qui


void ifd_lex_init()
{
    /* I'd like to do this as a const but it needs everything in this */
    /* file and already the bits are too big for some compilers */
    ifd_lex.name = "ifd";
    ifd_lex.num_entries = ifd_num_entries;
    ifd_lex.entry_index = (lexicon_entry *) ifd_lex_entry;
    ifd_lex.phones = (unsigned char *) ifd_lex_phones;
    ifd_lex.phone_table = (char **) ifd_phone_table;
    //ifd_lex.syl_boundary = ifd_syl_boundary;
    //ifd_lex.addenda = (char ***) addenda;
    //ifd_lex.lts_rule_set = (cst_lts_rewrites *) &italian_lts_rewrites;
    ifd_lex.lts_function = italian_lts_function;
    //cst_val *(*lts_function)(const struct lexicon_struct *l, const char *word, const char *pos);
}
