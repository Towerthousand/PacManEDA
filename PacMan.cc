#include "Game.hh"
#include "Registry.hh"

#include <fstream>
#include <getopt.h>

using namespace std;



void help (int argc, char** argv) {
	cout << "Usage: " << argv[0] << " [options] player1 player2 ... [< start.pac] [> game.pac] " << endl;
	cout << "Available options:" << endl;
	cout << "--seed=seed\t-s seed\t\tset random seed (default: time)" << endl;
	cout << "--input=file\t-i input\tset input file (default: stdin)" << endl;
	cout << "--output=file\t-o output\tset output file (default: stdout)" << endl;
	cout << "--list\t\t-l\t\tlist registered players" << endl;
	cout << "--version\t-v\t\tprint version" << endl;
	cout << "--help\t\t-h\t\tprint help" << endl;
}


void version () {
	cout << "pacman v1." << endl;
	cout << "compiled " << __TIME__ << " " << __DATE__ << endl;
}




int main (int argc, char** argv) {

	int won = 0;
	int nump = 1;
	if (argc == 1) {
		help(argc, argv);
		return EXIT_SUCCESS;
	}

	struct option long_options[] = {
	{"seed",           required_argument,  0, 's'},
	{"input",          required_argument,  0, 'i'},
	{"output",         required_argument,  0, 'o'},
	{"list",           no_argument,        0, 'l'},
	{"version",        no_argument,        0, 'v'},
	{"help",           no_argument,        0, 'h'},
	{0, 0, 0, 0}
};

	char* ifile = 0;
	char* ofile = 0;
	int seed = -1;
	vector<string> names;

	while (true) {
		int option_index = 0;
		int c = getopt_long(
					argc, argv,
					"s:i:o:lvh",
					long_options, &option_index
					);

		if (c == -1) break;

		switch (c) {

			case 's':
				seed = s2i(optarg);
				break;

			case 'i':
				ifile = optarg;
				break;

			case 'o':
				ofile = optarg;
				break;

			case 'l':
				Registry::print_players(cout);
				return EXIT_SUCCESS;

			case 'v':
				version();
				return EXIT_SUCCESS;

			case 'h':
				help(argc, argv);
				return EXIT_SUCCESS;

			default:
				return EXIT_FAILURE;
		}   }

	while (optind < argc) {
		names.push_back(argv[optind++]);
	}

	if (seed < 0) seed = time(0);
	srand(seed);

	for(int i = 0; i < nump; ++i) {
		istream* is = ifile ? new ifstream(ifile) : &cin ;
		ostream* os = ofile ? new ofstream(ofile) : &cout;

		if(Game::run(names, *is, *os) == 0) ++won;

		if (ifile) delete is;
		if (ofile) delete os;
	}
	cerr << "TOTAL: " << (float(won)/float(nump))*100 << "% WON" << endl;
}

