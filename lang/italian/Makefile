###########################################################################
##                                                                       ##
##                        ITALIAN FLITE                                  ##
##                                                                       ##
## Italian Flite is subject to the Mozilla Public License Version 1.1    ##
## (the "License"); you may not use this file except in compliance with  ##
## the License. You may obtain a copy of the License at                  ##
## http://www.mozilla.org/MPL/                                           ##
##                                                                       ##
## Note: Italian Flite is based on Flite which is a free software        ##
## distributed under a different licence. See the LICENSE file for more  ##
## information.                                                          ##
##                                                                       ##
## Software distributed under the License is distributed on an "AS IS"   ##
## basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.       ##
## See the License for the specific language governing rights and        ##
## limitations under the License.                                        ##
##                                                                       ##
## The Original Code is composed by all the Italian language specific    ##
## parts (tokenization, LTS rules, lexicon, part of speech tagger,       ##
## duration and intonation modules), the PaIntE implementation, the link ##
## with the MBROLA synthesis algorithm and the connection with the LAME  ##
## MP3 encoder.                                                          ##
##                                                                       ##
## Contributor(s):                                                       ##
## Fabio Tesser <tesser@pd.istc.cnr.it>                                  ##
## Piero Cosi   <cosi@pd.istc.cnr.it>                                    ##
##                                                                       ##
########################################################################### 
##             Author:  Fabio Tesser <tesser@pd.istc.cnr.it>             ##
##               Date:  October 2004                                     ##
###########################################################################
##                                                                       ##
##  Italian general functions/models                                     ##
##                                                                       ##
###########################################################################

TOP=../..
DIRNAME=lang/italian
BUILD_DIRS = 
ALL_DIRS= 
H = italian.h it_text.h it_phrasing_cart.h \
    it_ffeatures.h it_f0.h \
    it_phrasing.h it_intonation.h \
    sillabe1_lts_rewrites.h sillabe2_lts_rewrites.h \
    it_lexical_insertion.h diphones_map.h \

PROSODYTYPE = f0_cart

ifeq ($(PROSODYTYPE),f0_cart)
H += it_durz_ds_cart.h it_int_accent_ds_cart.h \
     it_f0z_start_cart.h it_f0z_mid_cart.h it_f0z_end_cart.h 
endif

ifeq ($(PROSODYTYPE),painte)
H += it_durz_cart.h it_int_accent_cart.h \
     painte_CW_NUM_cart.h
endif

SRCS = it_phoneset.c it_gpos.c it_text.c it_expand.c \
       it_phrasing_cart.c \
       it_ffeatures.c it_f0_model.c \
       it_phrasing.c it_intonation.c \
       sillabe1_lts_rewrites.c sillabe2_lts_rewrites.c \
       it_lexical_insertion.c diphones_map.c ita_map.c \

ifeq ($(PROSODYTYPE),f0_cart)
SRCS += italian-f0_cart.c it_dur_ds_stats.c it_durz_ds_cart.c it_int_accent_ds_cart.c \
        it_f0z_start_cart.c it_f0z_mid_cart.c it_f0z_end_cart.c 
endif
ifeq ($(PROSODYTYPE),painte)
SRCS += italian-painte.c it_dur_stats.c it_durz_cart.c it_int_accent_cart.c \
        painte_CW_NUM_cart.c PaIntE_file_CB.c
endif

OBJS = $(SRCS:.c=.o)
SCM=
FILES = Makefile $(SCM) $(SRCS) $(H)
LIBNAME = flite_italian
LOCAL_INCLUDES = 

ALL = .build_lib

include $(TOP)/config/common_make_rules
