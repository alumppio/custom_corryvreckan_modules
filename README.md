# Default corryvreckan installation
To install the default version of corryvreckan you need to have [ROOT](https://root.cern.ch/building-root) installed, with the GenVector component.

**First you need to download the source code** 
```
$ git clone https://gitlab.cern.ch/corryvreckan/corryvreckan.git
```
**Then you need to compile and install it**
```
$ cd corryvreckan
$ mkdir build && cd build/
$ cmake ..
$ make install
```

## To use my custom modules

I have made quite a lot of changes to the default corryvreckan installation. I don't remember all of them, but I included a folder that contains most (or almost all) of the modules I modified or created. You need to copy this to some folder presumably not inside corryvreckan, as I don't known if this will cause errors. Then you need to manually remove the modules that I have modified, and only copy those modules that I have created as new modules. One way you could do this is that you go in the *corryvreckan/src/modules* folder and use *rm -r* to delete the directories.

```
>> For the modified modules
$ cd corryvreckan/src/modules
$ rm -r Tracking4D
$ cp path_to_modified_Tracking4D .
... 
```


```
>> For the new modules
$ cp path_to_new_module . (assming you are in .../corryvreckan/src/modules)
```

