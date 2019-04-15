#include "materials.h"

#include <iostream>

Material::Material(const std::string &name, const std::vector<float> &abs,
                   const std::vector<float> &nfis, const std::vector<float> &fis,
                   const std::vector<float> &chi,
                   const std::vector<std::vector<float>> &scat)
    : ng(abs.size()),
      xstr(ng),
      xsab(abs),
      xsnf(nfis),
      xsf(fis),
      xsch(chi),
      xssc(ng, ng, 0.0f),
      name(name)
{
    // A vector of the outscatter for each group. This is nice for making scatter CDFs
    std::vector<std::vector<float>> scat_by_col(ng, std::vector<float>(ng));
    std::vector<float> outscatter(ng, 0.0);
    for (int from_g = 0; from_g < ng; ++from_g) {
        for (int to_g = 0; to_g < ng; ++to_g) {
            xssc(to_g, from_g) = scat[to_g][from_g];
            scat_by_col[from_g][to_g] = scat[to_g][from_g];
            outscatter[from_g] += xssc(to_g, from_g);
        }
    }

	// Get the outscatter CDF for each group
    scatter_cdf.reserve(ng);
    for (int from_g = 0; from_g < ng; ++from_g) {
        scatter_cdf.push_back(ScatterCDF(scat_by_col[from_g]));
	}

    for (int ig = 0; ig < ng; ++ig) {
        xstr[ig] = xsab[ig] + outscatter[ig];
    }

	// Now that we have all of the interesting stuff, calculate the interaction CDF
    interaction_cdf.reserve(ng);
    for (int ig = 0; ig < ng; ++ig) {
        interaction_cdf.push_back(
            InteractionCDF(xstr[ig], outscatter[ig], xsf[ig], xsf[ig] + xsab[ig]));
    }

    return;
}

MaterialLibrary C5G7()
{
    MaterialLibrary library;

    {
        std::vector<float> abs{8.0248E-03f, 3.7174E-03f, 2.6769E-02f, 9.6236E-02f,
                               3.0020E-02f, 1.1126E-01f, 2.8278E-01f};
        std::vector<float> nfis{2.005998E-02f, 2.027303E-03f, 1.570599E-02f,
                                4.518301E-02f, 4.334208E-02f, 2.020901E-01f,
                                5.257105E-01f};
        std::vector<float> fis{7.21206E-03f, 8.19301E-04f, 6.45320E-03f, 1.85648E-02f,
                               1.78084E-02f, 8.30348E-02f, 2.16004E-01f};
        std::vector<float> chi{5.8791E-01f, 4.1176E-01f, 3.3906E-04f, 1.1761E-07f,
                               0.0000E+00f, 0.0000E+00f, 0.0000E+00f};

        std::vector<std::vector<float>> scat{
            {1.27537E-01f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {4.23780E-02f, 3.24456E-01f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {9.43740E-06f, 1.63140E-03f, 4.50940E-01f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {5.51630E-09f, 3.14270E-09f, 2.67920E-03f, 4.52565E-01f, 1.25250E-04f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 5.56640E-03f, 2.71401E-01f,
             1.29680E-03f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 1.02550E-02f,
             2.65802E-01f, 8.54580E-03f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 1.00210E-08f,
             1.68090E-02f, 2.73080E-01f}};
        Material uo2(std::string("UO2"), abs, nfis, fis, chi, scat);
        library.add_material(uo2);
    }

    {
        std::vector<float> abs{6.0105E-04f, 1.5793E-05f, 3.3716E-04f, 1.9406E-03f,
                               5.7416E-03f, 1.5001E-02f, 3.7239E-02f};
        std::vector<float> nfis{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        std::vector<float> fis{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        std::vector<float> chi{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };

        std::vector<std::vector<float>> scat{
            {4.44777E-02f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {1.13400E-01f, 2.82334E-01f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {7.23470E-04f, 1.29940E-01f, 3.45256E-01f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {3.74990E-06f, 6.23400E-04f, 2.24570E-01f, 9.10284E-02f, 7.14370E-05f,
             0.00000E+00f, 0.00000E+00f},
            {5.31840E-08f, 4.80020E-05f, 1.69990E-02f, 4.15510E-01f, 1.39138E-01f,
             2.21570E-03f, 0.00000E+00f},
            {0.00000E+00f, 7.44860E-06f, 2.64430E-03f, 6.37320E-02f, 5.11820E-01f,
             6.99913E-01f, 1.32440E-01f},
            {0.00000E+00f, 1.04550E-06f, 5.03440E-04f, 1.21390E-02f, 6.12290E-02f,
             5.37320E-01f, 2.48070E+00f},
        };
        Material moderator(std::string("Moderator"), abs, nfis, fis, chi, scat);
        library.add_material(moderator);
    }

	{
        std::vector<float> abs{1.0E+07f, 1.0E+07f, 1.0E+07f, 1.0E+07f,
                               1.0E+07f, 1.0E+07f, 1.0E+07f};
        std::vector<float> nfis{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        std::vector<float> fis{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        std::vector<float> chi{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };

        std::vector<std::vector<float>> scat{
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
        };
        Material moderator(std::string("Black"), abs, nfis, fis, chi, scat);
        library.add_material(moderator);
    }

	{
        std::vector<float> abs{0.0E+00f, 0.0E+00f, 0.0E+00f, 0.0E+00f,
                                   0.0E+00f, 0.0E+00f, 0.0E+00f};
        std::vector<float> nfis{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        std::vector<float> fis{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        std::vector<float> chi{
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };

        std::vector<std::vector<float>> scat{
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
            {0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f, 0.00000E+00f,
             0.00000E+00f, 0.00000E+00f},
        };
        Material moderator(std::string("Void"), abs, nfis, fis, chi, scat);
        library.add_material(moderator);
    }

    return library;
}
