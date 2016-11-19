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
/*  Italian text analysis functions                                      */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "flite.h"
#include "italian.h"
#include "it_text.h"
#include "cst_regex.h"
//fabio
#include "cst_lts_rewrites.h"
extern const cst_lts_rewrites italian_downcase_lts_rewrites;
char *word_lts_rewrites_word(const char *word, const cst_lts_rewrites *r);
//fabio

//static int text_splitable(const char *s,int i);
//static cst_val *state_name(const char *name,cst_item *t);

const char *italian_punctuation = "\".,:;!?(){}[]"; //tolto '`
const char *italian_prepunctuation = "\"({["; //tolto '`
const char *italian_singlecharsymbols = "";
const char *italian_whitespace = " \t\n\r";


//static const char *caratteri_accentati[] = {"á","à","é","è","í","ì","ó","ò","ú","ù","Á","À","É","È","Í","Ì","Ó","Ò","Ú","Ù",NULL};


static const unsigned char apostrofo_rxprog[] = {
	156, 
	6, 0, 26, 1, 0, 3, 11, 0, 6, 3, 
	0, 0, 8, 0, 5, 39, 0, 11, 0, 6, 
	3, 0, 0, 2, 0, 3, 0, 0, 0
};

static const cst_regex apostrofo_rx = {
	0,
	1,
	NULL,
	0,
	30,
	(char *) apostrofo_rxprog
};

static const unsigned char apice_finale_rxprog[] = {
	156, 
	6, 0, 20, 1, 0, 3, 11, 0, 6, 3, 
	0, 0, 8, 0, 5, 39, 0, 2, 0, 3, 
	0, 0, 0
};
static const cst_regex apice_finale_rx = {
	0,
	1,
	NULL,
	0,
	24,
	(char *) apice_finale_rxprog
};


static const unsigned char fourdigits_rxprog[] = {
	156, 
	6, 0, 65, 1, 0, 3, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	4, 0, 14, 48, 49, 50, 51, 52, 53, 54, 
	55, 56, 57, 0, 4, 0, 14, 48, 49, 50, 
	51, 52, 53, 54, 55, 56, 57, 0, 4, 0, 
	14, 48, 49, 50, 51, 52, 53, 54, 55, 56, 
	57, 0, 2, 0, 3, 0, 0, 0
};
static const cst_regex fourdigits_rx = {
	0,
	1,
	NULL,
	0,
	69,
	(char *) fourdigits_rxprog
};

static const unsigned char threedigits_rxprog[] = {
	156, 
	6, 0, 51, 1, 0, 3, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	4, 0, 14, 48, 49, 50, 51, 52, 53, 54, 
	55, 56, 57, 0, 4, 0, 14, 48, 49, 50, 
	51, 52, 53, 54, 55, 56, 57, 0, 2, 0, 
	3, 0, 0, 0
};
static const cst_regex threedigits_rx = {
	0,
	1,
	NULL,
	0,
	55,
	(char *) threedigits_rxprog
};

static const unsigned char romannums_rxprog[] = {
	156, 
	6, 0, 137, 1, 0, 3, 21, 0, 3, 6, 
	0, 36, 8, 0, 5, 73, 0, 6, 0, 8, 
	8, 0, 8, 73, 0, 6, 0, 3, 9, 0, 
	3, 6, 0, 8, 8, 0, 8, 73, 0, 6, 
	0, 3, 9, 0, 89, 6, 0, 9, 8, 0, 
	83, 73, 86, 0, 6, 0, 50, 8, 0, 5, 
	86, 0, 6, 0, 8, 8, 0, 8, 73, 0, 
	6, 0, 3, 9, 0, 3, 6, 0, 8, 8, 
	0, 8, 73, 0, 6, 0, 3, 9, 0, 3, 
	6, 0, 8, 8, 0, 8, 73, 0, 6, 0, 
	3, 9, 0, 30, 6, 0, 9, 8, 0, 24, 
	73, 88, 0, 6, 0, 18, 8, 0, 5, 88, 
	0, 10, 0, 10, 4, 0, 0, 86, 73, 88, 
	0, 31, 0, 3, 2, 0, 3, 0, 0, 0
};
static const cst_regex romannums_rx = {
	0,
	1,
	NULL,
	0,
	141,
	(char *) romannums_rxprog
};

static const unsigned char digitsslashdigits_rxprog[] = {
	156, 
	6, 0, 74, 1, 0, 3, 4, 0, 13, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 0, 10, 
	0, 17, 4, 0, 0, 48, 49, 50, 51, 52, 
	53, 54, 55, 56, 57, 0, 8, 0, 5, 47, 
	0, 4, 0, 13, 49, 50, 51, 52, 53, 54, 
	55, 56, 57, 0, 10, 0, 17, 4, 0, 0, 
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 
	0, 2, 0, 3, 0, 0, 0
};
static const cst_regex digitsslashdigits_rx = {
	0,
	1,
	NULL,
	0,
	78,
	(char *) digitsslashdigits_rxprog
};

static const unsigned char dottedabbrevs_rxprog[] = {
	156, 
	6, 0, 147, 1, 0, 3, 4, 0, 56, 97, 
	98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 
	108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 
	118, 119, 120, 121, 122, 65, 66, 67, 68, 69, 
	70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 
	80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 
	90, 0, 21, 0, 3, 6, 0, 64, 8, 0, 
	5, 46, 0, 4, 0, 56, 97, 98, 99, 100, 
	101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 
	111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 
	121, 122, 65, 66, 67, 68, 69, 70, 71, 72, 
	73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 
	83, 84, 85, 86, 87, 88, 89, 90, 0, 31, 
	0, 3, 6, 0, 6, 7, 0, 73, 6, 0, 
	3, 9, 0, 3, 2, 0, 3, 0, 0, 0
};
static const cst_regex dottedabbrevs_rx = {
	0,
	1,
	NULL,
	0,
	151,
	(char *) dottedabbrevs_rxprog
};


//??
static const unsigned char itmoney_rxprog[] = {
	156, 
	6, 0, 72, 1, 0, 3, 8, 0, 5, 36, 
	0, 11, 0, 18, 4, 0, 0, 48, 49, 50, 
	51, 52, 53, 54, 55, 56, 57, 44, 0, 6, 
	0, 34, 21, 0, 3, 6, 0, 25, 8, 0, 
	5, 46, 0, 11, 0, 17, 4, 0, 0, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	31, 0, 6, 6, 0, 3, 9, 0, 3, 2, 
	0, 3, 0, 0, 0
};
static const cst_regex itmoney_rx = {
	0,
	1,
	NULL,
	0,
	76,
	(char *) itmoney_rxprog
};


static const unsigned char sevenphonenumber_rxprog[] = {
	156, 
	6, 0, 112, 1, 0, 3, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	4, 0, 14, 48, 49, 50, 51, 52, 53, 54, 
	55, 56, 57, 0, 4, 0, 14, 48, 49, 50, 
	51, 52, 53, 54, 55, 56, 57, 0, 8, 0, 
	5, 45, 0, 4, 0, 14, 48, 49, 50, 51, 
	52, 53, 54, 55, 56, 57, 0, 4, 0, 14, 
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 
	0, 4, 0, 14, 48, 49, 50, 51, 52, 53, 
	54, 55, 56, 57, 0, 4, 0, 14, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 0, 2, 
	0, 3, 0, 0, 0
};
static const cst_regex sevenphonenumber_rx = {
	0,
	1,
	NULL,
	0,
	116,
	(char *) sevenphonenumber_rxprog
};

static const unsigned char illion_rxprog[] = {
	156, 
	6, 0, 25, 1, 0, 3, 10, 0, 6, 3, 
	0, 0, 8, 0, 10, 105, 108, 108, 105, 111, 
	110, 0, 2, 0, 3, 0, 0, 0
};
static const cst_regex illion_rx = {
	0,
	1,
	NULL,
	0,
	29,
	(char *) illion_rxprog
};


static const unsigned char numess_rxprog[] = {
	156, 
	6, 0, 31, 1, 0, 3, 11, 0, 17, 4, 
	0, 0, 48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 0, 8, 0, 5, 115, 0, 2, 0, 
	3, 0, 0, 0
};
static const cst_regex numess_rx = {
	0,
	1,
	NULL,
	0,
	35,
	(char *) numess_rxprog
};

static const unsigned char ordinal_number_rxprog[] = {
	156, 
	6, 0, 119, 1, 0, 3, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	10, 0, 18, 4, 0, 0, 48, 49, 50, 51, 
	52, 53, 54, 55, 56, 57, 44, 0, 21, 0, 
	3, 6, 0, 9, 8, 0, 69, 116, 104, 0, 
	6, 0, 9, 8, 0, 60, 84, 72, 0, 6, 
	0, 9, 8, 0, 51, 115, 116, 0, 6, 0, 
	9, 8, 0, 42, 83, 84, 0, 6, 0, 9, 
	8, 0, 33, 110, 100, 0, 6, 0, 9, 8, 
	0, 24, 78, 68, 0, 6, 0, 9, 8, 0, 
	15, 114, 100, 0, 6, 0, 9, 8, 0, 6, 
	82, 68, 0, 31, 0, 3, 2, 0, 3, 0, 
	0, 0
};
static const cst_regex ordinal_number_rx = {
	0,
	1,
	NULL,
	0,
	123,
	(char *) ordinal_number_rxprog
};



static const unsigned char orario1_rxprog[] = {
	156, 
	6, 0, 64, 1, 0, 3, 10, 0, 9, 4, 
	0, 0, 48, 49, 0, 4, 0, 14, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 0, 8, 
	0, 5, 58, 0, 10, 0, 13, 4, 0, 0, 
	48, 49, 50, 51, 52, 53, 0, 4, 0, 14, 
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 
	0, 2, 0, 3, 0, 0, 0
};
static const cst_regex orario1_rx = {
	0,
	1,
	NULL,
	0,
	68,
	(char *) orario1_rxprog
};

static const unsigned char orario2_rxprog[] = {
	156, 
	6, 0, 54, 1, 0, 3, 8, 0, 5, 50, 
	0, 4, 0, 8, 48, 49, 50, 51, 0, 8, 
	0, 5, 58, 0, 10, 0, 13, 4, 0, 0, 
	48, 49, 50, 51, 52, 53, 0, 4, 0, 14, 
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 
	0, 2, 0, 3, 0, 0, 0
};
static const cst_regex orario2_rx = {
	0,
	1,
	NULL,
	0,
	58,
	(char *) orario2_rxprog
};

static const unsigned char data1_rxprog[] = {
	156, 
	6, 0, 127, 1, 0, 3, 10, 0, 10, 4, 
	0, 0, 48, 49, 50, 0, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	8, 0, 5, 47, 0, 10, 0, 8, 8, 0, 
	0, 48, 0, 4, 0, 14, 48, 49, 50, 51, 
	52, 53, 54, 55, 56, 57, 0, 8, 0, 5, 
	47, 0, 10, 0, 17, 4, 0, 0, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 0, 10, 
	0, 17, 4, 0, 0, 48, 49, 50, 51, 52, 
	53, 54, 55, 56, 57, 0, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	4, 0, 14, 48, 49, 50, 51, 52, 53, 54, 
	55, 56, 57, 0, 2, 0, 3, 0, 0, 0
};
static const cst_regex data1_rx = {
	0,
	1,
	NULL,
	0,
	131,
	(char *) data1_rxprog
};


static const unsigned char data2_rxprog[] = {
	156, 
	6, 0, 114, 1, 0, 3, 8, 0, 5, 51, 
	0, 4, 0, 6, 48, 49, 0, 8, 0, 5, 
	47, 0, 10, 0, 8, 8, 0, 0, 48, 0, 
	4, 0, 14, 48, 49, 50, 51, 52, 53, 54, 
	55, 56, 57, 0, 8, 0, 5, 47, 0, 10, 
	0, 17, 4, 0, 0, 48, 49, 50, 51, 52, 
	53, 54, 55, 56, 57, 0, 10, 0, 17, 4, 
	0, 0, 48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 0, 4, 0, 14, 48, 49, 50, 51, 
	52, 53, 54, 55, 56, 57, 0, 4, 0, 14, 
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 
	0, 2, 0, 3, 0, 0, 0
};
static const cst_regex data2_rx = {
	0,
	1,
	NULL,
	0,
	118,
	(char *) data2_rxprog
};


static const unsigned char data3_rxprog[] = {
	156, 
	6, 0, 113, 1, 0, 3, 10, 0, 10, 4, 
	0, 0, 48, 49, 50, 0, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	8, 0, 6, 47, 49, 0, 4, 0, 7, 48, 
	49, 50, 0, 8, 0, 5, 47, 0, 10, 0, 
	17, 4, 0, 0, 48, 49, 50, 51, 52, 53, 
	54, 55, 56, 57, 0, 10, 0, 17, 4, 0, 
	0, 48, 49, 50, 51, 52, 53, 54, 55, 56, 
	57, 0, 4, 0, 14, 48, 49, 50, 51, 52, 
	53, 54, 55, 56, 57, 0, 4, 0, 14, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 
	2, 0, 3, 0, 0, 0
};
static const cst_regex data3_rx = {
	0,
	1,
	NULL,
	0,
	117,
	(char *) data3_rxprog
};
static const unsigned char data4_rxprog[] = {
	156, 
	6, 0, 100, 1, 0, 3, 8, 0, 5, 51, 
	0, 4, 0, 6, 48, 49, 0, 8, 0, 6, 
	47, 49, 0, 4, 0, 7, 48, 49, 50, 0, 
	8, 0, 5, 47, 0, 10, 0, 17, 4, 0, 
	0, 48, 49, 50, 51, 52, 53, 54, 55, 56, 
	57, 0, 10, 0, 17, 4, 0, 0, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 0, 4, 
	0, 14, 48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 0, 4, 0, 14, 48, 49, 50, 51, 
	52, 53, 54, 55, 56, 57, 0, 2, 0, 3, 
	0, 0, 0
};
static const cst_regex data4_rx = {
	0,
	1,
	NULL,
	0,
	104,
	(char *) data4_rxprog
};

static const unsigned char data_duemila_e_rxprog[] = {
	156, 
	6, 0, 35, 1, 0, 3, 10, 0, 6, 3, 
	0, 0, 8, 0, 6, 47, 48, 0, 4, 0, 
	14, 48, 49, 50, 51, 52, 53, 54, 55, 56, 
	57, 0, 2, 0, 3, 0, 0, 0
};
static const cst_regex data_duemila_e_rx = {
	0,
	1,
	NULL,
	0,
	39,
	(char *) data_duemila_e_rxprog
};

static const unsigned char double_comma_rxprog[] = {
	156, 
	6, 0, 48, 1, 0, 3, 10, 0, 17, 4, 
	0, 0, 48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 0, 8, 0, 5, 44, 0, 10, 0, 
	17, 4, 0, 0, 48, 49, 50, 51, 52, 53, 
	54, 55, 56, 57, 0, 2, 0, 3, 0, 0, 
	0
};
static const cst_regex double_comma_rx = {
	0,
	1,
	NULL,
	0,
	52,
	(char *) double_comma_rxprog
};

static const unsigned char double_point_rxprog[] = {
	156, 
	6, 0, 48, 1, 0, 3, 10, 0, 17, 4, 
	0, 0, 48, 49, 50, 51, 52, 53, 54, 55, 
	56, 57, 0, 8, 0, 5, 46, 0, 10, 0, 
	17, 4, 0, 0, 48, 49, 50, 51, 52, 53, 
	54, 55, 56, 57, 0, 2, 0, 3, 0, 0, 
	0
};
static const cst_regex double_point_rx = {
	0,
	1,
	NULL,
	0,
	52,
	(char *) double_point_rxprog
};


static const unsigned char alpha_and_accentati_rxprog[] = {
	156, 
	6, 0, 122, 1, 0, 3, 21, 0, 3, 6, 
	0, 33, 4, 0, 92, 97, 98, 99, 100, 101, 
	102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 
	122, 0, 6, 0, 33, 4, 0, 59, 65, 66, 
	67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 
	77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 
	87, 88, 89, 90, 0, 6, 0, 29, 4, 0, 
	26, 225, 224, 233, 232, 237, 236, 243, 242, 250, 
	249, 193, 192, 201, 200, 205, 204, 211, 210, 218, 
	217, 39, 96, 0, 31, 0, 3, 6, 0, 6, 
	7, 0, 104, 6, 0, 3, 9, 0, 3, 2, 
	0, 3, 0, 0, 0
};
static const cst_regex alpha_and_accentati_rx = {
	0,
	1,
	NULL,
	0,
	126,
	(char *) alpha_and_accentati_rxprog
};



static const unsigned char novowel_rxprog[] = {
	156, 
	6, 0, 46, 1, 0, 3, 11, 0, 37, 5, 
	0, 0, 97, 101, 105, 111, 117, 65, 69, 73, 
	79, 85, 225, 224, 233, 232, 237, 236, 243, 242, 
	250, 249, 193, 192, 201, 200, 205, 204, 211, 210, 
	218, 217, 0, 2, 0, 3, 0, 0, 0
};
static const cst_regex novowel_rx = {
	0,
	1,
	NULL,
	0,
	50,
	(char *) novowel_rxprog
};

static const unsigned char numeri_trattino_rxprog[] = {
	156, 
	6, 0, 69, 1, 0, 3, 21, 0, 3, 6, 
	0, 25, 11, 0, 17, 4, 0, 0, 48, 49, 
	50, 51, 52, 53, 54, 55, 56, 57, 0, 8, 
	0, 5, 45, 0, 31, 0, 3, 6, 0, 6, 
	7, 0, 34, 6, 0, 3, 9, 0, 3, 11, 
	0, 17, 4, 0, 0, 48, 49, 50, 51, 52, 
	53, 54, 55, 56, 57, 0, 2, 0, 3, 0, 
	0, 0
};
static const cst_regex numeri_trattino_rx = {
	0,
	1,
	NULL,
	0,
	73,
	(char *) numeri_trattino_rxprog
};



const cst_regex *apostrofo = &apostrofo_rx; // ".+'.+" 
const cst_regex *apice_finale = &apice_finale_rx; // ".+'"


const cst_regex *fourdigits = &fourdigits_rx; 
const cst_regex *threedigits = &threedigits_rx;
const cst_regex *romannums = &romannums_rx;
const cst_regex *digitsslashdigits = &digitsslashdigits_rx;
const cst_regex *dottedabbrevs = &dottedabbrevs_rx;
const cst_regex *itmoney = &itmoney_rx;
const cst_regex *sevenphonenumber = &sevenphonenumber_rx;
const cst_regex *illion = &illion_rx;
const cst_regex *numess = &numess_rx;
const cst_regex *ordinal_number = &ordinal_number_rx; 

const cst_regex *orario1 = &orario1_rx;
const cst_regex *orario2 = &orario2_rx;
const cst_regex *data1 = &data1_rx;
const cst_regex *data2 = &data2_rx;
const cst_regex *data3 = &data3_rx;
const cst_regex *data4 = &data4_rx;
const cst_regex *data_duemila_e = &data_duemila_e_rx;

const cst_regex *double_comma = &double_comma_rx;
const cst_regex *double_point = &double_point_rx;
const cst_regex *alpha_and_accentati = &alpha_and_accentati_rx;
const cst_regex *novowel = &novowel_rx;
const cst_regex *numeri_trattino = &numeri_trattino_rx;


void it_text_init()
{
}

void it_text_deinit()
{
}

static int rex_like(const cst_item *t)
{
    /* returns 1 if this is in a king like context */
    char *pn = cst_downcase(ffeature_string(t,"p.name"));
    char *ppn = cst_downcase(ffeature_string(t,"p.p.name"));
    int v = 0;
    
    if (cst_streq(pn,"luigi") ||
	cst_streq(pn,"enrico") ||
	cst_streq(pn,"charles") ||
	cst_streq(pn,"carlo") ||
	cst_streq(pn,"filippo") ||
	cst_streq(pn,"giorgio") ||
	cst_streq(pn,"edoardo") ||
	cst_streq(pn,"william") ||
	cst_streq(pn,"riccardo") ||
	cst_streq(pn,"giovanni") ||
	cst_streq(pn,"paolo") ||
	cst_streq(pn,"pietro") ||
	cst_streq(pn,"nicholas") ||
	cst_streq(pn,"nicola") ||
	cst_streq(pn,"federico") ||
	cst_streq(pn,"james") ||
	cst_streq(pn,"alfonso") ||
	cst_streq(pn,"ivan") ||
	cst_streq(pn,"napoleone") ||
	cst_streq(pn,"leo") ||
	cst_streq(pn,"leone") ||
	cst_streq(pn,"gregory") ||
	cst_streq(pn,"gregorio") ||
	cst_streq(pn,"alessandro") ||
	cst_streq(pn,"pierre"))
	v = 1;
    else if (cst_streq(ppn,"re") ||
	     cst_streq(ppn,"zar") ||
	     cst_streq(ppn,"imperatore") ||
	     cst_streq(ppn,"ceasar") ||
	     cst_streq(ppn,"cesare") ||
	     cst_streq(ppn,"barone") ||
	     cst_streq(ppn,"sultano") ||
	     cst_streq(ppn,"conte"))
	v = 1;

    cst_free(pn);
    cst_free(ppn);
    return v;
}

static int rex_like_f(const cst_item *t)
{
    /* returns 1 if this is in a king like context */
    char *pn = cst_downcase(ffeature_string(t,"p.name"));
    char *ppn = cst_downcase(ffeature_string(t,"p.p.name"));
    int v = 0;
    
    if (cst_streq(pn,"caterina") ||
	cst_streq(pn,"alessandra") ||
	cst_streq(pn,"elisabetta") ||
	cst_streq(pn,"maria"))
	v = 1;
    else if (cst_streq(ppn,"regina") ||
	     cst_streq(ppn,"duchessa") ||
	     cst_streq(ppn,"zarina") ||
	     cst_streq(ppn,"imperatrice") ||
	     cst_streq(ppn,"baronessa") ||
	     cst_streq(ppn,"contessa"))
	v = 1;

    cst_free(pn);
    cst_free(ppn);
    return v;
}

static int section_like(const cst_item *t)
{
    /* returns 1 if this is in a king like context */
    char *pn = cst_downcase(ffeature_string(t,"p.name"));
    char *nn = cst_downcase(ffeature_string(t,"n.name"));
    int v = 0;

    if (cst_streq(pn,"capitolo") ||
	cst_streq(pn,"verso") ||
	cst_streq(pn,"atto") ||
	cst_streq(pn,"libro") ||
	cst_streq(pn,"volume") ||
	cst_streq(pn,"apollo") ||
	cst_streq(pn,"trek") || //star trek?
	cst_streq(pn,"fortran"))
	v = 1;
    else if (cst_streq(nn,"capitolo") ||
	cst_streq(nn,"verso") ||
	cst_streq(nn,"atto") ||
	cst_streq(nn,"libro") ||
	cst_streq(nn,"volume"))
	v = 1;
    
    cst_free(pn);
    cst_free(nn);
    return v;
}

static int section_like_f(const cst_item *t)
{
    /* returns 1 if this is in a king like context */
    char *pn = cst_downcase(ffeature_string(t,"p.name"));
    char *nn = cst_downcase(ffeature_string(t,"n.name"));
    int v = 0;

    if (cst_streq(pn,"sezione") || //II sezione 
	cst_streq(pn,"parte") ||
	cst_streq(pn,"frase") ||
	cst_streq(pn,"scena"))
		v = 1;
    else if (cst_streq(nn,"sezione") || // sezione II
	cst_streq(nn,"parte") ||
	cst_streq(nn,"frase") ||
	cst_streq(nn,"scena") ||
	cst_streq(nn,"guerra")) // noi diciamo II guerra mondiale non  guerra II
		v = 1;
    
    cst_free(pn);
    cst_free(nn);    
    return v;
}



cst_utterance *it_textanalysis(cst_utterance *u)
{
    if (!feat_present(u->features, "tokentowords_func")) {
      utt_set_feat(u, "tokentowords_func", itemfunc_val(it_tokentowords));
     
    }
    
    return default_textanalysis(u);
}

static cst_val *it_tokentowords_one(cst_item *token, const char *name);
cst_val *it_tokentowords(cst_item *token)
{
  return it_tokentowords_one(token, item_feat_string(token, "name"));
}

static cst_val *add_break(cst_val *l)
{  
    /* add feature (break 1) to last item in this list */
    const cst_val *i;
    cst_val *t;
    cst_features *f;

    for (i=l; val_cdr(i); i=val_cdr(i));

    if (i)  /* might be empty list */
    {
	f = new_features();
	feat_set_string(f,"break","1");
	t = cons_val(val_car(i),features_val(f));
	set_car((cst_val *)i,t);
    }

    return l;
}

static cst_val *it_tokentowords_one(cst_item *token, const char *name)
{ 
    /* Return list of words that expand token/name */
  char *p, *aaa, *bbb, *bbbccc, *ccc;
    int i,j;
    cst_val *r;//, *s;
    const char *nsw = "";

    //printf("token_name %s name %s\n",item_name(token),name); 

    /* FIXME: For SAPI and friends, any tokens with explicit
       pronunciations need to be passed through as-is.  This should be
       done in the interface code rather than here once the
       tokentowords hook is accessible. AWB: no, they should set the
       nsw feature and this function should deal with it (doesn't yet though)*/
    if (item_feat_present(token,"phones"))
	return cons_val(string_val(name),NULL);
       
    if (item_feat_present(token,"nsw"))
	nsw = item_feat_string(token,"nsw");

    /* REGOLA per l'inglese... INIZIO 
    if ((cst_streq("a",name) || cst_streq("A",name)) &&
        ((item_next(token) == 0) ||
         (!cst_streq(name,item_name(token))) ||
         (!cst_streq("",ffeature_string(token,"punc")))))
      {   // if A is a sub part of a token, then its ey not ah 
	r = cons_val(string_val("_a"),0);
	} */  //...FINE
    // else 
    
    // per ACRONIMI E ABBREVIAZIONI 
    if (cst_regex_match(dottedabbrevs,name))
      { 
	//printf("ACRONIMI E ABBREVIAZIONI \n");
      	aaa = cst_strdup(name);
	for (i=j=0; aaa[i]; i++)
	    if (aaa[i] != '.')
	    {
		aaa[j] = aaa[i];
		j++;
	    }
	aaa[j] = '\0';
	r = it_exp_letters(aaa);
	cst_free(aaa);
    }
    /* a noi non serve...
    else if (cst_regex_match(cst_rx_commaint,name))
    {   // 99,999,999 
      //printf("99,999,999 \n");
	aaa = cst_strdup(name);
	for (j=i=0; i < strlen(name); i++)
	    if (name[i] != ',')
	    {
		aaa[j] = name[i];
		j++;
	    }
	aaa[j] = '\0';
	r = it_exp_real(aaa);
	cst_free(aaa);
	}*/
    /* else if (cst_regex_match(sevenphonenumber,name))
    {   // 234-3434 telephone numbers 
      //printf("234-3434 telephone numbers\n");
	p=strchr(name,'-');
	aaa = cst_strdup(name);
	aaa[strlen(name)-strlen(p)] = '\0';
	bbb = cst_strdup(p+1);
	r = val_append(add_break(it_exp_digits(aaa)),
		       it_exp_digits(bbb));
	cst_free(aaa);
	cst_free(bbb);
	}*/
    else if 
     ((cst_regex_match(threedigits,name) &&
      ((!cst_regex_match(cst_rx_digits,ffeature_string(token,"p.name"))
	&& cst_regex_match(threedigits,ffeature_string(token,"n.name"))
	&& cst_regex_match(fourdigits,ffeature_string(token,"n.n.name"))) ||
       (cst_regex_match(sevenphonenumber,ffeature_string(token,"n.name"))) ||
       (!cst_regex_match(cst_rx_digits,ffeature_string(token,"p.p.name"))
	&& cst_regex_match(threedigits,ffeature_string(token,"p.name"))
	&& cst_regex_match(fourdigits,ffeature_string(token,"n.name"))))) ||
      (cst_regex_match(fourdigits,name) &&
       (!cst_regex_match(cst_rx_digits,ffeature_string(token,"n.name"))
	&& cst_regex_match(threedigits,ffeature_string(token,"p.name"))
	&& cst_regex_match(threedigits,ffeature_string(token,"p.p.name")))))
    {
	/* part of a telephone number */
      //printf("part of a telephone number \n");     
	if (cst_streq("",ffeature_string(token,"punc")))
	    item_set_string(token,"punc",",");
	r = add_break(it_exp_digits(name));
    }
    else if (cst_regex_match(orario1,name) || cst_regex_match(orario2,name)  )
//cst_regex_match(numbertime,name))
    { 
      //printf("numbertime\n");
        p=strchr(name,':');
	aaa = cst_strdup(name);
	aaa[strlen(name)-strlen(p)] = '\0';
	bbb = cst_strdup(p+1);

	r = it_exp_number(aaa);
       
	if (!cst_streq("00",bbb))
	  r = val_append(r,cons_val(string_val("e"),it_exp_number(bbb)));// 12 e 30  id
	// r = add_break(r); 

	cst_free(aaa);
	cst_free(bbb);
    }
    else if (cst_regex_match(data1,name) || cst_regex_match(data2,name) || cst_regex_match(data3,name) || cst_regex_match(data4,name) )
      //cst_regex_match(numbertime,name))
    { 
      //printf("data aaa/bbb/ccc\n");
      //r = it_exp_id(name);
      p=strchr(name,'/');
      aaa = cst_strdup(name);
      aaa[strlen(name)-strlen(p)] = '\0';
      bbbccc = cst_strdup(p+1);
      
      p=strchr(bbbccc,'/');
      bbb= cst_strdup(bbbccc);
      bbb[strlen(bbbccc)-strlen(p)] = '\0';
      ccc = cst_strdup(p+1);
	    
      r = it_exp_number(aaa);
      if ((strlen(ccc) == 2) && (ccc[0] == '0'))
	r = val_append(r,val_append(it_exp_month(bbb),cons_val(string_val("del"),cons_val(string_val("duemila"),it_exp_number(ccc)))));
      else
	r = val_append(r,val_append(it_exp_month(bbb),cons_val(string_val("del"),it_exp_number(ccc))));
	// r = add_break(r); 
	cst_free(aaa);
	cst_free(bbbccc);
	cst_free(ccc);
	cst_free(bbb);
	
    }
    else if (cst_regex_match(numeri_trattino,name))
    {   /* 999-999-999 etc */
      //printf("999-999-999 etc\n");
	bbb = cst_strdup(name);
	for (r=0,aaa=p=bbb; *p; p++)
	{
	    if (*p == '-')
	    {
		*p = '\0';
		r = val_append(val_reverse(add_break(it_exp_digits(aaa))),r);
		aaa = p+1;
	    }
	}
	r = val_append(val_reverse(add_break(it_exp_digits(aaa))),r);
	r = val_reverse(r);
	cst_free(bbb);
    }
    else if (cst_regex_match(cst_rx_digits,name))
    {  
      /* string of digits (use cart to disambiguate) */
      //printf("string of digits (use cart to disambiguate)\n");
	if (cst_streq("nide",nsw))
	    r = it_exp_id(name);
	else 
	  r = it_exp_number(name); //mio default number altrimenmti quello sotto guarda us_text.c   

	/* fabio start: tolto num identification pronunciation
	   {
	    const cst_val *tv;
	    const char *ts;
	    char *rname;
	    
	    rname = cst_strdup(item_feat_string(token,"name"));
	    if (cst_streq(name,rname))
		tv = cart_interpret(token,&it_nums_cart);
	    else 
	    {   // in a recursive call 
		item_set_string(token,"name",name);
		tv = cart_interpret(token,&it_nums_cart);
		item_set_string(token,"name",rname);
	    }
	    cst_free(rname);
	    ts = val_string(tv);
	    if (cst_streq(ts,"ordinal"))
		r = it_exp_ordinal(name);
	    else if (cst_streq(ts,"digits"))
		r = it_exp_digits(name);
	    else if (cst_streq(ts,"year"))
		r = it_exp_id(name);
	    else
		r = it_exp_number(name);
	}   //fabio:end*/ 
    }
    else if (cst_regex_match(romannums,name))
    {   /* Romain numerals */
      //printf("Romain numerals\n");
      //if (cst_streq("",ffeature_string(token,"p.punc")) || (!item_prev(token)))
      //{   /* no preceeding punc */
	    char n[10];
	    sprintf(n,"%d",it_exp_roman(name));
	    if (rex_like(token) || section_like(token))
	      r = it_exp_ordinal(n);
	    else if (rex_like_f(token) || section_like_f(token))
	      r = it_exp_ordinal_f(n);
	    
	    //inglese se è section like en_exp_number
	    else
		r = it_exp_letters(name);
	    //	}
	    //	else
	    // r = it_exp_letters(name);
    }
    /* inglese-americano specifico
    else if (cst_regex_match(drst,name))  
    { 
      // St Andrew's St, Dr King Dr 
      //printf(" St Andrew's St, Dr King Dr\n");
	const char *street;
	const char *saint;
	if ((name[0] == 's') || (name[0] == 'S'))
	{
	    street = "street";
	    saint = "saint";
	}
	else
	{
	    street = "drive";
	    saint = "doctor";
	}
	if ((item_next(token) == 0) ||
	    strchr(item_feat_string(token,"punc"),','))
	    r = cons_val(string_val(street),NULL);
	else if (strchr(ffeature_string(token,"punc"),','))
	    r = cons_val(string_val(saint),NULL);
	else 
	{
	    const char *pname = ffeature_string(token,"p.name");
	    const char *nname = ffeature_string(token,"n.name");
	    if ((pname[0] >= 'A') && (pname[0] <= 'Z') &&
		(nname[0] >= 'a') && (nname[0] <= 'z'))
		r = cons_val(string_val(street),NULL);
	    else if ((pname[0] >= '0') && (pname[0] <= '9') &&
		     (nname[0] >= 'a') && (nname[0] <= 'z'))
		r = cons_val(string_val(street),NULL);
	    else if ((pname[0] >= 'a') && (pname[0] <= 'z') &&
		     (nname[0] >= 'A') && (nname[0] <= 'Z'))
		r = cons_val(string_val(saint),NULL);
	    else if (cst_streq(ffeature_string(token,"n.whitespace")," "))
		r = cons_val(string_val(saint),NULL);
	    else
		r = cons_val(string_val(street),NULL);
	}
	if (cst_streq(item_feat_string(token,"punc"),"."))
	    item_set_string(token,"punc","");
    }*/
    else if (cst_streq(name,"Mr"))
    {
	item_set_string(token,"punc","");
	r = cons_val(string_val("mister"),NULL);
    }
    else if (cst_streq(name,"Mrs"))
    {
	item_set_string(token,"punc","");
	r = cons_val(string_val("missus"),NULL);
    }
    else if ((strlen(name) == 1) &&
	     (name[0] >= 'A') &&
	     (name[0] <= 'Z') &&
	     (cst_streq(" ",ffeature_string(token,"n.whitespace"))) &&
	     (ffeature_string(token,"n.name")[0] >= 'A') &&
	     (ffeature_string(token,"n.name")[0] <= 'Z'))
    {
      //printf("Pronuncia 1 carattere ...\n");
	item_set_string(token,"punc","");
	aaa = word_lts_rewrites_word(cst_downcase(name), &italian_downcase_lts_rewrites); //cst_downcase(name);//fabio per codifica caratteri strani e accenti 
	/* regola inglese
	if (cst_streq(aaa,"a"))
	    r = cons_val(string_val("_a"),0);
	else
	    r = cons_val(string_val(aaa),0);
	*/
	r = cons_val(string_val(aaa),0);
	cst_free(aaa);
    }
    else if (cst_regex_match(double_comma,name) || cst_regex_match(double_point,name)  )
    {   /* real numbers */
      //printf("real number\n");
	r = it_exp_real(name);
    }
    else if (cst_regex_match(ordinal_number,name))
    {   /* explicit ordinals */
      //printf("explicit ordinals\n");
	aaa = cst_strdup(name);
	aaa[strlen(name)-2] = '\0';
	r = it_exp_ordinal(aaa);
	cst_free(aaa);
    }
    else if ((cst_regex_match(illion,name)) &&
	     (cst_regex_match(itmoney,ffeature_string(token,"p.name"))))
    {
      //printf("illion\n");
	r = cons_val(string_val(name),
		     cons_val(string_val("dollars"),NULL));
    }
    else if (cst_regex_match(itmoney,name)) // $123
    {
	/* IT money */
	//printf("money, money, money %s\n", name); 
	p = strchr(name,'.');

	if (cst_regex_match(illion,ffeature_string(token,"n.name")))
	{   /* carl sagan's billions and billions */
	    r = it_exp_real(&name[1]);
	}
	else if (!p)
	{
	    aaa = cst_strdup(&name[1]);
	    if (cst_streq("1",aaa))
		r = cons_val(string_val("dollar"),NULL);
	    else
		r = cons_val(string_val("dollars"),NULL);
	    r = val_append(it_tokentowords_one(token,aaa),r);
	    cst_free(aaa);
	}
	else if ((strlen(p) == 1) || (strlen(p) > 3))
	{   /* simply read as mumble point mumble */
	    r = val_append(it_exp_real(&name[1]),
			   cons_val(string_val("dollars"),NULL));
	}
	else
	{
	    aaa = cst_strdup(name);
	    aaa[strlen(name)-strlen(p)] = '\0';
	    for (i=j=0; aaa[i] != '\0'; i++)
	    {
		if (aaa[i] != ',')
		{
		    aaa[j] = aaa[i];
		    j++;
		}
	    }
	    aaa[j] = '\0';
	    if (cst_streq("00",p+1))
		r = 0;
	    else if (cst_streq("01",p+1))
		r = val_append(it_exp_number(p+1),
			       cons_val(string_val("cent"),NULL));
	    else
		r = val_append(it_exp_number(p+1),
			       cons_val(string_val("cents"),NULL));

	    if (cst_streq("1",aaa+1))
		r = cons_val(string_val("dollar"),r);
	    else
		r = cons_val(string_val("dollars"),r);

	    r = val_append(it_exp_number(aaa+1),r);
	    cst_free(aaa);
	}
    }
    else if (name[strlen(name)-1] == '%') 
      { 
	//printf("per cento\n");
	aaa = cst_strdup(name);
	aaa[strlen(aaa)-1] = '\0';
	r = val_append(it_tokentowords_one(token,aaa),
		       cons_val(string_val("per"),
				cons_val(string_val("cento"),NULL)));
	cst_free(aaa);

    }
    else if (cst_regex_match(numess,name)) 
    {   /* 60s and 7s and 9s */
      //printf("60s and 7s and 9s\n"); 
	aaa = cst_strdup(name);
	aaa[strlen(name)-1] = '\0';
	r = val_append(it_exp_number(aaa),
		       cons_val(string_val("'s"),0));
	cst_free(aaa);
    }

    /* tolto altrimenti mi splitta qui apostrofi e accenti!
       else if ((p=(cst_strrchr(name,'\''))))
    {
      //printf("cioò ghe nè n'apostrofo? %s \n", name);
	static const char *pc[] = { "'s", "'ll", "'ve", "'d", NULL };

	bbb = word_lts_rewrites_word(cst_downcase(p), &italian_downcase_lts_rewrites);//cst_downcase(p);//fabio per codifica caratteri strani e accenti 
	if (cst_member_string(bbb, pc))
	{
	  //printf("1\n");
	    aaa = cst_strdup(name);
	    aaa[strlen(name)-strlen(p)] = '\0';
	    r = val_append(it_tokentowords_one(token,aaa),
			   cons_val(string_val(bbb),0));
	    cst_free(aaa);
	}
	else if (cst_streq(p,"'tve")) // admittedly rare and weird
	{ //printf("2\n");
	    aaa = cst_strdup(name);
	    aaa[strlen(name)-strlen(p)+2] = '\0';
	    r = val_append(it_tokentowords_one(token,aaa),
			   cons_val(string_val("'ve"),0));
	    cst_free(aaa);
	}
	else
	{ //printf("3\n");
	    aaa = cst_strdup(name);
	    strcpy(&aaa[strlen(name)-strlen(p)],p+1);
	    r = it_tokentowords_one(token,aaa);
	    cst_free(aaa);
	}
	cst_free(bbb);
    }
    */

    else if ((cst_regex_match(digitsslashdigits,name)) &&
	     (cst_streq(name,item_name(token))))
    {   /* might be fraction, or not */
      //printf("might be fraction, or not\n");
	p=strchr(name,'/');
	aaa = cst_strdup(name);
	aaa[strlen(name)-strlen(p)] = '\0';
	bbb = cst_strdup(p+1);
	if ((cst_streq("1",aaa)) && (cst_streq("2",bbb)))
	    r = cons_val(string_val("un"),
			 cons_val(string_val("mezzo"),0));
	else if (atoi(aaa) < (atoi(bbb)))
	{
	   
	    if (atoi(aaa) > 1)
	       r = val_append(it_exp_number(aaa),it_exp_ordinal_p(bbb));
	    else
	       r = val_append(it_exp_number(aaa),it_exp_ordinal(bbb));
	   	   
	}
	else
	    r = val_append(it_exp_number(aaa),
			   cons_val(string_val("fratto"), 
				    it_exp_number(bbb)));

	if ((cst_regex_match(cst_rx_digits,ffeature_string(token,"p.name")))
	    && (item_prev(token)))  /* don't mistake "0" as a number */
	    r = cons_val(string_val("and"),r);
	cst_free(aaa);
	cst_free(bbb);
    }
    /* else if ((p=(strchr(name,'-'))))
    {   // aaa-bbb 
      //printf("aaa-bbb\n");
	aaa = cst_strdup(name);
	aaa[strlen(name)-strlen(p)] = '\0';
	bbb = cst_strdup(p+1);
	if (cst_regex_match(cst_rx_digits,aaa) &&
	    cst_regex_match(cst_rx_digits,bbb))
	{
	    item_set_string(token,"name",bbb);
	    r = it_tokentowords_one(token,bbb);
	    item_set_string(token,"name",aaa);
	    r = val_append(it_tokentowords_one(token,aaa),
			   cons_val(string_val("to"),r));
	    item_set_string(token,"name",name);
	}
	else
	    r = val_append(it_tokentowords_one(token,aaa),
			   it_tokentowords_one(token,bbb));
	cst_free(aaa);
	cst_free(bbb);
    }*/
    /*fabio tolto altimentoi mi slpitta perch - é ma dovrei forse inserire in (cst_rx_alpha e poi...
    //probbilmente cst_rx_alpha non contine i caratteri accentati!!!
 else if ((strlen(name) > 1) && (!cst_regex_match(cst_rx_alpha,name)))
   {   //its not just alphas 
     //printf("its not just alphas \n");
	for (i=0; name[i] != '\0'; i++)
	    if (text_splitable(name,i))
		break;
	aaa = cst_strdup(name);
	aaa[i+1] = '\0';
	bbb = cst_strdup(&name[i+1]);
	item_set_string(token,"nsw","nide");
	r = val_append(it_tokentowords_one(token,aaa),
		       it_tokentowords_one(token,bbb));
	cst_free(aaa);
	cst_free(bbb);
	}*/
    /* stati americani cosa me ne faccio?
    else if ((s = state_name(name,token)))
    {
      //printf("state name\n");
	r = s;
	}*/
    else if ((cst_regex_match(novowel,name)) || ((strlen(name) > 1) && (!cst_regex_match(alpha_and_accentati,name)))) 
      {	// Need common exception list 
	// unpronouncable list of alphas 
	//printf("unpronouncable\n");
	r = it_exp_letters(name);
    // buckets of other stuff missing 
      }
    else  // just a word 
    {
       //printf("just a word\n");
      aaa = word_lts_rewrites_word(cst_downcase(name), &italian_downcase_lts_rewrites);//cst_downcase(name);//fabio per codifica caratteri strani e accenti 
	r = cons_val(string_val(aaa),0);
	cst_free(aaa);
    }

    return r;
}

/*
static int text_splitable(const char *s,int i)
{
//should token be split abter this 

    if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",s[i]) &&
	strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",s[i+1]))
	return FALSE;
    else if (strchr("0123456789",s[i]) &&
	     strchr("0123456789",s[i+1]))
	return FALSE;
    else
	return TRUE;
}
*/


/* stati americani che me ne faccio?

static const char *states[99][5] =
{
  { "AL", "ambiguous", "alabama" , NULL, NULL },
  { "Al", "ambiguous", "alabama" , NULL, NULL },
  { "Ala", "", "alabama" , NULL, NULL },
  { "AK", "", "alaska" , NULL, NULL },
  { "Ak", "", "alaska" , NULL, NULL },
  { "AZ", "", "arizona" , NULL, NULL },
  { "Az", "", "arizona" , NULL, NULL },
  { "CA", "", "california" , NULL, NULL },
  { "Ca", "", "california" , NULL, NULL },
  { "Cal", "ambiguous", "california" , NULL, NULL },
  { "Calif", "", "california" , NULL, NULL },
  { "CO", "ambiguous", "colorado" , NULL, NULL },
  { "Co", "ambiguous", "colorado" , NULL, NULL },
  { "Colo", "", "colorado" , NULL, NULL },
  { "DC", "", "d" , "c", NULL },
  { "DE", "", "delaware" , NULL, NULL },
  { "De", "ambiguous", "delaware" , NULL, NULL },
  { "Del", "ambiguous", "delaware" , NULL, NULL },
  { "FL", "", "florida" , NULL, NULL },
  { "Fl", "ambiguous", "florida" , NULL, NULL },
  { "Fla", "", "florida" , NULL, NULL },
  { "GA", "", "georgia" , NULL, NULL },
  { "Ga", "", "georgia" , NULL, NULL },
  { "HI", "", "hawaii" , NULL, NULL },
  { "Hi", "ambiguous", "hawaii" , NULL, NULL },
  { "IA", "", "indiana" , NULL, NULL },
  { "Ia", "ambiguous", "indiana" , NULL, NULL },
  { "Ind", "ambiguous", "indiana" , NULL, NULL },
  { "ID", "ambiguous", "idaho" , NULL, NULL },
  { "IL", "ambiguous", "illinois" , NULL, NULL },
  { "Il", "ambiguous", "illinois" , NULL, NULL },
  { "ILL", "ambiguous", "illinois" , NULL, NULL },
  { "KS", "", "kansas" , NULL, NULL },
  { "Ks", "", "kansas" , NULL, NULL },
  { "Kans", "", "kansas" , NULL, NULL },
  { "KY", "ambiguous", "kentucky" , NULL, NULL },
  { "Ky", "ambiguous", "kentucky" , NULL, NULL },
  { "LA", "ambiguous", "louisiana" , NULL, NULL },
  { "La", "ambiguous", "louisiana" , NULL, NULL },
  { "Lou", "ambiguous", "louisiana" , NULL, NULL },
  { "Lous", "ambiguous", "louisiana" , NULL, NULL },
  { "MA", "ambiguous", "massachusetts" , NULL, NULL },
  { "Mass", "ambiguous", "massachusetts" , NULL, NULL },
  { "Ma", "ambiguous", "massachusetts" , NULL, NULL },
  { "MD", "ambiguous", "maryland" , NULL, NULL },
  { "Md", "ambiguous", "maryland" , NULL, NULL },
  { "ME", "ambiguous", "maine" , NULL, NULL },
  { "Me", "ambiguous", "maine" , NULL, NULL },
  { "MI", "", "michigan" , NULL, NULL },
  { "Mi", "ambiguous", "michigan" , NULL, NULL },
  { "Mich", "ambiguous", "michigan" , NULL, NULL },
  { "MN", "ambiguous", "minnestota" , NULL, NULL },
  { "Minn", "ambiguous", "minnestota" , NULL, NULL },
  { "MS", "ambiguous", "mississippi" , NULL, NULL },
  { "Miss", "ambiguous", "mississippi" , NULL, NULL },
  { "MT", "ambiguous", "montanna" , NULL, NULL },
  { "Mt", "ambiguous", "montanna" , NULL, NULL },
  { "MO", "ambiguous", "missouri" , NULL, NULL },
  { "Mo", "ambiguous", "missouri" , NULL, NULL },
  { "NC", "ambiguous", "north" , "carolina", NULL },
  { "ND", "ambiguous", "north" , "dakota", NULL },
  { "NE", "ambiguous", "nebraska" , NULL, NULL },
  { "Ne", "ambiguous", "nebraska" , NULL, NULL },
  { "Neb", "ambiguous", "nebraska" , NULL, NULL },
  { "NH", "ambiguous", "new" , "hampshire", NULL },
  { "NV", "", "nevada" , NULL, NULL },
  { "Nev", "", "nevada" , NULL, NULL },
  { "NY", "", "new" , "york", NULL },
  { "OH", "ambiguous", "ohio" , NULL, NULL },
  { "OK", "ambiguous", "oklahoma" , NULL, NULL },
  { "Okla", "", "oklahoma" , NULL, NULL },
  { "OR", "ambiguous", "oregon" , NULL, NULL },
  { "Or", "ambiguous", "oregon" , NULL, NULL },
  { "Ore", "ambiguous", "oregon" , NULL, NULL },
  { "PA", "ambiguous", "pennsylvania" , NULL, NULL },
  { "Pa", "ambiguous", "pennsylvania" , NULL, NULL },
  { "Penn", "ambiguous", "pennsylvania" , NULL, NULL },
  { "RI", "ambiguous", "rhode" , "island", NULL },
  { "SC", "ambiguous", "south" , "carlolina", NULL },
  { "SD", "ambiguous", "south" , "dakota", NULL },
  { "TN", "ambiguous", "tennesee" , NULL, NULL },
  { "Tn", "ambiguous", "tennesee" , NULL, NULL },
  { "Tenn", "ambiguous", "tennesee" , NULL, NULL },
  { "TX", "ambiguous", "texas" , NULL, NULL },
  { "Tx", "ambiguous", "texas" , NULL, NULL },
  { "Tex", "ambiguous", "texas" , NULL, NULL },
  { "UT", "ambiguous", "utah" , NULL, NULL },
  { "VA", "ambiguous", "virginia" , NULL, NULL },
  { "WA", "ambiguous", "washington" , NULL, NULL },
  { "Wa", "ambiguous", "washington" , NULL, NULL },
  { "Wash", "ambiguous", "washington" , NULL, NULL },
  { "WI", "ambiguous", "wisconsin" , NULL, NULL },
  { "Wi", "ambiguous", "wisconsin" , NULL, NULL },
  { "WV", "ambiguous", "west" , "virginia", NULL },
  { "WY", "ambiguous", "wyoming" , NULL, NULL },
  { "Wy", "ambiguous", "wyoming" , NULL, NULL },
  { "Wyo", "", "wyoming" , NULL, NULL },
  { "PR", "ambiguous", "puerto" , "rico", NULL },
  { NULL, NULL, "puerto" , "rico", NULL }
};

static cst_val *state_name(const char *name,cst_item *t)
{
    int s,j;
    int do_it = 0;
    cst_val *r = 0;

    for (s=0; states[s][0]; s++)
    {
	if (cst_streq(states[s][0],name))
	{
	    if (cst_streq(states[s][1],"ambiguous"))
	    {
		const char *pname = ffeature_string(t,"p.name");
		const char *nname = ffeature_string(t,"n.name");
		// previous name is capitalized 
		if (((strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ",pname[0])) &&
		     (strlen(pname) > 2) &&
		     (cst_regex_match(cst_rx_alpha,pname))) &&
		    ((strchr("abcdefghijklmnopqrstuvwxyz",nname[0])) ||
		     (item_next(t) == 0) ||
		     (cst_streq(".",item_feat_string(t,"punc"))) ||
		     (((strlen(nname) == 5 || (strlen(nname) == 10)) &&
		       cst_regex_match(cst_rx_digits,nname)))))
		    do_it = 1;
		else
		    do_it = 0;
	    }
	    else
		do_it = 1;

	    if (do_it)
	    {
		for (j=2; states[s][j]; j++)
		    r = cons_val(string_val(states[s][j]),r);
		return val_reverse(r);
	    }
	}
    }
    return r;

}



*/
