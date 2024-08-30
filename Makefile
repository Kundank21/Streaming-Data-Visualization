INCLUDE=/full/path/to/netcdf/include/
LIB=/full/path/to/netcdf/lib/

weather_simulation:
	mpicc -o weather_simulation weather_sim.c -I$(INCLUDE) -L$(LIB) -lnetcdf
