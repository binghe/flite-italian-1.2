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

static void flite_usage()
{
  printf("it_flite: Text To Speech synthesizer for italian based on flite.\n");
  //printf("IRST-ISTC Copyright 2004, all rights reserved\n");
  printf("flite: a small simple speech synthesizer\n");
    flite_version();
    printf("usage: it_flite TEXT/FILE [WAVEFILE]\n"
           "  Converts text in TEXTFILE to a waveform in WAVEFILE\n"
           "  If text contains a space the it is treated as a literal\n"
           "  textstring and spoken, and not as a file name\n"
           "  if WAVEFILE is unspecified or \"play\" the result is\n"
           "  played on the current systems audio device.  If WAVEFILE\n"
           "  is \"none\" the waveform is discarded (good for benchmarking)\n"
           "  Other options must appear before these options\n"
           "  --version   Output flite version number\n"
           "  --help      Output usage string\n"
           "  -o WAVEFILE Explicitly set output filename\n"
           "  -f TEXTFILE Explicitly set input filename\n"
           "  -t TEXT     Explicitly set input textstring\n"
           "  -p PHONES   Explicitly set input textstring and synthesize as phones\n"
           "  --set F=V   Set feature (guesses type)\n"
           "  -s F=V      Set feature (guesses type)\n"
           "  --seti F=V  Set int feature\n"
           "  --setf F=V  Set float feature\n"
           "  --sets F=V  Set string feature\n"
	   "  -b          Benchmark mode\n" //solo sintesi simbolica
	   "  -l          Loop endlessly\n" //fa loop
           "  -v          Verbose mode\n"
	   "Added commands:\n"
	   "--sets voice=XX Set the  voice, can be pc, a male voice (default), or lp, a female voice\n" 
	   "--sets lab_filename=LABFILE Set the output phoneme time text file\n"
	   "--seti mp3=1  Explicitly set mp3 output filetype\n"
	   "--seti mp3_bitrate=xxx Set mp3 bitrate to xxx (default 128 kBit/s)\n"
	   "--seti mp3_resamplerate=xxx Set mp3 resamplerate to xxx Hz\n"
	   "--sets mbrola_pho_filename=MBRPHOFILE Set the output mbrola pho text file\n"
	   "--setf duration_stretch=V  Set float duration_stretch feature\n"
	   //"--sets tags_position=\"n1 n2 n3 ...\" Set the tags word position for the lab_filename" 
	   );    
    exit(0);
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

    for (i=1; i<argc; i++)
      {
		      
	if (cst_streq(argv[i],"--version"))
	{
	    flite_version();
	    return 1;
	}

	else if (cst_streq(argv[i],"-h") ||
		 cst_streq(argv[i],"--help") ||
		 cst_streq(argv[i],"-?"))
	    flite_usage();
	else if (cst_streq(argv[i],"-v"))
	    flite_verbose = TRUE;
	else if (cst_streq(argv[i],"-l"))
	    flite_loop = TRUE;
	else if (cst_streq(argv[i],"-b"))
	{
	    flite_bench = TRUE;
	    break; /* ignore other arguments */
	}
	else if ((cst_streq(argv[i],"-o")) && (i+1 < argc))
	{
	    outtype = argv[i+1];
	    i++;
	}
	else if (cst_streq(argv[i],"-f") && (i+1 < argc))
	{
	    filename = argv[i+1];
	    explicit_filename = TRUE;
	    i++;
	}
	else if ((cst_streq(argv[i],"-set") || cst_streq(argv[i],"-s"))
		 && (i+1 < argc))
	{
	    ef_set(extra_feats,argv[i+1],0);
	    i++;
	}
	else if (cst_streq(argv[i],"--seti") && (i+1 < argc))
	{
	    ef_set(extra_feats,argv[i+1],"int");
	    i++;
	}
	else if (cst_streq(argv[i],"--setf") && (i+1 < argc))
	{
	    ef_set(extra_feats,argv[i+1],"float");
	    i++;
	}
	else if (cst_streq(argv[i],"--sets") && (i+1 < argc))
	{
	    ef_set(extra_feats,argv[i+1],"string");
	    i++;
	}
	else if (cst_streq(argv[i],"-p") && (i+1 < argc))
	{
	    filename = argv[i+1];
	    explicit_phones = TRUE;
	    i++;
	}
	else if (cst_streq(argv[i],"-t") && (i+1 < argc))
	{
	    filename = argv[i+1];
	    explicit_text = TRUE;
	    i++;
	}
	else if (filename)
	    outtype = argv[i];
	else
	    filename = argv[i];
    }

    //FABIO: controllo coerenza paremetri mp3
    if ((feat_present(extra_feats, "mp3_resamplerate") || feat_present(extra_feats, "mp3_bitrate")) && !get_param_int(extra_feats,"mp3",0))
      {
	printf("\nATTENZIONE: La modalita' save mp3 viene attivata se la linea di comando contiene:"
"--seti mp3=1"
"\nAltri parametri che possono essere impostati sono:"
"--seti mp3_bitrate=xxx (default 128)"
"--seti mp3_resamplerate=xxx (default 0)\n");
	return 0;
      }

     //FABIO: controllo coerenza paremetri tags position

    if (feat_present(extra_feats, "tags_position") && (!feat_present(extra_feats, "lab_filename")))  
      {
	printf("\nATTENZIONE: L'informazione tags_position senza lab_filename non serve.\n");
	return 0;
      }
    
    if (filename == NULL) filename = "-";  /* stdin */

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
    gettimeofday(&tv,NULL);
    time_start = (double)(tv.tv_sec)+(((double)tv.tv_usec)/1000000.0);

    if (explicit_phones)
	durs = flite_phones_to_speech(filename,v,outtype);
    else if ((strchr(filename,' ') && !explicit_filename) || explicit_text) 
      durs = flite_text_to_speech(filename,v,outtype);
    else
      durs = flite_file_to_speech(filename,v,outtype);
    
    gettimeofday(&tv,NULL);
    time_end = ((double)(tv.tv_sec))+((double)tv.tv_usec/1000000.0);

    if (flite_verbose || (flite_bench && bench_iter == ITER_MAX))
	printf("times faster than real-time: %f\n(%f seconds of speech synthesized in %f)\n",
	       durs/(float)(time_end-time_start),
	       durs,
	       (float)(time_end-time_start));

    if (flite_loop || (flite_bench && bench_iter++ < ITER_MAX))
	    goto loop;
    
    delete_features(extra_feats);
    voxdefs_UNREGISTER_VOX(voice_name,v);
 
    return 0;
}
