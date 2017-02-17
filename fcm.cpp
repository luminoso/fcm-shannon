#include <stdlib.h>
#include "fcm.h"

#define ALPHABET "abcdefghijklmnopqrstuvwxyz "
#define ALPHABET_LENGTH 27
#define LOWER_DECIMAL_ASCII_LETTER 97
#define PROBABILITY(occurrences, total, alpha) ( (occurrences + alpha) / (total + (ALPHABET_LENGTH*alpha)))

fcm::fcm(unsigned int order, ifstream *input_file, fstream *save_file, fstream *load_file,
         unsigned int number_characters, unsigned int number_lines, double probl_alpha) : k(order),
                                                                                          input(input_file),
                                                                                          outfile(save_file),
                                                                                          infile(load_file),
                                                                                          numChar(number_characters),
                                                                                          numLines(number_lines),
                                                                                          alpha(probl_alpha) {
    clog << "FCM initialized" << endl;

    p_statMatrix = make_unique<map<unsigned int, vector<unsigned int>>>();
    most_occurring = make_pair(0, 0.0);

    if (load_file->is_open()) {
        save_file->seekp(0);
        load(load_file);
        //finput->close();
    }

    // process input data
    occurrenceCounter();
    generate_sum_prob_Matrix();

    if (save_file->is_open()) {
        save_file->clear();
        save_file->seekp(0);
        save(save_file);
        save_file->close();
    }

    clog << "FCM initialized, NC|NL" << number_characters << "|" << number_lines << endl;
}

int fcm::save(fstream *s) {
    clog << "Saving map to file... ";

    if (s->fail()) {
        cerr << "fail";
        return 1;
    }
    archive::text_oarchive oa(*s);
    oa << *p_statMatrix;

    clog << "done." << endl;
    return 0;
}

int fcm::load(fstream *s) {
    if (s->fail())
        return 1;
    clog << "Reading map from file... ";
    archive::text_iarchive oa(*s);
    oa >> *p_statMatrix;
    clog << "done." << endl;
    return 0;
}

void fcm::occurrenceCounter() {

    unsigned int i = 0;     // debug info: loop counter
    char c;
    int pos;                // calculate index in the symbol column
    unsigned int map_pos;   // calculate index in the map for the k order value

    // circular buffer with the context and the current symbol
    circular_buffer<char> buffer(k + 1);

    // fill the buffer with context, discarding non-alphabet characters
    while (i < k && input->get(c)) {
        if (charToAlphabet(c) < 0)
            continue;
        buffer.push_back(c);
        i++;
    }

    // get a symbol and keep pushing new chars to the buffer while there's input to process
    while (input->get(c)) {

        // if symbol char is a non-alphabet character, just skip
        if (charToAlphabet(c) < 0)
            continue;

        // push symbol to the buffer. buffer contains context + symbol
        buffer.push_back(c);

        // calculate symbol index value
        pos = charToAlphabet(buffer.back());

        // calculate index given the context
        map_pos = mapPosCalc(buffer);

        // debug information: print circular buffer contents
        for (int j = 0; j < buffer.size(); j++)
            clog << "buffer[" << j << "]=" << buffer[j] << " ";
        clog << endl;

        // check if for the given context+symbol there's already data
        if (p_statMatrix->count(map_pos) == 0) {
            // initialize the array
            vector<unsigned int> array(ALPHABET_LENGTH, 0);

            // update counter at the symbol position
            array[pos] += 1;

            // add to map
            p_statMatrix->insert(pair<unsigned int, vector<unsigned int>>(map_pos, array));

        } else {
            // retrieve current vector from map
            vector<unsigned int> temp_vector = p_statMatrix->find(map_pos)->second;

            // debug information: show old a new value of the incremented counter
            clog << "Incrementing value at position " << map_pos << " of " << temp_vector[pos] << " to "
                 << temp_vector[pos] + 1 << endl;

            // increment counter in symbol pos
            temp_vector[pos] += 1;

            // erase old vector and insert updated vector
            p_statMatrix->erase(map_pos);
            p_statMatrix->insert(pair<unsigned int, vector<unsigned int>>(map_pos, temp_vector));

            // debug information: print the whole vector
            for (int u = 0; u < temp_vector.size(); u++)
                clog << temp_vector[u] << " ";
            clog << endl;
        }

        i++; // increment loop counter
        clog << "---------------------" << endl;
    }

}

int fcm::charToAlphabet(char letter) {

    char c;
    c = (char) tolower(letter);
    string *alphabet = new string(ALPHABET);

    size_t found = alphabet->find(c);

    if (found == string::npos) {
        clog << "Discarding non alphabet letter: '" << c << "'" << endl;
        return -1;
    } else {
        return (static_cast<int>(found));       // there is no problem in static_cast, found will never overflow
        // since map never goes beyond sizeof(unsigned int)
    }
}

unsigned int fcm::mapPosCalc(circular_buffer<char> buffer) {

    unsigned int i = 0;
    unsigned int sum = 0;

    while (i < k) {
        sum += charToAlphabet(buffer[i]) * pow(ALPHABET_LENGTH, i);

        // debug info: print how we calc map position
        clog << "(" << buffer[i] << " " << charToAlphabet(buffer[i]) << "*" << ALPHABET_LENGTH << "^" << i << ") + ";

        i++;
    }

    // debug info: print how we calc map position
    clog << "=" << int(sum) << endl;

    return sum;
}

string fcm::reverse_mapPosCalc(unsigned int number) {

    string str = "";
    unsigned int key;

    for (int i = 0; i < k; i++) {
        key = ((unsigned int) (number / pow(ALPHABET_LENGTH, i)) % ALPHABET_LENGTH);
        // TODO fix magic number
        str += (key != 26) ? key + LOWER_DECIMAL_ASCII_LETTER : ' ';
        number -= key * pow(ALPHABET_LENGTH, i);
    }

    return str;

}

void fcm::printStats() {

    // print a table with database information
    cout << endl;
    cout << setw(8) << "|";

    string *alphabet = new string(ALPHABET);

    for (unsigned int i = 0; i < alphabet->length(); i++)
        cout << setw(4) << alphabet->at(i) << " |";

    cout << endl;

    for (auto it : *p_statMatrix) {

        cout << "|" << setw(6) << reverse_mapPosCalc(it.first) << "|";
        vector<unsigned int> tmp = it.second;

        for (auto i : tmp)
            cout << setw(5) << i << " ";

        cout << "|" << endl;
    }
}

void fcm::printProbs() {

    // print a table with database information
    cout << "Probabilistic Matrix:" << endl;
    cout << endl;
    cout << setw(8) << "|";

    string *alphabet = new string(ALPHABET);

    for (unsigned int i = 0; i < alphabet->length(); i++)
        cout << setw(4) << alphabet->at(i) << " |";

    cout << endl;

    for (auto it : probMatrix) {

        cout << "|" << setw(6) << reverse_mapPosCalc(it.first) << "|";
        vector<double> tmp = it.second;

        for (auto i : tmp)
            cout << setw(5) << i << " ";

        cout << "|" << endl;
    }
}

unsigned int fcm::getSymbol(const char letter, const char *context) {
    unsigned int occurrences = 0;

    // mapPosCalc needs circular buffer, so let's provide one
    circular_buffer<char> buffer(k);

    // fill the buffer with context
    int i = 0;
    while (i < k) {
        buffer.push_back(context[i]);
        i++;
    }

    // calculate symbol index value
    int pos = charToAlphabet(letter);

    if (pos < 0) {
        // debug information
        clog << "getSymbol(): Error trying to find a non-existent symbol in alphabet: " << letter << endl;

        return 0;
    }

    unsigned int map_pos = mapPosCalc(buffer);

    if (p_statMatrix->count(map_pos) == 0) {
        clog << "No occurrences found" << endl;
        return 0;
    } else {
        vector<unsigned int> temp_vector = p_statMatrix->find(map_pos)->second;
        occurrences = temp_vector[pos];

        clog << "Symbol '" << letter << "' for context '" << context << "' occurrences is: " << occurrences << endl;
    }

    return occurrences;
}

double fcm::getEntropy() {

    string *alphabet = new string(ALPHABET);
    double hi;  // H(i)
    double sum = 0;
    double p;

    generate_sum_prob_Matrix();

    for (auto it : *p_statMatrix) {
        hi = 0;
        auto mit = sum_stat_Matrix.find(it.first);

        for (unsigned int i = 0; i < alphabet->length(); i++) {

            if (it.second[i] == 0)
                continue;

            // sum of Hi
            p = (double) it.second[i] / mit->second.first;
            hi += -(p * log2(p));

        }
        // ∑ Hi * Pi
        sum += hi * mit->second.second;
    }

    return sum;
}

void fcm::generate_sum_prob_Matrix() {

    unsigned int total_sum = 0;
    unsigned int vector_sum = 0;

    for (auto it : *p_statMatrix) {

        vector_sum = (unsigned int) accumulate(it.second.begin(), it.second.end(), 0);

        pair<unsigned int, double> sum_probl(vector_sum, 0);
        sum_stat_Matrix.insert(pair<unsigned int, pair<unsigned int, double>>(it.first, sum_probl));

        total_sum += vector_sum;
    }

    // now that we have H(i) we can calculate P(i)
    for (auto it : sum_stat_Matrix) {

        unsigned int pos = it.first;
        pair<unsigned int, double> tmp(it.second.first, PROBABILITY(it.second.first, (double) total_sum, alpha));

        sum_stat_Matrix.erase(it.first);

        sum_stat_Matrix.insert(pair<unsigned int, pair<unsigned int, double>>(pos, tmp));
    }

    /* TODO: table needs to be prettified
    clog << "______|__sum___|_prob___" << endl;
    for (auto it : sum_stat_Matrix) {
        clog << it.first << " | " << it.second.first << " | " << it.second.second << endl;
    }*/

}

void fcm::genText() {

    string gText = "", str_key, alphabet = ALPHABET;    // text as a whole, needs buffer to capture the last symbols inserted(read below)
    unsigned int l = 0, uint_key = 0;                   // text length
    random_device rd;                                   // generator must have a random device to provide entropy
    mt19937 gen(rd());                                  // Mersenne Twister Engine
    int lastPrint_idx = 0;

    /*                  Algorithm
     * 1 - Insert the most occurring key on matrix.
     * 2 - Convert string (with k size) to uint.
     * 3 - Fetch corresponding prob vector with the latter uint
     * 4 - Apply the vector to the distribution
     * 5 - Generate an index associated to the next letter inserted on the text [0-26]
     * 6 - Repeat from 2
     * */

    auto it = sum_stat_Matrix.begin();
    advance(it, rand() % sum_stat_Matrix.size());   // if rand is highlighted, just ignore. same err as atoi
    auto random_key = it->first;


    gText.append(reverse_mapPosCalc(random_key));   //get the first letters into the text, starting point
    l = (unsigned int) gText.length();              //-as in (l)ength

    str_key = gText.substr(l - k, l);

    for (int j = numLines; j > 0; j--) {
        for (int i = numChar; i > 0; i--) {

            circular_buffer<char> cb(str_key.size(), str_key.begin(), str_key.end());
            uint_key = mapPosCalc(cb);                                  //row selection

            vector<double> v_p = probMatrix.find(uint_key)->second;     //retrieve probabilities (from row)
            discrete_distribution<> d(v_p.begin(), v_p.end());          //distribute probabilities into distribution

            gText += alphabet.at(d(gen));                               //generate letter index with the latter
            l++;                                                        //size has increased

            str_key = gText.substr(l - k, l);                           //update buffer, new key will arise
        }
        cout << gText.substr(lastPrint_idx, l) << endl;
        lastPrint_idx = l;
    }

}

void fcm::calculateProbabilities() {

    vector<double> tmp_probabilities(ALPHABET_LENGTH);
    unsigned int sum;

    for (auto it : *p_statMatrix) {

        sum = accumulate(it.second.begin(), it.second.end(), 0);
        if (most_occurring.second < sum && reverse_mapPosCalc(most_occurring.first) != " ") {
            most_occurring.first = it.first;
            most_occurring.second = sum;
        }

        for (int j = 0; j < it.second.size(); j++)
            tmp_probabilities[j] = it.second[j] == 0 ? 0 : PROBABILITY(it.second[j], (double) sum, alpha);

        probMatrix.insert(pair<unsigned int, vector<double> >(it.first, tmp_probabilities));
    }
}