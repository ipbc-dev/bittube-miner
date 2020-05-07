# HowTo Use Xmr-Stak

## Content Overview
* [Configurations](#configurations)
* [Usage on Windows](#usage-on-windows)
* [Usage on Linux & macOS](#usage-on-linux--macos)
* [Command Line Options](#command-line-options)
* [Use different backends](#use-different-backends)
* [HTML and JSON API report configuraton](#html-and-json-api-report-configuraton)

## Configurations

Before you started the miner the first time there are no config files available.
Config files will be created at the first start.
The number of files depends on the available backends.
`config.txt` contains the common miner settings.
`pools.txt` contains the selected mining pools and currency to mine.
`amd.txt`, `cpu.txt` and `nvidia.txt` contains miner backend specific settings and can be used for further tuning ([Tuning Guide](tuning.md)).

Note: If the pool is ignoring the option `rig_id` in `pools.txt` to name your worker please check the pool documentation how a worker name can be set.

## Usage on Windows
1) Double click the `bittube-miner.exe` file
2) Fill in the pool url settings, currency, username and password

`set XMRSTAK_NOWAIT=1` disable the dialog `Press any key to exit.` for non UAC execution.


## Usage on Linux & macOS
1) Open a terminal within the folder with the binary
2) Start the miner with `./bittube-miner`

## Command Line Options

The miner allow to overwrite some of the settings via command line options.
Run `bittube-miner --help` to show all available command line options.

## Use Different Backends

On linux and OSX please add `./` before the binary name `bittube-miner`.

### CPU Only:
```
bittube-miner --noAMD --noNVIDIA
```

### NVIDIA/AMD Only:

The miner will automatically detect if CUDA (for NVIDIA GPUs) or OpenCL (for AMD GPUs) is available.

```
bittube-miner --noCPU
```

### NVIDIA via OpenCL

It is possible to use the OpenCl backend which is originally created for AMD GPUs with NVIDIA GPus.
Some NVIDIA GPUs can reach better performance with this backend.

```
bittube-miner --openCLVendor NVIDIA --noNVIDIA
```

## Docker image usage

You can run the Docker image the following way:

```
docker run --rm -it -u $(id -u):$(id -g) --name fireice-uk/bittube-miner -v "$PWD":/mnt bittube-miner
docker stop bittube-miner
docker run --rm -it -u $(id -u):$(id -g) --name fireice-uk/bittube-miner -v "$PWD":/mnt bittube-miner --config config.txt
```

Debug the docker image by getting inside:

```
docker run --entrypoint=/bin/bash --rm -it -u $(id -u):$(id -g) --name fireice-uk/bittube-miner -v "$PWD":/mnt bittube-miner
```

## HTML and JSON API report configuration

To configure the reports shown on the [README](../README.md) side you need to edit the httpd_port variable. Then enable wifi on your phone and navigate to [miner ip address]:[httpd_port] in your phone browser. If you want to use the data in scripts, you can get the JSON version of the data at url [miner ip address]:[httpd_port]/api.json
