#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include<string.h>
extern int huffman_encode(char *data, char *result, unsigned int result_size, char *frequencies, char *node_indexes,char *output);

void printNode(char *dest, char *node, unsigned char number_of_places) {
        if (node[0] == 0) // if inner node
                sprintf(dest, "[%lu", ((unsigned long *) (node + 4))[0]);// first bite is the char, next three are useles, only to optimize the addressig
                                                                          //(node start address multiple of 16 better than13), then comes 8 bytes for frequency
                                                                          // (we did not want to limit the size of the data to encode), then 2 ints representing
                                                                          //the index of the children in our list of nodes (called tree here)
        else {
                switch (node[0]) {
                        case 0:
                                sprintf(dest, "[\\0, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        case 7:
                                sprintf(dest, "[\\a, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        case 8:
                                sprintf(dest, "[\\b, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        case 9:
                                sprintf(dest, "[\\t, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        case 10:
                                sprintf(dest, "[\\n, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        case 11:
                                sprintf(dest, "[\\v, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        case 12:
                                sprintf(dest, "[\\f, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        case 13:
                                sprintf(dest, "[\\r, %lu", ((unsigned long *) (node + 4))[0]);
                                break;
                        default:
                                sprintf(dest, "[%c, %lu", node[0], ((unsigned long *) (node + 4))[0]);
                }
        }
        for (int i = (int) strlen (dest); i < (int) number_of_places - 1; i++)
                dest[i] = ' '; //fill with spaces untill desired width is reached
        dest[number_of_places - 1] = ']'; //close node
        dest[number_of_places] = '\0'; //close string

};

void fillGrid(char *subtree, unsigned short index_of_root, unsigned short grid[][1022], unsigned short level, unsigned short place_of_node, unsigned short displacement, unsigned short *min_place) {
        // displacement is how much to go right or left to draw the children at the below level
        if (displacement == 0)
                displacement = 1;
        grid[level][place_of_node] = 1 + index_of_root; // 1 + so that 0 can mean empty, while index 0 becomes 1
        if (subtree[index_of_root << 4] == 0) { //if root of subtree is inner node call rekursiv for the two children as roots of their own subtrees at the level below
                fillGrid(subtree, ((unsigned short *) (subtree + (index_of_root << 4) + 12))[0], grid, level + 1, place_of_node - displacement, displacement >> 1, min_place);
                fillGrid(subtree, ((unsigned short *) (subtree + (index_of_root << 4) + 12))[1], grid, level + 1, place_of_node + displacement, displacement >> 1, min_place);
        } else if (place_of_node < *min_place) //if leaf with smaller place than min_place
                *min_place = place_of_node;
}; // result is like a grid, each node (its index) goes in the correct grid square

unsigned short countLeaves(char *tree) {
        unsigned short i = 0;
        while (tree[(i << 4)] != 0) // only inner nodes have 0 for a character
                i++;
        return i;
}; // leaves are always at the top of the list of nodes

void printTree(char *tree, unsigned short number_of_nodes, unsigned long max_frequency) {
        unsigned short number_of_leaves = countLeaves(tree);
        static unsigned short grid[255][1022]; // worst case of height is like in the exercise announcement (number of leaves as height), for width is the full binary tree (also number of leaves as width,
                                               // with leaves spaced to have place for the father node above them) (255*4 cause we need min 1 displacement between children and father at the lowest level,
                                               // so top displacement should be equal to the max possible height (for a specified # of nodes), and length of top line is is 4*topdisplacement
        unsigned short min_place = 1022;
        fillGrid(tree, number_of_nodes - 1, grid, 0, 2 * number_of_leaves, number_of_leaves, &min_place); // root at the middle of the top level
        _Bool draw_edge = 0;
        int width_of_node_representation = 7 + log10l((long double) max_frequency);
        unsigned int char_counter = 0; // number of grid cells in current line
        for (unsigned short level = 0; level < number_of_leaves; level++) {
                //draw nodes
                for (unsigned short place = 0; place < 4 * number_of_leaves + 2; place++) {
                        if (grid[level][place] == 0) {
                                if (level < 254 && grid[level + 1][place] != 0) {
                                        draw_edge = !draw_edge;
                                        if (draw_edge) {
                                                if (char_counter >= min_place)
                                                        for (int i = 0; i < width_of_node_representation; i++)
                                                                printf(" ");
                                                char_counter++;
                                                continue;
                                        }
                                }
                                if (char_counter >= min_place)
                                        for (int i = 0; i < width_of_node_representation; i++)
                                                printf(draw_edge ? "_" : " ");
                                char_counter++; //this is to make the tree stick to the left of the screen, ignore the empty section in the left of the grid
                        } else {
                                char node[width_of_node_representation + 1]; // string representing the node
                                printNode(node, tree + ((grid[level][place] - 1) << 4), (unsigned char) width_of_node_representation);
                                printf("%s", node);
                                char_counter++;
                        }
                }
                //draw vertical lines at the next line
                char_counter = 0;
                printf("\n");
                for (unsigned short place = 0; place < 4 * number_of_leaves + 2; place++) {
                        if (grid[level][place] == 0) {
                                if (level < 254 && grid[level + 1][place] != 0) {
                                        draw_edge = !draw_edge;
                                        if (draw_edge) {
                                                if (char_counter >= min_place)
                                                        for (int i = 0; i < width_of_node_representation - 1; i++)
                                                                printf(" ");
                                                printf("/");
                                                char_counter++;
                                                continue;
                                        } else {
                                                printf("\\");
                                                if (char_counter >= min_place)
                                                        for (int i = 0; i < width_of_node_representation - 1; i++)
                                                                printf(" ");
                                                char_counter++;
                                                continue;
                                        }
                                }
                                if (char_counter >= min_place)
                                        for (int i = 0; i < width_of_node_representation; i++)
                                                printf(" ");
                                char_counter++; //this is to make the tree stick to the left of the screen, ignore the empty section in the left of the grid
                        } else {
                                for (int i = 0; i < width_of_node_representation; i++)
                                        printf(" ");
                                char_counter++;
                        }
                }
                printf("\n");
                char_counter = 0;
        }
}; // creates the tree grid and prints it out usind printNode(), but leaves empty space in each line according to the grid, the nodes would fall into the right place

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Error: missing arguments. Expected: string to encode, result to save the code in, and the maximum size of result.\n");
		return -1;
	}
	char *endptr;
	long long result_size_input = strtoll(argv[3], &endptr, 10);
	if (!(endptr[0] == 0)) {
		printf("Error: the third argument must be an unsigned int, but was not a valid number.\n");
		return -1;
	}
	if (result_size_input < 0 || result_size_input > UINT_MAX) {
		printf("Error: the third argument must be an unsigned int, but was out or range. Please enter a number between 19 and %u\n", UINT_MAX);
	}
	unsigned int result_size = (unsigned int) result_size_input;
	if (result_size < 19) {
		printf("Error: result cannot be smaller than 19.\n");
		return -1;
	}
        //char* output = malloc(sizeof(char)*result_size);
	char *data = argv[1];
 char result[result_size];
        char output [result_size];
	static char frequencies[2048]; //256 qwords
 	static char node_indexes[512]; // max 255 16 byte leaves, then merge two, one less address, and so on
	int e = huffman_encode(data, result, result_size, frequencies, node_indexes, output);
	printTree(output + 2, ((unsigned short *) output)[0], (unsigned long) strlen(data));
	printf("\nLine wrap should be off for the tree to diplay correctly\n");
	 if (e == -1) {
		printf("Error: not enough place in result, maximum size of result is too small.\n");
		return -1;
	}
	 else {
		 int encode_file = open("encoding.bin",O_WRONLY | O_CREAT,0666), tree_file = open("tree.bin",O_WRONLY | O_CREAT,0644);
		 if (write(encode_file,result,ceil(e/64.0)*8) == -1)
			abort();
		 if(write(tree_file,output,result_size) == -1)
			abort();
		 close(encode_file);
		 close(tree_file);
		 printf("Sucessfully encoded, result is in file encoding.bin, tree in tree.bin\n Code length = %d\n",e);
		 
	 }
	return 0;
}
