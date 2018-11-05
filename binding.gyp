{
  "targets": [
    {
       "target_name": "wrapper",
       "sources": [
            "backend/wrapper.cpp",
            "backend/TBPsys/src/base.cpp",
            "backend/TBPsys/src/segment.cpp",
            "backend/TBPsys/src/interpretation.cpp",
            "backend/TBPsys/src/average.cpp",
            "backend/TBPsys/src/transferfunction.cpp"
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
