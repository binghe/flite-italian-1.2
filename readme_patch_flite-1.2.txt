		         It_flite
    Text To Speech synthesizer for italian based on flite.
             version 0.75 RELEASE October 2004
                        IRST - ISTC

 (IRST: Centro per la ricerca scientifica e tecnologica Trento)
			    -
(ISTC: Istituto di Scienze e Tecnologie della Cognizione Padova)
		     
        http://tcc.itc.it/research/i3p/itaflite.html 
	 
It_flite è un sintetizzatore da testo per l'italiano, che ha come priorità 
la velocità di sintesi, la snellezza del programma e la portabilità in piccoli 
sistemi hardware come i PDA. 
La sua struttura è basata su flite (http://www.speech.cs.cmu.edu/flite), mentre 
i componenti lessico-prosodici e difonoci sono stati inseriti in it_flite 
portando i moduli della versione per l'italiano di Festival in ANSI C.
(http://www.pd.istc.cnr.it/TTS/It-FESTIVAL.htm). 

L'agoritmo di sintesi utilizzato da It_flite è Mbrola 
(http://tcts.fpms.ac.be/synthesis/mbrola.html) il quale viene ompiegato tramite una 
libreria dinamica ed i relativi database it3 e it4, registrati rispettivamente 
all'ISTC e all'IRST.

Questa versione inoltre consente il la codifica in MP3 dell'output, utilizzando la 
libreria dinamica Lame (http://lame.sourceforge.net). 

Autori: 
Fabio Tesser <tesser@pd.istc.cnr.it>
Piero Cosi   <cosi@pd.istc.cnr.it>


----------------------------------
patch-0.5_flite-1.2.tar.gz README:
----------------------------------

Patch che sistema due problemi: 

1) Rispetto alle regole di trascrizione lts_rewrites_rules tipo x [ x ] x = x  provenienti da festival ci sono delle differenze.
 - Modificato il file cst_lts_rewrites.c funzione match loop. In quanto c'era una differenza nelle regole tipo all * [d] dd * = d1.
 - Modificato il file make_lts_rewrite.scm: inversione di *  e + nel contensto destro.	Tale file è comunque presente anche nella versione developer di pc_flite.
   Per creare le lts puoi da linea di comando:
	$ festival -b make_lts_rewrite.scm '(ltsrewritestoC "my_lts/italian_lts.scm" "my_lts")' > my_lts/out 

2) In cst_synth.c:
   Corretto Pospunctuation invece di post punctuation...   
   
   Funzione per salvare i fonemi con ledurate dopo il modulo cvart di durata.
   linea di comando:		 
   --sets lab_filename=LABFILE Set the output phoneme time text file
   Inoltre Funzione per i tag position
   Funzione per il mapping dei difoni non presenti.   
 
3) cst_lexicon.c e cst_lexicon.h funzioni che mi restituiscono il tPOS, quando si trova una parola nel lessico.
   
4) Inoltre ulteriore modifica nel file flite.c che sistema la lettura-tokenizzazione di file senza a capo a fine frase: //FABIO EOF senza a capo.

5) cst_synth.c Aggiunto controlli per zscore > 3 e < 0.01 
   flite.c modificato utt_break in modo che divida le utterance dopo due a capo "\n"
   

FILELIST: 

$ ls -R
.:
flite-1.2-release

./flite-1.2-release:
include  make_lts_rewrite.scm  readme_patch_flite-1.2.txt  src

./flite-1.2-release/include:
cst_lexicon.h

./flite-1.2-release/src:
lexicon  synth

./flite-1.2-release/src/lexicon:
cst_lexicon.c  cst_lts_rewrites.c

./flite-1.2-release/src/synth:
cst_synth.c  flite.c

