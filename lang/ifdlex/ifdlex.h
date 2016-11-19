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
/*             Author:  Fabio Tesser <tesser@pd.istc.cnr.it>             */
/*               Date:  October 2004                                     */
/*************************************************************************/
/*                                                                       */
/*  IFD Lexicon definition                                               */
/*                                                                       */
/*************************************************************************/

#ifndef _IFDLEX_H
#define _IFDLEX_H

#ifdef __cplusplus
extern "C" {
#endif

void ifd_lex_init(void);
  
//int ifd_syl_boundary(const cst_item *i,const cst_val *v);
  
extern cst_lexicon ifd_lex;

#ifdef __cplusplus
};
#endif

#endif /* _IFDLEX_H */
