/**
 * @file
 * @brief Definition of module EventLoaderAPV25
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#ifndef EventLoaderAPV25_H
#define EventLoaderAPV25_H 1

#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TBranchElement.h>
#include <TLeaf.h>

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "core/module/Module.hpp"
#include "objects/Pixel.hpp"

namespace corryvreckan {
  /** @ingroup Modules
   * @brief Module to do function
   *
   * More detailed explanation of module
   */
  class EventLoaderAPV25 : public Module {

    public:
      /**
       * @brief Constructor for this unique module
       * @param config Configuration object for this module as retrieved from the steering file
       * @param detector Pointer to the detector for this module instance
       */
      EventLoaderAPV25(Configuration& config, std::shared_ptr<Detector> detector);

      /**
       * @brief Destructor deletes the internal objects read from ROOT Tree
       */
      ~EventLoaderAPV25() override;


      /**
       * @brief [Initialise this module]
       */
      void initialize() override;

      /**
       * @brief [Run the function of this module]
       */
      StatusCode run(const std::shared_ptr<Clipboard>& clipboard) override;

      /**
       * @brief [Finalise module]
       */
      void finalize(const std::shared_ptr<ReadonlyClipboard>& clipboard) override;

    private:
      /**
       * @brief Internal object storing objects and information to construct a message from tree
       */

			bool MakePlaneClusters(std::vector<std::tuple<int16_t, int16_t, int8_t>> &hitsPlane, int which_plane);
			int MatchPlaneClusters(std::vector<std::tuple<int16_t, int16_t, int16_t>> &clustersX, 
															std::vector<std::tuple<int16_t, int16_t, int16_t>> &clustersY);
			
      std::shared_ptr<Detector> m_detector;

      std::string m_inputFile;
      TFile * data_file;
      TTree * data_tree;
			int detectorID;
			std::string strip_axis="";

      TTreeReader *reader;
      TTreeReaderValue<int> *evtID;
      TTreeReaderValue<int> *nch;
			

      TTreeReaderArray<int> *detID;
      TTreeReaderArray<int> *strip;
      TTreeReaderArray<int> *planeID;
      TTreeReaderArray<int> *hitTimebin;

			
			std::vector<TTreeReaderArray<int16_t>*> *adcs; 

			// Hits_Plane < strip, maxAdc, hitIndex>
			std::vector<std::tuple<int16_t, int16_t, int8_t>> Hits_Plane_X;
			std::vector<std::tuple<int16_t, int16_t, int8_t>> Hits_Plane_Y;
    

			// Cluster_Plane < strip, sumAdcs, cluster_size >
			std::vector<std::tuple<int16_t, int16_t, int16_t>> Clusters_Plane_X;
			std::vector<std::tuple<int16_t, int16_t, int16_t>> Clusters_Plane_Y;


      // XYclusters: < x_strip, y_strip, sum_charge, sum_clustSizes, Y_charge/X_charge >
		  std::vector<std::tuple<int, int, int, int, double>> curXYclusters;

			// Container for all matched xy clusters
		  std::vector<std::vector<std::tuple<int, int, int, int, double>>> allClusters;



			TH1F * TempPlaneClusterHistogram;
			TH1F * curMaxHitWaveform;
			TH1F * maxHitWaveform_x;
			TH1F * maxHitWaveform_y;


			Long64_t number_of_entries=0;
			int m_eventNumber=0;
  };

} // namespace corryvreckan
#endif // EventLoaderAPV25_H
