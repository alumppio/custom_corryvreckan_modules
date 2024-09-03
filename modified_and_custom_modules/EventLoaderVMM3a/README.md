---
# SPDX-FileCopyrightText: 2017-2023 CERN and the Corryvreckan authors
# SPDX-License-Identifier: CC-BY-4.0 OR MIT
---
# EventLoaderVMM3a
**Maintainer**: Antti Lumppio	(antti.lumppio@helsinki.fi)
**Module Type**: *DETECTOR* **Detector Type**: *<GEM detector>*  
**Status**: Immature

### Description
This module is used in the long pixel approach. The module reads in VMM3a data in vmm-sdat hits TTree format. From this hits data, Pixel objects for each strip hit are created.

### Parameters
* `file_input`: The input data file that contains the hits TTree.

### Plots produced
No plots are produced.

### Usage
```toml
[EventLoaderVMM3a]
file_input = "data_file"


```
