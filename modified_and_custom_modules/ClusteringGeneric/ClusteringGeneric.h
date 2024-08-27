/**
 * @file
 * @brief Definition of module ClusteringGeneric
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#ifndef ClusteringGeneric_H
#define ClusteringGeneric_H 1

#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
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
#include "objects/Cluster.hpp"

namespace corryvreckan {
  /** @ingroup Modules
   * @brief Module to do function
   *
   * More detailed explanation of module
   */
  class ClusteringGeneric : public Module {

    public:
      /**
       * @brief Constructor for this unique module
       * @param config Configuration object for this module as retrieved from the steering file
       * @param detector Pointer to the detector for this module instance
       */
      ClusteringGeneric(Configuration& config, std::shared_ptr<Detector> detector);

      /**
       * @brief Destructor deletes the internal objects read from ROOT Tree
       */
      ~ClusteringGeneric() override;


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

      std::shared_ptr<Detector> m_detector;

      int m_eventNumber;
			double noise_cut;
      std::string fileInput;

      TTreeReader *reader;
      TTreeReaderArray<int> *evtID;
      TTreeReaderArray<int> *detID;
      TTreeReaderArray<int> *planeID;
      TTreeReaderArray<float> *clustPos;
      TTreeReaderArray<float> *clustADCs; 
      // TTreeReaderArray<int> *clustSize;

			std::vector<float> temp_cluster_Xpos_container;
			std::vector<float> temp_cluster_Ypos_container;

			std::vector<float> temp_cluster_Xcharge_container;
			std::vector<float> temp_cluster_Ycharge_container;
			float x, y, charge_x, charge_y;

      // Cluster histograms
      TH1F* clusterCharge;
      TH1F* clusterMultiplicity;
			TH1F* clusterSize_x;
			TH1F* clusterSize_y;
      TH2F* clusterPositionGlobal;
      TH2F* clusterPositionLocal;

			// Cluster charge histograms
    	TH1F* ClusterXCharge;
    	TH1F* ClusterYCharge;
			TH1F* ClusterXvsYCharge;


			ROOT::Math::XYVector spatial_cut_;

		
			int number_of_misses=0;
			int number_of_clusters=0;

			// Preclustering cluster plot
			TH2F* rawClusters;


  };

} // namespace corryvreckan
#endif // ClusteringGeneric_H
