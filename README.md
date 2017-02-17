# Finite Context Model

FCM is a Shannon predictor implementation developed at [Aveiro University](https://www.ua.pt) in the course [47979 - Audio And Video Coding](https://www.ua.pt/ensino/uc/5211) for academic purposes and intents to implement the method of estimating the entropy and redundancy of a language as described in the paper *Prediction and Entropy of Printed English* by C.E. Shannon (1950).

Redundancy computation is the baseline for data and text compressors. Calculating the most frequent symbols and afterwards coding them with variable length codes (such as Huffman or Shannon-Fano) leads to better efficiency, by using smaller codes for most frequent symbols with smaller codes and more infrequent with longer codes.

This program includes the needed statistical calculation for a *k* configurable depth (how many letters to look backwards), resulting in a Markov chain that can be used to implement the Shannon predictor, resulting in a the text generator described by Shannon in his paper.

A longer and more detailed description of this project is available at [description.pdf](https://github.com/luminoso/fcm-shannon/blob/master/description.pdf)


## How to run

### Install dependencies and build

To build you'll need gcc, cmake and boost devel libraries. Install them according to you distribution. Example for Fedora:

    # dnf install cmake gcc-c++ boost-devel
    
The simplest way is to run the following cmake commands at the root directory:

    $ cmake .
    $ cmake --build .

### Usage examples

There's a help output when running with *-h* switch:

| Switch| Description                                            |
| ----- | ------------------------------------------------------ | 
| -k    | specify order (default: 1)                             |
| -f    | read data from file (default: none)                    |
| -o    | save data to file (default: none)                      |
| -s    | print statistics (optional)                            |
| -c    | number of characters per line (default: 100)           |
| -l    | number of lines to generate text                       |
| -d    | print all debug messages                               |
| -a    | specify alpha for probability calculation (default: 0) |
| -h    | display this help                                      |
| (file)| file to read from (if not specified read from stdin)   |

A few examples how to use FCM

1. Process with order 3 (k=3), save computed data to *save.dat* the document *os_maias.txt*
   
        ./fcm -k 3 -f save.dat example.txt

2. Process with order 1 (k=1) and print the resulting statistical computed probability statistical table

        ./fcm -s -k 1 example.txt
       
3. Print previously computed calculation saved in the file *save.dat*

        ./fcm -f save.dat -s


## Example Results

The recognizability of words and text improves with order and the amount of the input text processed.

1. k = 1 (order 1)

        i yinkepay fin wnd s t d mowall rd is ithe ame yis m wi tyotthingththe-
        centicorinigh thevime ughee trnger d aryou o sowind ithin y hte tre ayid
        inde the is bueveanth
    
2. k = 3
 
        utt i befor its thing aint the valley of darkness or he shepherd a mothe
        shepher real hard that vengeance upon and it men bless fore hes or hes
        of lost could mr he
    
3. k = 6

        He is truly his brothers and you will know i am the selfish id like that thin
        aint the truth is youre the weak through the truth the valley of darkness
        for he is
. 
## Licence

MIT
