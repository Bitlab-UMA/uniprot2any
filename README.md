# uniprot2any

This is a simple C++ program to convert the IDs of matches found with BLASTX on a UNIPROT database search to any other ID provided by official mapping files.

Usage is very simple, download the mapping file from:

ftp://ftp.uniprot.org/pub/databases/uniprot/current_release/knowledgebase/idmapping

(At time of writing, it is the file named "idmapping.dat.gz")

Unzip the file and then you can run the program as follows:

uniprot2gi idmapping.dat input.blastx GI output.blastx

where idmapping.dat corresponds to the file you just unzipped and input.blastx is your blastx file with uniprot IDs. The 3rd field, "GI" is used to specify to which type you wish to convert, e.g. you could also run:

uniprot2gi idmapping.dat input.blastx KEGG output.blastx

or any other. The last file is the output file with the converted IDs. Those IDs that are not found in the mapping file are left untouched.

Happy converting!

