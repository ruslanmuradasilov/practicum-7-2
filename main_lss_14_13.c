#include "lss_14_13.h"
#include <time.h>

int fl_d = 0, fl_e = 0, fl_p = 0, fl_t = 0, fl_h = 0, fl_q = 0;

int _strlen(char const *input)
{
    int length = 0;
    while (input[length] != '\0')
        length++;
    return length;
}

int validateFile(char *filename)
{
    FILE *f = fopen(filename, "rb");
    return f != NULL;
}

int validateParameters(int argc, char **argv)
{
    int i, cnt = 0;
    if (argc == 1)
        return 0;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && _strlen(argv[i]) == 2)
            switch (argv[i][1])
            {
                case 'd':
                    fl_d = 1;
                    break;
                case 'e':
                    fl_e = 1;
                    break;
                case 'p':
                    fl_p = 1;
                    break;
                case 't':
                    fl_t = 1;
                    break;
                case 'h':
                    fl_h = 1;
                    break;
                case '?':
                    fl_q = 1;
                default:
                    return 2;
            }
        else
            cnt++;
    }

    if (cnt > 2)
        return 1;

    return 0;
}

int readInputData(char *inputFile, double **A, double **B, int *n)
{
    int i, checkInput;
    FILE *in = fopen(inputFile, "r");

    checkInput = fscanf(in, "%d", n);
    if (checkInput == EOF)
        return 6;

    if (checkInput == 0)
        return 7;

    if (*n <= 0)
        return 7;

    *A = malloc((*n) * (*n) * sizeof(double));
    *B = malloc((*n) * sizeof(double));

    for (i = 0; i < (*n) * (*n); i++)
    {
        checkInput = fscanf(in, "%lf", (*A + i));
        if (checkInput == EOF)
            return 8;
        if (checkInput == 0)
            return 9;
    }
    for (i = 0; i < (*n); i++)
    {
        checkInput = fscanf(in, "%lf", (*B + i));
        if (checkInput == EOF)
            return 8;
        if (checkInput == 0)
            return 9;
    }

    return 0;
}

void writeOutputData(char *outputFile, int n, const double *X, int result)
{
    int i;
    FILE *out = fopen(outputFile, "w");
    if (result)
        fprintf(out, "%d\n", 0);
    else
    {
        fprintf(out, "%d\n", n);
        for (i = 0; i < n; i++)
            fprintf(out, "%1.9lf\n", *(X + i));
    }
}

void print(int n, const double *A)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            printf("%1.9lf ", *(A + i * n + j));

        printf("\n");
    }
    printf("\n");
}

void printHelp()
{
    printf("Usage: lss [input_file_name] [output_file_name] [options]\n"
           "Where options include:\n"
           " -d                print debug messages [default OFF]\n"
           " -e                print errors [default OFF]\n"
           " -p                print matrix [default OFF]\n"
           " -t                print execution time [default OFF]\n"
           " -h, -?            print this and exit");
}

int main(int argc, char *argv[])
{
    int n, i, rtn, setInput = 0;
    double *A, *B, *X, *tmp;
    char *inputFile = "lss_14_13_in.txt";
    char *outputFile = "lss_14_13_out.txt";

    switch (validateParameters(argc, argv))
    {
        case 1:
            if (fl_e) printf("ValidationError: Wrong syntax of parameters. There are more than two filenames\n");
            return 2;
        case 2:
            if (fl_e)
                printf("ValidationError: Wrong syntax of parameters. There is no such parameter or you haven't"
                       " set value to one of the parameters\n");
            return 3;
        default:
            break;
    }

    if (fl_q || fl_h)
    {
        printHelp();
        return 0;
    }

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            if (!setInput)
            {
                if (i != 1)
                {
                    if (fl_e) printf("ValidationError: Wrong order of parameters.\n");
                    return 4;
                }
                inputFile = argv[i];
                if (!validateFile(inputFile))
                {
                    if (fl_e) printf("ValidationError: There is no such file.\n");
                    return 5;
                }
                setInput = 1;
            } else
            {
                if (i != 2)
                {
                    if (fl_e) printf("ValidationError: Wrong order of parameters.\n");
                    return 4;
                }
                outputFile = argv[i];
            }
        }
    }

    if (!setInput)
    {
        if (!validateFile(inputFile))
        {
            if (fl_e) printf("ValidationError: There is no such file.\n");
            return 5;
        }
    }

    switch (readInputData(inputFile, &A, &B, &n))
    {
        case 6:
            if (fl_e) printf("ValidationError: File is empty.\n");
            return 6;
        case 7:
            if (fl_e) printf("ValidationError: n is not a positive integer.\n");
            return 7;
        case 8:
            if (fl_e) printf("ValidationError: Not enough elements in the matrix.\n");
            return 8;
        case 9:
            if (fl_e) printf("ValidationError: One of the elements of the matrix is not a number.\n");
            return 9;
        default:
            break;
    }

    X = (double *) malloc(n * sizeof(double));
    tmp = malloc((size_t) lss_memsize_14_13(n));

    clock_t start = clock();
    rtn = lss_14_13(n, A, B, X, tmp);
    clock_t end = clock();

    if (rtn == -1)
        if (fl_e) printf("ValidationError. Matrix is not symmetric.\n");
    if (rtn == -2)
        if (fl_e) printf("ValidationError. Zero leading minor is found.\n");

    if (fl_p) print(n, A);

    if (fl_t) printf("Execution time: %f\n", ((double) (end - start)) / CLOCKS_PER_SEC);

    writeOutputData(outputFile, n, X, rtn);

    free(A);
    free(B);
    free(X);
    free(tmp);

    return rtn;
}