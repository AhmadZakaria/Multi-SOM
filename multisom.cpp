#include <string>
#include <cmath>
#include <fstream>

#include"Eigen/StdVector"

#include "som.cpp"

template<int G_DIM, int X_DIM>
class MultiSOM {
  private:
    int N = X_DIM;
    int G = G_DIM;
    int epoch = 0;
    int epoch_max;
    float eta_0;
    float eta_final;
    float s_0 = 4.0f; // size of gaussian of h
    float s = 4.0f; // size of gaussian of h
    float s2inv = 1.0f / (s * s); // 1/s^2 (to be used efficiently)

    std::vector<std::shared_ptr< SOM<G_DIM, X_DIM>> > soms;
  public:
    explicit MultiSOM(int m, int k, VectorXi f) {
        for (int som = 0; som < m; ++som) {
            std::shared_ptr< SOM<G_DIM, X_DIM>> s(new SOM<G_DIM, X_DIM>(f, k));
            soms.push_back(s);
        }
    }

    MultiSOM(const MultiSOM&) = delete;
    MultiSOM& operator= (const MultiSOM&) = delete;
    ~MultiSOM() = default;

    void train(MatrixXd inputs, float eta0, float etafinal, int iters = 1) {
        eta_0 = eta0;
        eta_final = etafinal;

        epoch = 0;
        epoch_max = inputs.rows() * iters;

        for (int var = 0; var < iters; ++var)
            for (int p = 0; p < inputs.rows(); ++p) {
                VectorXd p_i = inputs.row(p);
                float shortest = FLT_MAX;
                int shortest_idx = -1;
                for (int som = 0; som < soms.size(); ++som) {
                    float d = (soms[som])->stimulate(p_i, true, true);
                    if (d <= shortest) {
                        shortest = d;
                        shortest_idx = som;
                    }
                }
//            std::cout << "shortest: " << shortest << ", idx: " << shortest_idx << ", p=" << p << ", pi=" << p_i << std::endl;

                // apply only to the winner SOM
                soms[shortest_idx]->apply_last_stimulus(eta(), s2inv);
                timestep();
            }

    }

    void output_SOMS(std::string filename = "PA-D.net") {
        // all together
        std::ofstream file2(filename, std::ofstream::out | std::ofstream::trunc);
        if (file2.is_open()) {
            for (int som = 0; som < soms.size(); ++som) {
                MatrixXd c = soms[som]->get_centers();
                file2 << c << "\n";
            }
        }
        file2.close();


        //separate SOMs
        for (int som = 0; som < soms.size(); ++som) {
            std::ofstream file(filename + std::to_string(som), std::ofstream::out | std::ofstream::trunc);
            if (file.is_open()) {
//                MatrixXd c = soms[som]->get_centers();
                soms[som]->output_SOM_grid(file);
            }
            file.close();
        }
    }



    void timestep() {
        update_s2inv();
        epoch++;
        std::cerr << "epoch " << epoch << ": s: " << s << std::endl;
    }
    void set_gaussian_size(float _s) {
        s = _s;
        s2inv = 1.0f / (s * s);
    }
    void update_s2inv() {
        s = s_0 * std::pow((0.1f / s_0), ((float)epoch / (float)epoch_max));
        s2inv = 1.0f / (s * s);

    }

    float eta() { // learning rate as a function of time
        return eta_0 * std::pow((eta_final / eta_0), ((float)epoch / (float)epoch_max));
    }
};
