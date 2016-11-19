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
/*  IT Text analysis                                                     */
/*                                                                       */
/*************************************************************************/
#ifndef _IT_TEXT_H__
#define _IT_TEXT_H__

#include "cst_val.h"
#include "cst_hrg.h"
#include "cst_cart.h"
#include "cst_regex.h"

cst_val *it_exp_number(const char *numstring);
cst_val *it_exp_digits(const char *numstring);
cst_val *it_exp_month(const char *numstring);
cst_val *it_exp_id(const char *numstring);
cst_val *it_exp_ordinal(const char *numstring);
cst_val *it_exp_ordinal_f(const char *numstring);
cst_val *it_exp_ordinal_p(const char *rawnumstring);
cst_val *it_exp_letters(const char *lets);
cst_val *it_exp_real(const char *numstring);
int it_exp_roman(const char *roman);


extern const cst_cart it_nums_cart;

void it_text_init();
cst_utterance *it_textanalysis(cst_utterance *u);
cst_val *it_tokentowords(cst_item *token);

int it_aswd(const char *w);

extern const char *italian_punctuation;
extern const char *italian_prepunctuation;
extern const char *italian_singlecharsymbols;
extern const char *italian_whitespace;

extern const cst_regex *apostrofo;
extern const cst_regex *apice_finale;


#endif

