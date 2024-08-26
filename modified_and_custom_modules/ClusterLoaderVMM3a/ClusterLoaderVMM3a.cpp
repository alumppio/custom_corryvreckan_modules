/**
 * @file
 * @brief Implementation of module ClusterLoaderVMM3a
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include "ClusterLoaderVMM3a.h"
#include <memory>
#include "core/utils/log.h"
#include <iomanip>

using namespace corryvreckan;

ClusterLoaderVMM3a::ClusterLoaderVMM3a(Configuration& config, std::shared_ptr<Detector> detector)
    : Module(config, detector), m_detector(detector) {

      m_inputFile = config_.get<std::string>("file_input");
      LOG(DEBUG) << "Input file name: " << m_inputFile;
      LOG(DEBUG) << "Detector name: " << m_detector->getName();
			config_.setDefault<std::string>("pos_input_type", "pos");
			pos_input_type_ = config_.get<std::string>("pos_input_type"); 

			}

void ClusterLoaderVMM3a::initialize() {
		
    LOG(DEBUG) << "Initialize ClusterLoaderVMM3a for " << m_detector->getName();

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
		adc0 = new TTreeReaderValue<uint16_t>(*reader, "adc0");
		adc1 = new TTreeReaderValue<uint16_t>(*reader, "adc1");
    

		// If wanted can use the charge2 cluster position
		if (pos_input_type_ == "charge2") {
      pos0 = new TTreeReaderValue<double>(*reader, "pos0_charge2");
      pos1 = new TTreeReaderValue<double>(*reader, "pos1_charge2");
		} 

		// By default using the COM position
		else {
			pos0 = new TTreeReaderValue<double>(*reader, "pos0");
			pos1 = new TTreeReaderValue<double>(*reader, "pos1");
		}

		// Initialise member variables
    m_entry = 0;
}

StatusCode ClusterLoaderVMM3a::run(const std::shared_ptr<Clipboard>& clipboard) {
    reader->SetLocalEntry(m_entry);

    auto event = clipboard->getEvent();

    // Make a container for pixels
    PixelVector pixelContainer;


    // Using the first hit as reference, reset the time at the start of every event
    if (current_first==-1) current_first=**time0;

    while (true){

      // Current time is the reference to the first entry of the event defined by the metronome
      current_time = **time0-current_first;

      LOG(DEBUG) << std::fixed << std::setprecision(10) << "(Entry " << m_entry << "):  current_time  =  " << current_time << ",  **time  =  " << **time0;


      // Data has to be in the event timewindow, using event->duration()
      // If current_time > event->duration(), breaking the event

      if (current_time > event->duration()){
        LOG(DEBUG) << "============== Data outside event, breaking =============="; break;
        }


      // Change the detector names to match the corryvreckan geometry file! 
      if(strcmp(m_detector->getName().c_str(), "GEMXY1")==0) detectorID=1;
      if(strcmp(m_detector->getName().c_str(), "GEMXY2")==0) detectorID=2;
      if(strcmp(m_detector->getName().c_str(), "GEMXY3")==0) detectorID=3;
      if(strcmp(m_detector->getName().c_str(), "GEMXY4")==0) detectorID=4;


      // Pixel args: 
      //  for x-plane: [detector_name, pos0 (col), pos1 (row), raw (set to 1 if not known), adc0+adc1 (charge), time0]

      if (detectorID==static_cast<int>(**det) && **pos0 != 0 && **pos1 != 0){
				
				auto pixel = std::make_shared<Pixel>(m_detector->getName(), **pos0, **pos1, 1, **adc0+**adc1, **time0);
				
        LOG(DEBUG) << "Cluster in Detector: " << m_detector->getName();
        LOG(DEBUG) << "Making Pixel =  det  :  col(pos0)  : row(pos1)  :  charge(adc0+adc1)  :  time0";
        LOG(DEBUG) << std::fixed << std::setprecision(15) <<  "      " << static_cast<int>(**det) << "      " << **pos0 << "      " << **pos1 << "      " << **adc0+**adc1 << "      " << **time0;
				pixelContainer.push_back(pixel);
      }
      else {LOG(DEBUG) << "  No clusters in " << m_detector->getName();}

      // Next entry
      m_entry++;

      // Check again if there are any entries left, if not, end the run
      if (m_entry > number_of_entries){
        LOG(DEBUG) << "Run ended, all entries read";
        break;
        }
			// Otherwise read in the next entry as the event hasn't ended
      else {reader->SetLocalEntry(m_entry);}
    } // while(true)

		// End the run if all entries read
    if (m_entry > number_of_entries){return StatusCode::EndRun;}



    // Reset the current event reference
    current_first = -1;

    // Add data to clipboard, for each detector
    clipboard->putData(pixelContainer, m_detector->getName());


    // Return value telling analysis to keep running
    return StatusCode::Success;
}

void ClusterLoaderVMM3a::finalize(const std::shared_ptr<ReadonlyClipboard>&) {

  // Event = many entries, m_entry = cluster number, in our case
  LOG(DEBUG) << "Analysed " << m_entry << " entries";

	}
