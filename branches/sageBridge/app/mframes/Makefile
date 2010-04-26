build: machinefile_gen init_magic_frames

machinefile_gen: machinefile_code/machinefile_gen
	cp machinefile_code/machinefile_gen .
	
init_magic_frames: init_code/init_magic_frames
	cp init_code/init_magic_frames .

clean:
	rm machinefile_gen
	rm init_magic_frames
	cd machinefile_code && make clean
	cd init_code && make clean
