#include <iostream>

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

#include "Eigen/Core"

#include "multisom.cpp"

using namespace std;
namespace helper {
//  Windows
#ifdef _WIN32
#include <Windows.h>
double get_wall_time() {
    LARGE_INTEGER time, freq;
    if (!QueryPerformanceFrequency(&freq)) {
        //  Handle error
        return 0;
    }
    if (!QueryPerformanceCounter(&time)) {
        //  Handle error
        return 0;
    }
    return (double)time.QuadPart / freq.QuadPart;
}
double get_cpu_time() {
    FILETIME a, b, c, d;
    if (GetProcessTimes(GetCurrentProcess(), &a, &b, &c, &d) != 0) {
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
                     ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    } else {
        //  Handle error
        return 0;
    }
}

//  Posix/Linux
#else
#include <time.h>
#include <sys/time.h>
double get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time, NULL)) {
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}
#endif
}

template<typename M>
M load_csv (const std::string& path, char del = ',') {
    std::ifstream indata;
    indata.open(path);
    std::string line;
    std::vector<double> values;
    uint rows = 0;
    while (std::getline(indata, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        while (std::getline(lineStream, cell, del)) {
            if (cell.size() > 0)
                values.push_back(std::stod(cell));
        }
        ++rows;
    }
    return Map<const Matrix<typename M::Scalar, M::RowsAtCompileTime, M::ColsAtCompileTime, RowMajor>>(values.data(), rows, values.size() / rows);
}

void test1() {
    MatrixXd inputs = load_csv<MatrixXd>("./train_PA-D.dat", ' ');
    cout << inputs.rows() << ", " << inputs.cols() << endl;
    const int m = 4,
              g = 2,
              k = 200,
              x_dim = 2;
    Vector2i f ;
    f << 10, 20;
    MultiSOM<g, x_dim> msom(m, k, f);

    double begin = helper::get_wall_time();
    double begin_cpu = helper::get_cpu_time();

    msom.train(inputs, 1, 0.1, 10);

    double end = helper::get_wall_time();
    double end_cpu = helper::get_cpu_time();

    double elapsed_secs = double(end - begin) ;
    double elapsed_secs_cpu = double(end_cpu - begin_cpu);
    cerr << "Elapsed time    : " << elapsed_secs << "s" << endl;
    cerr << "Elapsed cpu time: " << elapsed_secs_cpu << "s" << endl;
    msom.output_SOMS();
}

void test2() {
    const int m = 4,
              g = 2,
              k = 400,
              x_dim = 2;
    MatrixXd train1 = (MatrixXd::Random(500, 2) * 0.125f) + MatrixXd::Constant(500, 2, 0.125f) ; // unit square random
    MatrixXd train2 = (MatrixXd::Random(500, 2) * 0.125f ) + MatrixXd::Constant(500, 2, 0.5f); // unit square random
    MatrixXd train3 = (MatrixXd::Random(500, 2) * 0.125f ) + MatrixXd::Constant(500, 2, 1.0f); // unit square random

    MatrixXd all_train(1500, 2);
    all_train << train1 , train2 , train3;

    std::ofstream file("random_train.dat", std::ofstream::out | std::ofstream::trunc);
    if (file.is_open()) {
        file << "## train1\n" << train1 << "\n\n\n";
        file << "## train2\n" << train2 << "\n\n\n";
        file << "## train3\n" << train3;
    }
    file.close();

    Vector2i f ;
    f << 20, 20;
    MultiSOM<g, x_dim> msom(m, k, f);

    double begin = helper::get_wall_time();
    double begin_cpu = helper::get_cpu_time();

    msom.train(all_train, 1, 0.5, 10);

    double end = helper::get_wall_time();
    double end_cpu = helper::get_cpu_time();

    double elapsed_secs = double(end - begin) ;
    double elapsed_secs_cpu = double(end_cpu - begin_cpu);
    cerr << "Elapsed time    : " << elapsed_secs << "s" << endl;
    cerr << "Elapsed cpu time: " << elapsed_secs_cpu << "s" << endl;
    msom.output_SOMS();
}

void test3()
{

    ROFL roflNetwork(2, 10);

	double traininExamples[100][10] = { {0,0},{0,1} };

	std::ifstream roflFile("RoflFile.txt");
	std::string line;
	int count = 0;
	while (std::getline(roflFile, line))
	{
		std::istringstream iss(line);

		for (int dimension = 0; dimension<10; dimension++)
		{
			iss >> traininExamples[count][dimension];
		}
	}


	roflNetwork.Train(traininExamples, 2);
}

int main() {
    test1();
}

