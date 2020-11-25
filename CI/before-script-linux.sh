#!/bin/bash

set -ex
ccache -s || echo "CCache is not available."
mkdir build && cd build
ls ..
ls ../libwebrtc/include/
ls ../libwebrtc/include/api
cmake -DBUILD_CAPTIONS=ON -DBUILD_BROWSER=ON -DCEF_ROOT_DIR="../cef_binary_${CEF_BUILD_VERSION}_linux64" -DLibWebRTC_DIR="../libwebrtc/cmake" -DWEBRTC_INCLUDE_DIR="../libwebrtc/include" -DWEBRTC_LIB="../libwebrtc/lib/libwebrtc.a" ..
