# Clause

Chatopera Language Understanding Service

https://github.com/chatopera/clause

<img src="https://user-images.githubusercontent.com/3538629/63484912-ecb5ed00-c4d3-11e9-8a66-7bc32fee6fe7.png" width="500">

## Featured

- JDK, Node.js, gcc, Maven3
- thrift
- jsoncpp
- libevent
- Eigen3
- Alibaba ASR [Documentation](https://help.aliyun.com/document_detail/84569.html?spm=a2c4g.11186623.6.576.37c24efed2clkq)
- gflags
- protobuf
- gtest
- glog
- gperftools
- thrift
- rocketmq

Check out `thirdparty` for more.

## Prerequisites

You will need:

- Ubuntu 16.04 (\*required)
- CMake 3.1+ installed
- If you prefer to code in a great IDE, I highly recommend [VSCode](https://code.visualstudio.com/). It is fully compatible with this project.

## Bootstrap

Take some efforts to install dependencies, these dependencies are split into two level: System level and User level.

- System Level softwares are installed with `apt-get install` or installed to `/usr/local`, these softwares are shared between users. You should be very careful when dealing with system level softwares, it may block other people.

- User Level softwares are installed inside the project source directory which is `YOUR_GIT_REPO/include`, `YOUR_GIT_REPO/lib` and `YOUR_GIT_REPO/share` usually.

**So, when bringing up a new development environment with a bare machine, it should better install System Level Softwares by one person just once. Following developers just install User Level Softwares.**

Note, to install the softwares successfully, you need `sudo` authority without password, or you are using `root`.

Enable `sudo` without password in Ubuntu.

```
sudo visudo
## Add this line
YOUR_ACCOUNT ALL=NOPASSWD: ALL
```

### System Level Softwares

```
./admin/install-3rd/system.sh
```

It would take ~3 hrs, this script would execute every command in `cmake/system` in order.

| script              | description                                                                     | dependencies |
| ------------------- | ------------------------------------------------------------------------------- | ------------ |
| 001.corretto-jdk.sh | install amazon jdk, Default `JAVA_HOME=/usr/lib/jvm/java-1.8.0-amazon-corretto` | None         |
| 001.nodejs.sh       | install nodejs                                                                  | None         |
| 002.maven.sh        | install maven3                                                                  | jdk          |

### User Level Softwares

```
./admin/install-3rd/user.sh
```

It would take ~3 hrs, this script would execute every command in `cmake/user` in order.

### Install specific software

You can install each components manually, find the scrips in `cmake/install`, the software has dependencies with others, consider you would want to install `openpose` which depends on `opencv` and `caffe`, first, execute `./005.opencv.sh`, then execute `006.caffe.sh`, at last run `007.opencv.sh`, the priority is defined in the prefix of script name.

## Development

```
# build
mkdir -p build/debug
cd build/debug
cmake ../..
make
```

## Project Structure

There are three empty folders: `lib`, `bin`, and `include`. Those are populated by `make install`.

The rest should be obvious: `src` is the sources.

Now we can build this project, and below we show three separate ways to do so.

## File Locations

- `src/MODULE` — C++ code that ultimately compiles into a library or binary
- `src/MODULE/**_test.cpp` - C++ code to test each component.
- `bin/`, `lib`, `include` are all empty directories, until the `make install` install the project artifacts there.

## Format Code

Currently, we use [Astyle](http://astyle.sourceforge.net/astyle.html) as code formatter to keep algin code, the style guide is set in `.astylerc`.

Run code formatter.

```
./admin/formatter.sh
cat ./.astylerc # check out settings.
# Find more options
# http://astyle.sourceforge.net/astyle.html#_Usage
--style=google
--indent=spaces=2
--indent-switches
--indent-preprocessor
--break-blocks
--pad-oper
--recursive
--formatted
--suffix=none
```

It is a great habit to keep code style following the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) standard.

## Try to create new module?

Check out [src/sample](src/sample) as a template or skeleton.

## Help

- [Get started with CMake](https://github.com/chatopera/cmake-get-started)

## License

Copyright 2019 北京华夏春松科技有限公司 <https://www.chatopera.com>. All rights reserved.
This software and related documentation are provided under a license agreement containing
restrictions on use and disclosure and are protected by intellectual property laws.
Except as expressly permitted in your license agreement or allowed by law, you may not use,
copy, reproduce, translate, broadcast, modify, license, transmit, distribute, exhibit, perform,
publish, or display any part, in any form, or by any means. Reverse engineering, disassembly,
or decompilation of this software, unless required by law for interoperability, is prohibited.
