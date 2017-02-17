#ifndef CAV_GMZ_FCM_H
#define CAV_GMZ_FCM_H


#include <iostream>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>
#include <boost/circular_buffer.hpp>
#include <numeric>
#include <random>
#include <boost/foreach.hpp>
//serialization includes
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

using namespace std;
using namespace boost;

class fcm {
public:

    /**
    * FCM constructor
    * @param order to process the text
    * @param input_file stream with the data to process
    * @param save_file if given, save the results of the processing to a file
    * @param load_file if given, load previous processing from a file
    * @param number_characters number of characters of generated text to print every line
    * @param number_lines number of lines of generated text to print
    * @param probl_alpha probability estimation alpha value
    * @return none
    */
    fcm(unsigned int order, ifstream *input_file, fstream *save_file, fstream *load_file,
        unsigned int number_characters, unsigned int number_lines, double probl_alpha);


    /**
     * Returns the corrent counter for a symbol given a context
     * @param letter symbol to look for
     * @param context to query
     * @return occurrences
     */
    unsigned int getSymbol(const char letter, const char *context);

    /**
    * Converts the statistics of each occurrence to probabilities.
    * The probability matrix will provide the chance of a certain symbol to occur.
    */
    void calculateProbabilities();

    /**
     * Calculates the accumulated entropy of the text already processed using the foruma ∑ Hi*Pi, where Hi is -Sum from
     * a to z of P(i)*Log2(P(i)) and P(i) : probability of the occurece of the symbol
     * @return accumulated entropy
     */
    double getEntropy();

    /**
     * Looks up the symbools table and prints to stdout in a table format
     */
    void printStats();

    /**
     * Prints the generated probability matrix
     */
    void printProbs();

    /**
     * Generates text acording to the results gathered from the analyzed sources.
     * The statistical matrix will provide the relation between each symbol and the respective chance of occurrence
     */
    void genText();


private:

    /**
     * Statistic matrix that contains the counter occurence for each symbol of the alphabet for a given context.
     * Therefore, each line represents a context and columns represent the symbol.
     */
    unique_ptr<map<unsigned int, vector<unsigned int>>> p_statMatrix;

    /**
     * Analogous matrix to statMatrix, but the values are the computed probability of a symbol for the given context.
     * This matrix is calculated using calculateProbabilities()
     */
    map<unsigned int, vector<double> > probMatrix; //probabilities matrix, obtained through the latter

    /**
     * Matrix containing statistical information computed from statMatrix. Each line represents a context and column
     * has a pair with the sum of the line and the probability of that line
     */
    map<unsigned int, pair<unsigned int, double>> sum_stat_Matrix;

    /**
     * Stores the most occurring occurrence and it's context
     */
    pair<unsigned int, double> most_occurring;

    /**
     * Context order
     */
    unsigned int k;

    /**
     * Number of lines to generate by the text generator
     */
    unsigned int numLines;

    /**
     * Number of chars per generated line by the text generator
     */
    unsigned int numChar;

    /**
     * Stream containing the input file to be processed
     */
    ifstream *input;

    /**
     * Stream to be used to save the databases
     */
    fstream *outfile;

    /**
     * Stream to read and load an stored database
     */
    fstream *infile;

    /**
     * Alpha value to be used in the computed probabilities
     */
    double alpha;


    /**
     * Generates a new matrix containing two rows: the sum of the line and the probability of that line
     * according to current context
     */
    void generate_sum_prob_Matrix();

    /**
    * 'Adding the occurence of a symbol to the model (updating the corresponding conditioning context)
    * This function looks for the input and process it to a given context of order 'k'
    */
    void occurrenceCounter();

    /**
     * Converts an character to our alphabet code using ASCII decimal values as reference
     * @param letter to convert to decimal value according to alphabeet
     * @return the decimal code if possible and -1 when out of range
     */
    int charToAlphabet(char letter);

    /**
     * Calculates the index in the map according the an order n using c character
     * the formula to calculate index is: index = c1^n + c2^n-1 + ... + c
     * Note that c1 should be converted according to charToAlphabet()
     * @param buffer buffer of symbols including, context (symbol, if included, is discarded)
     * @return index for the given context
     */
    unsigned int mapPosCalc(circular_buffer<char> buffer);

    /**
     * Loads p_statMatrix from specified file
     * @param s ofsteram to the file to load
     * @return 0 if save successsfull 1 if unsuccesful
     */
    int load(fstream *s);

    /**
     * Saves p_statMatrix to the specified file
     * @param s ifstream to save
     * @return 0 if save successsfull 1 if unsuccesful
     */
    int save(fstream *s);

    /**
     * Decodes an enocoded string for printing stats table
     * @param number with encoded string
     * @return string of the encoded number
     */
    string reverse_mapPosCalc(unsigned int number);

};

#endif //CAV_GMZ_FCM_H
