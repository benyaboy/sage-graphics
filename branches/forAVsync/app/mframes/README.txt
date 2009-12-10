_____________To compile:

1.
  cd init_code
  make
  cd ..
2.
  cd machine_gen
  make
  cd ..
3.
  make

______________Dataset file example:

1. NOSAGE
2. MagicFrames 0 0
3. 11400 2800
4. 512 512
5. 117
6. /data/evl/MagicCarpet/animations/jt_slices 1

1 -- SAGE or NOSAGE
2 -- Application Name, Horizontal/Vertical offset 4 the window
3 -- Resolution of the global image
4 -- Resolution of the data tiles
5 -- Number of frames
6 -- Location of the dataset and number of color components
