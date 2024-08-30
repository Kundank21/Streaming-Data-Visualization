#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <netcdf.h>
#include <mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>


#define NX 64
#define NY 64
#define DT 60.0 // Time step in seconds
#define DX 1000.0 // Spatial step in meters
#define U0 10.0 // Initial horizontal wind velocity (m/s)
#define V0 5.0 // Initial vertical wind velocity (m/s)
#define KX 0.00001 // Diffusion coefficient for X-direction
#define KY 0.00001 // Diffusion coefficient for Y-direction
#define KZ 0.00001 // Diffusion coefficient for Z-direction

void initializeField(double field[][NX][NY], int numSteps) {
    srand(time(NULL));
    for (int t = 0; t < numSteps; t++) {
        for (int i = 0; i < NX; i++) {
            for (int j = 0; j < NY; j++) {
                field[t][i][j] = rand() % 100;
            }
        }
    }
}

// void writeFieldToNetCDF(double field[][NX][NY], int numSteps, int sizeY) {
//     char filename[50];
//     sprintf(filename, "output.nc");

//     int ncid;
//     int retval;

//     if ((retval = nc_create(filename, NC_CLOBBER, &ncid)) != NC_NOERR) {
//         fprintf(stderr, "Error creating NetCDF file: %s\n", nc_strerror(retval));
//         return;
//     }

//     int dimids[3];
//     int varid;
//     int dim_sizes[3] = {numSteps, NX, NY};

//     if ((retval = nc_def_dim(ncid, "time", numSteps, &dimids[0])) != NC_NOERR ||
//         (retval = nc_def_dim(ncid, "x", NX, &dimids[1])) != NC_NOERR ||
//         (retval = nc_def_dim(ncid, "y", NY, &dimids[2])) != NC_NOERR) {
//         fprintf(stderr, "Error defining NetCDF dimensions: %s\n", nc_strerror(retval));
//         nc_close(ncid);
//         return;
//     }

//     if ((retval = nc_def_var(ncid, "field", NC_DOUBLE, 3, dimids, &varid)) != NC_NOERR) {
//         fprintf(stderr, "Error defining NetCDF variable: %s\n", nc_strerror(retval));
//         nc_close(ncid);
//         return;
//     }

//     if ((retval = nc_enddef(ncid)) != NC_NOERR) {
//         fprintf(stderr, "Error ending NetCDF definition mode: %s\n", nc_strerror(retval));
//         nc_close(ncid);
//         return;
//     }

//     size_t start[3] = {0, 0, 0};
//     size_t count[3] = {1, NX, NY};

//     for (int t = 0; t < numSteps; t++) {
//         start[0] = t;
//         count[0] = 1;
//         if ((retval = nc_put_vara_double(ncid, varid, start, count, &field[t][0][0])) != NC_NOERR) {
//             fprintf(stderr, "Error writing data to NetCDF file: %s\n", nc_strerror(retval));
//             nc_close(ncid);
//             return;
//         }
//     }

//     if ((retval = nc_close(ncid)) != NC_NOERR) {
//         fprintf(stderr, "Error closing NetCDF file: %s\n", nc_strerror(retval));
//         return;
//     }

//     printf("Saved field data to %s\n", filename);
// }

void simulateWeather(double field[][NX][NY], int rank, int num_processes, int numSteps) {
    double tempField[numSteps][NX][NY];

    for (int t = 0; t < numSteps; t++) {
        // Advection
        for (int i = 0; i < NX; i++) {
            for (int j = 0; j < NY; j++) {
                int i_prev = ((int)(i - U0 * DT / DX + NX)) % NX;
                int j_prev = ((int)(j - V0 * DT / DX + NY)) % NY;
                tempField[t][i][j] = field[t][i_prev][j_prev];
            }
        }

        // Diffusion
        for (int i = 0; i < NX; i++) {
            for (int j = 0; j < NY; j++) {
                double laplacian = (field[t][(i + 1) % NX][j] + field[t][(i - 1 + NX) % NX][j]
                                    + field[t][i][(j + 1) % NY] + field[t][i][(j - 1 + NY) % NY]
                                    - 4 * field[t][i][j]) / (DX * DX);
                tempField[t][i][j] += (KX * laplacian + KY * laplacian) * DT;
            }
        }
        if (rank == 0) {
            // Create a socket
            int server_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (server_socket == -1) {
                perror("Socket creation failed");
                exit(EXIT_FAILURE);
            }
            printf("Socket Created\n");

            // Bind the socket to an address and port
            struct sockaddr_in server_address;
            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(5566); // Replace with the desired port
            server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

            if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
                perror("Bind failed");
                exit(EXIT_FAILURE);
            }
            printf("binded\n");

            // Listen for incoming connections
            if (listen(server_socket, 5) == -1) {
                perror("Listen failed");
                exit(EXIT_FAILURE);
            }
            printf("Listening\n");

            // Accept a client connection
            int client_socket = accept(server_socket, NULL, NULL);
            if (client_socket == -1) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }
            printf("Connected\n");

            // Send a string to the client
            // char message[] = "Hello from C server!";
            printf("%f,%f,%f,%f\n",tempField[t][0][0],tempField[t][0][1],tempField[t][1][0],tempField[t][1][1]);
            // send(client_socket, tempField[t], sizeof(tempField[t]), 0);
            for (int i = 0; i < NX; i++) {
                for (int j = 0; j < NY; j++) {
                    // Send a chunk of data to the client
                    double data=tempField[t][i][j];
                    send(client_socket, &data, sizeof(data), 0);
                }
            }

            // Close the sockets
            close(client_socket);
            close(server_socket);
        }
    }

    // Write the field to NetCDF
    // if (rank == 0) {
    //     // writeFieldToNetCDF(tempField, numSteps, NX);
    //     char *ip="172.23.145.96";
    //     int port=5566;

    //     int server_sock, client_sock;
    //     struct sockaddr_in server_addr, client_addr;
    //     socklen_t addr_size;
    //     // double buffer[numSteps][NX][NY];
    //     int n;

    //     server_sock=socket(AF_INET,SOCK_STREAM,0);
    //     if(server_sock<0)
    //     {
    //         perror("[-]Socket error");
    //         exit(1);
    //     }
    //     printf("[+]TCP server socket created.\n");

    //     memset(&server_addr,'\0',sizeof(server_addr));
    //     server_addr.sin_family=AF_INET;
    //     server_addr.sin_port=port;
    //     server_addr.sin_addr.s_addr=inet_addr(ip);

    //     n=bind(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
    //     if(n<0)
    //     {
    //         perror("[-]Bind error");
    //         exit(1);
    //     }
    //     printf("[+]Bind to the port number: %d\n",port);

    //     listen(server_sock,5);
    //     printf("Listening...\n");

    //     while(1)
    //     {
    //         addr_size=sizeof(client_addr);
    //         client_sock=accept(server_sock,(struct sockaddr*)&client_addr,&addr_size);
    //         printf("[+]Client connected.\n");

    //         // bzero(buffer, 1024);
    //         // bzero(tempField, 1024);
    //         // recv(client_sock,buffer,sizeof(buffer),0);
    //         printf("%f,%f,%f,%f\n",tempField[0][0][0],tempField[0][0][1],tempField[2][0][0],tempField[2][0][1]);
    //         send(client_sock,tempField,sizeof(tempField),0);
    //         // send(client_sock,tempField,sizeof(double)*100*64*64,0);
    //         // printf("Client: %s\n",buffer);
    //         // printf("Client: %s\n",tempField);

    //         // bzero(tempField,1024);
    //         // char buffer 
    //         // // strcpy(tempField,"Hi, this is server.");
    //         // // printf("Server: %s\n",tempField);
    //         // send(client_sock,tempField,sizeof(tempField),0);

    //         close(client_sock);
    //         printf("[+]Client disconnected.\n\n");
    //     }
    // }
}

int main(int argc, char **argv) {
    int rank, num_processes;
    int numSteps;

    if (argc != 2) {
        printf("Usage: %s <numSteps>\n", argv[0]);
        return 1;
    }

    numSteps = atoi(argv[1]);
    if (numSteps <= 0) {
        printf("numSteps must be a positive integer.\n");
        return 1;
    }

    double field[numSteps][NX][NY];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    if (NX % num_processes != 0) {
        if (rank == 0) {
            fprintf(stderr, "Number of processes must evenly divide NX.\n");
        }
        MPI_Finalize();
        return 1;
    }

    initializeField(field, numSteps); // Initialize the initial field for all time steps

    // Distribute the initial field to all processes
    MPI_Bcast(&field[0][0][0], numSteps * NX * NY, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    simulateWeather(field, rank, num_processes, numSteps);

    printf("Weather simulation completed.\n");

    MPI_Finalize();
    return 0;
}
