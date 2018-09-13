#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <iostream>
#include <string.h>
#include <ctype.h>

/*

// Type-ID examples

UniProtKB-ID
Gene_ORFName
GI

*/


void load_map_file(FILE * mapfile, std::unordered_map<std::string, std::string> * ht, char * type_ID) {
    char buffer[1024];
    char s1[1024], s2[1024], s3[1024];
    s1[0] = s2[0] = s3[0] = '\0';

    fseek(mapfile, 0, SEEK_END);
    size_t size = ftell(mapfile);
    if(size == 0) { fprintf(stderr, "Error, Mapfile is empty\n"); exit(-1); }

    uint64_t loaded = 0;
    size_t prev = 0;
    rewind(mapfile);
    std::cout << "File has " << size << " bytes, or " << size / (1024 * 1024) << " MBs\n";
    while(!feof(mapfile)) {
        char * ptr = fgets(buffer, 1024, mapfile);
        if(buffer[0] != '\0' && ptr != NULL) {
            loaded += (uint64_t) strlen(buffer);
            if((loaded*100 / (uint64_t) size) > prev) { fprintf(stdout, "Loaded %d %%\n", (int) (loaded*100 / size)); prev = loaded * 100 / size; }
            sscanf(buffer, "%s %s %s", s1, s2, s3);
            std::string str1(s1);
            std::string str2(s2);
            std::string str3(s3);
            // Check the type ID
            if(strcmp(str2.c_str(), type_ID) == 0) {
                //std::cout << str1 << " " << str2 << " " << str3 << std::endl;
                std::pair<std::string, std::string> tuple(str1, str3);
                ht->insert(tuple);
            }
        }
    }
}

void extract_token(char * s, uint64_t slen, char delimiter, char * token) {

    uint64_t i = 0, j = 0;
    while(i < slen && s[i] != delimiter) ++i;
    ++i;
    while(i < slen && s[i] != delimiter) {
        token[j++] = s[i++];
    }
}

char * get_lowercase(char * a) {
    uint64_t i = 0;
    char * b = (char *) malloc(strlen(a));
    while(a[i] != '\0'){
        b[i] = tolower(a[i]);
        ++i;
    }
    return b;
} 

void associate(FILE * blastx, std::unordered_map<std::string, std::string> * ht, FILE * output, char * type_ID) {
    char buffer[1024]; buffer[0] = '\0';
    char * lowercase_type_ID = get_lowercase(type_ID);
    unsigned char delete_next_line = 0;

    while(!feof(blastx)) {
        if(fgets(buffer, 1024, blastx) > 0) {
            
            if(delete_next_line == 1) {
                // Just do not print anything until we find Length=
                if(strncmp(&buffer[0], "Length=", 7) == 0) {
                    delete_next_line = 0;
                    fprintf(output, "%s", buffer);
                }
            }
            else if(buffer[0] == '>') {
                // This has to be re-written
                delete_next_line = 1;
                // Find hash value 

                char token[1024]; token[0] = '\0';
                extract_token(buffer, strlen(buffer), '|', token);
                std::string input(token);

                std::unordered_map<std::string, std::string>::const_iterator tuple = ht->find(input);
                if (tuple != ht->end()) fprintf(output, "> %s|%s| \n", lowercase_type_ID, tuple->second.c_str());
                else fprintf(output, "%s", buffer);

            } else {
                fprintf(output, "%s", buffer);
            }
        }
    }
    free(lowercase_type_ID);
}


int main(int argc, char ** av) {

    if(argc != 5) { fprintf(stderr, "Bad argument(s), use: ./uniprot2gi <mapfile> <blastx> <typeID> <output>\n"); exit(-1); }

    FILE * mapfile, * blastx, * output;
    char type_ID[64]; type_ID[0] = '\0';
    mapfile = fopen(av[1], "rt");
    if(mapfile == NULL) { fprintf(stderr, "Can not open mapping file"); exit(-1); }
    blastx = fopen(av[2], "rt");
    if(blastx == NULL) { fprintf(stderr, "Can not open blastx file"); exit(-1); }
    strcpy(type_ID, av[3]);
    output = fopen(av[4], "wt");
    if(output == NULL) { fprintf(stderr, "Can not open output file"); exit(-1); }

    fprintf(stdout, "Loading file %s with type-id %s\n", av[1], type_ID);
    

    // Hash table to store associations
    // Make it string, string to be able to save any type of association
    std::unordered_map<std::string, std::string> * ht = new std::unordered_map<std::string, std::string>();
    load_map_file(mapfile, ht, type_ID);
    fclose(mapfile);

    fprintf(stdout, "Associating UNIPROT ids to %s\n", type_ID);
    associate(blastx, ht, output, type_ID);
    fclose(blastx);
    fclose(output);

    delete ht;

    
    fprintf(stdout, "Done\n");
    
    return 0;
}
