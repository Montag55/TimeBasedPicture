{
  "targets": [
    {
       "target_name": "wrapper",
       "sources": [
            "backend/wrapper.cpp",
            "backend/TBPsys/src/paint.cpp",
            "backend/TBPsys/src/base.cpp",
            "backend/TBPsys/src/segment.cpp",
            "backend/TBPsys/src/interpretation.cpp",
            "backend/TBPsys/src/average.cpp",
            "backend/TBPsys/src/transferfunction.cpp",
            "backend/TBPsys/src/boost.cpp",
            "backend/TBPsys/src/boostColor.cpp",
            "backend/TBPsys/src/reduce.cpp",
            "backend/TBPsys/src/overplott.cpp",
            "backend/TBPsys/src/circularfade.cpp",
            "backend/TBPsys/src/utils.cpp",
            "backend/TBPsys/src/timefadepoints.cpp",
            "backend/TBPsys/src/singleimage.cpp"

       ],
       "include_dirs": [
         "backend/TBPsys/include/"
        ],
       "libraries": [
           "/usr/local/lib/libopencv_*.so"
        ],
        'cflags_cc!': ['-fno-rtti', '-fno-exceptions'],
        'cflags_cc+': ['-frtti', '-fexceptions', '-w']
    }
  ]

}
