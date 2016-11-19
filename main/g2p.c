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
/*  It_flite top level program                                           */
/*                                                                       */
/*************************************************************************/

#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <unistd.h>

#include "flite.h"
#include "it_flite_version.h"

//#include "voxdefs.h"
//per avere multi voice devo togliere l'include del preprocessore, perch� � una cosa che si decide all'esecuzione.
/*cst_voice *REGISTER_VOX(const char *voxdir);
  cst_voice *UNREGISTER_VOX(cst_voice *vox);*/

cst_voice *register_irstifd_ita_pc(const char *voxdir);
cst_voice *register_irstifd_ita_lp(const char *voxdir);
void unregister_irstifd_ita_pc(cst_voice *vox);
void unregister_irstifd_ita_lp(cst_voice *vox);

cst_voice *voxdefs_REGISTER_VOX(const char *voice_name,const char *voxdir) 
{
  cst_voice *vox;
  if (cst_streq(voice_name,"lp") || cst_streq(voice_name,"LP"))
    {//presi da voxdefs di lp 
      /*const char* VOXNAME="irstifd_ita_lp";
      const char* VOXHUMAN="Loredana";
      const char* VOXGENDER="female";
      const float VOXVERSION=1.0;
      const int SYNTH_MODULES_DEBUG=1;*/
      vox=register_irstifd_ita_lp(voxdir);
    }
  else //default (cst_streq(voice_name,"pc") || cst_streq(voice_name,"PC"))
    {//presi da voxdefs di pc
      /*const char* VOXNAME="irstifd_ita_pc";
      const char* VOXHUMAN="Piero";
      const char* VOXGENDER="male";
      const float VOXVERSION=1.0;
      const int SYNTH_MODULES_DEBUG=1;*/
      vox=register_irstifd_ita_pc(voxdir);
    } 
  return vox;
}

void voxdefs_UNREGISTER_VOX(const char *voice_name,cst_voice *vox) 
{
  if (cst_streq(voice_name,"lp") || cst_streq(voice_name,"LP"))
    //preso da voxdefs di lp
    unregister_irstifd_ita_lp(vox);
  else //default
    //preso da voxdefs di pc
    unregister_irstifd_ita_pc(vox);
}
      
     
static void flite_version()
{
    printf("  version: %s-%s-%s %s\n",
	   FLITE_PROJECT_PREFIX,
	   FLITE_PROJECT_VERSION,
	   FLITE_PROJECT_STATE,
	   FLITE_PROJECT_DATE);
    printf("  IRST-ISTC Copyright 2004, all rights reserved\n" );
}


static void ef_set(cst_features *f,const char *fv,const char *type)
{
    /* set feature from fv (F=V), guesses type if not explicit type given */
    const char *val;
    char *feat;

    if ((val = strchr(fv,'=')) == 0)
    {
	fprintf(stderr,
		"flite: can't find '=' in featval \"%s\", ignoring it\n",
		fv);
    }
    else
    {
	feat = cst_strdup(fv);
	feat[strlen(fv)-strlen(val)] = '\0';
	val = val+1;
	if ((type && cst_streq("int",type)) ||
	    ((type == 0) && (cst_regex_match(cst_rx_int,val))))
	    feat_set_int(f,feat,atoi(val));
	else if ((type && cst_streq("float",type)) ||
		 ((type == 0) && (cst_regex_match(cst_rx_double,val))))
	    feat_set_float(f,feat,atof(val));
	else
	    feat_set_string(f,feat,val);
	/* I don't free feat, because feats think featnames are const */
	/* which is true except in this particular case          */
    }
}

int main(int argc, char **argv)
{
    struct timeval tv;
    cst_voice *v;
    const char *filename;
    const char *outtype;
    char *voice_name;
    int i;
    float durs;
    double time_start, time_end;
    int flite_verbose, flite_loop, flite_bench;
    int explicit_filename, explicit_text, explicit_phones;
#define ITER_MAX 3
    int bench_iter = 0;
    cst_features *extra_feats;

    filename = 0;
    outtype = "play";   /* default is to play */
    flite_verbose = FALSE;
    flite_loop = FALSE;
    flite_bench = FALSE;
    explicit_text = explicit_filename = explicit_phones = FALSE;
    extra_feats = new_features();

    flite_init();

	if (argc < 2) {
		fprintf(stderr,"USAGE: %s <synt string>\n",argv[0]);
		exit(1);
	}

	filename = argv[1];
	explicit_text = TRUE;

    voice_name=get_param_string(extra_feats,"voice","pc");
    v = voxdefs_REGISTER_VOX(voice_name,NULL);
    
    feat_copy_into(extra_feats,v->features);
    durs = 0.0;
    //Qui sarebbe da caricare mbrola.dll e mbrola database per caricarlo solo una volta nel caso di frasi multi-utterence. extern? global? const?
    if (flite_bench)
    {
	outtype = "none";
	filename = "A whole joy was reaping, but they've gone south, you should fetch azure mike.";
	explicit_text = TRUE;
    }

loop:   
	if ((strchr(filename,' ') && !explicit_filename) || explicit_text)
		flite_text_to_phones(filename,v);
    
    if (flite_loop || (flite_bench && bench_iter++ < ITER_MAX))
	    goto loop;
    
    delete_features(extra_feats);
    voxdefs_UNREGISTER_VOX(voice_name,v);
 
    return 0;
}
