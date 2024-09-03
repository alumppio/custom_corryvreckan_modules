---
# SPDX-FileCopyrightText: 2017-2023 CERN and the Corryvreckan authors
# SPDX-License-Identifier: CC-BY-4.0 OR MIT
---
# ClusterLoaderVMM3a
**Maintainer**: Antti Lumppio (antti.lumppio@helsinki.fi)
**Module Type**: *DETECTOR* **Detector Type**: *<GEM detector>*  
**Status**: Immature

### Description
This module loads in vmm-sdat VMM3a data from clusters\_detector ROOT TTree. From these matched XY clusters, Pixel objects are created with pos0 as the column and pos1 as the row. The pixel's charge is the sum of the X- and Y-plane cluster charges. 


### Parameters
* `file\_input`: The ROOT file name that contains the clusters\_detector TTree.
* `pos\_input\_type`: Specifies the reconstructed position type from vmm-sdat that you want to use to create the Pixel objects. Currently supports only `pos` and `charge2_pos`, defaults to `pos`.



### Plots produced
No plots produced. 

### Usage
```toml
[ClusterLoaderVMM3a]
file_input = "data_file"

```
