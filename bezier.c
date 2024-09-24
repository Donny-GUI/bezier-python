#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <numpy/arrayobject.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


#define MIN_SPEED 600.0 // Minimum speed in pixels per second
#define MAX_SPEED 800.0 // Maximum speed in pixels per second

// Function to generate a random speed within the defined range
double get_random_speed() {
    return MIN_SPEED + (rand() / (RAND_MAX + 1.0)) * (MAX_SPEED - MIN_SPEED);
}

// Function to generate Pascal's triangle row
static void pascal_row(int n, int *row) {
    row[0] = row[n] = 1;
    for (int k = 1; k < n; k++) {
        row[k] = row[k - 1] * (n - k) / k;
    }
}

/**
 * @brief Returns a random integer between min and max inclusive
 * @param[in] min Minimum value of the range
 * @param[in] max Maximum value of the range
 * @return A random integer between min and max inclusive
 */
int randIntBetween(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}
float randomHundredthFloat() {
    return 0.01f + ((float)rand() / (float)RAND_MAX) *( 0.05f - 0.01f);  // Scale to [0, 0.5]
}

// Combined function to generate Bézier curve points
static PyObject* calculate_curve(PyObject *self, PyObject *args) {
    double x1, y1, x2, y2, deviation;

    // Parse the input arguments
    if (!PyArg_ParseTuple(args, "(dd)(dd)dd", &x1, &y1, &x2, &y2, &deviation)) {
        return NULL;
    }


    // Calculate the distance between initial and final points
    double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    double max_deviation = deviation * distance;

    // Generate control points
    double control_1_x = x1 + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;
    double control_1_y = y1 + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;
    double control_2_x = x2 + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;
    double control_2_y = y2 + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;

    // Create output array for Bézier curve points
    npy_intp dims[2] = {101, 2}; // 101 points, 2 coordinates (x, y)
    PyObject *points_array = PyArray_SimpleNew(2, dims, NPY_DOUBLE);

    // Handle error in array creation
    if (points_array == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create NumPy array.");
        return NULL;
    }

    // Get a pointer to the array data
    double *points = (double *)PyArray_DATA(points_array);

    // Prepare Pascal's triangle row
    int num_control_points = 4; // We have 4 control points
    int *row = (int *)malloc(num_control_points * sizeof(int));
    
    // malloc error handle
    if (row == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to allocate memory for Pascal's triangle row.");
        return NULL;
    }

    pascal_row(num_control_points - 1, row);

    // Calculate Bézier curve points
    for (int t_index = 0; t_index <= 100; t_index++) {
        double t = t_index / 100.0;
        double x = 0.0;
        double y = 0.0;

        // Calculate point coordinates using the Bézier formula
        for (int i = 0; i < num_control_points; i++) {
            double coeff = row[i] * pow(t, i) * pow(1 - t, num_control_points - 1 - i);
            if (i == 0) {
                x += coeff * x1;
                y += coeff * y1;
            } else if (i == 1) {
                x += coeff * control_1_x;
                y += coeff * control_1_y;
            } else if (i == 2) {
                x += coeff * control_2_x;
                y += coeff * control_2_y;
            } else if (i == 3) {
                x += coeff * x2;
                y += coeff * y2;
            }
        }

        // Store point coordinates in the output array
        points[2 * t_index] = x;
        points[2 * t_index + 1] = y;
    }

    free(row);
    return points_array;
}

// Method definitions for the module
static PyMethodDef BezierMethods[] = {
    {"calculate_curve", calculate_curve, METH_VARARGS, "Calculate Bézier curve points from initial and final points, with deviation"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef beziermodule = {
    PyModuleDef_HEAD_INIT,
    "bezier",   // name of module
    NULL, // module documentation
    -1,       // size of per-interpreter state of the module
    BezierMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit_bezier(void) {
    srand(time(0)); // Seed the random number generator once during module initialization
    import_array(); // Necessary for NumPy
    return PyModule_Create(&beziermodule);
}