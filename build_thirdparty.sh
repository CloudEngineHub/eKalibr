# update submodules
echo "----------------------------------------------------"
echo "update submodules remotely, it may take some time..."
echo "----------------------------------------------------"
git submodule update --init --recursive
if [ $? -ne 0 ]; then
    echo "--------------------------------------------"
    echo "error occurs when updating submodules, exit!"
    echo "--------------------------------------------"
    exit
fi

# shellcheck disable=SC2046
EKALIBR_ROOT_PATH=$(cd $(dirname $0) || exit; pwd)
echo "the root path of 'ctraj': ${EKALIBR_ROOT_PATH}"

# build tiny-viewer
echo "----------------------------------"
echo "build thirdparty: 'tiny-viewer'..."
echo "----------------------------------"

mkdir ${EKALIBR_ROOT_PATH}/thirdparty/tiny-viewer-build
# shellcheck disable=SC2164
cd "${EKALIBR_ROOT_PATH}"/thirdparty/tiny-viewer-build

cmake ../tiny-viewer
echo current path: $PWD
echo "-----------------------------"
echo "start making 'tiny-viewer'..."
echo "-----------------------------"
make -j8
cmake --install . --prefix ${EKALIBR_ROOT_PATH}/thirdparty/tiny-viewer-install

# build opengv
echo "-----------------------------"
echo "build thirdparty: 'opengv'..."
echo "-----------------------------"

mkdir ${EKALIBR_ROOT_PATH}/thirdparty/opengv-build
# shellcheck disable=SC2164
cd "${EKALIBR_ROOT_PATH}"/thirdparty/opengv-build

cmake ../opengv
echo current path: $PWD
echo "------------------------"
echo "start making 'opengv'..."
echo "------------------------"
make -j8
cmake --install . --prefix "${EKALIBR_ROOT_PATH}/thirdparty/opengv-install"