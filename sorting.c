#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // Include the pthread library for threading

#define SIZE    ( sizeof(list)/sizeof(*list))
#define NUM_THREADS 3

int list[] = {7, 12, 19, 3, 18, 4, 2, -5, 6, 15, 8};
int result[SIZE]  = {0};

/* structures for passing data to worker threads */ 
typedef struct
{
    int *	subArray;
    unsigned int size;
} SortingThreadParameters;


// Function to swap two elements in an array
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to partition the array and return the pivot index
int partition(int arr[], int low, int high) {
    int pivot = arr[high]; // Choose the last element as pivot
    int i = (low - 1); // Index of smaller element

    for (int j = low; j <= high - 1; j++) {
        // If current element is smaller than or equal to pivot
        if (arr[j] <= pivot) {
            i++; // Increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Function to implement the Quicksort algorithm
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        // Partition the array and get the pivot index
        int pi = partition(arr, low, high);

        // Recursively sort elements before and after the pivot
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

typedef struct
{
    SortingThreadParameters left;
    SortingThreadParameters right;
} MergingThreadParameters;

void *Sort (void *params) {
    printf("Sort\n");

    int n = ((SortingThreadParameters*)params)->size;

    printf("Unsorted sub-array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", ((SortingThreadParameters*)params)->subArray[i]);
    }
    printf("\n");

    quickSort(((SortingThreadParameters*)params)->subArray, 0, n - 1);

    printf("Sorted sub-array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", ((SortingThreadParameters*)params)->subArray[i]);
    }
    printf("\n");

    pthread_exit((void *)params);
}

void *Merge (void *params) {
    printf("Merge\n");

    int leftSize = ((MergingThreadParameters*)params)->left.size;
    int rightSize = ((MergingThreadParameters*)params)->right.size;

    int resultSize = leftSize + rightSize;
    int result[resultSize];

    int leftIndex = 0;
    int rightIndex = 0;

    printf("Merged and sorted Array: ");

    for(int i = 0; i < resultSize; ++i) {

        if(leftIndex >= leftSize) {
            result[i] = ((MergingThreadParameters*)params)->right.subArray[rightIndex];
            ++rightIndex;
        } else if (rightIndex >= rightSize) {
            result[i] = ((MergingThreadParameters*)params)->left.subArray[leftIndex];
            ++leftIndex;
        } else if(((MergingThreadParameters*)params)->left.subArray[leftIndex] < ((MergingThreadParameters*)params)->right.subArray[rightIndex]) {
            result[i] = ((MergingThreadParameters*)params)->left.subArray[leftIndex];
            ++leftIndex;
        } else {
            result[i] = ((MergingThreadParameters*)params)->right.subArray[rightIndex];
            ++rightIndex;
        }

        printf("%d ", result[i]);
    }
    printf("\n");

    pthread_exit(result);
}

int main() {

    pthread_t leftSortThread;
    pthread_t rightSortThread;
    pthread_t mergeThread;

    printf("Unsorted List: ");
    for(int i = 0; i < SIZE; ++i) {
        printf("%d ", list[i]);
    }
    printf("\n");

    SortingThreadParameters * paramsLeft =	malloc(	sizeof( SortingThreadParameters ) );
    paramsLeft->subArray	=	list;	
    paramsLeft->size	=	SIZE/2;	
    /* Now create the first sorting thread	passing	it paramsLeft as a parameter */
    // ...	
    pthread_create(&leftSortThread, NULL, Sort, paramsLeft);


    SortingThreadParameters * paramsRight =	malloc( sizeof( SortingThreadParameters ) );
    paramsRight->subArray	=	list +	paramsLeft->size;
    paramsRight->size	=	SIZE -	paramsLeft->size;  // taking difference addresses an add number of elements
    /* Now create the second sorting thread	passing it paramsRight as a parameter */
    // ...			
    pthread_create(&rightSortThread, NULL, Sort, paramsRight);


    //	wait for the sorting threads to complete
    // ...
    void *returnLeft;
    void *returnRight;

    pthread_join(leftSortThread, &returnLeft);
    pthread_join(rightSortThread, &returnRight);

    paramsLeft = (SortingThreadParameters*) returnLeft;
    paramsRight = (SortingThreadParameters*) returnRight;

    MergingThreadParameters * paramsMerge = malloc( sizeof( MergingThreadParameters ) );

    paramsMerge->left	= *paramsLeft;
    paramsMerge->right = *paramsRight;

    /* Now create the merging thread passing it paramsMerge as a parameter */ 
    // ...
    pthread_create(&mergeThread, NULL, Merge, paramsMerge);

    pthread_join(mergeThread, NULL);

    return 0;
}
