// Long message attack
#include "numbers_shorthands.h"
#include "hash.h"

#include "dict.h"

// deadweight
// #include <bits/types/struct_timeval.h>

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <unistd.h> // access functoin 
//#include "memory.h" // memory monitor 
//#include <sys/time.h> // moved timing.h 
//#include <assert.h>
#include "config.h"
#include "timing.h"
//#include "types.h" // probably deadweight
#include "util_char_arrays.h"
#include "shared.h" // shared variables for duplicate 
#include "memory.h"
#include "util_files.h"
#include "common.h"

int n_discarded_bits(){
  int idx_size = (int) ceil((L_RECEIVER - log2(SIMD_LEN)));
  int ndiscarded_bits = ceil( (8*N) -idx_size*8 -(8 * VAL_SIZE_BYTES ) -DIFFICULTY) ;

  if (ndiscarded_bits < 0)
    return 0;
  return ndiscarded_bits;
}

u64 n_needed_candidates(){
  u64 nneeded_candidates = (1LL<<n_discarded_bits()) / NSERVERS;
  if (nneeded_candidates == 0)
    return 1 ;
  return nneeded_candidates;
}

/* @todo the information here is inacuurate */
void print_attack_information(){
  printf("\nL=%f, L_RECEIVER=%f, N=%d, DIFFICULTY=%d,\n"
         "|idx| = %dbytes, NELELEMNTS_BUCKET=%d, NSEERVERS=%d,\n"
         "NSLOTS_MY_NODE=%llu, NPROBES_MAX=%d, VAL_SIZE=%d\n"
         "NDEFINED BYTES=%d, NCND_NEEDED/receiver=%llu≈2^%0.4f,\n"
	 "NDISCARDED_BITS=%d\n"
	 "AVX_SIZE=%dbits, mask_ones=%d, mask_shifted=%u\n"
	 "NPROBES LOOK UP = %d\n",
	 L,
	 L_RECEIVER,
	 N,
	 DIFFICULTY,
	 (int) ceil((L_RECEIVER - log2(SIMD_LEN))/8.0),
	 SIMD_LEN,
	 NSERVERS,
	 NSLOTS_MY_NODE,
	 NPROBES_MAX,
	 VAL_SIZE_BYTES,
	 DEFINED_BYTES,
	 n_needed_candidates(),
	 log2(n_needed_candidates()),
	 n_discarded_bits(),
	 AVX_SIZE,
	 (1<<DIFFICULTY) - 1,
	 DIST_PT_MASK,
	 (int) (NPROBES_MAX/SIMD_LEN));

  puts("-------------------------------\n");
  
}


int is_dist_state(u8 state[HASH_STATE_SIZE]){
  /* check if the last X bits  if we read digest as little endian are zeros  */
  /* terms on the left get all 1s, term on right move 1s to the end */
  static const u8 ones = ( (1LL<<DIFFICULTY) - 1) << (8-DIFFICULTY);
  u8 last_8bits = ( (u8*) state)[N-1];
    
  /* For now, we are not going to have difficulyt more than 8 bits. It will  */
  return ( (  last_8bits  & ones) == 0 );
}

int is_dist_digest(u8 state[N]){
  /* check if the last X bits  if we read digest as little endian are zeros  */

  /* terms on the left get all 1s, term on right move 1s to the end */
  static const u8 ones = ( (1LL<<DIFFICULTY) - 1) << (8-DIFFICULTY);
  u8 last_8bits = ( (u8*) state)[N-1];
    
  /* For now, we are not going to have difficulyt more than 16 bits. It will  */
  /* take 330 years using my laptop! The longest recorded life is 122 years!  */
  return ( (  last_8bits  & ones) == 0 );
}


/* int is_dist_msg(u8 M[HASH_INPUT_SIZE]){ */
/*   WORD_TYPE state[NWORDS_STATE] = {HASH_INIT_STATE}; */
/*   hash_single(state, M); */

/*   return is_dist_state((u8*) state); */
/* } */



u32 to_which_server(u8 state[HASH_STATE_SIZE])
{
  // ==========================================================================+
  // Summary: Given a state. Decide to which server we send it to.             |
  // --------------------------------------------------------------------------+
  // INPUTS:                                                     nn              |
  // `Mstate` : Array of bytes of the digest.                                  |
  // --------------------------------------------------------------------------+
  // Given a state. Decide to which server we send to                          |
  // """ One potential idea is to do:                                          |
  // server <-- h % nserver                                                    |
  // h'     <-- h / nserver """                                                |
  //
  // --------------------------------------------------------------------------+
  /* 1- convert sttate to a u32 (digest || nserver || dist_pt)  */
  /*  -> (nserver || dist_pt) 32 bits */
  /* 2- remove the distinguished bits by shifting  (nserver ) */
  /* 3- keep only the bits that holds nserver (nserver) it may have extra bit */
  /* 4- Compute server number by taking computing mod nservers */
  u32 snd_to_server  = ((u32*) &state[N-4])[0];
			

  /* since the last bits are the distinguished point and thery are zero */
  /* and they are the most significant bits in snd_to_server*/
  /* no shift nor mask is needed */
  snd_to_server = (snd_to_server % (0x7fffffff)) % NSERVERS;

  return snd_to_server;
} 



void transpose_state(u32 dest[restrict 16*8],
		     u32 src[restrict 16*8])
{
  /// list of states -> transoposed states
  for (int lane = 0; lane < 16; lane++) {
    dest[lane + 0*16] = src[8*lane + 0];
    dest[lane + 1*16] = src[8*lane + 1];
    dest[lane + 2*16] = src[8*lane + 2];
    dest[lane + 3*16] = src[8*lane + 3];
    dest[lane + 4*16] = src[8*lane + 4];
    dest[lane + 5*16] = src[8*lane + 5];
    dest[lane + 6*16] = src[8*lane + 6];
    dest[lane + 7*16] = src[8*lane + 7];
  }

}


void untranspose_state(u32 dest[restrict 16*8],
		       u32 src[restrict  8*16])
{
  /* looks stupid but I am not passing dimension as an argument */
  /// What's is actually stupid is not documenting your functions.
  /// transponse state -> list of 16 states one after the other
    for (int lane = 0; lane < 16; lane++) {
      dest[8*lane + 0] = src[lane + 0*16];
      dest[8*lane + 1] = src[lane + 1*16];
      dest[8*lane + 2] = src[lane + 2*16];
      dest[8*lane + 3] = src[lane + 3*16];
      dest[8*lane + 4] = src[lane + 4*16];
      dest[8*lane + 5] = src[lane + 5*16];
      dest[8*lane + 6] = src[lane + 6*16];
      dest[8*lane + 7] = src[lane + 7*16];
  }

}







void copy_transposed_digest(u8 *digest, u32 *tr_state, int lane)
{
  /// todo: document this function 
  for (int i = 0; i<(N/WORD_SIZE); ++i) 
    memcpy(&digest[i*WORD_SIZE],
	   &tr_state[lane + i*16],
	   WORD_SIZE);


  /* copy the rest of the bytes */
  memcpy(&digest[(N/WORD_SIZE)*WORD_SIZE],
	 &tr_state[lane + (N/WORD_SIZE)*16],
	 N - (N/WORD_SIZE)*WORD_SIZE );

}




