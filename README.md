# Default corryvreckan installation
To install the default version of corryvreckan you need to have [ROOT](https://root.cern.ch/building-root) installed, with the GenVector component.

**First you need to download the source code** 
```
$ git clone https://gitlab.cern.ch/corryvreckan/corryvreckan.git
```
**Then you need to compile, but not install it until you have added my custom modules**
```
$ cd corryvreckan
$ mkdir build && cd build/
$ cmake ..
```

## To use my custom modules

I have made quite a lot of changes to the default corryvreckan installation. I don't remember all of them, but I included a folder that contains most (or almost all) of the modules I modified or created. You need to copy this to some folder presumably not inside corryvreckan, as I don't known if this will cause errors. Then you have to go to the source code modules folder *corryvreckan/src/modules/* and copy the modules that I have created. If the module is already present in the default version you have created with cmake, then the module I have in my folder is a modified one and you need to overwrite it in the default version.

### Copy my repository
```
$ git clone https://github.com/alumppio/custom_corryvreckan_modules.git <desired_path>
```

### Copying the modified modules and the new modules to <corryvreckan/src/modules>
```
>> For the modified modules
$ cd corryvreckan/src/modules
$ rm -r Tracking4D
$ cp <path_to_modified_Tracking4D> .
>> For the new modules
$ cp <all_other_modules> . 
```

### Installing the custom corryvreckan vesrion

**After this you need to install corryvreckan!** This means that you only need to go to the build folder and run *make install*. Also everytime you change something in the modules, you need run *make install* so the changes will be applied. You should alias this *make install* in the build folder to some command, makes life easier.


>> After making changes to modules go to the corryvreckan/build folder and run. Also if you are using lxplus remember to also source etc/setup\_lxplus.sh in corryvreckan folder



```
>> If you are using lxplus, in <corryvreckan>
$ source etc/setup_lxplus.sh
>> Installing in <corryvreckan/build>
$ make install
```


## Getting started with corryvreckan

There is a list of tutorials and references on the [corryvreckan project website](https://project-corryvreckan.web.cern.ch/project-corryvreckan/page/publications/). You can also find the detailed user manual that describes the whole framework under the documentations and references tab ([link for manual-v2.0.1](https://project-corryvreckan.web.cern.ch/project-corryvreckan/usermanual/corryvreckan-manual-v2.0.1.pdf)).

There is also a quite detailed [lab course on silicon sensors](https://www.physi.uni-heidelberg.de/Einrichtungen/FP/anleitungen/F96.pdf) where corryvreckan is used.

### Some tips for troubleshooting

If your are looking at plots and they seem weird, check the binning and how it is filled in the source code of the module. 
