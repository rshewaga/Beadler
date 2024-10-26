# Overview
A simple text processor to convert subtitle files downloaded from TubiTV to proper .srt formatted files.

# How to Build
```
mkdir build
cd build
..\run_cmake_script.bat
cmake --build . --config Release
```

# How to Use
```
Tubi_Subs.exe [repo dir]\example.txt
>> Output to [input path]\example.srt
```

# Description
Subtitle files downloaded from TubiTV are contained in a custom .JSON format.

```
[
    {
        "text": ["- [Narrator] In our toughest moments,"],
        "index": 1,
        "start": 1094,
        "end": 3378
    }, {
        "text": ["they respond within seconds.", "(phone rings)"],
        "index": 2,
        "start": 3378,
        "end": 5714
    }
]
```

Tubi_Subs formats this data into a [standard SRT subtitle file format](https://docs.fileformat.com/video/srt/#example-of-srt).

```
1
00:00:01,094 --> 00:00:03,378
- [Narrator] In our toughest moments,

2
00:00:03,378 --> 00:00:05,714
they respond within seconds.
(phone rings)
```