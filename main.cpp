#include <iostream>
#include <getopt.h>
#include "fcm.h"

#define PROGRAM_NAME "FCM"
#define VERSION 20161010

#define print_name_version() cout << PROGRAM_NAME << " version " << VERSION << endl

using namespace std;

void print_help();

int main(int argc, char **argv) {

    if (argc == 1){
        cout << "Usage: -h for help" << endl;
        return 1;
    }

    // FCM variables
    unsigned int k = 1;             // order number
    unsigned int nc = 100;          // number of characters
    unsigned int nl = 10;           // number of lines
    double alpha = 0.0;             //avoid 0-probabilities

    ifstream indata;                // data to process
    fstream infile;                 // hashtable data file
    fstream outfile;                // hashtable save file
    fstream *p_infile = &infile;
    fstream *p_outfile = &outfile;
    string filename_save_load;
    bool printStats = false, debugMode = false;

    // Read options from arguments
    int opt;
    extern int opterr; // suppress getopt error message
    opterr = 0;

    while ((opt = getopt(argc, argv, "k:f:o:c:l:a:shvd")) != -1) {
        switch (opt) {
            case 'd':
                debugMode = true;
                break;
            case 'k': {
                if ((k = (unsigned) atoi(optarg)) == 0) {
                    cerr << "K argument '" << optarg << "' is invalid." << endl;
                    return 1;
                }
                //clog << "Using order (k): " << optarg << endl;
                break;
            }
            case 'f': {
                if (strcmp(filename_save_load.c_str(), optarg) == 0) {
                    // same file for save and loading, reusing stream
                    p_infile = p_outfile;
                } else {
                    infile.open(optarg, ios::out | ios::in);

                    if (infile.fail()) {
                        cerr << "Fail opening file '" << optarg << "' for reading" << endl;
                        return 1;
                    }

                    filename_save_load = optarg;
                }
                break;
            }
            case 'o': {
                if (strcmp(filename_save_load.c_str(), optarg) == 0) {
                    // same file for save and loading, reusing stream
                    p_outfile = p_infile;
                } else {
                    outfile.open(optarg, ios::out);

                    if (outfile.fail()) {
                        cerr << "Fail opening file '" << optarg << "' for writing" << endl;
                        return 1;
                    }

                    //clog << "Output file is: " << optarg << endl;
                }
                break;
            }
            case 's':
                /* program behaviour:
                 * run for file and print stats
                 */
                printStats = true;
                break;
            case 'c':
                if ((nc = (unsigned) atoi(optarg)) == 0) {
                    cerr << "C argument '" << optarg << "' is invalid." << endl;
                    return 1;
                }
                break;
            case 'l':
                if ((nl = (unsigned) atoi(optarg)) == 0) {
                    cerr << "L argument '" << optarg << "' is invalid." << endl;
                    return 1;
                }
                //clog << "Using order (k): " << optarg << endl;
                break;
            case 'a':
                if ((alpha = (double) stod(optarg)) == 0) {
                    cerr << "Alpha argument '" << optarg << "' is invalid." << endl;
                    return 1;
                }
                break;
            case 'h':
                print_help();
                return 0;
            case 'v':
                print_name_version();
                break;
            default:
                print_help();
                break;
        }
    }

    if (optind == argc) {
        if (printStats && infile.is_open()) {

            fcm n = fcm(k, &indata, &*p_outfile, &*p_infile, nc, nl, alpha);

            n.printStats();
            n.calculateProbabilities();
            n.genText();

            return 0;
        }

        cerr << "No file for processing specified nor printing stats from saved file" << endl;
        return 1;
    } else {
        string filename = argv[argc - 1];
        indata.open(filename);

        if (indata.fail()) {
            cerr << "Fail opening file '" << filename << "' for reading" << endl;
            return 1;
        }

        if (!debugMode) {
            clog.setstate(ios::failbit);
        }

        clog << "Using input stream for processing: " << filename << endl;

        fcm n = fcm(k, &indata, &*p_outfile, &*p_infile, nc, nl, alpha);

        if (printStats) {
            n.printStats();
            cout << "Entropy: " << n.getEntropy() << endl;
        }

        if (nl > 0) {
            cout << "Calculating probabilities... " << endl;
            n.calculateProbabilities();       //get probability matrix
            // n.printProbs(); TODO: prettify print
            cout << "Generating " << nl << " lines with " << nc << " chars:" << endl;
            n.genText();
        }

    }

    return 0;
}

void print_help() {
    print_name_version();
    cout << endl;
    cout << "Usage options:" << endl;
    cout << " -k       : specify order (default: 1)" << endl;
    cout << " -f       : read data from file (default: none)" << endl;
    cout << " -o       : save data to file (default: none)" << endl;
    cout << " -s       : print statistics (optional)" << endl;
    cout << " -c       : number of characters per line (default: 100)" << endl;
    cout << " -l       : number of lines to generate text" << endl;
    cout << " -d       : print all debug messages" << endl;
    cout << " -a       : specify alpha for probability calculation (default: 0)" << endl;
    cout << " -h       : display this help" << endl;
    cout << " (file)   : file to read from (if not specified read from stdin)" << endl;
    cout << endl;
    cout << " Example:" << endl;
    cout << " Read with order 3, load HashTable \"saved_hash\" and process \"os_maias.txt\"" << endl;
    cout << " ./fcm -k 3 -f saved_hash os_maias.txt" << endl;
    cout << endl;
    cout << " Generate text with 10 lines, each with 100 characters" << endl;
    cout << " ./fcm -k 6 -l 10 -c 100 os_maias.txt" << endl;
    cout << endl;
    cout << " Print statistics of \"os_maias.txt\"" << endl;
    cout << " ./fcm -s -k 1 os_maias.txt" << endl;
    cout << endl;
    cout << " Print stats from saved file" << endl;
    cout << " ./fcm -f save.dat -s" << endl;
}
