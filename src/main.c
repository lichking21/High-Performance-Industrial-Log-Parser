#include <stdlib.h>

int execute(int argc, char** argv);

int main(int argc, char** argv)
{
    return execute(argc, argv) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
