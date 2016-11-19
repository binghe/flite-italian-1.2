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
/*  Feature functions used by various cart trees etc                     */
/*  These have been create as needed, and as some of the trees are       */
/*  from University of Edinburgh's Festival system their names and       */
/*  semantics follow them                                                */
/*************************************************************************/

#include "cst_hrg.h"
#include "cst_phoneset.h"
#include "cst_regex.h"
#include "it_ffeatures.h"
//fabio
#include "cst_lts_rewrites.h"

extern const cst_lts_rewrites italian_downcase_lts_rewrites;
char *word_lts_rewrites_word(const char *word, const cst_lts_rewrites *r);
//fabio

extern const cst_regex *apostrofo;

static const cst_val *word_break(const cst_item *word);
static const cst_val *word_punc(const cst_item *word);
static const cst_val *gpos(const cst_item *word);
static const cst_val *word_numsyls(const cst_item *word);
static const cst_val *ssyl_in(const cst_item *syl);
static const cst_val *ssyl_out(const cst_item *syl);
static const cst_val *syl_in(const cst_item *syl);
static const cst_val *syl_out(const cst_item *syl);
static const cst_val *syl_break(const cst_item *syl);
static const cst_val *syl_codasize(const cst_item *syl);
static const cst_val *syl_onsetsize(const cst_item *syl);
static const cst_val *ph_vc(const cst_item *p);
static const cst_val *ph_vlng(const cst_item *p);
static const cst_val *ph_vheight(const cst_item *p);
static const cst_val *ph_vrnd(const cst_item *p);
static const cst_val *ph_vfront(const cst_item *p);
static const cst_val *ph_ctype(const cst_item *p);
static const cst_val *ph_cplace(const cst_item *p);
static const cst_val *ph_cvox(const cst_item *p);
static const cst_val *accented(const cst_item *p);

// fabio added
static const cst_val *Simbolic_word_end_punct(const cst_item *word);
static const cst_val *Simbolic_word_start_prepunctuation(const cst_item *word);
static const cst_val *word_start_prepunctuation(const cst_item *word);
//
static const cst_val *pos_in_word(const cst_item *syl);
//static const cst_val *pbreak(const cst_item *word);
static const cst_val *apo(const cst_item *word);
static const cst_val *ph_semi(const cst_item *p);
static const cst_val *word_duration(const cst_item *word);
static const cst_val *type(const cst_item *phrase);
static const cst_val *next_stress(const cst_item *syl);
static const cst_val *last_stress(const cst_item *syl);
static const cst_val *syl_numphones(const cst_item *syl);
//
static const cst_val *Accent_boundary_syl(const cst_item *syl);
static const cst_val *syl_onset_type(const cst_item *s);
static const cst_val *syl_coda_type(const cst_item *s);
static const cst_val *prima_geminata_stop(const cst_item *seg); 
static const cst_val *ph_gem(const cst_item *p);
static const cst_val *phrasepos_in_utt(const cst_item *phrase);
static const cst_val *words_out(const cst_item *word);
static const cst_val *word_number(const cst_item *word);
static const cst_val *position_in_phrase(const cst_item *syl);
static const cst_val *phrase_num_syls(const cst_item *phrase);
static const cst_val *content_words_out(const cst_item *s);
static const cst_val *content_words_in(const cst_item *s);
static const cst_val *since_punctuation(const cst_item *word);
static const cst_val *until_punctuation(const cst_item *word);
static const cst_val *pos_in_phrase(const cst_item *s);
static const cst_val *next_accent(const cst_item *syl);
static const cst_val *asyl_out(const cst_item *syl);
static const cst_val *syl_accent(const cst_item *syl);

DEF_STATIC_CONST_VAL_STRING(val_string_onset,"onset");
DEF_STATIC_CONST_VAL_STRING(val_string_coda,"coda");
DEF_STATIC_CONST_VAL_STRING(val_string_initial,"initial");
DEF_STATIC_CONST_VAL_STRING(val_string_single,"single");
DEF_STATIC_CONST_VAL_STRING(val_string_final,"final");
DEF_STATIC_CONST_VAL_STRING(val_string_mid,"mid");
DEF_STATIC_CONST_VAL_STRING(val_string_numeric,"numeric");
DEF_STATIC_CONST_VAL_STRING(val_string_number,"number");
DEF_STATIC_CONST_VAL_STRING(val_string_month,"month");
DEF_STATIC_CONST_VAL_STRING(val_string_day,"day");
DEF_STATIC_CONST_VAL_STRING(val_string_other,"_other_");
DEF_STATIC_CONST_VAL_STRING(val_string_a,"a");
DEF_STATIC_CONST_VAL_STRING(val_string_flight,"flight");
DEF_STATIC_CONST_VAL_STRING(val_string_to,"to");
DEF_STATIC_CONST_VAL_STRING(val_string_empty,"");
// fabio added
DEF_STATIC_CONST_VAL_STRING(val_string_A,"A");
DEF_STATIC_CONST_VAL_STRING(val_string_B,"B");
DEF_STATIC_CONST_VAL_STRING(val_string_C,"C");
DEF_STATIC_CONST_VAL_STRING(val_string_D,"D");
DEF_STATIC_CONST_VAL_STRING(val_string_E,"E");
DEF_STATIC_CONST_VAL_STRING(val_string_F,"F");
DEF_STATIC_CONST_VAL_STRING(val_string_G,"G");
DEF_STATIC_CONST_VAL_STRING(val_string_H,"H");
DEF_STATIC_CONST_VAL_STRING(val_string_BHO,"BHO");
DEF_STATIC_CONST_VAL_STRING(val_string_BB,"BB");
//DEF_STATIC_CONST_VAL_STRING(val_string_B,"B"); già definito
DEF_STATIC_CONST_VAL_STRING(val_string_mB,"mB");
DEF_STATIC_CONST_VAL_STRING(val_string_NB,"NB");
DEF_STATIC_CONST_VAL_STRING(val_string_dichiarativa,"dichiarativa");
DEF_STATIC_CONST_VAL_STRING(val_string_completativa,"completativa");
DEF_STATIC_CONST_VAL_STRING(val_string_esplicativa,"esplicativa");
DEF_STATIC_CONST_VAL_STRING(val_string_pausativa,"pausativa");
DEF_STATIC_CONST_VAL_STRING(val_string_interrogativa,"interrogativa");
DEF_STATIC_CONST_VAL_STRING(val_string_esclamativa,"esclamativa");
DEF_STATIC_CONST_VAL_STRING(val_string_sospensiva,"sospensiva");
DEF_STATIC_CONST_VAL_STRING(val_string_riportativa,"riportativa");
DEF_STATIC_CONST_VAL_STRING(val_string_sstar,"s*");
DEF_STATIC_CONST_VAL_STRING(val_string_spercent,"s%");
DEF_STATIC_CONST_VAL_STRING(val_string_s,"s");
DEF_STATIC_CONST_VAL_STRING(val_string_voiceless,"-V");
DEF_STATIC_CONST_VAL_STRING(val_string_voiced_obstruents_but_no_sonorants,"+V-S");
DEF_STATIC_CONST_VAL_STRING(val_string_sonorants,"+S");
DEF_STATIC_CONST_VAL_STRING(val_string_NONE,"NONE");

//attenzione word_numsyls num_digits syl_numphones
//val_int_n() non funzione dovrebbe essere val_string_n();
   
//painte

static const cst_val *syl_accent(const cst_item *syl)
{
  //bisognerà forse predisporre per multi?
  if (item_feat_present(syl,"accent"))
    return string_val(item_feat_string(syl,"accent"));
  else
    return &val_string_NONE;
}

static const cst_val *pos_in_phrase(const cst_item *s)
{
    // position of word in phrase
    cst_item *nn = item_as(s,"Phrase");
    cst_item *p;
    int pos=0;

    for (p = item_daughter(item_parent(nn)); p; p=item_next(p),pos++)
      if (item_equal(p,nn))
	    return int_val(pos);
    // don't think you can get here
    return int_val(pos);
}

static const cst_val *since_punctuation(const cst_item *word)
{
  //(since_punctuation word)
  //Number of words since last punctuation or beginning of utterance.
  if (!item_prev(word))
    return int_val(0);
  else if (!cst_streq(val_string(Simbolic_word_end_punct(item_prev(word))),"0"))
    return int_val(0);
  else 
    return int_val(val_int(since_punctuation(item_prev(word))) + 1);
}

static const cst_val *until_punctuation(const cst_item *word)
{
  //"(until_punctuation word)
  //Number of words until next punctuation or end of utterance."  
  if (!item_next(word))
    return int_val(0);
  else if(!cst_streq(val_string(Simbolic_word_end_punct(word)),"0"))
    return int_val(0);
  else 
    return int_val(val_int(until_punctuation(item_next(word))) + 1);
}

static const cst_val *content_words_out(const cst_item *s)
{
    cst_item *nn = item_as(s,"Phrase");
    cst_item *p;
    int pos=0;

    for (p=item_next(nn); p; p=item_next(p))
    {
      if (cst_streq(val_string(gpos(item_as(p,"Word"))),"content"))
	    pos++;
    }
    // don't think you can get here
    return int_val(pos);
}

static const cst_val *content_words_in(const cst_item *s)
{
    cst_item *nn = item_as(s,"Phrase");
    cst_item *p;
    int pos=0;

    for (p=item_prev(nn); p; p=item_prev(p))
    {
      if (cst_streq(val_string(gpos(item_as(p,"Word"))),"content"))
	    pos++;
    }
    // don't think you can get here
    return int_val(pos);
}


static const cst_val *position_in_phrase(const cst_item *syl)
{ 
  //"Syllable.lisp_position_in_phrase
  //Returns the position of the syl in the phrase (0 is initial, 1 is final). (GM)")
  int ph_num_syl= val_int(ffeature(syl,"R:SylStructure.parent.R:Phrase.parent.phrase_num_syls"));
  if ((ph_num_syl == 0) || (ph_num_syl == 1))
    return float_val(0);
  else
    return float_val(((float)val_int(syl_in(syl)))/(float)(ph_num_syl-1));
}

static const cst_val *phrase_num_syls(const cst_item *phrase)
{
  //"Phrase.lisp_num_syls
  //Returns the number of syllables in a phrase as defined by the phrase relation. (GM)")
  int sylcount = 0;
  cst_item *w, *syl;
  for (w=item_daughter(phrase); w; w=item_next(w))
    {
      for (syl=item_daughter(item_as(item_as(w,"Word"),"SylStructure")); syl; syl=item_next(syl))
	sylcount++;
    }
  return int_val(sylcount);
}

static const cst_val *words_out(const cst_item *word)
{
  cst_item *w_ph;
  int num=0;
  for (w_ph=item_as(word,"Phrase"); w_ph; w_ph=item_next(w_ph))
    num++;
  return int_val(num);  
}

static const cst_val *word_number(const cst_item *word)
{
  cst_item *w;
  int num=1;
  for (w=word->relation->head;!item_equal(w,word); w=item_next(w))
    num++;
  return int_val(num);  
}
   
static const cst_val *phrasepos_in_utt(const cst_item *phrase)
{
  // Phrase.lisp_phrasepos_in_utt
  // Returns the position of the phrase in the utterance (0 for first and 1 for last). (GM)") 
  int count = -1;
  int pos = -1;
  cst_item *ph;
  ph = phrase->relation->head; // ; first phrase
  for (; ph;ph=item_next(ph)) {
    count++;
    if (item_equal(ph,phrase))
      pos=count;
  }
  if (count == 0) 
    return float_val(0);
  else 
    return float_val(pos/count);
}		  



static const cst_val *ph_gem(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"gem");
}

static const cst_val *prima_geminata_stop(const cst_item *seg) 
{  
  if (item_next(seg)) 
    if ( cst_streq("s",val_string(ph_ctype(seg))) && cst_streq("+",val_string(ph_gem(seg))) && cst_streq(item_name(seg),item_name(item_next(seg))) && (item_next(item_parent(item_as(seg, "SylStructure")))) ) // non è alla fine di una parola: esiste il successore della sillaba nella parola.  
      return VAL_STRING_1; //val_int_n(1);
    else 
      return VAL_STRING_0; //val_int_n(0);
  else 
    return VAL_STRING_0; //val_int_n(0);
}
  
static const cst_val *Accent_boundary_syl(const cst_item *syl) 
  {
  cst_item *ss;
  ss = item_as(syl,"SylStructure");
  
  if ((item_feat_present(syl, "accent")) && (cst_streq(item_feat_string(syl, "accent"), "Accented")))
    return &val_string_sstar;
  else if (((!cst_streq(item_feat_string(item_parent(ss),"pbreak"),"NB")) || (!cst_streq(val_string(Simbolic_word_end_punct(item_parent(ss))),"0"))) &&  (!item_next(ss))) //è a fine parola: non esiste il successore della sillaba nella parola.  
    return &val_string_spercent;
  else
    return &val_string_s;
    }

int ph_is_vowel_item(cst_item *ph)
{
  // TRUE if this phone is a vowel, assumes the feature vc is used
  return (cst_streq("+",val_string(ph_vc(ph))));
}

int ph_is_obstruent_item(cst_item *ph)
{
  // TRUE if this phone is a obstruent
  char *v = val_string(ph_ctype(ph));   
  return (cst_streq(v,"s") || cst_streq(v,"f") || cst_streq(v,"a"));  
}

int ph_is_sonorant_item(cst_item *ph)
{
  // TRUE if this phone is a sonorant
  return !ph_is_obstruent_item(ph);
}

int ph_is_voiced_item(cst_item *ph)
{
  // TRUE if this phone is a sonorant
  return (cst_streq("+",val_string(ph_cvox(ph))));
}

static const cst_val *syl_onset_type(const cst_item *s)
{
    // Return van Santen's classification of onset type in to one
    // of three forms:
    //   -V    contains only voiceless consonants
    //   +V-S  contains voiced obstruents but no sonorants
    //   +S    contains just sonorants
    cst_item *nn = item_as(s,"SylStructure");
    cst_item *p;
    int vox=FALSE;
    int sonorant=FALSE;

    for (p=item_daughter(nn); item_next(p) != 0; p=item_next(p))
    {
	if (ph_is_vowel_item(p))
	    break;
	if (ph_is_voiced_item(p))
	    vox = TRUE;
	if (ph_is_sonorant_item(p))
	    sonorant = TRUE;
    }

    if (item_equal(p,item_daughter(nn))) // null-onset case
      return &val_string_voiced_obstruents_but_no_sonorants; //EST_Val("+V-S");  // EST_Val("+V-S");
    else if (sonorant)
	return &val_string_sonorants; //EST_Val("+S"); EST_Val("+S");
    else if (vox)
	return &val_string_voiced_obstruents_but_no_sonorants; //EST_Val("+V-S");EST_Val("+V-S");
    else
	return &val_string_voiceless; //EST_Val("-V"); EST_Val("-V");
}
 
static const cst_val *syl_coda_type(const cst_item *s)
{
    // Return van Santen's classification of onset type in to one
    // of three forms:
    //   -V    contains only voiceless consonants
    //   +V-S  contains voiced obstruents but no sonorants
    //   +S    contains just sonorants

  //DEF_STATIC_CONST_VAL_STRING(val_string_voiceless,"-V");
  //DEF_STATIC_CONST_VAL_STRING(val_string_voiced_obstruents_but_no_sonorants,"+V-S");
  //DEF_STATIC_CONST_VAL_STRING(val_string_sonorants,"+S");

    cst_item *nn = item_as(s,"SylStructure");
    cst_item *p;
    int vox=FALSE;
    int sonorant=FALSE;
    
    for (p=item_daughter(nn); item_next(p) != 0; p=item_next(p))
      {      
	if (ph_is_vowel_item(p))
	  break;
      }
    
    if (item_next(p) == 0)         // empty coda
      return &val_string_sonorants; //EST_Val("+S");

    for (p=item_next(p); p != 0; p=item_next(p))
    {
      if (ph_is_voiced_item(p))
	    vox = 1;
      if (ph_is_sonorant_item(p))
	    sonorant = 1;
    }
    
    if (sonorant)
      return &val_string_sonorants; //EST_Val("+S"); 
    else if (vox)
      return &val_string_voiced_obstruents_but_no_sonorants; //EST_Val("+V-S");
    else
      return &val_string_voiceless; //EST_Val("-V");
}

//FABIO
static const cst_val *syl_numphones(const cst_item *syl)
{
    // Number of phones in syllable
  cst_item *d;
  int c;
  
  for (c=0,d=item_daughter(item_as(syl,"SylStructure"));
       d;
       d=item_next(d),c++);
  
  return val_int_n(c);
}


static const cst_val *next_stress(const cst_item *syl)
{
  if (syl == 0)
    return val_string_n(0);
  else if (cst_streq("1",val_string(item_feat(syl,"stress"))))
    return val_string_n(0);
  else
    return val_string_n(1+val_int(next_stress(item_next(syl))));
}

static const cst_val *last_stress(const cst_item *syl)
{
  if (syl == 0)
    return val_string_n(0);
  else if (cst_streq("1",val_string(item_feat(syl,"stress"))))
    return val_string_n(0);
  else
    return val_string_n(1+val_int(last_stress(item_prev(syl))));
}


static const cst_val *type(const cst_item *phrase)
{
  const char *pname;  
  pname=val_string(Simbolic_word_end_punct(item_as(item_last_daughter(phrase),"Word")));
  if (cst_streq("A",pname))
    return &val_string_dichiarativa;
  else if (cst_streq("B",pname))
    return &val_string_completativa;
  else if (cst_streq("D",pname))
    return &val_string_esplicativa;
  else if (cst_streq("C",pname))
    return &val_string_pausativa;
  else if (cst_streq("F",pname))
    return &val_string_interrogativa;
  else if (cst_streq("E",pname))
    return &val_string_esclamativa;
  else if (cst_streq("G",pname))
    return &val_string_sospensiva;
  else if (cst_streq("H",pname))
    return &val_string_riportativa;
  else 
    return &val_string_BHO;
}

static const cst_val *word_duration(const cst_item *word)
{
  const cst_item *n = item_as(word,"SylStructure");
  //nel codice festival c'era un controllo ...  
  cst_item *fd,*ld;
  fd = item_daughter(item_daughter(n));
  ld = item_last_daughter(item_last_daughter(n));

  if (ld == 0)
    return VAL_STRING_0;
  cst_item *ps = item_prev(item_as(fd,"Segment"));
  if (ps == 0)
    return float_val(item_feat_float(ld,"end"));
  else
    // return val_string_n(val_float(item_feat(ld,"end"))-val_float(item_feat(ps,"end")));
    return float_val(item_feat_float(ld,"end")- item_feat_float(ps,"end"));
}
  
// modified by fabio ... non dovrebbe servire più visto che ho messo pbreak nelle funzioni
/*static const cst_val *pbreak(const cst_item *word)
{
    cst_item *ww,*pp;
    const char *pname;

    ww = item_as(word,"Phrase");

    if ((ww == NULL) || (item_next(ww) != 0))
	return &val_string_NB;
    else
    {
	pp = item_parent(ww);
	pname = val_string(item_feat(pp,"name"));
	if (cst_streq("BB",pname))
	    return &val_string_BB;
	else if (cst_streq("B",pname))
	    return &val_string_B;
	else if (cst_streq("mB",pname))
	    return &val_string_mB;
	else 
	    return &val_string_NB;
    }
}
*/

static const cst_val *pos_in_word(const cst_item *syl)
{
  // position of syllable in word
  //  EST_Item *nn = as(s,"SylStructure");
  const cst_item *nn = item_as(syl,"SylStructure");
  //EST_Item *p;
  cst_item *p;
  int pos=0;
  for (p=item_daughter(item_parent(nn)); p; p=item_next(p),pos++)
    if (item_equal(p,nn))
      return val_string_n(pos);
  // don't think you can get here
  return val_string_n(pos);
}

static const cst_val *Simbolic_word_end_punct(const cst_item *word)
{
  //Categorie di punctuation at end of related WORD, otherwise 0 o BHO
  const char *punct;
  punct= val_string(word_punc(word));
  //printf("Simbolic... word %s punc \n", punct); 
  if ((cst_streq("0",punct)) || (cst_streq("",punct)))
    return VAL_STRING_0;
  else if ((cst_streq(".",punct)) || (cst_streq(").",punct)) || (cst_streq(".»",punct)) || (cst_streq(".\"",punct)) || (cst_streq(".-",punct)))
    return &val_string_A;
  else if ((cst_streq(";",punct)) || (cst_streq(");",punct)) || (cst_streq(";»",punct)) || (cst_streq(";\"",punct)) || (cst_streq(";-",punct)))
    return &val_string_B;
  else if ((cst_streq(",",punct)) || (cst_streq("),",punct)) || (cst_streq(",»",punct)) || (cst_streq(",\"",punct)) || (cst_streq(",-",punct)))
    return &val_string_C;
  else if ((cst_streq(":",punct)) || (cst_streq("):",punct)) || (cst_streq(":»",punct)) || (cst_streq(":\"",punct)) || (cst_streq(":-",punct)))
    return &val_string_D;
  else if ((cst_streq("!",punct)) || (cst_streq(")!",punct)) || (cst_streq("!»",punct)) || (cst_streq("!\"",punct)) || (cst_streq("!-",punct)))
    return &val_string_E;
  else if ((cst_streq("?",punct)) || (cst_streq(")?",punct)) || (cst_streq("?»",punct)) || (cst_streq("?\"",punct)) || (cst_streq("?-",punct)) || (cst_streq("?).",punct)) || (cst_streq("...?»",punct)) || (cst_streq("...?\"",punct)))
    return &val_string_F;
  else if ((cst_streq("..",punct)) || (cst_streq("...",punct)) || (cst_streq("...»",punct)) || (cst_streq("...\"",punct)))
    return &val_string_G;
  else if ((cst_streq("-",punct)) || (cst_streq("\"",punct)) || (cst_streq("»",punct)) || (cst_streq("\"",punct)) || (cst_streq(")",punct)) || (cst_streq("]",punct)) || (cst_streq("}",punct)))
    return &val_string_H;
  else 
    return &val_string_BHO;
}

static const cst_val *Simbolic_word_start_prepunctuation(const cst_item *word)
{
  //Categorie di pre_punctuation at end of related WORD, otherwise 0 o BHO
  const char *punct;
  punct= val_string(word_start_prepunctuation(word));
  //printf("Simbolic... word %s punc \n", punct); 
  if ((cst_streq("0",punct)) || (cst_streq("",punct)))
    return VAL_STRING_0; 
  else if ((cst_streq("-",punct)) || (cst_streq("\"",punct)) || (cst_streq("«",punct)) || (cst_streq("\"",punct)) || (cst_streq("(",punct)) || (cst_streq("[",punct)) || (cst_streq("{",punct)))
    return &val_string_H;
  else 
    return &val_string_BHO;
}    

static const cst_val *word_start_prepunctuation(const cst_item *word)
{
    cst_item *ww;
    const cst_val *v;

    ww = item_as(word,"Token");

    if ((ww != NULL) && (item_next(ww) != 0))
	v = &val_string_empty;
    else
	v = ffeature(item_parent(ww),"prepunctuation");

    /*     printf("word_start_prepunctuation word %s prepunct %s\n",
	   item_feat_string(ww,"name"),
	   val_string(v)); */

    return v;

}


static const cst_val *word_punc(const cst_item *word)
{
    cst_item *ww;
    const cst_val *v;

    ww = item_as(word,"Token");

    if ((ww != NULL) && (item_next(ww) != 0))
	v = &val_string_empty;
    else
	v = ffeature(item_parent(ww),"punc");

    /* printf("word_punc word %s punc %s\n",
	   item_feat_string(ww,"name"),
	   val_string(v)); */

    return v;

}

// modified by fabio
static const cst_val *word_break(const cst_item *word)
{
    cst_item *ww,*pp;
    const char *pname;

    ww = item_as(word,"Phrase");

    if ((ww == NULL) || (item_next(ww) != 0))
	return VAL_STRING_1;
    else
    {
	pp = item_parent(ww);
	pname = val_string(item_feat(pp,"name"));
	if (cst_streq("BB",pname))
	    return VAL_STRING_4;
	else if (cst_streq("B",pname))
	    return VAL_STRING_3;
	else if (cst_streq("mB",pname))
	    return VAL_STRING_2;
	else 
	    return VAL_STRING_1;
    }
}

DEF_STATIC_CONST_VAL_STRING(val_string_content,"content");

static const cst_val *gpos(const cst_item *word)
{
    /* Guess at part of speech (function/content) */
    const char *w;
    int s,t;

    w = item_feat_string(word,"name");

    for (s=0; it_gpos[s]; s++)
    {
	for (t=1; it_gpos[s][t]; t++)
	    if (cst_streq(w,val_string(it_gpos[s][t])))
		return it_gpos[s][0];
    }

    return (cst_val *)&val_string_content;
}

static const cst_val *apo(const cst_item *word)
{
  const char *w;
  w = item_feat_string(word,"name");
  if (cst_regex_match(apostrofo,w))
    return val_string_n(1);
  else 
    return val_string_n(0);
}	



static const cst_val *word_numsyls(const cst_item *word)
{
    cst_item *d;
    int c;
    
    for (c=0,d=item_daughter(item_as(word,"SylStructure"));
	 d;
	 d=item_next(d),c++);

    return val_int_n(c);
}

static const cst_val *ssyl_in(const cst_item *syl)
{
    /* Number of stressed syllables since last major break */
    const cst_item *ss,*p,*fs;
    int c;

    ss = item_as(syl,"Syllable");

    fs = path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.daughter.R:SylStructure.daughter");

    /* This should actually include the first syllable, but Festival's
       doesn't. */
    for (c=0, p=item_prev(ss); 
	 p && (!item_equal(p,fs)) && (c < CST_CONST_INT_MAX);
	 p=item_prev(p))
    {
	if (cst_streq("1",item_feat_string(p,"stress")))
	    c++;
    }
    
    return val_string_n(c);  /* its used randomly as int and float */
}

static const cst_val *ssyl_out(const cst_item *syl)
{
    /* Number of stressed syllables until last major break */
    const cst_item *ss,*p,*fs;
    int c;

    ss = item_as(syl,"Syllable");

    fs = path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.daughtern.R:SylStructure.daughter");

    for (c=0, p=item_next(ss); 
	 p && (c < CST_CONST_INT_MAX); 
	 p=item_next(p))
    {
	if (cst_streq("1",item_feat_string(p,"stress")))
	    c++;
	if (item_equal(p,fs))
	    break;
    }
    
    return val_string_n(c);  /* its used randomly as int and float */
}

static const cst_val *syl_in(const cst_item *syl)
{
    /* Number of syllables since last major break */
    const cst_item *ss,*p,*fs;
    int c;

    ss = item_as(syl,"Syllable");

    fs = path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.daughter.R:SylStructure.daughter");

    for (c=0, p=ss; 
	 p && (c < CST_CONST_INT_MAX); 
	 p=item_prev(p),c++)
	if (item_equal(p,fs))
	    break;
    return val_string_n(c);
}

static const cst_val *syl_out(const cst_item *syl)
{
    /* Number of syllables until next major break */
    cst_item *ss,*p;
    const cst_item *fs;
    int c;

    ss = item_as(syl,"Syllable");

    fs = path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.daughtern.R:SylStructure.daughtern");

    for (c=0, p=ss; 
	 p && (c < CST_CONST_INT_MAX); 
	 p=item_next(p),c++)
	if (item_equal(p,fs))
	    break;
    return val_string_n(c);
}

static const cst_val *syl_break(const cst_item *syl)
{
    /* Break level after this syllable */
    cst_item *ss;

    ss = item_as(syl,"SylStructure");

    if (ss == NULL)
	return VAL_STRING_1;  /* hmm, no sylstructure */
    else if (item_next(ss) != NULL)
	return VAL_STRING_0;  /* word internal */
    else if (item_parent(ss) == NULL)  /* no parent */
	return VAL_STRING_1;
    else
	return word_break(item_parent(ss));
}

static const cst_val *syl_codasize(const cst_item *syl)
{
  // number of segments in the coda
  cst_item *d;
  int c;
    
  for (c=1,d=item_last_daughter(item_as(syl,"SylStructure"));
       d;
       d=item_prev(d),c++)
    {
      if (cst_streq("+",val_string(ph_vc(d))))
	break;
    }
  return val_string_n(c);
}

static const cst_val *syl_onsetsize(const cst_item *syl)
{
    cst_item *d;
    int c;
    
    for (c=0,d=item_daughter(item_as(syl,"SylStructure"));
	 d;
	 d=item_next(d),c++)
    {
	if (cst_streq("+",val_string(ph_vc(d))))
	    break;
    }

    return val_string_n(c);
}

static const cst_val *ph_vc(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"vc");
}
static const cst_val *ph_vlng(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"vlng");
}
static const cst_val *ph_vheight(const cst_item *p)
{
   return phone_feature(item_phoneset(p),item_name(p),"vheight");
}
static const cst_val *ph_vrnd(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"vrnd");
}
static const cst_val *ph_vfront(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"vfront");
}
static const cst_val *ph_ctype(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"ctype");
}
static const cst_val *ph_cplace(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"cplace");
}
static const cst_val *ph_cvox(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"cvox");
}

static const cst_val *ph_semi(const cst_item *p)
{
    return phone_feature(item_phoneset(p),item_name(p),"semi");
}


static const cst_val *accented(const cst_item *syl)
{
    if ((item_feat_present(syl,"accent")) ||
	(item_feat_present(syl,"endtone")))
	return VAL_STRING_1;
    else
	return VAL_STRING_0;
}
/*
static const cst_val *asyl_in(const cst_item *syl)
{
// Number of accented syllables since last major break //
    const cst_item *ss,*p,*fs;
    int c;

    ss = item_as(syl,"Syllable");

    fs = path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.daughter.R:SylStructure.daughter");

    for (c=0, p=ss; 
	 p && (c < CST_CONST_INT_MAX); 
	 p=item_prev(p))
    {
	if (val_int(accented(p)) == 1)
	    c++;
	if (item_equal(p,fs))
	    break;
    }
    
    return val_string_n(c);
}
*/

//fatta io
static const cst_val *asyl_in(const cst_item *syl)
{
  // Number of accented syllables since last major break
  cst_item *nn = item_as(syl,"Syllable");
  // The first syllable in the phrase
  
  cst_item *fsyl = path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.daughter.R:SylStructure.daughter");
  cst_item *p;
  int count;
  if (item_equal(nn,fsyl)) return int_val(0);
  for (count=0,p=item_prev(nn); (p != 0) && (!item_equal(p,fsyl)); p=item_prev(p))
    if (val_int(accented(p)))
      count ++;
       
  return int_val(count);
}
  


//..fatta io
static const cst_val *asyl_out(const cst_item *syl)
{
    // Number of accented syllables to next phrase break 
    cst_item *nn = item_as(syl,"Syllable");
    // The last syllable in the phrase
    cst_item *lsyl = path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.daughtern.R:SylStructure.daughtern");
    //as(daughtern(last(parent(s,"SylStructure"),"Phrase"),"SylStructure"),"Syllable");
    cst_item *p;
    int count;

    if (item_equal(nn,lsyl)) return int_val(0);
    for (count=0,p=item_next(nn); (p != 0); p=item_next(p))
    {
      if (val_int(accented(p)))
	count ++;
      if (item_equal(p,lsyl)) break;
    }
    return int_val(count);
}


static const cst_val *seg_coda_ctype(const cst_item *seg, const char *ctype)
{
    const cst_item *s;
    const cst_phoneset *ps = item_phoneset(seg);
    
    for (s=item_last_daughter(item_parent(item_as(seg,"SylStructure")));
	 s;
	 s=item_prev(s))
    {
	if (cst_streq("+",phone_feature_string(ps,item_feat_string(s,"name"),
					       "vc")))
	    return VAL_STRING_0;
	if (cst_streq(ctype,phone_feature_string(ps,item_feat_string(s,"name"),
					       "ctype")))
	    return VAL_STRING_1;
    }

    return VAL_STRING_0;
}

static const cst_val *seg_onset_ctype(const cst_item *seg, const char *ctype)
{
    const cst_item *s;
    const cst_phoneset *ps = item_phoneset(seg);
    
    for (s=item_daughter(item_parent(item_as(seg,"SylStructure")));
	 s;
	 s=item_next(s))
    {
	if (cst_streq("+",phone_feature_string(ps,item_feat_string(s,"name"),
					       "vc")))
	    return VAL_STRING_0;
	if (cst_streq(ctype,phone_feature_string(ps,item_feat_string(s,"name"),
						 "ctype")))
	    return VAL_STRING_1;
    }

    return VAL_STRING_0;
}

static const cst_val *seg_coda_fric(const cst_item *seg)
{
    return seg_coda_ctype(seg,"f");
}

static const cst_val *seg_onset_fric(const cst_item *seg)
{
    return seg_onset_ctype(seg,"f");
}

static const cst_val *seg_coda_stop(const cst_item *seg)
{
    return seg_coda_ctype(seg,"s");
}

static const cst_val *seg_onset_stop(const cst_item *seg)
{
    return seg_onset_ctype(seg,"s");
}

static const cst_val *seg_coda_nasal(const cst_item *seg)
{
    return seg_coda_ctype(seg,"n");
}

static const cst_val *seg_onset_nasal(const cst_item *seg)
{
    return seg_onset_ctype(seg,"n");
}

static const cst_val *seg_coda_glide(const cst_item *seg)
{
    if (seg_coda_ctype(seg,"r") == VAL_STRING_0)
	    return seg_coda_ctype(seg,"l");
    return VAL_STRING_1;
}

static const cst_val *seg_onset_glide(const cst_item *seg)
{
    if (seg_onset_ctype(seg,"r") == VAL_STRING_0)
	    return seg_onset_ctype(seg,"l");
    return VAL_STRING_1;
}

static const cst_val *seg_onsetcoda(const cst_item *seg)
{
    const cst_item *s;
    const cst_phoneset *ps = item_phoneset(seg);
    
    for (s=item_next(item_as(seg,"SylStructure"));
	 s;
	 s=item_next(s))
    {
	if (cst_streq("+",phone_feature_string(ps,item_feat_string(s,"name"),
					       "vc")))
	    return (cst_val *)&val_string_onset;
    }
    return (cst_val *)&val_string_coda;
}

static const cst_val *segment_duration(const cst_item *seg)
{
    const cst_item *s = item_as(seg,"Segment");

    if (!s)
	return VAL_STRING_0;
    else if (item_prev(s) == NULL)
	return item_feat(s,"end");
    else
	/* It should be okay to construct this as it will get
           dereferenced when the CART interpreter frees its feature
           cache. */
	return float_val(item_feat_float(s,"end")
			 - item_feat_float(item_prev(s),"end"));
}

static const cst_val *pos_in_syl(const cst_item *seg)
{
  const cst_item *s;
  int c;
  s=item_as(seg,"SylStructure");
  if (s)
    {
      for (c=-1; s; s=item_prev(s),c++);
      return val_string_n(c);
    }
  else
    return val_string_n(0);
}

static const cst_val *position_type(const cst_item *syl)
{
    const cst_item *s = item_as(syl,"SylStructure");

    if (s == 0)
	return (cst_val *)&val_string_single;
    else if (item_next(s) == 0)
    {
	if (item_prev(s) == 0)
	    return (cst_val *)&val_string_single;
	else
	    return (cst_val *)&val_string_final;
    }
    else if (item_prev(s) == 0)
	return (cst_val *)&val_string_initial;
    else
	return (cst_val *)&val_string_mid;
}

static const cst_val *sub_phrases(const cst_item *syl)
{
    const cst_item *s;
    int c;
    
    for (c=0,s=path_to_item(syl,"R:SylStructure.parent.R:Phrase.parent.p");
	 s && (c < CST_CONST_INT_MAX); 
	 s=item_prev(s),c++);

    return val_string_n(c);
}

/*static const cst_val *last_accent(const cst_item *syl)
{
    const cst_item *s;
    int c;
    
    for (c=0,s=item_as(syl,"Syllable");
	 s && (c < CST_CONST_INT_MAX); 
	 s=item_prev(s),c++)
    {
	if (accented(s))
	    return val_string_n(c);
    }

    return val_string_n(c);
}*/

//fatto io ...
static const cst_val *last_accent(const cst_item *syl)
{ 
  // Number of syllables since last accented syllable
  cst_item *nn = item_as(syl,"Syllable");
  cst_item *p;
  int count;
  
  for (count=0,p=item_prev(nn); p != 0; p=item_prev(p),count++)
    if (val_int(accented(p)))
      return int_val(count);
  return int_val(count);
}

//fatto io ...
static const cst_val *next_accent(const cst_item *syl)
{
  // Number of syllables to next accented syllable
  cst_item *nn = item_as(syl,"Syllable");
  cst_item *p;
  int count;
  
  for (count=0,p=item_next(nn); p != 0; p=item_next(p),count++)
    if (val_int(accented(p)))
      return int_val(count);
  return int_val(count);
}



static const cst_val *syl_final(const cst_item *seg)
{   /* last segment in a syllable */
    const cst_item *s = item_as(seg,"SylStructure");

    if (!s || (item_next(s) == NULL))
	return VAL_STRING_1;
    else
	return VAL_STRING_0;
}

static const cst_val *num_digits(const cst_item *token)
{   
    const char *name = item_feat_string(token,"name");

    return val_int_n(strlen(name));
}

static const cst_val *month_range(const cst_item *token)
{   
    int v = item_feat_int(token,"name");

    if ((v > 0) && ( v < 32))
	return VAL_STRING_1;	
    else
	return VAL_STRING_0;	
}

static const cst_val *token_pos_guess(const cst_item *token)
{   
    const char *name = item_feat_string(token,"name");
    char *dc = word_lts_rewrites_word(cst_downcase(name), &italian_downcase_lts_rewrites);//cst_downcase(name);//fabio per codifica caratteri strani e accenti 
    const cst_val *r;

    if (cst_regex_match(cst_rx_digits,dc))
	r = &val_string_numeric;
    else if ((cst_regex_match(cst_rx_double,dc)) ||
	(cst_regex_match(cst_rx_double,dc)))
	r = &val_string_number;
    else if (cst_streq(dc,"jan") ||
	cst_streq(dc,"january") ||
	cst_streq(dc,"feb") ||
	cst_streq(dc,"february") ||
	cst_streq(dc,"mar") ||
	cst_streq(dc,"march") ||
	cst_streq(dc,"apr") ||
	cst_streq(dc,"april") ||
	cst_streq(dc,"may") ||
	cst_streq(dc,"jun") ||
	cst_streq(dc,"june") ||
	cst_streq(dc,"jul") ||
	cst_streq(dc,"july") ||
	cst_streq(dc,"aug") ||
	cst_streq(dc,"august") ||
	cst_streq(dc,"sep") ||
	cst_streq(dc,"sept") ||
	cst_streq(dc,"september") ||
	cst_streq(dc,"oct") ||
	cst_streq(dc,"october") ||
	cst_streq(dc,"nov") ||
	cst_streq(dc,"november") ||
	cst_streq(dc,"dec") ||
	cst_streq(dc,"december"))
	r = &val_string_month;
    else if (cst_streq(dc,"sun") ||
	cst_streq(dc,"sunday") ||
	cst_streq(dc,"mon") ||
	cst_streq(dc,"monday") ||
	cst_streq(dc,"tue") ||
	cst_streq(dc,"tues") ||
	cst_streq(dc,"tuesday") ||
	cst_streq(dc,"wed") ||
	cst_streq(dc,"wednesday") ||
	cst_streq(dc,"thu") ||
	cst_streq(dc,"thurs") ||
	cst_streq(dc,"thursday") ||
	cst_streq(dc,"fri") ||
	cst_streq(dc,"friday") ||
	cst_streq(dc,"sat") ||
	cst_streq(dc,"saturday"))
	r = &val_string_day;
   /* ignoring the "token_most_common" condition, does get used */
    else if (cst_streq(dc,"a"))
	r =  &val_string_a;
    else if (cst_streq(dc,"flight"))
	r =  &val_string_flight;
    else if (cst_streq(dc,"to"))
	r =  &val_string_to;
    else
	r =  &val_string_other;
    cst_free(dc);
    return r;
}

void it_ff_register(cst_features *ffunctions)
{
    ff_register(ffunctions, "word_break",word_break);
    ff_register(ffunctions, "word_punc",word_punc);
    ff_register(ffunctions, "gpos",gpos);
    ff_register(ffunctions, "word_numsyls",word_numsyls);
    ff_register(ffunctions, "ssyl_in",ssyl_in);
    ff_register(ffunctions, "ssyl_out",ssyl_out);
    ff_register(ffunctions, "syl_in",syl_in);
    ff_register(ffunctions, "syl_out",syl_out);
    ff_register(ffunctions, "syl_break",syl_break);
    ff_register(ffunctions, "old_syl_break",syl_break);
    ff_register(ffunctions, "syl_onsetsize",syl_onsetsize);
    ff_register(ffunctions, "syl_codasize",syl_codasize);
    ff_register(ffunctions, "ph_vc",ph_vc);
    ff_register(ffunctions, "ph_vlng",ph_vlng);
    ff_register(ffunctions, "ph_vheight",ph_vheight);
    ff_register(ffunctions, "ph_vfront",ph_vfront);
    ff_register(ffunctions, "ph_vrnd",ph_vrnd);
    ff_register(ffunctions, "ph_ctype",ph_ctype);
    ff_register(ffunctions, "ph_cplace",ph_cplace);
    ff_register(ffunctions, "ph_cvox",ph_cvox);
    ff_register(ffunctions, "accented",accented);
    ff_register(ffunctions, "asyl_in",asyl_in);
    ff_register(ffunctions, "seg_coda_fric",seg_coda_fric);
    ff_register(ffunctions, "seg_onset_fric",seg_onset_fric);
    ff_register(ffunctions, "seg_coda_stop",seg_coda_stop);
    ff_register(ffunctions, "seg_onset_stop",seg_onset_stop);
    ff_register(ffunctions, "seg_coda_nasal",seg_coda_nasal);
    ff_register(ffunctions, "seg_onset_nasal",seg_onset_nasal);
    ff_register(ffunctions, "seg_coda_glide",seg_coda_glide);
    ff_register(ffunctions, "seg_onset_glide",seg_onset_glide);
    ff_register(ffunctions, "seg_onsetcoda",seg_onsetcoda);
    ff_register(ffunctions, "pos_in_syl",pos_in_syl);
    ff_register(ffunctions, "position_type",position_type);
    ff_register(ffunctions, "sub_phrases",sub_phrases);
    ff_register(ffunctions, "last_accent",last_accent);
    ff_register(ffunctions, "syl_final",syl_final);
    ff_register(ffunctions, "num_digits",num_digits);
    ff_register(ffunctions, "month_range",month_range);
    ff_register(ffunctions, "token_pos_guess",token_pos_guess);
    ff_register(ffunctions, "segment_duration",segment_duration);
    //fabio added
    ff_register(ffunctions, "Simbolic_word_end_punct",Simbolic_word_end_punct);
    ff_register(ffunctions, "word_start_prepunctuation",word_start_prepunctuation);
    ff_register(ffunctions, "Simbolic_word_start_prepunctuation",Simbolic_word_start_prepunctuation);
    ff_register(ffunctions, "pos_in_word",pos_in_word);
    //ff_register(ffunctions, "pbreak",pbreak);
    ff_register(ffunctions, "apo",apo);
    ff_register(ffunctions, "ph_semi",ph_semi);
    ff_register(ffunctions, "word_duration",word_duration); 
    ff_register(ffunctions, "type",type); 
    ff_register(ffunctions, "next_stress",next_stress); 
    ff_register(ffunctions, "last_stress",last_stress); 
    ff_register(ffunctions, "syl_numphones",syl_numphones); 
    ff_register(ffunctions, "Accent_boundary_syl",Accent_boundary_syl); 
    ff_register(ffunctions, "syl_onset_type",syl_onset_type); 
    ff_register(ffunctions, "syl_coda_type",syl_coda_type); 
    ff_register(ffunctions, "prima_geminata_stop",prima_geminata_stop); 
    ff_register(ffunctions, "ph_gem",ph_gem); 
    ff_register(ffunctions, "phrasepos_in_utt",phrasepos_in_utt); 
    ff_register(ffunctions, "word_number",word_number); 
    ff_register(ffunctions, "words_out",words_out); 
    ff_register(ffunctions, "position_in_phrase",position_in_phrase); 
    ff_register(ffunctions, "phrase_num_syls",phrase_num_syls); 
    ff_register(ffunctions, "content_words_out",content_words_out); 
    ff_register(ffunctions, "content_words_in",content_words_in); 
    ff_register(ffunctions, "since_punctuation",since_punctuation); 
    ff_register(ffunctions, "until_punctuation",until_punctuation); 
    ff_register(ffunctions, "pos_in_phrase",pos_in_phrase); 
    ff_register(ffunctions, "next_accent",next_accent); 
    ff_register(ffunctions, "asyl_out",asyl_out); 
    ff_register(ffunctions, "syl_accent",syl_accent); 
    //ff_register(ffunctions, "",); 
    //ff_register(ffunctions, "",); 
    //ff_register(ffunctions, "",); 
    //ff_register(ffunctions, "",); 
    //ff_register(ffunctions, "",);  
    //ff_register(ffunctions, "",); 
    //ff_register(ffunctions, "",); 
    //ff_register(ffunctions, "",); 
    //ff_register(ffunctions, "",); 


}
