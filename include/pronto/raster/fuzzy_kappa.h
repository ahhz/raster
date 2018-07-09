//
//=======================================================================
// Copyright 2016-2017
// Author: Alex Hagen-Zanker
// University of Surrey
//
// Distributed under the MIT Licence (http://opensource.org/licenses/MIT)
//=======================================================================
//
// This implements the Fuzzy Kappa method

#pragma once

#include <pronto/raster/distance_transform.h>
#include <pronto/raster/io.h>
#include <pronto/raster/traits.h>
#include <pronto/raster/transform_raster_view.h>
#include <pronto/raster/vector_of_raster_view.h>

#include <algorithm>
#include <map>    // maps are used to create distributions
#include <vector> // vectors are used to store arrays

namespace pronto {
  namespace raster {
   
    typedef std::map<double, int, std::greater<double> > distribution;

    template<class T>
    using matrix = std::vector< std::vector<T> >;

    ////////////////////////////////////////////////////////////////////////////////
    // Fuzzy Kappa can be based on any distance decay function. The function may 
    // be set by one or more parameters. Therefore it is passed as a functor
    //
    class exponential_decay 
    {
    public:
      exponential_decay(double halving) : m_halving(halving)
      {
      }

      double operator()(double d) const
      {
        return pow(0.5, d / m_halving);
      }

      double m_halving;
    };

    class one_neighbour
    {
    public:
      one_neighbour(double value) : m_value(value)
      { }

      double operator()(double d) const
      {
        if (d == 0) 
          return 1.0;
        else if (d < 2) 
          return m_value;
        else return 0.0;
      }
    private:
      double m_value;
    };
    
    class gdal_raster_maker
    {
    public:
      template<class T>
      using raster_type = gdal_raster_view<T>;

      template<class T, class U>
      raster_type<T> create(const raster_type<U>& model) const
      {
        return create_temp_from_model<T>(model);
      }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // This function takes two distribution and returns the expected minimum value 
    // when a number is sampled from both functions
    //
    double expected_minumum_of_two_distributions(
      const distribution& distriA, // sorted pairs of values(high to low) and counts
      const distribution& distriB,
      double totalA,               //total count
      double totalB)
    {
      distribution::const_iterator iterA = distriA.begin();
      distribution::const_iterator iterB = distriB.begin();

      double pCum = 0;         // cumulative probability p(min(A,B) <= x)
      double sumA = 0;         // intermediate for pCumA
      double sumB = 0;         // intermediate for pCumB
      double pCumA = 0;        // cumulative probabilty p(A<=x)
      double pCumB = 0;        // cumulative probability p(B<=x)
      double x = 1;            // x
      double expected = 0;     // Integral of x * p(x) over x

      while (iterA != distriA.end() || iterB != distriB.end()) {
        // In order to pass all values in both distributions advance to the 
        // largest next value, except if already at the end of the series 
        if (iterB == distriB.end() || (iterA != distriA.end() 
          && iterA->first > iterB->first)) {
          x = iterA->first;
          sumA += iterA->second;
          pCumA = sumA / totalA;
          iterA++;
        }
        else {
          x = iterB->first;
          sumB += iterB->second;
          pCumB = sumB / totalB;
          iterB++;
        }

        const double pCumPrevious = pCum;
        pCum = pCumA * pCumB; // p(A>=x AND B>=x )
        expected += (pCum - pCumPrevious) * x;
      }
      return expected;
    }
       
    ////////////////////////////////////////////////////////////////////////////////
    // This is the entry function to calculate Fuzzy Kappa (improved), 
    // Returns false if there are no cells to compare (Fuzzy Kappa = 0)
    // Returns false if all cells in both maps are uniform (Fuzzy Kappa = 1)
    //
    template<class RasterA, class RasterB, class RasterMask, class RasterOut,
    class DistanceDecay, class RasterMaker>
      bool fuzzy_kappa_2009(
      RasterA& mapA,              // input: first map
      RasterB& mapB,              // input: second map
      RasterMask& mask,           // input: mask map
      int nCatsA, int nCatsB,     // dimension: number of categories in legends
      const matrix<double>& m,    // parameter: categorical similarity matrix
      DistanceDecay f,            // parameter: distance decay function
      RasterOut& comparison,      // result: similarity map
      RasterMaker maker,          // RasterMaker::raster<T> r = maker.create<T>(model)
      double& fuzzykappa)         // result: improved fuzzy kappa
    {
        // for use with std::get
        using a_value = typename traits<RasterA>::value_type;
        using b_value = typename traits<RasterB>::value_type;
        using mask_value = typename traits<RasterMask>::value_type;
        using out_value = typename traits<RasterOut>::value_type;
        using temp_raster = typename RasterMaker::raster_type<double>;

        // Nearest neighbour distances for all categories in both maps
        std::vector<temp_raster> distancesA;
        std::vector<temp_raster> distancesB;
        std::vector<bool> has_cat_a;
        std::vector<bool> has_cat_b;

        // calculate Euclidean distances
        for (int catA = 0; catA < nCatsA; ++catA) {
          temp_raster dist = maker.create<double>(mapA);
          bool has = euclidean_distance_transform(mapA, dist, catA);
          has_cat_a.push_back(has);
          distancesA.push_back(dist);
        }

        for (int catB = 0; catB < nCatsB; catB++) {
          temp_raster dist = maker.create<double>(mapB);
          bool has = euclidean_distance_transform(mapB, dist, catB);
          has_cat_b.push_back(has);
          distancesB.push_back(dist);
        }

        // convert distances to similarity
        auto distance_to_similarity_ab = [&](const std::vector<double>& distance_a)
        {
          std::vector<double> sim_to_b(nCatsB, 0);
        
          for (int cat_a = 0; cat_a < nCatsA; ++cat_a) {
            double distance_weight = has_cat_a[cat_a] ? f(distance_a[cat_a]) : 0.0;
            for (int cat_b = 0; cat_b < nCatsB; ++cat_b) {
              double cat_weight = m[cat_a][cat_b];
              double sim = distance_weight * cat_weight;
              if (sim > sim_to_b[cat_b]) sim_to_b[cat_b] = sim;
            }
          }

          return sim_to_b;
        };

        auto distance_to_similarity_ba = [&](const std::vector<double>& distance_b)
        {
          std::vector<double> sim_to_a(nCatsA, 0);

          for (int cat_b = 0; cat_b < nCatsB; ++cat_b) {
            double distance_weight = has_cat_b[cat_b] ? f(distance_b[cat_b]) : 0.0;
            for (int cat_a = 0; cat_a < nCatsA; ++cat_a) {
              double cat_weight = m[cat_a][cat_b];
              double sim = distance_weight * cat_weight;
              if (sim > sim_to_a[cat_a]) sim_to_a[cat_a] = sim;
            }
          }
          return sim_to_a;
        };

        auto similarity_b = transform(distance_to_similarity_ab
          , raster_vector(distancesA));

        auto similarity_a = transform(distance_to_similarity_ba
          , raster_vector(distancesB));

        // initialize tallies
        matrix<distribution> distributionA(nCatsA, std::vector<distribution>(nCatsB));
        matrix<distribution> distributionB(nCatsB, std::vector<distribution>(nCatsA));

        std::vector<int> catCountsA(nCatsA, 0);
        std::vector<int> catCountsB(nCatsB, 0);

        double mean = 0.0;
        int count = 0;

        // cell operation for tally and local sim
        auto tally_and_local_sim = [&](
          const std::vector<double>& sim_a, const std::vector<double>& sim_b,
          int cat_a, int cat_b, bool mask)
        {
          if (!mask) return -1.0;
          ++catCountsA[cat_a];
          ++catCountsB[cat_b];
          for (int ccat_a = 0; ccat_a < nCatsA; ++ccat_a) {
            ++distributionB[cat_b][ccat_a][sim_a[ccat_a]];
          }
          for (int ccat_b = 0; ccat_b < nCatsB; ++ccat_b) {
            ++distributionA[cat_a][ccat_b][sim_b[ccat_b]];
          }
          double sim = std::min(sim_a[cat_a], sim_b[cat_b]);
          mean += sim;
          ++count;
          return sim;
        };

        // Calculate local_sim and tallies
        auto local_sim = transform(tally_and_local_sim, similarity_a, 
          similarity_b, mapA, mapB, mask);
        
        assign(comparison, local_sim);// also does the tallying

        if (count == 0) {
          fuzzykappa = 0;
          return false;
        }
        mean /= count;

        // Calculate expected similarity
        double expected = 0;
        const double squaredTotal = (double)(count)*(double)(count);
        for (int catA = 0; catA < nCatsA; catA++){
          for (int catB = 0; catB < nCatsB; catB++){
            // The if statement avoids division by zero
            if (catCountsA[catA] > 0 && catCountsB[catB] > 0) {
              const double pCats = (double)(catCountsA[catA]) * (double)(catCountsB[catB])
                / squaredTotal;
              const double eCats = expected_minumum_of_two_distributions(
                distributionA[catA][catB], distributionB[catB][catA], catCountsA[catA],
                catCountsB[catB]);

              expected += pCats * eCats;
            }
          }
        }

        // If all cells are identical to each other
        if (expected == 1) {
          fuzzykappa = 1;
          return false;
        }

        // Calculate Fuzzy Kappa 
        fuzzykappa = (mean - expected) / (1.0 - expected);
        return true;
    }
  }
}

