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
/*  Italian text expanders                                               */
/*                                                                       */
/*  numbers, digits, ids (years), money                                  */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "it_text.h"

static const char * const digit2num[] = {
    "zero",
    "uno",
    "due",
    "tre",
    "quattro",
    "cinque",
    "sei",
    "sette",
    "otto",
    "nove" };

static const char * const digit2teen[] = {
    "dieci",  /* shouldn't get called */
    "undici",
    "dodici",
    "tredici",
    "quattordici",
    "quindici",
    "sedici",
    "diciassette",
    "diciotto",
    "diciannove" };

static const char * const digit2enty[] = {
    "zero",  /* shouldn't get called */
    "dieci",
    "venti",
    "trenta",
    "quaranta",
    "cinquanta",
    "sessanta",
    "settanta",
    "ottanta",
    "novanta" };

static const char * const ord2num1digit[] = {
    "zeresimo",
    "primo",
    "secondo",
    "terzo",
    "quarto",
    "quinto",
    "sesto",
    "settimo",
    "ottavo",
    "nono" };

static const char * const ord2num[] = {
    "",
    "unesimo",
    "duesimo",
    "treesimo",
    "quattresimo",
    "cinquesimo",
    "seiesimo",
    "settesimo",
    "ottesimo",
    "novesimo" };


static const char * const ord2teen[] = {
    "decimo",  /* shouldn't get called */
    "undicesimo",
    "dodicesimo",
    "tredicesimo",
    "quattordicesimo",
    "quindicesimo",
    "sedicesimo",
    "diciassettesimo",
    "diciottosimo",
    "diciannovesimo" };

static const char * const ord2enty[] = {
    "zeresimo",  /* shouldn't get called */
    "decimo",
    "ventesimo",
    "trentesimo",
    "quarantesimo",
    "cinquantesimo",
    "sessantesimo",
    "settantesimo",
    "ottantesimo",
    "novantesimo" };

//ordinali femminili


static const char * const ord2num1digit_f[] = {
    "zeresima",
    "prima",
    "seconda",
    "terza",
    "quarta",
    "quinta",
    "sesta",
    "settima",
    "ottava",
    "nona" };

static const char * const ord2num_f[] = {
    "",
    "unesima",
    "duesima",
    "treesima",
    "quattresima",
    "cinquesima",
    "seiesima",
    "settesima",
    "ottesima",
    "novesima" };


static const char * const ord2teen_f[] = {
    "decima",  /* shouldn't get called */
    "undicesima",
    "dodicesima",
    "tredicesima",
    "quattordicesima",
    "quindicesima",
    "sedicesima",
    "diciassettesima",
    "diciottosima",
    "diciannovesima" };

static const char * const ord2enty_f[] = {
    "zeresima",  /* shouldn't get called */
    "decima",
    "ventesima",
    "trentesima",
    "quarantesima",
    "cinquantesima",
    "sessantesima",
    "settantesima",
    "ottantesima",
    "novantesima" };

//ordinali plurali


static const char * const ord2num1digit_p[] = {
    "zeresimi",
    "primi",
    "secondi",
    "terzi",
    "quarti",
    "quinti",
    "sesti",
    "settimi",
    "ottavi",
    "noni" };

static const char * const ord2num_p[] = {
    "",
    "unesimi",
    "duesimi",
    "treesimi",
    "quattresimi",
    "cinquesimi",
    "seiesimi",
    "settesimi",
    "ottesimi",
    "novesimi" };


static const char * const ord2teen_p[] = {
    "decimi", 
    "undicesimi",
    "dodicesimi",
    "tredicesimi",
    "quattordicesimi",
    "quindicesimi",
    "sedicesimi",
    "diciassettesimi",
    "diciottosimi",
    "diciannovesimi" };

static const char * const ord2enty_p[] = {
    "zeresimi",  /* shouldn't get called */
    "decimi",
    "ventesimi",
    "trentesimi",
    "quarantesimi",
    "cinquantesimi",
    "sessantesimi",
    "settantesimi",
    "ottantesimi",
    "novantesimi" };

//----------------

static const char * const month[] = {
    "gennaio", 
    "febbraio",
    "marzo",
    "aprile",
    "maggio",
    "giungno",
    "luglio",
    "agosto",
    "settembre",
    "ottobre",
    "novembre",
    "dicembre" };


cst_val *it_exp_number(const char *numstring)
{
    /* Expand given token to list of words pronouncing it as a number */
    int num_digits = strlen(numstring);
    char part[4];
    cst_val *p;
    int i;

    if (num_digits == 0)
	return NULL;
    else if (num_digits == 1)
	return it_exp_digits(numstring);
    else if (num_digits == 2)
    {
	if (numstring[0] == '0')
	{
	    if (numstring[1] == '0')
		return 0;
	    else
		return cons_val(string_val(digit2num[numstring[1]-'0']),0);
	}
	else if (numstring[1] == '0')
	    return cons_val(string_val(digit2enty[numstring[0]-'0']),0);
	else if (numstring[0] == '1')
	    return cons_val(string_val(digit2teen[numstring[1]-'0']),0);
	else 
	    return cons_val(string_val(digit2enty[numstring[0]-'0']),
			    it_exp_digits(numstring+1));
    }
    else if (num_digits == 3)
    {
	if (numstring[0] == '0')
	    return it_exp_number(numstring+1);
	else if(numstring[0] == '1')
	  return cons_val(string_val("cento"),
				   it_exp_number(numstring+1));
	else
	  return cons_val(string_val(digit2num[numstring[0]-'0']),
			  cons_val(string_val("cento"),
				   it_exp_number(numstring+1)));
    }
    
    else if (num_digits == 4)
    {
	if (numstring[0] == '0')
	    return it_exp_number(numstring+1);
	else if(numstring[0] == '1')
	  return cons_val(string_val("mille"),
				   it_exp_number(numstring+1));
	else
	  return cons_val(string_val(digit2num[numstring[0]-'0']),
			  cons_val(string_val("mila"),
				   it_exp_number(numstring+1)));
    }


    else if (num_digits < 7)
    {
	for (i=0; i < num_digits-3; i++)
	    part[i] = numstring[i];
	part[i]='\0';
	p = it_exp_number(part);
	if (p == 0)  /* no thousands */
	    return it_exp_number(numstring+i);
	else
	    return val_append(p,cons_val(string_val("mila"),
					 it_exp_number(numstring+i)));
    }

    else if (num_digits == 7)
    {
	if (numstring[0] == '0')
	    return it_exp_number(numstring+1);
	else if(numstring[0] == '1')
	  return cons_val(string_val("un milione"),
				   it_exp_number(numstring+1));
	else
	  return cons_val(string_val(digit2num[numstring[0]-'0']),
			  cons_val(string_val("milioni"),
				   it_exp_number(numstring+1)));
    }
    else if (num_digits < 10)
    {
	for (i=0; i < num_digits-6; i++)
	    part[i] = numstring[i];
	part[i]='\0';
	p = it_exp_number(part);
	if (p == 0)  /* no millions */
	    return it_exp_number(numstring+i);
	else
	    return val_append(p,cons_val(string_val("milioni"),
					 it_exp_number(numstring+i)));
    }

    else if (num_digits == 10)
    {
	if (numstring[0] == '0')
	    return it_exp_number(numstring+1);
	else if(numstring[0] == '1')
	  return cons_val(string_val("un bilione"),
				   it_exp_number(numstring+1));
	else
	  return cons_val(string_val(digit2num[numstring[0]-'0']),
			  cons_val(string_val("bilioni"),
				   it_exp_number(numstring+1)));
    }


    else if (num_digits < 13)
    {   /* If there are pendantic brits out there, tough! */
	for (i=0; i < num_digits-9; i++)
	    part[i] = numstring[i];
	part[i]='\0';
	p = it_exp_number(part);
	if (p == 0)  /* no billions */
	    return it_exp_number(numstring+i);
	else
	    return val_append(p,cons_val(string_val("bilioni"),
					 it_exp_number(numstring+i)));
    }
    else  /* Way too many digits here, to be a number */
    {
	return it_exp_digits(numstring);
    }
}


cst_val *it_exp_ordinal(const char *rawnumstring)
{   // return ordinal for digit string 
    // default maschile primo, secondo...
    cst_val *card, *o;
    const cst_val *t;
    const char *l;
    const char *ord;
    char *numstring;
    int i,j;

    int num_digits = strlen(rawnumstring);
   
    numstring = cst_strdup(rawnumstring);
    for (j=i=0; i < strlen(rawnumstring); i++)
	if (rawnumstring[i] != ',')
	{
	    numstring[j] = rawnumstring[i];
	    j++;
	}
    numstring[j] = '\0';
    card = val_reverse(it_exp_number(numstring));
    cst_free(numstring);

    l = val_string(val_car(card));
    ord = 0;
    //printf("num_dig: %d\n",num_digits);
    if (num_digits == 1)
      for (i=0; i<10; i++)
	if (cst_streq(l,digit2num[i]))
	  ord = ord2num1digit[i];
    if (!ord)
      for (i=0; i<10; i++)
	if (cst_streq(l,digit2num[i]))
	  ord = ord2num[i];
    if (!ord)
	for (i=0; i<10; i++)
	    if (cst_streq(l,digit2teen[i]))
		ord = ord2teen[i];
    if (!ord)
	for (i=0; i<10; i++)
	    if (cst_streq(l,digit2enty[i]))
		ord = ord2enty[i];
    if (cst_streq(l,"cento"))
	ord = "centesimo";
    if (cst_streq(l,"mille"))
	ord = "millesimo";
    if (cst_streq(l,"billione"))
	ord = "bilionesimo";
    if (!ord)  /* dunno, so don't convert anything */
	return card;
    o = cons_val(string_val(ord),0);
    for (t=val_cdr(card); t; t=val_cdr(t))
	o = cons_val(val_car(t),o);
    delete_val(card);
    return o;
}


cst_val *it_exp_ordinal_f(const char *rawnumstring)
{   // return ordinal for digit string 
    // femminile prima, seconda...
    cst_val *card, *o;
    const cst_val *t;
    const char *l;
    const char *ord;
    char *numstring;
    int i,j;

    int num_digits = strlen(rawnumstring);
   
    numstring = cst_strdup(rawnumstring);
    for (j=i=0; i < strlen(rawnumstring); i++)
	if (rawnumstring[i] != ',')
	{
	    numstring[j] = rawnumstring[i];
	    j++;
	}
    numstring[j] = '\0';
    card = val_reverse(it_exp_number(numstring));
    cst_free(numstring);

    l = val_string(val_car(card));
    ord = 0;
    //printf("num_dig: %d\n",num_digits);
    if (num_digits == 1)
      for (i=0; i<10; i++)
	if (cst_streq(l,digit2num[i]))
	  ord = ord2num1digit_f[i];
    if (!ord)
      for (i=0; i<10; i++)
	if (cst_streq(l,digit2num[i]))
	  ord = ord2num_f[i];
    if (!ord)
	for (i=0; i<10; i++)
	    if (cst_streq(l,digit2teen[i]))
		ord = ord2teen_f[i];
    if (!ord)
	for (i=0; i<10; i++)
	    if (cst_streq(l,digit2enty[i]))
		ord = ord2enty_f[i];
    if (cst_streq(l,"cento"))
	ord = "centesima";
    if (cst_streq(l,"mille"))
	ord = "millesima";
    if (cst_streq(l,"billione"))
	ord = "bilionesima";
    if (!ord)  /* dunno, so don't convert anything */
	return card;
    o = cons_val(string_val(ord),0);
    for (t=val_cdr(card); t; t=val_cdr(t))
	o = cons_val(val_car(t),o);
    delete_val(card);
    return o;
}

cst_val *it_exp_ordinal_p(const char *rawnumstring)
{   // return ordinal for digit string 
    // plurale primi, secondi...
    cst_val *card, *o;
    const cst_val *t;
    const char *l;
    const char *ord;
    char *numstring;
    int i,j;

    int num_digits = strlen(rawnumstring);
   
    numstring = cst_strdup(rawnumstring);
    for (j=i=0; i < strlen(rawnumstring); i++)
	if (rawnumstring[i] != ',')
	{
	    numstring[j] = rawnumstring[i];
	    j++;
	}
    numstring[j] = '\0';
    card = val_reverse(it_exp_number(numstring));
    cst_free(numstring);

    l = val_string(val_car(card));
    ord = 0;
    //printf("num_dig: %d\n",num_digits);
    if (num_digits == 1)
      for (i=0; i<10; i++)
	if (cst_streq(l,digit2num[i]))
	  ord = ord2num1digit_p[i];
    if (!ord)
      for (i=0; i<10; i++)
	if (cst_streq(l,digit2num[i]))
	  ord = ord2num_p[i];
    if (!ord)
	for (i=0; i<10; i++)
	    if (cst_streq(l,digit2teen[i]))
		ord = ord2teen_p[i];
    if (!ord)
	for (i=0; i<10; i++)
	    if (cst_streq(l,digit2enty[i]))
		ord = ord2enty_p[i];
    if (cst_streq(l,"cento"))
	ord = "centesimi";
    if (cst_streq(l,"mille"))
	ord = "millesimi";
    if (cst_streq(l,"billione"))
	ord = "bilionesimi";
    if (!ord)  /* dunno, so don't convert anything */
	return card;
    o = cons_val(string_val(ord),0);
    for (t=val_cdr(card); t; t=val_cdr(t))
	o = cons_val(val_car(t),o);
    delete_val(card);
    return o;
}

cst_val *it_exp_month(const char *numstring)
{
  // Expand numstring as month number
  if ((strlen(numstring) == 2) && (numstring[0] == '0'))
    return cons_val(string_val(month[atoi(numstring+1)-1]),0); //char2int? /////
  else 
    return cons_val(string_val(month[atoi(numstring)-1]),0);
  //return cons_val(string_val("gennembre"),0);
}


// a noi serve veramete? no
cst_val *it_exp_id(const char *numstring)
{
    /* Expand numstring as pairs as in years or ids */
    char aaa[3];

    if ((strlen(numstring) == 4) && 
	(numstring[2] == '0') &&
	(numstring[3] == '0'))
    {
	if (numstring[1] == '0')
	    return it_exp_number(numstring); /* 2000, 3000 */
	else
	{
	    aaa[0] = numstring[0];
	    aaa[1] = numstring[1];
	    aaa[2] = '\0';
	    return val_append(it_exp_number(aaa),
			      cons_val(string_val("mila"),0));
	}
    }
    else if ((strlen(numstring) == 2) && (numstring[0] == '0'))
	return 	it_exp_digits(&numstring[1]);
    else if (((strlen(numstring) == 4) && 
	 ((numstring[1] == '0'))) ||
	(strlen(numstring) < 3))
	return it_exp_number(numstring);
    else if (strlen(numstring)%2 == 1)
    {
	return cons_val(string_val(digit2num[numstring[0]-'0']),
			it_exp_id(&numstring[1]));
    }
    else 
    {
	aaa[0] = numstring[0];
	aaa[1] = numstring[1];
	aaa[2] = '\0';
	return val_append(it_exp_number(aaa),it_exp_id(&numstring[2]));
    }
}

cst_val *it_exp_real(const char *numstring)
{
    char *aaa, *p;
    cst_val *r;

    if (numstring && (numstring[0] == '-'))
	r = cons_val(string_val("meno"),
		     it_exp_real(&numstring[1]));
    else if (numstring && (numstring[0] == '+'))
	r = cons_val(string_val("più"),
		     it_exp_real(&numstring[1]));
    else if (((p=strchr(numstring,'e')) != 0) ||
	     ((p=strchr(numstring,'E')) != 0))
    {
	aaa = cst_strdup(numstring);
	aaa[strlen(numstring)-strlen(p)] = '\0';
	r = val_append(it_exp_real(aaa),
		       cons_val(string_val("e"),
				it_exp_real(p+1)));
	cst_free(aaa);
    }
    else if ((p=strchr(numstring,'.')) != 0)
    {
	aaa = cst_strdup(numstring);
	aaa[strlen(numstring)-strlen(p)] = '\0';
	r = val_append(it_exp_number(aaa),
		       cons_val(string_val("punto"),
				it_exp_digits(p+1)));
	cst_free(aaa);
    }
    else if ((p=strchr(numstring,',')) != 0)
    {
	aaa = cst_strdup(numstring);
	aaa[strlen(numstring)-strlen(p)] = '\0';
	r = val_append(it_exp_number(aaa),
		       cons_val(string_val("virgola"),
				//it_exp_digits(p+1)));
				it_exp_number(p+1)));
	cst_free(aaa);
    }


    else
	r = it_exp_number(numstring);  /* I don't think you can get here */

    return r;
}

cst_val *it_exp_digits(const char *numstring)
{
    /* Expand given token to list of words pronouncing it as digits */
    cst_val *d = 0;
    const char *p;

    for (p=numstring; *p; p++)
    {
	if ((*p >= '0') && (*p <= '9'))
	    d = cons_val(string_val(digit2num[*p-'0']),d);
	else
	    d = cons_val(string_val("umpty"),d);
    }

    return val_reverse(d);
}

cst_val *it_exp_letters(const char *lets)
{
    /* returns these as list of single char symbols */
    char *aaa;
    cst_val *r;
    int i;

    aaa = cst_alloc(char,2);
    aaa[1] = '\0';
    for (r=0,i=0; lets[i] != '\0'; i++)
    {
	aaa[0] = lets[i];
	if (isupper((int)aaa[0])) 
	    aaa[0] = tolower((int)aaa[0]);
	if (strchr("0123456789",aaa[0]))
	    r = cons_val(string_val(digit2num[aaa[0]-'0']),r);
	//else if (cst_streq(aaa,"a")) regola per l'inglese-americano 
 	//    r = cons_val(string_val("_a"),r); 
	else 
	  // {
	    // printf("%s\n",aaa);
	    r = cons_val(string_val(aaa),r);
	// }
    }
    cst_free(aaa);

    return val_reverse(r);
}

int it_exp_roman(const char *roman)
{
    int val;
    const char *p;
    val = 0;

    for (p=roman; *p != 0; p++)
    {
	if (*p == 'X')
	    val += 10;
	else if (*p == 'V')
	    val += 5;
	else if (*p == 'I')
	{
	    if (p[1] == 'V')
	    {
		val += 4;
		p++;
	    }
	    else if (p[1] == 'X')
	    {
		val += 9;
		p++;
	    }
	    else 
		val += 1;
	}
    }
    return val;
}


