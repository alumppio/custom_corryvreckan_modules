/**
 * @file
 * @brief Definition of module VMM3aStripDataPreserver
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
    class VMM3aStripDataPreserver : public Module {

    public:
        /**
         * @brief Constructor for this unique module
         * @param config Configuration object for this module as retrieved from the steering file
         * @param detectors Pointer to the detectors for this module instance
         */
        VMM3aStripDataPreserver(Configuration& config, std::vector<std::shared_ptr<Detector>> detectors);
        ~VMM3aStripDataPreserver() {}

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
        Long64_t m_entry;
        Long64_t first_entry_index;
        Long64_t number_of_entries;
        Long64_t number_of_tracks;
        std::string m_inputFile;
        std::string title;
        double clustering_time;
        int detID;

        TFile* data_file;
        TTree* data_tree;

        TTreeReader *reader;
        TTreeReaderValue<unsigned char> *det;
        TTreeReaderValue<double> *time0;
        TTreeReaderValue<uint16_t> *size0;
        TTreeReaderValue<uint16_t> *size1;

        TTreeReaderArray<double> *strips0;
        TTreeReaderArray<double> *strips1;
        TTreeReaderArray<double> *adcs0;
        TTreeReaderArray<double> *adcs1;

        std::map<std::string, TH1F*> clusterSize_x;
        std::map<std::string, TH1F*> clusterSize_y;
        std::map<std::string, TH1F*> clusterTime;
        std::map<std::string, TH1F*> stripCharge_x;
        std::map<std::string, TH1F*> stripCharge_y;
        std::map<std::string, TH1F*> stripPos_x;
        std::map<std::string, TH1F*> stripPos_y;

        // TTree variables
				unsigned char treeDetID;
        double treeTime;
        uint16_t treeSize0;
        uint16_t treeSize1;
        std::vector<double> vStrips0;
        std::vector<double> vStrips1;
        std::vector<double> vADCS0;
        std::vector<double> vADCS1;

        std::string m_treefileName;

        TFile* m_outputFile;
        TTree* m_outputTree{};
    };

} // namespace corryvreckan
