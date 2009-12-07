/*--------------------------------------------------------------------------*/
/* Volume Rendering Application                                             */
/* Copyright (C) 2006-2007 Nicholas Schwarz                                 */
/*                                                                          */
/* This software is free software; you can redistribute it and/or modify it */
/* under the terms of the GNU Lesser General Public License as published by */
/* the Free Software Foundation; either Version 2.1 of the License, or      */
/* (at your option) any later version.                                      */
/*                                                                          */
/* This software is distributed in the hope that it will be useful, but     */
/* WITHOUT ANY WARRANTY; without even the implied warranty of               */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser  */
/* General Public License for more details.                                 */
/*                                                                          */
/* You should have received a copy of the GNU Lesser Public License along   */
/* with this library; if not, write to the Free Software Foundation, Inc.,  */
/* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                    */
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

/*--------------------------------------------------------------------------*/

int main(int argc, char** argv) {
  
  // Check command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: GenerateBrick outfile extent\n");
    exit(1);
  }

  // Get size
  int size = atoi(argv[2]);

  // Attempt to open file for writing
  FILE* outfptr = NULL;
  if ((outfptr = fopen(argv[1], "w")) == NULL) {
    fprintf(stderr, "Can't open file %s for writing.\n", argv[1]);
    exit(1);
  }

  // Data value to write
  unsigned short value = 0;

  // Generate and write data
  for (int z = 0 ; z < size ; z++) {
    for (int y = 0 ; y < size ; y++) {
      for (int x = 0 ; x < size ; x++) {

        //if (fseeko(outfptr, (z*size*size)+(y*size)+x, SEEK_SET) != 0) {
        //  fprintf(stderr, "Error setting file position.\n");
        //}

        // Make value
        value = 
          (unsigned short) (int) (((long double) z / (long double) size) * 
                                  (long double) 65535.0);

        // Write value
        if (fwrite(&value, sizeof(unsigned short), 1, outfptr) != 1) {
          fprintf(stderr, "Error writting value.\n");
        }
        
      }
    }
  }

  // Close file
  fclose(outfptr);

  // Return
  return 0;

}

/*--------------------------------------------------------------------------*/
