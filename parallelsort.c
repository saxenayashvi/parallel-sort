#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Function to perform quicksort
void quicksort(int arr[], int left, int right) {
    if (left < right) {
        int pivot = arr[left];
        int i = left, j = right;
        while (i < j) {
            while (i < j && arr[j] >= pivot)
                j--;
            if (i < j)
                arr[i++] = arr[j];
            while (i < j && arr[i] <= pivot)
                i++;
            if (i < j)
                arr[j--] = arr[i];
        }
        arr[i] = pivot;
        quicksort(arr, left, i - 1);
        quicksort(arr, i + 1, right);
    }
}

int Main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    if (rank == 0) {
        // Get the size of the array from the user
        printf("Enter the size of the array: ");
        scanf("%d", &n);
    }

    // Broadcast the size of the array to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = n / size;
    int* local_arr = (int*)malloc(local_n * sizeof(int));
    int* arr = NULL;

    if (rank == 0) {
        // Allocate memory for the array and get the elements from the user
        arr = (int*)malloc(n * sizeof(int));
        printf("Enter %d elements of the array:\n", n);
        for (int i = 0; i < n; i++) {
            scanf("%d", &arr[i]);
        }
    }

    // Scatter the data to all processes
    MPI_Scatter(arr, local_n, MPI_INT, local_arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform local quicksort
    quicksort(local_arr, 0, local_n - 1);

    // Gather the sorted data back to the root process
    MPI_Gather(local_arr, local_n, MPI_INT, arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Only the root process prints the sorted array
    if (rank == 0) {
        printf("Sorted Array:\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }

    free(local_arr);
    if (rank == 0) {
        free(arr);
    }

    MPI_Finalize();

    return 0;
}