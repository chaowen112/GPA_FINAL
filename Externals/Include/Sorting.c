//
//  Sorting.c
//  OpenGL 4
//
//  Created by Michael on 2/7/14.
//  Copyright (c) 2014 michael. All rights reserved.
//

#include <stdio.h>

void printArray(float *a, int n)
{
    printf("Data\n");
    for (int i=0;i<n;i++)
        printf("%f\n", a[i]);
}

void printA(int *a, int n)
{
    printf("Elements\n");
    for (int i=0;i<n;i++)
        printf("%i\n", a[i]);
}

//a = array, i and j are elements of the array
void swapElements(float *a, int i, int j)
{
    float t = a[i];
    a[i] = a[j];
    a[j] = t;
}

void swapElementsInt(int *a, int i, int j)
{
    int t = a[i];
    a[i] = a[j];
    a[j] = t;
}

//a = array, b = element array that also needs to be sorted, n = length of array
void insertionSort(float *a, int*b, int n)
{
    for (int i=1;i<n;i++)
    {
        for (int k=i;k>0 && a[k]<a[k-1];k--)
        {
            swapElements(a, k, k-1);
            swapElementsInt(b, k, k-1);
        }
    }
}