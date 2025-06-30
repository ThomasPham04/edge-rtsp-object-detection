SET(PADDLE_LIB_DIR "/opt/paddle_inference")
include_directories("${PADDLE_LIB_DIR}/third_party/install/protobuf/include")
include_directories("${PADDLE_LIB_DIR}/third_party/install/glog/include")
include_directories("${PADDLE_LIB_DIR}/third_party/install/gflags/include")
include_directories("${PADDLE_LIB_DIR}/third_party/install/xxhash/include")
include_directories("${PADDLE_LIB_DIR}/third_party/install/zlib/include")
include_directories("${PADDLE_LIB_DIR}/third_party/install/onnxruntime/include")
include_directories("${PADDLE_LIB_DIR}/third_party/install/paddle2onnx/include")
include_directories("${PADDLE_LIB_DIR}/third_party/boost")
include_directories("${PADDLE_LIB_DIR}/third_party/eigen3")
include_directories("${PADDLE_LIB_DIR}/paddle/include")
set (PADDLE_LIBS
        ${PADDLE_LIB_DIR}/paddle/lib/libpaddle_inference.so
        ${PADDLE_LIB_DIR}/paddle/lib/libcommon.so
        ${PADDLE_LIB_DIR}/paddle/lib/libphi.so
        ${PADDLE_LIB_DIR}/third_party/install/cryptopp/lib/libcryptopp.a
        ${PADDLE_LIB_DIR}/third_party/install/glog/lib/libglog.a
        ${PADDLE_LIB_DIR}/third_party/install/openblas/lib/libopenblas.a
        ${PADDLE_LIB_DIR}/third_party/install/protobuf/lib/libprotobuf.a
        ${PADDLE_LIB_DIR}/third_party/install/utf8proc/lib/libutf8proc.a
        ${PADDLE_LIB_DIR}/third_party/install/xxhash/lib/libxxhash.a
        ${PADDLE_LIB_DIR}/third_party/install/gflags/lib/libgflags.a
        )