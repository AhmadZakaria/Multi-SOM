#include <cfloat>
#include <fstream>
#include <limits>

#include <boost/multi_array.hpp>
#include <boost/array.hpp>

#include "Eigen/Dense"

using namespace Eigen;

template<int G_DIM, int X_DIM>
class SOM {
  public:
    typedef VectorXf tValue;
    typedef boost::multi_array<tValue, G_DIM> gridArray;
    typedef boost::multi_array<float, G_DIM> distArray;
    typedef typename gridArray::index gridIndex;
    typedef boost::array<gridIndex, G_DIM> gridIndexArray;
    //ctor
    /* f: side lengths of g*/
    explicit SOM(VectorXi f , int K) {
        n = X_DIM;
        g = G_DIM;
        gridIndexArray shape;
        int ii = 0;
        for (int var = 0; var < G_DIM; ++var) {
            shape[var] = f.data()[ii++];
        }
        grid.resize(shape);
        k = K;
        init_centers();

        calc_distances(grid.num_elements());
    }
    SOM(const SOM&) = delete;
    SOM& operator= (const SOM&) = delete;
    ~SOM() = default;

    void init_centers() {
        Ck.resize(k, n);
        Ck = (MatrixXd::Random(k, n) * 0.5f ) + MatrixXd::Constant(k, n, 1.0f) ; // unit cube random
    }

    /**
     * @brief stimulate get SOM response to a new stimulus
     * @param pattern stimulus
     * @param save_input if true, SOM will remember this stimulus,
     *              and it can be applied by
     *              calling apply_last_stimulus();
     * @param save_dist if true, SOM will remember this stimulus' distance ,
     *              from the winner.
     * @return float shortest distance.
     */
    float stimulate(VectorXd pattern, bool save_input = false, bool save_dist = false) {
        int nearest_idx = -1;
        float shortest_sq_dist = FLT_MAX;
        VectorXd shortest_dist(Ck.cols());
        last_stimulus_winner_dist.resize(Ck.cols());
        for (int c_idx = 0; c_idx < Ck.rows(); ++c_idx) {
            MatrixXd row = Ck.row(c_idx);
            row -=  pattern;
            float dis = row.squaredNorm();
            if (dis <= shortest_sq_dist) {
                nearest_idx = c_idx;
                shortest_sq_dist = dis;
                shortest_dist = row;
            }
        }
        if(save_input) {
            last_stimulus = pattern;
        }
        if(save_dist) {
            last_stimulus_winner_dist_sqnorm = shortest_sq_dist;
            last_stimulus_winner_dist = shortest_dist;
            last_stimulus_winner_index = nearest_idx;
        }
        return shortest_sq_dist;
    }

    void apply_last_stimulus(float eta, float s2inv) {
        for (int c_idx = 0; c_idx < Ck.rows(); ++c_idx) {
            MatrixXd distance = Ck.row(c_idx) ;
            distance -= last_stimulus;
            VectorXd delta_C = (eta * h(dist(c_idx, last_stimulus_winner_index), s2inv))
                               * distance;
            Ck.row(c_idx) -= delta_C;
        }
    }

    MatrixXd get_centers() {
        return Ck;
    }
    /**
     * @brief output_SOM outputs grid lines in gnuplot format
     * @param file output stream to use
     */
    void output_SOM_grid(std::ofstream& file) {
        for (int i = 0; i < grid.num_elements(); ++i) {
            for (int j = i; j < grid.num_elements(); ++j) {
                if ((dists[i][j] - 1.0f / G_DIM) < std::numeric_limits<float>::epsilon()) { //adjacent
                    file << Ck.row(i) << std::endl;
                    file << Ck.row(j) << "\n\n\n";
                }
            }
        }

    }
  private:

    gridArray grid;
    distArray dists;
    int g; // dimensionality of the grid
    VectorXi f; // edge lengths of the grid

    int n; // input dimension
    MatrixXd Xp; // input


    int k; // number of neurons/centers
    MatrixXd Ck; // centers

    inline float dist(int i, int j) {// dist function
        return dists[i][j];
    }

    float h(float dis, float s2inv) { // neighborhood function //gaussian
        return exp((-0.5 * dis * dis * s2inv));
    }

    void calc_distances(int num_elements) {
        typename distArray::extent_gen extents;
        dists.resize(extents[num_elements][num_elements]);
        gridIndexArray index_i;
        gridIndexArray index_j;
        tValue* p_i = grid.data();


        for (int i = 0; i < num_elements; ++i) {
            index_i = getIndexArray( grid, p_i++ );
            tValue* p_j = grid.data();
            for (int j = 0; j < num_elements; ++j) {
                index_j = getIndexArray( grid, p_j++ );
                dists[i][j] = get_dist(index_i, index_j);
            }
        }
    }

    // helper functions for the multi-dimentional grids
    gridIndex getIndex(const gridArray& m, const tValue * requestedElement, const unsigned short int direction) {
        int offset = requestedElement - m.origin();
        return(offset / m.strides()[direction] % m.shape()[direction] +  m.index_bases()[direction]);
    }

    gridIndexArray getIndexArray( const gridArray& m, const tValue * requestedElement ) {
        gridIndexArray _index ;
        for ( unsigned int dir = 0; dir < G_DIM; dir++ ) {
            _index[dir] = getIndex( m, requestedElement, dir );
        }

        return _index;
    }

    float get_dist(gridIndexArray i , gridIndexArray j) {
        assert(i.size() == j.size());
        float res = 0.0f;
        for (int idx = 0; idx < i.size(); ++idx) {
            res += fabs(i[idx] - j[idx]);
        }
        return res / G_DIM;
    }


    // for performance, and to avoid redundant calculations
    VectorXd last_stimulus;
    int last_stimulus_winner_index;
    VectorXd last_stimulus_winner_dist;
    float last_stimulus_winner_dist_sqnorm;

};
