/**
 * @file
 * @brief Definition of module TreeWriter
 *
 * @copyright Copyright (c) 2017-2020 CERN and the Corryvreckan authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#ifndef TreeWriter_H
#define TreeWriter_H 1

#include <Math/Point3D.h>
#include <Math/Vector3D.h>
#include <Math/Point2D.h>
#include <Math/Vector2D.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>

#include "core/module/Module.hpp"
#include "objects/Track.hpp"
#include "objects/Cluster.hpp"

namespace corryvreckan {
  /** @ingroup Modules
  */
  class TreeWriter : public Module {

    public:
      // Constructors and destructors
      //TreeWriter(Configuration& config, std::shared_ptr<Detector> detector);
      TreeWriter(Configuration& config, std::vector<std::shared_ptr<Detector>> detectors);
      ~TreeWriter() {}

      // Functions
      void initialize() override;
      StatusCode run(const std::shared_ptr<Clipboard>& clipboard) override;
      void finalize(const std::shared_ptr<ReadonlyClipboard>& clipboard) override;

    private:
      std::shared_ptr<Detector> m_detector;

      int eventID;
      int filledEvents;
			int emptyEvents;

      std::vector<std::string> m_objectList;
      std::map<std::string, Object*> m_objects;

			double xyCharge_gem1;
			double xyCharge_gem2;
			double xyCharge_gem3;

			double clustPos_x_gem1;
			double clustPos_x_gem2;
			double clustPos_x_gem3;

			double clustPos_y_gem1;
			double clustPos_y_gem2;
			double clustPos_y_gem3;

			double trackIntercept_x_gem1;
			double trackIntercept_x_gem2;
			double trackIntercept_x_gem3;

			double trackIntercept_y_gem1;
			double trackIntercept_y_gem2;
			double trackIntercept_y_gem3;

			ROOT::Math::XYZPoint intercept_at_800mm;
			
			ROOT::Math::XYZVector trackDirection;

      TFile* m_outputFile;
      TTree* m_outputTree{};

      // Config parameters
      std::string m_fileName;
      std::string m_treeName;
  };
} // namespace corryvreckan
#endif // TreeWriter_H
