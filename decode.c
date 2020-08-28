#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
#include <math.h>
#include <unistd.h>
extern int huffman_decode(char *data, char *result, unsigned int result_size, unsigned int encoding_size,char *tree);

void printTree(char *tree, short unsigned int number_of_nodes) {

};

int main(int argc, char **argv) {
	printf("tree should be in tree.bin, result in encoding.bin");
	if (argc < 3) {
		printf("Error: missing arguments. Expected: code length (gotten from encode), result size.\n");
		return -1;
	}
	char *endptr;
	long long result_size_input = strtoll(argv[2], &endptr, 10);
	if (!(endptr[0] == 0)) {
		printf("Error: the third argument must be an unsigned int, but was not a valid number.\n");
		return -1;
	}
  long long encode_size_input = strtoll(argv[1], &endptr, 10);
  if (!(endptr[0] == 0)) {
    printf("Error: the third argument must be an unsigned int, but was not a valid number.\n");
    return -1;
  }
	if (result_size_input < 0 || result_size_input > UINT_MAX) {
		printf("Error: the third argument must be an unsigned int, but was out or range. Please enter a number between 19 and %u\n", UINT_MAX);
	}
	unsigned int result_size = (unsigned int) result_size_input;
  unsigned int encode_size = (unsigned int) encode_size_input;
	if (result_size < 19) {
		printf("Error: result cannot be smaller than 19.\n");
		return -1;
	}
        //char* output = malloc(sizeof(char)*result_size);
   int encode_file = open("encoding.bin",O_RDONLY), tree_file = open("tree.bin",O_RDONLY);
	char *data = malloc(ceil(encode_size/64.0)*8*sizeof(char));
  if (read(encode_file,data,ceil(encode_size/64.0)*8) == -1)
		abort;
	char result[result_size];
  char tree [5000];
  if ( read(tree_file,tree,5000) == -1)
	abort();
  close(encode_file);
  close(tree_file);
	int e = huffman_decode(data, result, result_size, encode_size, tree);

	 if (e == -1) {
		printf("Error: not enough place in result, maximum size of result is too small.\n");
		return -1;
	}
	 else{
		 printf("Sucessfully decoded:\n %s\n",result);
	 }
	return 0;
}
