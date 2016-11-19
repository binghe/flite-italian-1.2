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
/*   F0 models:                                                          */
/*  1)Dummy F0 modelling for phones, copied directly from us_f0_model.c  */
/*  2)Cart model                                                         */
/*  3)Painte Model                                                       */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>
#include "cst_hrg.h"
#include "cst_phoneset.h"
#include "it_f0.h"
#include "cst_cart.h"
//x painte
#include <math.h>

//---------------- Dummy F0 modelling for phones -------------------------------
cst_utterance *it_f0_model(cst_utterance *u)
{  
  /* F0 target model */
    cst_item *s,*t;
    cst_relation *targ_rel;
    float mean, stddev;

    targ_rel = utt_relation_create(u,"Target");
    mean = get_param_float(u->features,"target_f0_mean", 100.0);
    mean *= get_param_float(u->features,"f0_shift", 1.0);
    stddev = get_param_float(u->features,"target_f0_stddev", 12.0);

    s=relation_head(utt_relation(u,"Segment"));
    t = relation_append(targ_rel,NULL);
    item_set_float(t,"pos",0.0);
    item_set_float(t,"f0",mean+stddev);

    s=relation_tail(utt_relation(u,"Segment"));
    t = relation_append(targ_rel,NULL);

    item_set_float(t,"pos",item_feat_float(s,"end"));
    item_set_float(t,"f0",mean-stddev);

    return u;
}

//---------------- General functions -------------------------------------------------------
static void add_target_point(cst_relation *targ,float pos, float f0)
{
    cst_item *t;

    //printf("target %f at %f\n",f0,pos); 
    t = relation_append(targ,NULL);
    item_set_float(t,"pos",pos);
    /* them there can sometimes do silly things, so guard for that */
    if (f0 > 500.0)
	item_set_float(t,"f0",500.0);
    else if (f0 < 50.0)
	item_set_float(t,"f0",50.0);
    else
	item_set_float(t,"f0",f0);
}

static int post_break(cst_item *syl)
{
    if ((item_prev(syl) == 0) ||
	(cst_streq("pau",
		   ffeature_string(syl,
				   "R:SylStructure.daughter.R:Segment.p.name"))))
	return TRUE;
    else
	return FALSE;
}

static int pre_break(cst_item *syl)
{
    if ((item_next(syl) == 0) ||
	(cst_streq("pau",
		   ffeature_string(syl,
				   "R:SylStructure.daughtern.R:Segment.n.name"))))
	return TRUE;
    else
	return FALSE;
}

static float vowel_mid(cst_item *syl)
{
    /* return time point mid way in vowel in this syl */
    cst_item *s;
    cst_item *ts;
    const cst_phoneset *ps = item_phoneset(syl);

    ts = item_daughter(item_as(syl,"SylStructure"));
    for (s=ts; s; s = item_next(s))
    {
	if (cst_streq("+", phone_feature_string(ps,item_feat_string(s,"name"),
						"vc")))
	{
	    return (item_feat_float(s,"end")+
		    ffeature_float(s,"R:Segment.p.end"))/2.0;
	}
    }

    /* no segments, shouldn't happen */
    if (ts == 0)
	return 0;

    /* no vowel in syllable, shouldn't happen */
    return (item_feat_float(ts,"end")+
	    ffeature_float(ts,"R:Segment.p.end"))/2.0;
}



/* model mean and stddev take from f2b/kal_diphone */
//da mettere quello di ds
#define model_mean 170.0
#define model_stddev 34
#define map_f0(v,m,s) ((((v-model_mean)/model_stddev)*s)+m)
#define z_map_f0(v,m,s) ((v*s)+m)

//------------------------ Cart model -------------------------------------------
cst_utterance *it_f0_model_cart(cst_utterance *u)
{
  /* Dopo il modulo di predizione del pitch ci deve essere il modulo di mappatura dei difoni non esistenti.*/
  /*F0 model 
;;; Using CART models rather than LR models generate target points
;;; in a way similar to the way that Int_Targets_LR does
;;;
;;; To use this train three CART trees that predict start, mid and
;;; end values F0 (cf. the features syl_startpitch, syl_midpitch and
;;; syl_endpitch).  Then in your voice definition 
;;;;;;;;;;;;;;;;;;;;;;;;IFD;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;;; Inoltre utilizza la tecnica Zscore
;;;  
;;; (set! F0start_tree_z f2b_F0start_tree_z)
;;; (set! F0mid_tree_z f2b_F0mid_tree_z)
;;; (set! F0end_tree_z f2b_F0end_tree_z)
;;; (set! int_params_z
;;; 	'((target_f0_mean 110) (target_f0_std 15) ;;(110 15)
;;; 	  (model_f0_mean 143.763992) (model_f0_std 35.481682))) ;;(143.763992 35.481682)
;;; (Parameter.set 'Int_Target_Method Int_Targets_Tree_Z)
  */
  
  cst_cart *f0_tree_start;
  cst_cart *f0_tree_mid;
  cst_cart *f0_tree_end;
  
  f0_tree_start = val_cart(feat_val(u->features,"f0_start_cart"));
  f0_tree_mid = val_cart(feat_val(u->features,"f0_mid_cart"));
  f0_tree_end = val_cart(feat_val(u->features,"f0_end_cart"));
   
  cst_item *syl, *t, *nt;
  cst_relation *targ_rel;
  float mean, stddev, local_mean, local_stddev;
  float start, mid, end, lend;
  float seg_end;
  
  targ_rel = utt_relation_create(u,"Target");
  mean = get_param_float(u->features,"int_f0_target_mean", 100.0);
  mean *= get_param_float(u->features,"f0_shift", 1.0);
  stddev = get_param_float(u->features,"int_f0_target_stddev", 12.0);
  
  lend = 0;
  for (syl=relation_head(utt_relation(u,"Syllable"));
       syl;
       syl = item_next(syl))
    
    {
      if (!item_daughter(item_as(syl,"SylStructure")))
	continue;  /* no segs in syl */
      
      local_mean = ffeature_float(syl,"R:SylStructure.parent.R:Token.parent.local_f0_shift");
      if (local_mean)
	local_mean *= mean;
      else
	local_mean = mean;
      local_stddev = ffeature_float(syl,"R:SylStructure.parent.R:Token.parent.local_f0_range");
      if (local_stddev == 0.0)
	local_stddev = stddev;
      
      start=val_float(cart_interpret(syl,f0_tree_start));
      //printf("start %f\n", start);
      mid=val_float(cart_interpret(syl,f0_tree_mid));	
      //printf("mid %f\n", mid);
      
      if (post_break(syl))
	lend = start; //z_map_f0(start,local_mean,local_stddev);
      add_target_point(targ_rel,
		       ffeature_float(syl,
				      "R:SylStructure.daughter.R:Segment.p.end"),
		       z_map_f0((start+lend)/2.0,local_mean,local_stddev));
      
      //printf("start map %f\n", z_map_f0((start+lend)/2.0,local_mean,local_stddev));
      
      add_target_point(targ_rel,
		       vowel_mid(syl),
		       z_map_f0(mid,local_mean,local_stddev));
      //printf("mid map %f\n",z_map_f0(mid,local_mean,local_stddev));
      lend = end; //z_map_f0(end,local_mean,local_stddev);
      if (pre_break(syl))
	{
	  end=val_float(cart_interpret(syl,f0_tree_end));
	  //printf("end %f\n", end);
	  add_target_point(targ_rel,
			   ffeature_float(syl,"R:SylStructure.daughtern.end"),
			   z_map_f0(end,local_mean,local_stddev));
	  //printf("end map %f\n", z_map_f0(end,local_mean,local_stddev));
	}
    }
  
  //printf(".... Guarantee targets go from start to end of utterance\n");
  /* Guarantee targets go from start to end of utterance */
  t = relation_head(targ_rel);
  if (t == 0)
    add_target_point(targ_rel,0,mean);
  else if (item_feat_float(t,"pos") > 0)
    {
      nt = item_prepend(t,NULL);
      item_set_float(nt,"pos",0.0);
      item_set_float(nt,"f0",item_feat_float(t,"f0"));
    }
  
  t = relation_tail(targ_rel);
  seg_end = item_feat_float(relation_tail(utt_relation(u,"Segment")),"end");
  if (item_feat_float(t,"pos") < seg_end)
    add_target_point(targ_rel,seg_end,item_feat_float(t,"f0"));
  
    return u;
}


//----------------------- PaIntE modules ------------------------------------//
// ---- Painte = [a1 a2 b c1 c2 d] ----- //
/* Parametri da settare a livello di lingua e voce :

in italian.c:
f0 painte model
cart:
    feat_set(v->features,"f0_model_func",uttfunc_val(&main_painte_to_f0));
    feat_set(v->features,"painte_CW_NUM_cart",cart_val(&painte_CW_NUM_cart));
lista:
    feat_set(v->features,"PaIntE_file_CB",userdata_val(&PaIntE_file_CB));

in irstifd_ita_pc.c:
feat_set_float(v->features,"int_f0_target_mean",100);//110.0);
feat_set_float(v->features,"default_pr_base",60);//110.0);
feat_set_float(v->features,"default_pr_top",350);//110.0);
*/

void set_painte(cst_item *item, const float *params, int pr_norm)
{ 
  //se pr_norm=1 si suppone che i valori in params siano normalizzati
  //se pr_norm=0 si suppone che i valori in params non siano normalizzati
  item_set_float(item,"a1",params[0]);
  item_set_float(item,"a2",params[1]);
  item_set_float(item,"b" ,params[2]);
  if (pr_norm) {
    item_set_float(item,"c1_prnorm",params[3]);
    item_set_float(item,"c2_prnorm",params[4]);
    item_set_float(item,"d_prnorm" ,params[5]);
  } else {
    item_set_float(item,"c1",params[3]);
    item_set_float(item,"c2",params[4]);
    item_set_float(item,"d" ,params[5]);
  }
}

void set_painte_utt(cst_utterance *u, cst_item *s, const float *params, int pr_norm)
{ 
  //se pr_norm=1 si suppone che i valori in params siano normalizzati
  //se pr_norm=0 si suppone che i valori in params non siano normalizzati
 
  cst_item *item;
  int is_new_item = 0;
  // create relations if necessary
  if (!utt_relation_present(u,"PaIntE")) {
    //printf("crea la relazione painte\n");
    utt_relation_create(u,"PaIntE");
  }
  
  if (!utt_relation_present(u,"ParamIntonation"))
    utt_relation_create(u,"ParamIntonation");
  // add new item to relation if necessary  
  //è necessaria?
  if (!item_as(s,"ParamIntonation"))
    relation_append(utt_relation(u,"ParamIntonation"),s);  
  
  // create a new PaIntE item and init it if necessary
  if (item_daughter(item_as(s,"ParamIntonation")) == 0) 
    {
      item = relation_append(utt_relation(u,"PaIntE"),NULL);
      item_set_float(item,"end",ffeature_float(s,"R:SylStructure.daughtern.end"));
      is_new_item = 1;
    }
  else {
    item = item_daughter(item_as(s,"ParamIntonation"));
  }
  set_painte(item, params, pr_norm);
  // add item as daughter to relation Intonation
  if (is_new_item)
    item_add_daughter(item_as(s,"ParamIntonation"),item);
 }

cst_utterance *painte_pr_normdenorm_events(cst_utterance *u, int norm) {
  // questa funzione sarà da vedere se voglio usare i semitoni...
  // norm=1: passa da denormalizzate a normalizzate
  // norm=0: passa da normalizzate a denormalizzate

  float *painte;
  painte = (float *) malloc(sizeof(float) * size_painte_vec);
  cst_item *s, *p;
  float top, base;
  int i;
 //fab:inserito per evitare il blocco quando non ha predetto nessun CB PaIntE
 if (!utt_relation_present(u,"PaIntE")) {
    cst_errmsg("No PaIntE events found in utterance.\n"); 
    return u;
 } else {
  
  for (i=0, p=relation_head(utt_relation(u,"PaIntE")); p; p = item_next(p)) 
    {  
      // get the appropriate syllable
      s = item_as(item_parent(item_as(p,"ParamIntonation")),"Syllable");
      // get painte event
      painte = get_painte(p, norm, painte);
      // try to get pitch range from Phrase relation 
      if (item_feat_present(s,"R:SylStructure.parent.R:Phrase.parent.pr_top"))
	top = item_feat_float(s,"R:SylStructure.parent.R:Phrase.parent.pr_top");
      if (item_feat_present(s,"R:SylStructure.parent.R:Phrase.parent.pr_base"))
	base = item_feat_float(s,"R:SylStructure.parent.R:Phrase.parent.pr_base");
      // if not defined in Phrase relation: default pitch range
      if (! (int)(top))  
	top  = get_param_float(u->features,"default_pr_top",150);
      if (! (int)(base)) 
	base = get_param_float(u->features,"default_pr_base",100);
       // normalization
      
      if (!norm) {
	painte[3] = painte[3] / (top-base);
	painte[4] = painte[4] / (top-base);
	painte[5] = (painte[5]-base) / (top-base);
	set_painte_utt(u, s, painte, 1);
      }
      // denormalization
      else {
	painte[3] = painte[3] * (top-base);
	painte[4] = painte[4] * (top-base);
	painte[5] = painte[5] * (top-base) + base;
	set_painte_utt(u, s, painte, 0);
      }      
    }
  free(painte);
  return u;
 }
}

//-----------------------
void add_pillar(TimeMap *tm, float val1, float val2)
{
  // sort the new value into the matix
  // as long as second value is in order
  int i;
  for (i=tm->num_rows; i>=0;i--) {
    if (i==0 || tm->a_0[i-1]<=val1){ 
      if ((i>0 && tm->a_1[i-1]>= val2) || (i<tm->num_rows && tm->a_1[i+1]<= val2)) {
	cst_errmsg("TimeMap::add_pillar: y-val of new pillar (%f , %f) not in order.\n", val1, val2);
	return;
      }
      tm->a_0[i] = val1;
      tm->a_1[i] = val2;
      break;
    }
    else if  (tm->a_0[i-1]> val1){ // shift
      tm->a_0[i] = tm->a_0[i-1];
      tm->a_1[i] = tm->a_1[i-1];
    }
    else {
      cst_errmsg("TimeMap::add_pillar: You cannot enter 2 pillars (%f , %f) with the same time.\n" ,val1,val2);
    }
  }
  //alla fine aggiungo: 
  tm->num_rows=tm->num_rows+1;
}

float invmap(TimeMap *tm, const float val)
{
  int n=tm->num_rows;
  if (n<2) 
    cst_errmsg("TimeMap::map: Need 2 or more pillars to map.\n");

  if (val < tm->a_1[0]-eps)
     cst_errmsg("Warning! TimeMap::map: Value %f is smaller than first pillar %f\n",val, tm->a_1[0]);
    
  float range1, range2;
  int i;
  for (i=0; i<n-1;i++) {
    if (val <= tm->a_1[i+1]) {
      range1 = tm->a_0[i+1] - tm->a_0[i];
      range2 = tm->a_1[i+1] - tm->a_1[i];
      
      if (range2 == 0) return tm->a_0[i]; // 2 pillars can be identical
      return tm->a_0[i]+(val - tm->a_1[i]) * range1/range2;
    }
  }
  if (val > tm->a_1[n-1]+eps)
    cst_errmsg("Warning! TimeMap::map: Value %f is larger than last pillar%f\n", val, tm->a_1[n-1]);
  
  range1 = tm->a_0[n] - tm->a_0[n-1];
  range2 = tm->a_1[n] - tm->a_1[n-1];

  if (range2 == 0) return tm->a_0[n-1]; // 2 pillars can be identical
  return tm->a_0[n-1]+(val - tm->a_1[n-1]) * range1/range2;
}

static float rise_sigmoid(float a, float b, float x) { 
  return 1/(1+exp(-a*(b-x)+overlap));}

static float fall_sigmoid(float a, float b, float x) { 
  return 1/(1+exp(-a*(x-b)+overlap));}

/* sumof2sigmoid approximation function with 6 parameters */
static float approx_pfun_func(float p[], float x)
{
  float a1=p[0], a2=p[1], b=p[2], c1=p[3], c2=p[4], d=p[5];
  return d - c1*rise_sigmoid(a1,b,x) - c2*fall_sigmoid(a2,b,x);
}


cst_utterance *painte_CW_NUM_syl(cst_utterance *u) 
{ 
  //Predice il CW_NUM PaIntE usando PaIntE_CW_NUM_tree
  cst_item *s; 
  cst_cart *painte_CW_NUM_cart;
  painte_CW_NUM_cart = val_cart(feat_val(u->features,"painte_CW_NUM_cart"));
  for(s=relation_head(utt_relation(u,"Syllable")); s; s=item_next(s)) { 
    //fab solo non s
    if (!cst_streq(ffeature_string(s,"R:SylStructure.Accent_boundary_syl"),"s"))
      item_set_int(s,"CW_NUM",val_int(cart_interpret(s,painte_CW_NUM_cart)));
    //printf("Predizione cart:%d\n",val_int(cart_interpret(s,painte_CW_NUM_cart)));
  }
  return u;
}

cst_val *list_elem_n(cst_val *list, int n)
{  
  if (n == 0) 
    return val_car(list);
  else 
    return list_elem_n(val_cdr(list), n-1);
}

float *get_param_from_val(cst_val *list, float *p) 
{  
  int i;
  for (i=0;i<6;i++)
    { 
      p[i]=val_float(val_car(list));
      list=val_cdr(list);
    }
  return p;
}

cst_utterance *set_painte_CW_NUM_syl(cst_utterance *u)
{
  //Setta i parametri PaIntE CW in tutte le sillabe
  float *p;
  p = (float *) malloc(sizeof(float) * size_painte_vec);
  cst_item *s;
  cst_val *PaIntE_file_CB;
  PaIntE_file_CB = val_userdata(feat_val(u->features,"PaIntE_file_CB"));
  int codeword_num;
  
  for(s=relation_head(utt_relation(u,"Syllable")); s; s=item_next(s)) 
    {
      if (!cst_streq(ffeature_string(s,"R:SylStructure.Accent_boundary_syl"),"s")) {	
	codeword_num = item_feat_int(s,"CW_NUM");
	if (codeword_num  != 0) {
	  p=get_param_from_val(list_elem_n(val_car(PaIntE_file_CB), codeword_num-1),p);
	  //printf("PAINTE:\n"); 
	  //val_print(stdout,list_elem_n(val_car(PaIntE_file_CB), codeword_num-1)); printf("\n"); 
	  set_painte_utt(u, s, p, 0); //0 perchè in p normalizzati //ATTENZIONE nel vecchio era il contrario!
	}
      }
    }
  free(p);
  return u;
}

cst_utterance *main_painte_to_f0(cst_utterance *u) 
{
  /* printf("painte_CW_NUM_syl\n");
  u=painte_CW_NUM_syl(u);
  printf("set_painte_CW_NUM_syl\n");
  u=set_painte_CW_NUM_syl(u);
  printf("painte_pr_normdenorm_events\n");
  u=painte_pr_normdenorm_events(u,1);
  printf("painte_to_f0\n");
  u=painte_to_f0(u);
  print_target_relation:\n");
  print_target_relation(u);*/

 
  //return painte_to_f0(painte_pr_normdenorm_events(set_painte_CW_NUM_syl(painte_CW_NUM_syl(u)),1));

  //come le ultime voci utilizza i mel"
  
  /*printf("painte_CW_NUM_syl\n");
  u=painte_CW_NUM_syl(u);
  printf("set_painte_CW_NUM_syl\n");
  u=set_painte_CW_NUM_syl(u);
  //printf("painte_pr_normdenorm_events\n");
  //u=painte_pr_normdenorm_events(u,1);
  printf("painte_to_f0\n");
  u=painte_to_f0(u);
  printf("print_target_relation:\n");
  print_target_relation(u);
  printf("mel2frq_target:\n");
  u=mel2frq_target(u);
  printf("print_target_relation:\n");
  print_target_relation(u);*/
  

  return mel2frq_target(painte_to_f0(set_painte_CW_NUM_syl(painte_CW_NUM_syl(u))));
 
  //return u;
}

void print_target_relation(cst_utterance *u)
{
  cst_item *t;
  for (t=relation_head(utt_relation(u,"Target")); t; t=item_next(t))
   printf("Target: %f %f\n",item_feat_float(t,"pos"),item_feat_float(t,"f0"));
}

cst_utterance *painte_to_f0(cst_utterance *u) {
  cst_item *p, *syl;
  float *intparams;
  intparams = (float*) malloc(sizeof (float)*size_painte_vec);
  TimeMap *tm;
  tm = (TimeMap*) malloc(sizeof(TimeMap));
  float win_start, win_end, win_start_n, win_end_n; 
  char *silence = get_param_string(u->features,"silence", "#");
  if (!utt_relation_present(u,"PaIntE")) {
    cst_errmsg("No PaIntE events found in utterance.\n"); 
    return u;
  } else {
    for (p=relation_head(utt_relation(u,"PaIntE")); p; p = item_next(p) ) {
    syl = item_as(item_parent(item_as(p,"ParamIntonation")),"Syllable");
    intparams = get_painte(p, 0, intparams);// 0 stà per non normalizzati in frequenza sono hz.
    // the size of the reconstruction window (without tobi labels)
    define_window_from_syl(syl, &win_start_n, &win_end_n); //, &method);
    //costruisce TimeMap
    tm = construct_timemap_from_syl(syl,tm);
    invert(tm); // for DE-normalization we need inverted map
    // translate into the non-normalized world
    win_start = map(tm,win_start_n);
     
    win_end   = map(tm,win_end_n);
    // generate a f0 painte from the parameters (might be time-normaized)
    // genera painte e add_target 3 punti per segmento che ha painte 
    
    //sillaba precedente:
    if (win_start_n < 0) 
      insert_f0_in_segment_in_a_sillable(item_prev(syl), intparams, tm, win_start, silence);
    //sillaba corrente:
    insert_f0_in_segment_in_a_sillable(syl, intparams, tm, win_start, silence);
    //sillaba successiva:
    if (win_end_n > 1) 
      insert_f0_in_segment_in_a_sillable(item_next(syl), intparams, tm, win_start, silence);
  } 
  //  fine loop sugli eventi
  free(tm);
  free(intparams);
  //interpolation and smoothing
  //printf("iterpolation_and_smoothing(u)\n");
  return iterpolation_and_smoothing(u);
  }
}

// PaInte=[a1 a2 b c1 c2 d]
float *get_painte(cst_item *painte, int pr_norm, float *p) 
{
  p[0]=item_feat_float(painte,"a1");
  p[1]=item_feat_float(painte,"a2");
  p[2]=item_feat_float(painte,"b");
  if (pr_norm) 
    {
      p[3]=item_feat_float(painte,"c1_prnorm");
      p[4]=item_feat_float(painte,"c2_prnorm");
      p[5]=item_feat_float(painte,"d_prnorm");
    }
  else 
    {
      p[3]=item_feat_float(painte,"c1");
      p[4]=item_feat_float(painte,"c2");
      p[5]=item_feat_float(painte,"d");
    }  
  return p;
}

/*
La seguente definizione della finestra: va contro la mia tecnica che invece è di 
sovraposizione degli effetti,la rivredò quando farò il modello additivo. 
Ma tuttavia è buona, per la funzione di dominanza sull'accento.
*/

void define_window_from_syl(cst_item *s, float *win_start, float *win_end)   
{
  if (item_prev(s) && (ffeature_int(s, "p.syl_break") < 2 )) { 
    if (!ffeature_int(s,"p.accented"))
      *win_start = -1.0;      // begin with prev syl
    else
      *win_start = -0.5;      // mid prev syl (less overlapping of accents) 
  }
  else 
    *win_start   = 0.0;       // begin with this syl
  if (item_next(s) &&  (ffeature_int(s, "syl_break") < 2 )) {   
    if (!ffeature_int(s,"n.accented")) 
      *win_end   = 2.0;       // to end next syl
    else
      *win_end   = 1.5;       // to mid next syl
  }
  else
    *win_end     = 1.0;       // to end this syl
}

TimeMap *construct_timemap_from_syl(cst_item *s,TimeMap *tm)
{
  tm->num_rows=0;
  if (item_prev(s)) 
    add_pillar(tm, ffeature_float(s,"p.R:SylStructure.daughter.R:Segment.p.end"), -1);
  add_pillar(tm, ffeature_float(s,"R:SylStructure.daughter.R:Segment.p.end"), 0);
  add_pillar(tm, ffeature_float(s,"R:SylStructure.daughtern.end"), 1);
  if (item_next(s))
    add_pillar(tm, ffeature_float(s,"n.R:SylStructure.daughtern.end"), 2);
  return tm;
}

void invert(TimeMap *tm) {
  int n = tm->num_rows;
  float val;
  int i;
  for(i=0; i<n; i++) {
    val = tm->a_0[i];
    tm->a_0[i] = tm->a_1[i];
    tm->a_1[i] = val;
  }
}

float map(TimeMap *tm, const float val)
{
  int n=tm->num_rows;
  if (n<2) 
    cst_errmsg("TimeMap::map: Need 2 or more pillars to map.\n");

  if (val < tm->a_0[0]-eps)
     cst_errmsg("Warning! TimeMap::map: Value %f is smaller than first pillar %f\n",val, tm->a_0[0]);

  float range1, range2;
  int i;
  for (i=0; i<n-1;i++) {
    if (val <= tm->a_0[i+1]) {
      range1 = tm->a_0[i+1]-tm->a_0[i];
      range2 = tm->a_1[i+1]-tm->a_1[i];
      if (range1 == 0) return tm->a_1[i]; // 2 pillars can be identical
      return tm->a_1[i]+(val-tm->a_0[i]) * range2/range1;
    }
  }

  if (val > tm->a_0[n-1]+eps)
     cst_errmsg("Warning! TimeMap::map: Value %f is larger than last pillar%f\n",val,tm->a_0[n-1]);
  range1 = tm->a_0[n]-tm->a_0[n-1];
  range2 = tm->a_1[n]-tm->a_1[n-1];
  if (range1 == 0) return tm->a_1[n-1]; // 2 pillars can be identical
  return tm->a_1[n-1]+(val-tm->a_0[n-1]) * range2/range1;
}

void insert_f0_in_segment_in_a_sillable(cst_item *syl, const float *painte, TimeMap *tm, float win_start, char *silence) {
  float start, mid, end, f0;
  syl=item_as(syl,"SylStructure");
  cst_item *seg;
  int num_overlap;
  for(seg=item_daughter(syl); seg ; seg=item_next(seg))
    { 
      start = segment_start(item_as(seg,"Segment"));
      mid = segment_mid(item_as(seg,"Segment"));
      end = item_feat_float(item_as(seg,"Segment"),"end");
      //printf("start: %f, mid: %f, end: %f, win_start%f\n",start,mid,end,win_start); 
      // all'inizio del segment: 
      if (((item_prev(item_as(seg,"Segment")) == 0) || (cst_streq(item_name(item_prev(item_as(seg,"Segment"))),silence))) && (win_start <= start))
	{	 
	  f0=approx_pfun_func(painte, invmap(tm, start)); 
	  if (item_feat_present(seg,"f0_s"))
	    {  //c'è già un punto presente
	      num_overlap=item_feat_int(seg,"num_overlap_s") + 1;
	      f0 = (f0 + (item_feat_float(seg,"f0_s") * num_overlap))/(num_overlap+1);
	      item_set_float(seg,"f0_s",f0);
	      item_set_int(seg,"num_overlap_s",num_overlap);
	    } 
	  else 
	    { // nuovo 
	      item_set_float(seg,"f0_s",f0);
	      item_set_int(seg,"num_overlap_s",0);
	    }
	}
      // a metà del segment: 
      if (win_start <= mid) 
	{
	  f0=approx_pfun_func(painte, invmap(tm, mid)); 	    
	  if (item_feat_present(seg,"f0_m"))
	    {  //c'è già un punto presente
	      num_overlap=item_feat_int(seg,"num_overlap_m") + 1;
	      f0 = (f0 + (item_feat_float(seg,"f0_m") * num_overlap))/(num_overlap+1);
	      item_set_float(seg,"f0_m",f0);
	      item_set_int(seg,"num_overlap_m",num_overlap);
	    } 
	  else 
	    { // nuovo 
	      item_set_float(seg,"f0_m",f0);
	      item_set_int(seg,"num_overlap_m",0);
	    }
	}
      // alla fine del segment:
      if (win_start <= end)
	{
	  f0=approx_pfun_func(painte, invmap(tm, end)); 	 
	  if (item_feat_present(seg,"f0_e"))
	    {  //c'è già un punto presente
	      num_overlap=item_feat_int(seg,"num_overlap_e") + 1;
	      f0 = (f0 + (item_feat_float(seg,"f0_e") * num_overlap))/(num_overlap+1);
	      item_set_float(seg,"f0_e",f0);
	      item_set_int(seg,"num_overlap_e",num_overlap);
	    } 
	  else 
	    { // nuovo 
	      item_set_float(seg,"f0_e",f0);
	      item_set_int(seg,"num_overlap_e",0);
	    }
	}
    }
}

cst_utterance *iterpolation_and_smoothing(cst_utterance *u)
{
  cst_relation *targ_relation = utt_relation_create(u,"Target");
  cst_relation *seg_relation = utt_relation(u,"Segment"); 
  char *silence=get_param_string(u->features,"silence", "#");
  float x, x1, x2, y1, y2, alfa, beta;
  cst_item *seg, *seg_pivot;  
  int calcola_alfa=1, metti_precedente_valore=0;
  y1= get_param_float(u->features,"int_f0_target_mean", 100.0);
  x1 = 0;
  for (seg=relation_head(seg_relation); seg; seg=item_next(seg)) {
    // se è silenzio non fa nulla.
    if (!cst_streq(item_name(seg),silence)) {     
        
      if (item_feat_present(seg,"f0_s")) {
	x1=segment_start(seg);
	y1=item_feat_float(seg,"f0_s");
	add_target_point(targ_relation, x1, y1);
	calcola_alfa=1;
      }
      if (item_feat_present(seg,"f0_m")) {
	x1=segment_mid(seg);
	y1=item_feat_float(seg,"f0_m");
	add_target_point(targ_relation, x1, y1);
	calcola_alfa=1;
      }
      if (item_feat_present(seg,"f0_e")) {
	x1=item_feat_float(seg,"end");
	y1=item_feat_float(seg,"f0_e");
	add_target_point(targ_relation, x1, y1);
	calcola_alfa=1;
      }
      else if (!(item_feat_present(seg,"f0_s") || item_feat_present(seg,"f0_m"))) 
	{//deve interpolare: se non ho il coeff angolare devo calcolarlo
	  if (calcola_alfa == 1) {
	    for (seg_pivot=seg; seg_pivot && !(item_feat_present(seg_pivot,"f0_s") || item_feat_present(seg_pivot,"f0_m") || item_feat_present(seg_pivot,"f0_e")); seg_pivot=item_next(seg_pivot));
	    if (!seg_pivot) {
	      	//vuol dire che non trova punti di f0 in avanti per interpolare 
		//allora mette il precedente valore di f0 
	      metti_precedente_valore=1;
	    } else {
	      //preso pivot 
	      if (item_feat_present(seg_pivot,"f0_s"))
		{
		  x2=segment_start(seg_pivot);
		  y2=item_feat_float(seg_pivot,"f0_s");
		}
	      else if (item_feat_present(seg_pivot,"f0_m"))
		{
		  x2=segment_mid(seg_pivot);
		  y2=item_feat_float(seg_pivot,"f0_m");
		}
	      else if (item_feat_present(seg_pivot,"f0_e"))
		{
		  x2=item_feat_float(seg_pivot,"end");
		  y2=item_feat_float(seg_pivot,"f0_e");
		}
	      alfa=(y2-y1)/(x2-x1);
	      beta=y1-alfa*x1;
	      calcola_alfa=0;
	    } 	  
	  }
	  //non calcola_alfa
	  //se il precedente è silenzio, metti anche start: 
	  //caso che non ci sia prev: primo seg? non dovrebbe succedere perchè il primo seg è sempre#
	  if (cst_streq(item_name(item_prev(seg)),silence)) {
	    x=segment_start(seg);
	    if (metti_precedente_valore)
	      add_target_point(targ_relation,x, y1);
	    else
	      add_target_point(targ_relation,x, beta+alfa*x);
	  }
	  x=segment_mid(seg);
	  if (metti_precedente_valore)
	    add_target_point(targ_relation,x, y1);
	  else
	    add_target_point(targ_relation,x, beta+alfa*x);
	  
	  x=item_feat_float(seg,"end");
	  if (metti_precedente_valore)
	    add_target_point(targ_relation,x, y1);
	  else
	    add_target_point(targ_relation,x, beta+alfa*x);
	}
    }
  }
  return u;
}


cst_utterance *mel2frq_target(cst_utterance *u) {
  //printf("XXXX1\n");
  //cst_relation *targ_relation = utt_relation(u,"Target");
  float hz_streach, mel_streach; //x1, x2, y1, y2, alfa, beta;
  int pitch_male2female_conversion; 
  //cst_item *t;
  hz_streach = get_param_float(u->features,"hz_streach", 1);
  mel_streach = get_param_float(u->features,"mel_streach", 1);
  pitch_male2female_conversion = get_param_int(u->features,"pitch_male2female_conversion", 0);
  
 cst_item *t;
 for (t=relation_head(utt_relation(u,"Target")); t; t=item_next(t)) {
   //  for (t=relation_head(targ_relation); t; t=item_next(t)) {
   //printf("Target: %f %f\n",item_feat_float(t,"pos"),item_feat_float(t,"f0"));
   // printf("XXXX\n");
   // printf("target %f \n",item_feat_float(t,"f0")); 
   if (pitch_male2female_conversion == 1) 
      //	;;HZ=700*(exp(mel/1127.01048)-1);
     item_set_float(t,"f0",  
		    //136.5*log(hz_streach*(700*(exp(mel_streach*item_feat_float(t,"f0")/1127.01048)-1)))-428.8);
		    male2female(hz_streach*(700*(exp(mel_streach*item_feat_float(t,"f0")/1127.01048)-1))));
   else
     item_set_float(t,"f0",  
		    hz_streach*(700*(exp(mel_streach*item_feat_float(t,"f0")/1127.01048)-1)));
  }
  return u;
}


static float male2female(float fm) { 
  return 136.5*log(fm)-428.8; }
  

static float segment_mid(cst_item *seg)
{
  float start;
  if (item_prev(seg))
    start = item_feat_float(item_prev(seg),"end");
  else
    start = 0;
  return start + (item_feat_float(seg,"end")-start)/2;
}
    
static float segment_start(cst_item *seg)
{
  float start;
  if (item_prev(seg))
    start = item_feat_float(item_prev(seg),"end");
  else
    start = 0;
  return start;
}
    
