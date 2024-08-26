/**
 * @file
 * @brief Implementation of module VMM3aStripDataPreserver
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include "VMM3aStripDataPreserver.h"
#include <TDirectory.h>

using namespace corryvreckan;

VMM3aStripDataPreserver::VMM3aStripDataPreserver(Configuration& config, std::vector<std::shared_ptr<Detector>> detectors)
    : Module(config, std::move(detectors)) {

      m_inputFile = config_.get<std::string>("file_input");
      LOG(DEBUG) << "Input file name: " << m_inputFile;
      clustering_time = config_.get<double>("clustering_time");
      m_treefileName = config_.get<std::string>("output_tree_name");
    }


void VMM3aStripDataPreserver::initialize() {


    // Initializing the histograms

    for(auto& aDetector : get_regular_detectors(true)) {

      auto detectorID = aDetector->getName();

      TDirectory* directory = getROOTDirectory();
      TDirectory* local_directory = directory->mkdir(detectorID.c_str());

      if(local_directory == nullptr) {
        throw RuntimeError("Cannot create or access local ROOT directory for module " + this->getUniqueName());
      }

      local_directory->cd();


      LOG(DEBUG) << "Initialize ClusterLoaderVMM3a plots for " << detectorID;

      title = detectorID + " time0;time;entries";
      clusterTime[detectorID] = new TH1F("time0", title.c_str(), 100000, 3000000000000, 6000000000000);

      title = detectorID + " X Strip cluster size;Size;entries";
      clusterSize_x[detectorID] = new TH1F("clusterSize_x", title.c_str(), 30, 0, 30);

      title = detectorID + " Y Strip cluster size ;Size;entries";
      clusterSize_y[detectorID] = new TH1F("clusterSize_y", title.c_str(), 30, 0, 30);

      title = detectorID + " Charge X strips;Charge[e];entries";
      stripCharge_x[detectorID] = new TH1F("strip_charges_x", title.c_str(), 2000, -0.5, 1999.5);

      title = detectorID + " Charge Y strips;Charge[e];entries";
      stripCharge_y[detectorID] = new TH1F("strip_charges_y", title.c_str(), 2000, -0.5, 1999.5);

      title = detectorID + " X Strip positions;Strip_pos;entries";
      stripPos_x[detectorID] = new TH1F("strip_pos_x", title.c_str(), 256, 0, 256);

      title = detectorID + " Y Strip positions;Strip_pos;entries";
      stripPos_y[detectorID] = new TH1F("strip_pos_y", title.c_str(), 256, 0, 256);

    }



    // For the clusters_detector ROOT file reading

    data_file = TFile::Open(m_inputFile.c_str());
    if (!data_file || data_file->IsZombie()){
      LOG(DEBUG) << "Failed to open the data file: " << m_inputFile;
      throw ModuleError("Error in opening TFile");
    }

    data_tree = dynamic_cast<TTree*>(data_file->Get("clusters_detector"));
    if (!data_tree) {
        LOG(ERROR) << "Failed to retrieve TTree 'hits' from file";
        throw ModuleError("Failed to retrieve TTree 'cluster_detector'");
    }

    number_of_entries = data_tree->GetEntries();
    LOG(DEBUG) << "Number of entries in data_tree " << number_of_entries;

    reader = new TTreeReader("clusters_detector", data_file);

    det = new TTreeReaderValue<unsigned char>(*reader, "det");
    time0 = new TTreeReaderValue<double>(*reader, "time0");
    size0 = new TTreeReaderValue<uint16_t>(*reader, "size0");
    size1 = new TTreeReaderValue<uint16_t>(*reader, "size1");

    adcs0 = new TTreeReaderArray<double>(*reader, "adcs0");
    adcs1 = new TTreeReaderArray<double>(*reader, "adcs1");
    strips0 = new TTreeReaderArray<double>(*reader, "strips0");
    strips1 = new TTreeReaderArray<double>(*reader, "strips1");

    // --- For the output TTree ---

    // Create output file and directories
    auto path = createOutputFile(m_treefileName, "root");
    m_outputFile = new TFile(path.c_str(), "RECREATE");
    LOG(DEBUG) << "Made and moved to output file: " << path;
    gDirectory->Delete("tree;*");
    m_outputTree = new TTree("VMM3a_track_hit_strip_data", "VMM3a_track_hit_strip_data" );

    m_outputTree->Branch("time0", &treeTime);
		m_outputTree->Branch("det", &treeDetID);
    m_outputTree->Branch("size0", &treeSize0);
    m_outputTree->Branch("size1", &treeSize1);
    
		m_outputTree->Branch("adcs0", &vADCS0);
    m_outputTree->Branch("adcs1", &vADCS1);
    m_outputTree->Branch("strips0", &vStrips0);
    m_outputTree->Branch("strips1", &vStrips1);


    // Initialise member variables
    m_entry = 0;
    number_of_tracks = 0;
    first_entry_index = 0;
}

StatusCode VMM3aStripDataPreserver::run(const std::shared_ptr<Clipboard>& clipboard) {

    reader->SetLocalEntry(m_entry);

    // Get tracks from the clipboard
    auto tracks = clipboard->getData<Track>();

    // If there are tracks start to look for the strip data
    if (!tracks.empty()){

      for (auto& track : tracks){



        while(reader->Next()){

          // Iterate until we get an entry inside the set time
          // interval (clustering_time), when found store the index
          if(**time0 > (track->timestamp() - clustering_time/2)){
            m_entry = reader->GetCurrentEntry();
            LOG(DEBUG) << "Found first entry of the track: m_entry = " << m_entry;
            break;
            }

          } // while(reader.Next())

				// Store the index
				first_entry_index = m_entry;

				LOG(DEBUG) << "track timestamp = " << std::fixed << std::setprecision(15) << track->timestamp();
				LOG(DEBUG) << "time0** - (track->timestamp() - clustering_time/2) = " << **time0 - (track->timestamp() - clustering_time/2);


        for (auto& detector : get_regular_detectors(true)) {

          // Restart the loop for each detector
          m_entry = first_entry_index;
					LOG(DEBUG) << "first_entry_index  =  " << first_entry_index << ",  m_entry  =  " << m_entry;


          auto detectorID = detector->getName();

          // Change the detector names to match the corryvreckan geometry file!
          if(strcmp(detectorID.c_str(), "GEMXY1")==0) detID=1;
          if(strcmp(detectorID.c_str(), "GEMXY2")==0) detID=2;
          if(strcmp(detectorID.c_str(), "GEMXY3")==0) detID=3;
          if(strcmp(detectorID.c_str(), "GEMXY4")==0) detID=4;

					LOG(DEBUG) << "detectorID  =   " << detectorID << ",  detID  =  " << detID << ",  time0  =  " << **time0;

          // Loop over hits used in the clustering
          while(true){
            
						reader->SetLocalEntry(m_entry);

						// Clear containers
						vADCS0.clear();
						vADCS1.clear();
						vStrips0.clear();
						vStrips1.clear();

            LOG(DEBUG) << "Starting the loop over data";
            if (**time0 > (track->timestamp() + clustering_time/2) || m_entry > number_of_entries){
              LOG(DEBUG) << "Ending the loop over data";
              break;}

            // If correct detector and inside the time interval fill the histograms
            if (static_cast<int>(**det) == detID){
              LOG(DEBUG) << "Found track:  time0 = " << std::fixed << std::setprecision(15) << **time0 << ",  det = " << static_cast<int>(**det) << ",  size0 = " << **size0 << ",  size1 = " << **size1;
              clusterSize_x[detectorID]->Fill(**size0);
              clusterSize_y[detectorID]->Fill(**size1);
              clusterTime[detectorID]->Fill(**time0);
              treeSize0 = **size0;
              treeSize1 = **size1;
              treeTime = **time0;
              treeDetID = **det;


              for (size_t i=0; i<adcs0->GetSize(); i++){
                stripCharge_x[detectorID]->Fill(adcs0->At(i));
                stripPos_x[detectorID]->Fill(strips0->At(i));
                vADCS0.push_back(adcs0->At(i));
                vStrips0.push_back(strips0->At(i));
              }

              for (size_t j=0; j<adcs1->GetSize(); j++){
                stripCharge_y[detectorID]->Fill(adcs1->At(j));
                stripPos_y[detectorID]->Fill(strips1->At(j));
                vADCS1.push_back(adcs1->At(j));
                vStrips1.push_back(strips1->At(j));
							}

          		
							m_outputTree->Fill();
            } // if **det==detectorID

            m_entry++;

          } // while(true)

					
        }
      } // for(track : tracks)

			number_of_tracks++;
			LOG(DEBUG) << "number of tracks  =  " << number_of_tracks;

    } // if(!tracks.empty())


    // End the run if all entries read
    if (m_entry > number_of_entries){return StatusCode::EndRun;}


    // Return value telling analysis to keep running
    return StatusCode::Success;
}

void VMM3aStripDataPreserver::finalize(const std::shared_ptr<ReadonlyClipboard>&) { LOG(DEBUG) << "Analysed " << number_of_tracks << " tracks";
		
  // Writing out outputfile
  m_outputFile->Write();
  delete(m_outputFile);
  LOG(STATUS) << "Stored strip data for regular_detectors";

}

