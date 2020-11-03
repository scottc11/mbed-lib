#include "ArrayMethods.h"

int arr_max(int arr[], int n)
{
    int i;

    // Initialize maximum element
    int max = arr[0];

    // Traverse array elements from second and
    // compare every element with current max
    for (i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];

    return max;
}

int arr_min(int arr[], int n)
{
    int i;

    // Initialize min element
    int min = arr[0];

    // Traverse array elements from second and
    // compare every element with current min
    for (i = 1; i < n; i++)
        if (arr[i] < min)
            min = arr[i];

    return min;
}

int arr_average(int arr[], int n)
{
    // Find sum of array element
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += arr[i];

    return sum / n;
}


// Returns element closest to target in arr[] 
int arr_find_closest_int(int arr[], int n, int target) 
{ 
    // Corner cases 
    if (target <= arr[0]) {
        return arr[0];
    }
    if (target >= arr[n - 1]) {
        return arr[n - 1];
    }

    // Doing binary search 
    int i = 0, j = n, mid = 0; 
    while (i < j) { 
        mid = (i + j) / 2; 
  
        if (arr[mid] == target) 
            return arr[mid]; 
  
        /* If target is less than array element, 
            then search in left */
        if (target < arr[mid]) { 
  
            // If target is greater than previous 
            // to mid, return closest of two 
            if (mid > 0 && target > arr[mid - 1]) 
                return getClosestInt(arr[mid - 1], 
                                  arr[mid], target); 
  
            /* Repeat for left half */
            j = mid; 
        } 
  
        // If target is greater than mid 
        else { 
            if (mid < n - 1 && target < arr[mid + 1]) 
                return getClosestInt(arr[mid], 
                                  arr[mid + 1], target); 
            // update i 
            i = mid + 1;  
        } 
    } 
  
    // Only single element left after search 
    return arr[mid]; 
}

// Returns element closest to target in arr[]
float arr_find_closest_float(float arr[], int n, float target)
{
    // Corner cases
    if (target <= arr[0]) {
        return arr[0];
    }
        
    if (target >= arr[n - 1]) {
        return arr[n - 1];
    }

    // Doing binary search
    int i = 0, j = n, mid = 0;
    while (i < j)
    {
        mid = (i + j) / 2;

        if (arr[mid] == target)
            return arr[mid];

        /* If target is less than array element, 
            then search in left */
        if (target < arr[mid])
        {

            // If target is greater than previous
            // to mid, return closest of two
            if (mid > 0 && target > arr[mid - 1]) {
                return getClosestFloat(arr[mid - 1], arr[mid], target);
            }

            /* Repeat for left half */
            j = mid;
        }

        // If target is greater than mid
        else
        {
            if (mid < n - 1 && target < arr[mid + 1]) {
                return getClosestFloat(arr[mid], arr[mid + 1], target);
            }
                
            // update i
            i = mid + 1;
        }
    }

    // Only single element left after search
    return arr[mid];
}

// Method to compare which one is the more close. 
// We find the closest by taking the difference 
// between the target and both values. It assumes 
// that val2 is greater than val1 and target lies 
// between these two.
int getClosestInt(int val1, int val2, int target)
{ 
    if (target - val1 >= val2 - target) 
        return val2; 
    else
        return val1; 
}

float getClosestFloat(float val1, float val2, float target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}