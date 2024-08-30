# Weather_simulation
The weather phenomenon simulated in this code is a simplified model of atmospheric advection and diffusion. The code is a simple 2D weather simulation that models the movement of air parcels (advection) and the smoothing of weather properties (diffusion) over a grid. This model doesn't represent the full complexity of atmospheric processes but serves as a basic illustration of how numerical simulations can be used to study weather phenomena.

This code writes the output the field data to a NetCDF file named ```output.nc```. NetCDF is a file format often used for storing scientific data with metadata. It defines dimensions (time, x, y) and a variable ("field") to store the simulation data. The data is written to the NetCDF file in a loop over time steps.

NetCDF, which stands for Network Common Data Form, is a data format that provides a way to store and exchange scientific data in a self-describing and platform-independent manner. NetCDF was developed to facilitate the storage, access, and sharing of large datasets, particularly in the fields of Earth sciences, atmospheric sciences, oceanography, and climate modeling. To use it, you can simply type ncview output.nc (http://meteora.ucsd.edu/~pierce/ncview_home_page.html). A comprehensive list of functions and commands may be found in the NetCDF webpage.

NOTE: Make sure to have gcc installed. If not, use:
```
sudo apt-get update
sudo apt-get install gcc
```
To compile the code you need ```netcdf``` and ```mpich```. 
To install netcdf:
```
sudo apt-get install libnetcdf-dev
```
To install mpich:
```sudo apt-get install mpich```


If netcdf is not installed as shown above, make the following changes in the ```Makefile```:
```
INCLUDE=/full/path/to/netcdf/include/
LIB=/full/path/to/netcdf/lib/
```

To compile the code run:
```make```


To run the code

```mpirun -np 4 ./weather_simulation n (Where n is the number of time steps)```

To increase the output file size you can increase the number of time steps ```n```.

To change the size of the grid you can change the size of ```NX and NY``` in the code.
