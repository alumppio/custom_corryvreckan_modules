/**
 * @file
 * @brief Implementation of module EventLoaderVMM3a
 *
 * @copyright Copyright (c) 2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include "EventLoaderVMM3a.h"
#include <memory>
#include "core/utils/log.h"
#include <iomanip>

using namespace corryvreckan;

EventLoaderVMM3a::EventLoaderVMM3a(Configuration& config, std::shared_ptr<Detector> detector)
    : Module(config, detector), m_detector(detector) {
			
			
			m_inputFile = config_.get<std::string>("file_input");
      LOG(DEBUG) << "Input file name: " << m_inputFile;
      LOG(DEBUG) << "Detector name: " << m_detector->getName();
			}

void EventLoaderVMM3a::initialize() {
		
		LOG(DEBUG) << "Initialize EventLoaderVMM3a for " << m_detector->getName();
		
		data_file = TFile::Open(m_inputFile.c_str());
		if (!data_file || data_file->IsZombie()){
			LOG(DEBUG) << "Failed to open the data file: " << m_inputFile;
			throw ModuleError("Error in opening TFile");
		}

		data_tree = dynamic_cast<TTree*>(data_file->Get("hits"));
    if (!data_tree) {
        LOG(ERROR) << "Failed to retrieve TTree 'hits' from file";
        throw ModuleError("Failed to retrieve TTree 'hits'");
    }

		
		number_of_entries = data_tree->GetEntries();
		LOG(DEBUG) << "Number of entries in data_tree " << number_of_entries;

		reader = new TTreeReader("hits", data_file);

		det = new TTreeReaderValue<unsigned char>(*reader, "det");
		plane = new TTreeReaderValue<unsigned char>(*reader, "plane");
		time = new TTreeReaderValue<double>(*reader, "time");
		adc = new TTreeReaderValue<uint16_t>(*reader, "adc");
		pos = new TTreeReaderValue<uint16_t>(*reader, "pos");
		over_threshold = new TTreeReaderValue<bool>(*reader, "over_threshold");
		
    // Initialise member variables
    m_entry = 0;
}

StatusCode EventLoaderVMM3a::run(const std::shared_ptr<Clipboard>& clipboard) {
		
		reader->SetLocalEntry(m_entry);
		
		auto event = clipboard->getEvent();

		// Make a container for ''pixels'' for both planes
		PixelVector Xstrip_pixelContainer;
		PixelVector Ystrip_pixelContainer;
		

		// Using the first hit as reference, reset the time at the start of every event
		if (current_first==-1) current_first=**time;

		while (true){
			
			// Current time is the reference to the first entry of the event defined by the metronome
			current_time = **time-current_first;

			LOG(DEBUG) << std::fixed << std::setprecision(10) << "(Entry " << m_entry << "):  current_time  =  " << current_time << ",  **time  =  " << **time;
			

			// Data has to be in the event timewindow, using event->duration()
			// If current_time > event->duration(), breaking the event
			
			if (current_time > event->duration()){
				LOG(DEBUG) << "============== Data outside event, breaking =============="; break;
				}


			// Change the detector names to match the corryvreckan geometry file! 
			if(strcmp(m_detector->getName().c_str(), "GEMX1")==0 || strcmp(m_detector->getName().c_str(), "GEMY1")==0) detectorID=1;
			if(strcmp(m_detector->getName().c_str(), "GEMX2")==0 || strcmp(m_detector->getName().c_str(), "GEMY2")==0) detectorID=2;
			if(strcmp(m_detector->getName().c_str(), "GEMX3")==0 || strcmp(m_detector->getName().c_str(), "GEMY3")==0) detectorID=3;
			if(strcmp(m_detector->getName().c_str(), "GEMX4")==0 || strcmp(m_detector->getName().c_str(), "GEMY4")==0) detectorID=4;

			if(strcmp(m_detector->getName().c_str(), "GEMX1")==0) strip_axis="X";
			if(strcmp(m_detector->getName().c_str(), "GEMY1")==0) strip_axis="Y";
			if(strcmp(m_detector->getName().c_str(), "GEMX2")==0) strip_axis="X";
			if(strcmp(m_detector->getName().c_str(), "GEMY2")==0) strip_axis="Y";
			if(strcmp(m_detector->getName().c_str(), "GEMX3")==0) strip_axis="X";
			if(strcmp(m_detector->getName().c_str(), "GEMY3")==0) strip_axis="Y";
			if(strcmp(m_detector->getName().c_str(), "GEMX4")==0) strip_axis="X";
			if(strcmp(m_detector->getName().c_str(), "GEMY4")==0) strip_axis="Y";


			// Pixel args: 
			// 	for x-plane: [detector_name, pos (col), 0 (row), raw (set to 1 if not known), adc, time]
			// 	for y-plane: [detector_name, 0 (col), pos (row), raw (set to 1 if not known), adc, time]

			// Has to be the correct plane and strip
			if (static_cast<int>(**plane)==0 && detectorID==static_cast<int>(**det) && strip_axis=="X" && **over_threshold == 1){
				auto pixel = std::make_shared<Pixel>(m_detector->getName(), **pos, 0, 1, **adc, **time);
				pixelContainer.push_back(pixel);
				LOG(DEBUG) << "HIT in Detector: " << m_detector->getName();
				LOG(DEBUG) << "  det  :  plane  :  pos  :  adc  :  **time";
				LOG(DEBUG) << std::fixed << std::setprecision(15) <<  "  " << static_cast<int>(**det) << "  " << static_cast<int>(**plane) << "  " << **pos << "  " << **adc << "  " << **time;
			}
			else {LOG(DEBUG) << "  No hits in x-plane";}
			
			if (static_cast<int>(**plane)==1 && detectorID==static_cast<int>(**det) && strip_axis=="Y" && **over_threshold == 1){
				auto pixel = std::make_shared<Pixel>(m_detector->getName(), 0, **pos, 1, **adc, **time);
				pixelContainer.push_back(pixel);
				LOG(DEBUG) << "HIT in Detector: " << m_detector->getName();
				LOG(DEBUG) << "  det  :  plane  :  pos  :  adc  :  **time";
				LOG(DEBUG) << std::fixed << std::setprecision(15) <<  "  " << static_cast<int>(**det) << "  " << static_cast<int>(**plane) << "  " << **pos << "  " << **adc << "  " << **time;
			}
			else {LOG(DEBUG) << "  No hits in y-plane";}
			
			// Next entry
			m_entry++;

			// Check again if there are any entries left, if not, end the run
			if (m_entry > number_of_entries){
				LOG(DEBUG) << "Run ended, all entries read";
				break;
				}
			else {reader->SetLocalEntry(m_entry);}
		
		} // while(true)


		// Clustering the individual strip hits
		

		// End the run if all entries read
		if (m_entry > number_of_entries){return StatusCode::EndRun;}

		// Reset the current event reference
		current_first = -1;

		// Add data to clipboard, for each detector
   	clipboard->putData(pixelContainer, m_detector->getName());
	
		
		// Return value telling analysis to keep running
    return StatusCode::Success;
}





void EventLoaderVMM3a::finalize(const std::shared_ptr<ReadonlyClipboard>&) { 
	
	// Event = many entries, m_entry = hit, in our case
	LOG(DEBUG) << "Analysed " << m_entry << " hits"; }
