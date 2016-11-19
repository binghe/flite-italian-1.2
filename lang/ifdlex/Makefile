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
##  IFDLEX                                                               ##
##                                                                       ##
###########################################################################

TOP=../..
DIRNAME=lang/ifdlex
BUILD_DIRS = 
ALL_DIRS= 
H = ifdlex.h \
    italian_downcase_lts_rewrites.h\
    italian_lts_rewrites.h \
    italian_stress1a_lts_rewrites.h \
    italian_stress1b_lts_rewrites.h \
    italian_stress1c_lts_rewrites.h \
    italian_stress4a_lts_rewrites.h \
    italian_stress4b_lts_rewrites.h \
    italian_stress4c_lts_rewrites.h \
    italian_stress4d_lts_rewrites.h \
    italian_stress5a_lts_rewrites.h \
    italian_stress5b_lts_rewrites.h \
    italian_stress6a_lts_rewrites.h \
    italian_stress7a_lts_rewrites.h \
    italian_stress7b_lts_rewrites.h \
    italian_stress8a_lts_rewrites.h \
    italian_stress8b_lts_rewrites.h \
    italian_stress8c_lts_rewrites.h \
    italian_stress9a_lts_rewrites.h \
    italian_stress9b_lts_rewrites.h \
    italian_stress10a_lts_rewrites.h \
    italian_stress11a_lts_rewrites.h \
    italian_stress12a_lts_rewrites.h \
    italian_stress13a_lts_rewrites.h \
    italian_stress14a_lts_rewrites.h \
    italian_stress15a_lts_rewrites.h \
    italian_stress15b_lts_rewrites.h \
    italian_stress15c_lts_rewrites.h \
    italian_stress16a_lts_rewrites.h \
    italian_stress16b_lts_rewrites.h \
    italian_stress17a_lts_rewrites.h \
    italian_stress17b_lts_rewrites.h \
    italian_stress17c_lts_rewrites.h \
    italian_grafo1_lts_rewrites.h \
    italian_grafo2_lts_rewrites.h \
    italian_grafo3_lts_rewrites.h \
    italian_grafo4_lts_rewrites.h \
    italian_grafo5_lts_rewrites.h \
    italian_grafo6_lts_rewrites.h \
    italian_grafo7_lts_rewrites.h \
    italian_grafo8_lts_rewrites.h \
    italian_grafo9_lts_rewrites.h \
    italian_grafo10_lts_rewrites.h

SRCS =  ifd_lex.c\
    italian_downcase_lts_rewrites.c \
    italian_lts_rewrites.c \
    italian_stress1a_lts_rewrites.c \
    italian_stress1b_lts_rewrites.c \
    italian_stress1c_lts_rewrites.c \
    italian_stress4a_lts_rewrites.c \
    italian_stress4b_lts_rewrites.c \
    italian_stress4c_lts_rewrites.c \
    italian_stress4d_lts_rewrites.c \
    italian_stress5a_lts_rewrites.c \
    italian_stress5b_lts_rewrites.c \
    italian_stress6a_lts_rewrites.c \
    italian_stress7a_lts_rewrites.c \
    italian_stress7b_lts_rewrites.c \
    italian_stress8a_lts_rewrites.c \
    italian_stress8b_lts_rewrites.c \
    italian_stress8c_lts_rewrites.c \
    italian_stress9a_lts_rewrites.c \
    italian_stress9b_lts_rewrites.c \
    italian_stress10a_lts_rewrites.c \
    italian_stress11a_lts_rewrites.c \
    italian_stress12a_lts_rewrites.c \
    italian_stress13a_lts_rewrites.c \
    italian_stress14a_lts_rewrites.c \
    italian_stress15a_lts_rewrites.c \
    italian_stress15b_lts_rewrites.c \
    italian_stress15c_lts_rewrites.c \
    italian_stress16a_lts_rewrites.c \
    italian_stress16b_lts_rewrites.c \
    italian_stress17a_lts_rewrites.c \
    italian_stress17b_lts_rewrites.c \
    italian_stress17c_lts_rewrites.c \
    italian_grafo1_lts_rewrites.c \
    italian_grafo2_lts_rewrites.c \
    italian_grafo3_lts_rewrites.c \
    italian_grafo4_lts_rewrites.c \
    italian_grafo5_lts_rewrites.c \
    italian_grafo6_lts_rewrites.c \
    italian_grafo7_lts_rewrites.c \
    italian_grafo8_lts_rewrites.c \
    italian_grafo9_lts_rewrites.c \
    italian_grafo10_lts_rewrites.c 

LEXDIM = full

ifeq ($(LEXDIM),full)
SRCS += ifd_lex_phone-full.c ifd_lex_entry-full.c
endif

ifeq ($(LEXDIM),reduced)
SRCS += ifd_lex_phone-reduced.c ifd_lex_entry-reduced.c
endif 

OBJS = $(SRCS:.c=.o)
SCM=
FILES = Makefile $(SCM) $(SRCS) $(H)
LIBNAME = flite_ifdlex
LOCAL_INCLUDES = 

ALL = .build_lib

include $(TOP)/config/common_make_rules
