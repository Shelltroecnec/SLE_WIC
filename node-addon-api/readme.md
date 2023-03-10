# Nodejs Bindings for Ultrafab low level driver
This document describe how to compile a low level driver and creating a nodejs bindings. Nodejs bindings are wrapper functions that are directly called from the exported module in nodejs to call the C/C++ code.



Refer the [Node-addon-api](https://nodejs.org/api/addons.html#linking-to-libraries-included-with-nodejs) link for more details.


## How to add a C/C++ file to add in your low level driver
- add a new file in src directory with extension .c or .cc
- add newly file in bindings.gyp file inside the sorce, lets say test.c is new file we want to add so we should do it as below.
	```
	{
		"targets": [
			{
			"target_name": "`ultrafab_ll_addon`",
			"cflags!": [ "-fno-exceptions -lm -lpthread -Wunused-result" ],
			"cflags_cc!": [ "-fno-exceptions" ],
			"sources": [  
							"addon.cc", 
							"../src/main.c",
							"../src/test.c", 
			]
		}
	}
	```

## How to build the bindings
```
npm install
```
## how to Run the JS script to call the binding
```
node addon.js
```