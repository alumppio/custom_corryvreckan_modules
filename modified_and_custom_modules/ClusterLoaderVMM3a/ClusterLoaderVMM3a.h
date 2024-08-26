/**
 * @file
 * @brief Definition of module ClusterLoaderVMM3a
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <iostream>


#include "core/module/Module.hpp"
#include "core/utils/log.h"
#include "objects/Cluster.hpp"
#include "objects/Pixel.hpp"
#include "objects/Track.hpp"

namespace corryvreckan {
    /** @ingroup Modules
     * @brief Module to do function
     *
     * More detailed explanation of module
     */
    class ClusterLoaderVMM3a : public Module {

    public:
        /**
         * @brief Constructor for this unique module
         * @param config Configuration object for this module as retrieved from the steering file
         * @param detector Pointer to the detector for this module instance
         */
        ClusterLoaderVMM3a(Configuration& config, std::shared_ptr<Detector> detector);

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
        std::shared_ptr<Detector> m_detector;

        Long64_t m_entry;
        Long64_t number_of_entries;
        int detectorID;
        std::string strip_axis="";
        double current_first = -1;
        double current_time = -1;


        TFile* data_file;
        TTree* data_tree;

        TTreeReader *reader;
        TTreeReaderValue<double> * time0;
        TTreeReaderValue<unsigned char> *det;
        
				TTreeReaderValue<uint16_t> *adc0;
        TTreeReaderValue<uint16_t> *adc1;
        TTreeReaderValue<double> *pos0;
        TTreeReaderValue<double> *pos1;
        TTreeReaderValue<uint16_t> *size0;
        TTreeReaderValue<uint16_t> *size1;

				// Input parameters
				std::string m_inputFile;
				std::string pos_input_type_;

    };

} // namespace corryvreckan
